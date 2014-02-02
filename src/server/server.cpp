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
#include <utility/algorithm.hpp>
#include <fstream>
#include <iostream>
#include <algorithm>

namespace nlp {
    server::server() :
        m_manager{std::make_unique<manager>()} {
        auto file = std::ifstream("assets/motd.txt");
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << '\n';
        }
        m_manager->add_listener(273, [this](auto p_send) {
            return create_player(p_send);
        });
    }
    server::~server() {}
    void server::run() {
        for (;;) {
            m_manager->update();
            update();
            m_manager->collect();
            collect();
        }
    }
    ptr<player> server::create_player(ptr<packet_handler> p_send) {
        auto dist = std::uniform_int_distribution<uint32_t>{1, std::numeric_limits<uint32_t>::max()};
        uint32_t id;
        do {
            id = dist(m_rng);
        } while (m_players.find(id) != m_players.end());
        auto p = std::make_unique<player>(p_send, id, this);
        return m_players.emplace(id, std::move(p)).first->second;
    }
    ptr<game> server::create_game(std::string p_name) {
        auto dist = std::uniform_int_distribution<uint32_t>{1, std::numeric_limits<uint32_t>::max()};
        uint32_t id;
        do {
            id = dist(m_rng);
        } while (m_games.find(id) != m_games.end());
        auto g = std::make_unique<game>(p_name, id, this);
        return m_games.emplace(id, std::move(g)).first->second;
    }
    ptr<player> server::get_player(uint32_t p_id) const {
        auto it = m_players.find(p_id);
        if (it == m_players.end()) {
            return{};
        }
        return it->second;
    }
    ptr<game> server::get_game(uint32_t p_id) const {
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
    void server::update() {
        for (auto & p : m_players) {
            p.second->update();
        }
        for (auto & g : m_games) {
            g.second->update();
        }
    }
    void server::collect() {
        remove_if(m_players, [](auto & p) {
            return p.second->is_dead();
        });
        remove_if(m_games, [](auto & g) {
            return g.second->is_dead();
        });
    }
}
