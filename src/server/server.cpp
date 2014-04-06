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
#include "game.hpp"
#include <utility/framed_stream.hpp>

#pragma warning(push, 1)
#include <fstream>
#include <algorithm>
#include <iostream>
#pragma warning(pop)

namespace nlp {
    server::server() :
        m_loop{uv::loop_init()},
        m_tty{m_loop} {
        m_listener = uv::tcp::create(m_loop);
        m_listener.bind(uv::ip::create("0.0.0.0", 273));
        m_listener.listen([this](uv::stream p_stream) {
            auto dist = std::uniform_int_distribution<uint32_t>{1, std::numeric_limits<uint32_t>::max()};
            auto id = uint32_t{};
            do {
                id = dist(m_rng);
            } while (m_players.find(id) != m_players.end());
            m_players.emplace(std::piecewise_construct, std::make_tuple(id),
                              std::make_tuple(id, std::ref(*this), p_stream));
        }, 0x100);
        auto in = std::ifstream{"assets/motd.txt", std::ios::binary};
        auto line = std::string{};
        std::getline(in, line, '\0');
        std::cout << line << std::endl;
        std::cout << io::time << "Server listening on port 273" << std::endl;
    }
    void server::run() {
        m_loop->run();
    }
    game & server::create_game(std::string p_name) {
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
    void server::destroy_player(uint32_t p_id) {
        m_players.erase(p_id);
    }
    std::mt19937_64 & server::rng() {
        return m_rng;
    }
    uv::loop const & server::loop() const {
        return m_loop;
    }
    void server::for_game(std::function<void(game &)> p_func) {
        for (auto & game : m_games) {
            p_func(game.second);
        }
    }
    void server::for_player(std::function<void(player &)> p_func) {
        for (auto & player : m_players) {
            p_func(player.second);
        }
    }
}
