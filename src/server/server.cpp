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

#include "server.hpp"
#include "player.hpp"
#include "manager.hpp"
#include "game.hpp"

#pragma warning(push, 1)
#include <fstream>
#include <iostream>
#include <algorithm>
#pragma warning(pop)

namespace nlp {
    server::server() :
        m_loop{uv::loop::create()} {
        m_listener = uv::tcp::listen(m_loop, uv::ip::create("0.0.0.0", 273), [this](uv::tcp p_tcp) {
            auto dist = std::uniform_int_distribution<uint32_t>{1, std::numeric_limits<uint32_t>::max()};
            uint32_t id;
            do {
                id = dist(m_rng);
            } while (m_players.find(id) != m_players.end());
            auto it = m_players.emplace(std::piecewise_construct, std::make_tuple(id), std::make_tuple(id, this, p_tcp));
            p_tcp.disconnect([this, it] {
                m_players.erase(it.first);
            });
        });
    }
    void server::run() {
        m_loop.run();
    }
    ptr<game> server::create_game(std::string p_name) {
        auto dist = std::uniform_int_distribution<uint32_t>{1, std::numeric_limits<uint32_t>::max()};
        uint32_t id;
        do {
            id = dist(m_rng);
        } while (m_games.find(id) != m_games.end());
        auto it = m_games.emplace(std::piecewise_construct, std::make_tuple(id), std::make_tuple(p_name, id, this));
        return it.first->second;
    }
    ptr<player> server::get_player(uint32_t p_id) {
        auto it = m_players.find(p_id);
        if (it == m_players.end()) {
            return{};
        }
        return it->second;
    }
    ptr<game> server::get_game(uint32_t p_id) {
        auto it = m_games.find(p_id);
        if (it == m_games.end()) {
            return{};
        }
        return it->second;
    }
    uint32_t server::total_players() const {
        return static_cast<uint32_t>(m_players.size());
    }
    uint32_t server::total_games() const {
        return static_cast<uint32_t>(m_games.size());
    }
    std::mt19937_64 & server::rng() {
        return m_rng;
    }
}
