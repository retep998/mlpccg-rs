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
#include <utility/ptr.hpp>
#include <map>
#include <string>
#include <cstdint>
#include <random>

namespace nlp {
    class player;
    class game;
    class send_handler;
    class manager;
    class server {
    public:
        server();
        server(server const &) = delete;
        ~server();
        server & operator=(server const &) = delete;
        void run();
        ptr<player> create_player(ptr<send_handler>);
        ptr<game> create_game(std::string);
        std::mt19937_64 & rng();
        template <typename Func>
        void for_game(Func && p_func) {
            for (auto & g : m_games) {
                p_func(g.second);
            }
        }
        template <typename Func>
        void for_player(Func && p_func) {
            for (auto & p : m_players) {
                p_func(p.second);
            }
        }
    private:
        std::map<uint32_t, std::unique_ptr<game>> m_games;
        std::map<uint32_t, std::unique_ptr<player>> m_players;
        std::unique_ptr<manager> m_manager;
        std::mt19937_64 m_rng;
    };
}
