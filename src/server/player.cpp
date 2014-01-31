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
#include "send_handler.hpp"
#include <utility/format.hpp>
#include <utility/rng.hpp>
#include <iostream>
#include <map>
#include <set>

namespace nlp {
    std::unique_ptr<player> create(ptr<send_handler> p_send, uint32_t p_id, ptr<server> p_server) {
        return std::make_unique<player>(p_send, p_id, p_server);
    }
    player::player(ptr<send_handler> p_send, uint32_t p_id, ptr<server> p_server) :
        m_send{p_send},
        m_id{p_id},
        m_server{p_server} {
        nickname = "Pony" + to_hex_string(m_id);
        std::cout << time() << nickname << " connected." << std::endl;
        send_id();
        send_player_joined();
        send_game_created();
        for_each_player(
            p.send_player_joined(this);
        );
    }
    player::~player() {
        for_each_player(
            p.send_player_left(this);
        );
        std::cout << time() << nickname << " disconnected." << std::endl;
    }
    uint32_t player::get_id() const {
        return m_id;
    }
    std::string const & player::get_name() const {
        return nickname;
    }
    void player::update() {
        auto now = std::chrono::steady_clock::now();
        if (last_ping_id) {
            if (now - last_ping > std::chrono::seconds{20}) {
                disconnect();
            }
        } else if (now - last_ping > std::chrono::seconds{15}) {
            send_ping();
        }
    }
    void player::disconnect() {
        m_send->disconnect();
    }
    void player::start() {
        m_packet.clear();
    }
    void player::send() {
        m_send->send(m_packet);
    }
    void player::recv(sf::Packet & p) {
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
            if (last_ping_id && id == last_ping_id) {
                last_ping = std::chrono::steady_clock::now();
                last_ping_id = 0;
            }
        } break;
        case 0x0003: {
            std::cout << time() << nickname << " has renamed themselves to ";
            p >> nickname;
            if (nickname.empty()) {
                nickname = "Pony" + to_hex_string(m_id);
            }
            if (nickname.length() > 20) {
                nickname.resize(20);
            }
            std::cout << nickname << std::endl;
            for_each_player(
                p.send_player_joined(this);
            );
        } break;
        }
    }
    void player::send_pong(uint32_t p_id) {
        start();
        m_packet << uint16_t{0x0002} << p_id;
        send();
    }
    void player::send_id() {
        start();
        m_packet << uint16_t{0x0004} << m_id;
        send();
    }
    void player::send_ping() {
        start();
        std::uniform_int_distribution<uint32_t> dist{1, std::numeric_limits<uint32_t>::max()};
        last_ping_id = dist(rng);
        m_packet << uint16_t{0x0001} << last_ping_id;
        send();
    }
    void player::send_player_joined(ptr<player> p_player) {
        start();
        m_packet << uint16_t{0x000B};
        if (p_player) {
            m_packet << uint32_t{1};
            m_packet << p_player->get_id() << p_player->get_name();
        } else {
            m_packet << total();
            for_each_player(
                m_packet << p.get_id() << p.get_name();
            );
        }
        send();
    }
    void player::send_game_created(ptr<game> p_game) {
        start();
        m_packet << uint16_t{0x0006};
        if (p_game) {
            m_packet << uint32_t{1};
            m_packet << p_game->get_id() << p_game->get_name();
        } else {
            m_packet << game::total();
            for_each_game(
                m_packet << g.get_id() << g.get_name();
            );
        }
        send();
    }
    void player::send_player_left(ptr<player> p_player) {
        start();
        m_packet << uint16_t{0x000F};
        if (p_player) {
            m_packet << uint32_t{1};
            m_packet << p_player->get_id();
        } else {
            m_packet << total();
            for_each_player(
                m_packet << p.get_id();
            );
        }
        send();
    }
}
