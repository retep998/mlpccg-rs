//////////////////////////////////////////////////////////////////////////////
// NoLifePony - Pony Card Game                                              //
// Copyright © 2014 Peter Atashian                                          //
//                                                                          //
// This program is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU Affero General Public License as           //
// published by the Free Software Foundation, either version 3 of the       //
// License, or (at your option) any later version.                          //
//                                                                          //
// This program is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
// GNU Affero General Public License for more details.                      //
//                                                                          //
// You should have received a copy of the GNU Affero General Public License //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//////////////////////////////////////////////////////////////////////////////

#include "player.hpp"
#include "game.hpp"
#include "server.hpp"
#include "packet_handler.hpp"
#include <utility/format.hpp>
#include <SFML/Network/Packet.hpp>
#include <iostream>
#include <map>
#include <set>

namespace nlp {
    player::player(ptr<packet_handler> p_send, uint32_t p_id, ptr<server> p_server) :
        m_send{p_send},
        m_packet{std::make_unique<sf::Packet>()},
        m_id{p_id},
        m_server{p_server} {
        m_name = default_name();
        std::cout << time() << get_name() << " connected." << std::endl;
        send_id();
        send_player_joined();
        send_game_created();
        m_server->for_player([this](auto & p) {
            p.send_player_joined(this);
        });
    }
    player::~player() {
        m_server->for_player([this](auto & p) {
            p.send_player_left(this);
        });
        std::cout << time() << get_name() << " disconnected." << std::endl;
    }
    uint32_t player::get_id() const {
        return m_id;
    }
    std::string const & player::get_name() const {
        return m_name;
    }
    void player::update() {
        if (is_disconnected()) {
            return;
        }
        auto now = std::chrono::steady_clock::now();
        if (m_ping_id) {
            if (now - m_ping > std::chrono::seconds{20}) {
                std::cout << time() << get_name() << " timed out." << std::endl;
                disconnect();
            }
        } else if (now - m_ping > std::chrono::seconds{15}) {
            send_ping();
        }
    }
    bool player::is_disconnected() const {
        return m_disconnected;
    }
    std::string player::default_name() const {
        std::ostringstream ss;
        ss << std::hex << std::uppercase << m_id;
        return "Pony" + ss.str();
    }
    void player::disconnect() {
        if (!is_disconnected()) {
            m_disconnected = true;
            m_send->disconnect();
        }
    }
    void player::handle(sf::Packet & p) {
        uint16_t opcode{};
        p >> opcode;
        switch (opcode) {
        case 0x0001: {
            uint32_t id{};
            p >> id;
            send_pong(id);
        } break;
        case 0x0002: {
            uint32_t id{};
            p >> id;
            if (m_ping_id && id == m_ping_id) {
                m_ping = std::chrono::steady_clock::now();
                m_ping_id = 0;
            }
        } break;
        case 0x0003: {
            std::cout << time() << get_name() << " has renamed themselves to ";
            p >> m_name;
            if (m_name.length() > 20) {
                m_name.resize(20);
            }
            if (m_name.empty()) {
                m_name = default_name();
            }
            std::cout << get_name() << std::endl;
            m_server->for_player([this](auto & p) {
                p.send_player_joined(this);
            });
        } break;
        }
    }
    void player::start() {
        m_packet->clear();
    }
    void player::send() {
        m_send->handle(*m_packet);
    }
    void player::send_pong(uint32_t p_id) {
        start();
        *m_packet << uint16_t{0x0002} << p_id;
        send();
    }
    void player::send_id() {
        start();
        *m_packet << uint16_t{0x0004} << m_id;
        send();
    }
    void player::send_ping() {
        start();
        std::uniform_int_distribution<uint32_t> dist{1, std::numeric_limits<uint32_t>::max()};
        m_ping_id = dist(m_server->rng());
        *m_packet << uint16_t{0x0001} << m_ping_id;
        send();
    }
    void player::send_player_joined(ptr<player> p_player) {
        start();
        *m_packet << uint16_t{0x000B};
        if (p_player) {
            *m_packet << uint32_t{1};
            *m_packet << p_player->get_id() << p_player->get_name();
        } else {
            *m_packet << m_server->total_players();
            m_server->for_player([this](auto & p) {
                *m_packet << p.get_id() << p.get_name();
            });
        }
        send();
    }
    void player::send_game_created(ptr<game> p_game) {
        start();
        *m_packet << uint16_t{0x0006};
        if (p_game) {
            *m_packet << uint32_t{1};
            *m_packet << p_game->get_id() << p_game->get_name();
        } else {
            *m_packet << m_server->total_players();
            m_server->for_game([this](auto & g) {
                *m_packet << g.get_id() << g.get_name();
            });
        }
        send();
    }
    void player::send_player_left(ptr<player> p_player) {
        start();
        *m_packet << uint16_t{0x000F};
        if (p_player) {
            *m_packet << uint32_t{1};
            *m_packet << p_player->get_id();
        } else {
            *m_packet << m_server->total_players();
            m_server->for_player([this](auto & p) {
                *m_packet << p.get_id();
            });
        }
        send();
    }
}
