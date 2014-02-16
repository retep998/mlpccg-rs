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
#include "game.hpp"
#include "player.hpp"
#include <utility/ptr.hpp>
#include <utility/tcp.hpp>

#pragma warning(push, 1)
#include <map>
#include <string>
#include <cstdint>
#include <random>
#pragma warning(pop)

namespace nlp {
    class player;
    class game;
    class packet_handler;
    class server final {
    public:
        server();
        server(server const &) = delete;
        server(server &&) = delete;
        ~server() = default;
        server & operator=(server const &) = delete;
        server & operator=(server &&) = delete;
        void run();
        ptr<game> create_game(std::string);
        ptr<player> get_player(uint32_t);
        ptr<game> get_game(uint32_t);
        uint32_t total_players() const;
        uint32_t total_games() const;
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
    protected:
        void update();
        std::map<uint32_t, game> m_games;
        std::map<uint32_t, player> m_players;
        std::mt19937_64 m_rng;
        uv::tcp m_listener;
        uv::loop m_loop;
    };
}
