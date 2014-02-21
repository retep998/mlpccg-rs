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
#include <utility/tcp.hpp>
#include <utility/ptr.hpp>
#include <utility/framed_stream.hpp>
#include <utility/timer.hpp>
#include <utility/packet.hpp>

#pragma warning(push, 1)
#include <chrono>
#include <map>
#include <string>
#pragma warning(pop)

namespace nlp {
    class game;
    class server;
    class player final {
    public:
        player() = delete;
        player(player const &) = delete;
        player(player &&) = delete;
        player(uint32_t, server &, uv::stream);
        ~player() = default;
        player & operator=(player const &) = delete;
        player & operator=(player &&) = delete;
        uint32_t get_id() const;
        std::string const & get_name() const;
        void send_game_created(ptr<game>);
        void send_game_joined(ptr<game>);
        void send_game_deleted(ptr<game>);
        void send_player_left_game(ptr<player>);
        void send_player_joined_game(ptr<player>);
    private:
        std::string default_name() const;
        void handle(packet);
        void kill();
        void start(uint16_t);
        void send();
        void send_pong(uint32_t);
        void send_id();
        void send_player_joined(ptr<player>);
        void send_ping();
        void send_player_left(ptr<player>);
        void send_global_chat(ptr<player>, std::string const &);
        void send_game_chat(ptr<player>, std::string const &);
        void send_private_chat(ptr<player>, std::string const &);
        uint32_t m_ping_id{0};
        uint32_t m_id;
        std::string m_name;
        server & m_server;
        uv::timer m_timer;
        framed_stream m_stream;
        ptr<game> m_game;
        packet m_packet;
    };
}
