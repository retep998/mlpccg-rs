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

#pragma once
#include "recv_handler.hpp"
#include <utility/ptr.hpp>
#include <SFML/Network/Packet.hpp>
#include <chrono>
#include <map>

namespace nlp {
    class game;
    class send_handler;
    class player final : public recv_handler {
    public:
        static ptr<player> get(uint32_t);
        static uint32_t total();
        template <typename T>
        static void for_each(T func) {
            for (auto const & p : players) {
                func(*p.second);
            }
        }
#define for_each_player(lambda) player::for_each([this](player & p){lambda})
        static std::map<uint32_t, ptr<player>> players;
    public:
        player(ptr<send_handler>);
        ~player();
        uint32_t get_id() const;
        std::string const & get_name() const;
    private:
        void update();
        void disconnect();
        void start();
        void send();
        void recv(sf::Packet &);
        void send_pong(uint32_t);
        void send_id();
        void send_player_joined(ptr<player> = ptr<player>{});
        void send_ping();
        void send_game_created(ptr<game> = ptr<game>{});
        void send_player_left(ptr<player>);
        ptr<send_handler> m_send;
        std::chrono::steady_clock::time_point last_ping{std::chrono::steady_clock::now()};
        uint32_t last_ping_id{};
        sf::Packet m_packet;
        std::string nickname;
        uint32_t id;
        std::shared_ptr<game> current_game;
    };
}
