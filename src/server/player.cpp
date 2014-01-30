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

namespace nlp {
    namespace {
        std::map<uint32_t, ptr<player>> players;
    }
    player::player(ptr<send_handler> send) : send{send} {
        std::uniform_int_distribution<uint32_t> dist{std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()};
        do {
            id = dist(rng);
        } while (players.find(id) != players.end());
        players.emplace(id, this);
        nickname = "Pony" + to_hex_string(id);
        std::cout << time() << nickname << " connected." << std::endl;
        //ID packet
        packet.clear();
        packet << uint16_t{0x0004} << id;
        send->send(packet);
        //Player update packet
        packet.clear();
        packet << uint16_t{0x000B} << static_cast<uint32_t>(players.size());
        for (auto const & it : players) {
            packet << it.first << it.second->nickname;
        }
        send->send(packet);
        //Game create packet
        packet.clear();
        packet << uint16_t{0x0006} << static_cast<uint32_t>(game::games.size());
        for (auto const & it : game::games) {
            packet << it.first << it.second->get_name();
        }
        send->send(packet);
    }
    player::~player() {
        std::cout << time() << nickname << " disconnected." << std::endl;
        players.erase(id);
    }
    uint32_t player::get_id() const {
        return id;
    }
    std::string const & player::get_name() const {
        return nickname;
    }
    ptr<player> player::get(uint32_t id) {
        auto it = players.find(id);
        if (it != players.end()) {
            return it->second;
        }
        return{};
    }
    void player::recv(sf::Packet & p) {
        uint16_t opcode{};
        p >> opcode;
        switch (opcode) {
        case 0x0001: {
            uint32_t id{};
            p >> id;
            packet.clear();
            packet << uint16_t{0x0002} << id;
            send->send(packet);
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
            std::cout << time() << nickname <<  " has renamed themselves to ";
            p >> nickname;
            if (nickname.empty()) {
                nickname = "Pony" + to_hex_string(id);
            }
            if (nickname.length() > 20) {
                nickname.resize(20);
            }
            std::cout << nickname << std::endl;
        } break;
        }
    }
    void player::update() {
        auto now = std::chrono::steady_clock::now();
        if (last_ping_id) {
            if (now - last_ping > std::chrono::seconds{20}) {
                send->disconnect();
            }
        } else if (now - last_ping > std::chrono::seconds{15}) {
            std::uniform_int_distribution<uint32_t> dist{1, std::numeric_limits<uint32_t>::max()};
            packet.clear();
            last_ping_id = dist(rng);
            packet << uint16_t{0x0001} << last_ping_id;
            send->send(packet);
        }
    }
}
