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
#include "packet_handler.hpp"
#include <utility/ptr.hpp>
#include <chrono>
#include <map>
#include <string>

namespace sf {
    class Packet;
}
namespace nlp {
    class game;
    class server;
    class player final : public packet_handler {
    public:
        player(ptr<packet_handler>, uint32_t, ptr<server>);
        ~player();
        uint32_t get_id() const;
        std::string const & get_name() const;
        void update();
        bool is_dead() const;
        void send_game_created(ptr<game>);
        void send_game_joined(ptr<game>);
        void send_game_deleted(ptr<game>);
        void send_player_left_game(ptr<player>);
        void send_player_joined_game(ptr<player>);
    private:
        std::string default_name() const;
        void handle(sf::Packet &) override;
        void kill() override;
        void send(sf::Packet &);
        void send_pong(uint32_t);
        void send_id();
        void send_player_joined(ptr<player>);
        void send_ping();
        void send_player_left(ptr<player>);
        void send_global_chat(ptr<player>, std::string const &);
        void send_game_chat(ptr<player>, std::string const &);
        void send_private_chat(ptr<player>, std::string const &);
        ptr<packet_handler> m_send;
        std::chrono::steady_clock::time_point m_ping{std::chrono::steady_clock::now()};
        uint32_t m_ping_id{};
        std::string m_name;
        uint32_t m_id;
        ptr<game> m_game;
        ptr<server> m_server;
        bool m_dead = false;
    };
}
