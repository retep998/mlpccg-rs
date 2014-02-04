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

#include "game.hpp"
#include "player.hpp"
#include "server.hpp"
#include <map>

namespace nlp {
    game::game(std::string p_name, uint32_t p_id, ptr<server> p_server) :
        m_name{p_name},
        m_id{p_id},
        m_server{p_server} {
        m_server->for_player([this](auto & p) {
            p.send_game_created(this);
        });
    }
    game::~game() {
        for (auto & p : m_players) {
            remove_player(p);
        }
        m_server->for_player([this](auto & p) {
            p.send_game_deleted(this);
        });
    }
    uint32_t game::get_id() const {
        return m_id;
    }
    std::string const & game::get_name() const {
        return m_name;
    }
    void game::add_player(ptr<player> p_player) {
        auto it = m_players.insert(p_player);
        if (it.second) {
            p_player->send_game_joined(this);
        }
        p_player->send_player_joined_game({});
        for (auto & p : m_players) {
            p->send_player_joined_game(p_player);
        }
    }
    void game::remove_player(ptr<player> p_player) {
        auto it = m_players.erase(p_player);
        if (it) {
            p_player->send_game_joined({});
        }
        for (auto & p : m_players) {
            p->send_player_left_game(p_player);
        }
    }
    bool game::is_dead() const {
        return m_players.empty();
    }
    void game::update() {

    }
    uint32_t game::total_players() const {
        return static_cast<uint32_t>(m_players.size());
    }
}
