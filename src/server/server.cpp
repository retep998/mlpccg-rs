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

namespace nlp {
    server::server() :
        m_manager{std::make_unique<manager>()} {
        m_manager->add_listener(273, [this](auto p_send) {
            return create_player(p_send);
        });
    }
    server::~server() {}
    auto server::run()->void {
        for (;;) {
            m_manager->update();
        }
    }
    auto server::create_player(ptr<packet_handler> p_send)->ptr<player> {
        auto dist = std::uniform_int_distribution<uint32_t>{1, std::numeric_limits<uint32_t>::max()};
        uint32_t id;
        do {
            id = dist(m_rng);
        } while (m_players.find(id) != m_players.end());
        auto p = player::create(p_send, id, this);
        if (!p) {
            return{};
        }
        return m_players.emplace(id, std::move(p)).first->second;
    }
    auto server::create_game(std::string p_name)->ptr<game> {
        return{};
    }
}
