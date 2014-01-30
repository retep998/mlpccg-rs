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
#include "send_handler.hpp"
#include <utility/format.hpp>
#include <utility/rng.hpp>
#include <iostream>

namespace nlp {
    player::player(ptr<send_handler> send) : send{send} {
        std::uniform_int<uint64_t> dist(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
        id = dist(rng);
        packet.clear();
        packet << uint16_t{0x0004} << id;
        send->send(packet);
    }
    player::~player() {

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
            std::cout << time() << nickname << " has renamed themselves to ";
            p >> nickname;
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
