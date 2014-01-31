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
#include "recv_handler.hpp"

namespace nlp {
    server::server() :
        m_manager{std::make_unique<manager>()} {
        m_manager->add_listener(273, [this](ptr<send_handler> p_send) {
            return create_player(p_send);
        });
    }
    server::~server() {}
    void server::run() {
        for (;;) {
            m_manager->update();
        }
    }
    ptr<player> server::create_player(ptr<send_handler> p_send) {
        auto p = std::make_unique<player>(p_send);
        if (!p) {
            return{};
        }
        auto ret = ptr<player>{p};
        auto dist = std::uniform_int_distribution<uint32_t>{1, std::numeric_limits<uint32_t>::max()};
        for (;;) {
            m_players.emplace()
        }
    }
    ptr<game> server::create_game(std::string p_name) {

    }
}
