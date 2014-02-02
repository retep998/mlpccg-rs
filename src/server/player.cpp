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
        m_id{p_id},
        m_server{p_server} {
        m_name = default_name();
        log() << "Connected." << std::endl;
        send_id();
        send_player_joined({});
        send_player_joined(this);
        send_game_created({});
        m_server->for_player([this](auto & p) {
            p.send_player_joined(this);
        });
    }
    player::~player() {}
    uint32_t player::get_id() const {
        return m_id;
    }
    std::string const & player::get_name() const {
        return m_name;
    }
    void player::update() {
        if (is_dead()) {
            return;
        }
        auto now = std::chrono::steady_clock::now();
        if (m_ping_id) {
            if (now - m_ping > std::chrono::seconds{20}) {
                log() << "Timed out." << std::endl;
                kill();
            }
        } else if (now - m_ping > std::chrono::seconds{15}) {
            send_ping();
        }
    }
    bool player::is_dead() const {
        return m_dead;
    }
    std::string player::default_name() const {
        std::ostringstream ss;
        ss << std::hex << std::uppercase << m_id;
        return "Pony" + ss.str();
    }
    std::ostream & player::log() const {
        std::cout << time() << '|' << get_name() << '|';
        return std::cout;
    }
    void player::kill() {
        if (!is_dead()) {
            m_dead = true;
            if (m_game) {
                m_game->remove_player(this);
            }
            m_server->for_player([this](auto & p) {
                p.send_player_left(this);
            });
            m_send->kill();
            log() << "Disconnected." << std::endl;
        }
    }
    void player::handle(sf::Packet & p) {
        auto opcode = uint16_t{};
        p >> opcode;
        switch (opcode) {
        case 0x0001: {
            uint32_t id{};
            p >> id;
            send_pong(id);
        } break;
        case 0x0002: {
            auto id = uint32_t{};
            p >> id;
            if (m_ping_id && id == m_ping_id) {
                m_ping = std::chrono::steady_clock::now();
                m_ping_id = 0;
            }
        } break;
        case 0x0003: {
            auto new_name = std::string{};
            p >> new_name;
            if (new_name.length() > 20) {
                new_name.resize(20);
            } else if (new_name.empty()) {
                new_name = default_name();
            }
            log() << "Renamed to " << new_name << std::endl;
            m_name = new_name;
            m_server->for_player([this](auto & p) {
                p.send_player_joined(this);
            });
        } break;
        case 0x0007: {
            if (m_game) {
                m_game->remove_player(this);
            }
            auto game_name = std::string{};
            p >> game_name;
            auto g = m_server->create_game(game_name);
            g->add_player(this);
        } break;
        case 0x0008: {
            if (m_game) {
                m_game->remove_player(this);
            }
            auto id = uint32_t{};
            p >> id;
            auto g = m_server->get_game(id);
            g->add_player(this);
        } break;
        }
    }
    void player::send(sf::Packet & p_packet) {
        m_send->handle(p_packet);
    }
    void player::send_ping() {
        auto packet = sf::Packet{};
        auto dist = std::uniform_int_distribution<uint32_t>{1, std::numeric_limits<uint32_t>::max()};
        m_ping_id = dist(m_server->rng());
        packet << uint16_t{0x0001} << m_ping_id;
        send(packet);
    }
    void player::send_pong(uint32_t p_id) {
        auto packet = sf::Packet{};
        packet << uint16_t{0x0002} << p_id;
        send(packet);
    }
    void player::send_id() {
        auto packet = sf::Packet{};
        packet << uint16_t{0x0004} << m_id;
        send(packet);
    }
    void player::send_game_deleted(ptr<game> p_game) {
        auto packet = sf::Packet{};
        packet << uint16_t{0x0005};
        if (p_game) {
            packet << uint32_t{1};
            packet << p_game->get_id();
        } else {
            packet << uint32_t{0};
        }
        send(packet);
    }
    void player::send_game_created(ptr<game> p_game) {
        auto packet = sf::Packet{};
        packet << uint16_t{0x0006};
        if (p_game) {
            packet << uint32_t{1};
            packet << p_game->get_id() << p_game->get_name();
        } else {
            packet << m_server->total_games();
            m_server->for_game([this, &packet](auto & g) {
                packet << g.get_id() << g.get_name();
            });
        }
        send(packet);
    }
    void player::send_game_joined(ptr<game> p_game) {
        auto packet = sf::Packet{};
        packet << uint16_t{0x0009};
        if (p_game) {
            log() << "Joined game " << p_game->get_name() << std::endl;
            packet << p_game->get_id();
        } else {
            log() << "Left game" << std::endl;
            packet << uint32_t{0};
        }
        m_game = p_game;
        send(packet);
    }
    void player::send_player_joined(ptr<player> p_player) {
        auto packet = sf::Packet{};
        packet << uint16_t{0x000B};
        if (p_player) {
            packet << uint32_t{1};
            packet << p_player->get_id() << p_player->get_name();
        } else {
            packet << m_server->total_players();
            m_server->for_player([this, &packet](auto & p) {
                packet << p.get_id() << p.get_name();
            });
        }
        send(packet);
    }
    void player::send_player_joined_game(ptr<player> p_player) {
        auto packet = sf::Packet{};
        packet << uint16_t{0x000C};
        if (p_player) {
            packet << uint32_t{1};
            packet << p_player->get_id();
        } else {
            packet << uint32_t{0};
        }
        send(packet);
    }
    void player::send_player_left_game(ptr<player> p_player) {
        auto packet = sf::Packet{};
        packet << uint16_t{0x000D};
        if (p_player) {
            packet << uint32_t{1};
            packet << p_player->get_id();
        } else {
            packet << uint32_t{0};
        }
        send(packet);
    }
    void player::send_player_left(ptr<player> p_player) {
        auto packet = sf::Packet{};
        packet << uint16_t{0x000F};
        if (p_player) {
            packet << uint32_t{1};
            packet << p_player->get_id();
        } else {
            packet << m_server->total_players();
            m_server->for_player([this, &packet](auto & p) {
                packet << p.get_id();
            });
        }
        send(packet);
    }
}
