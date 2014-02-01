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
#include <set>
#include <string>
#include <functional>
#include <memory>
#include <map>

namespace nlp {
    class player;
    class server;
    class game final {
    public:
        game(std::string, uint32_t, ptr<server>);
        ~game();
        uint32_t get_id() const;
        std::string const & get_name() const;
    private:
        std::set<ptr<player>> m_players;
        ptr<player> m_player_one, m_player_two;
        std::string m_name;
        uint32_t m_id;
        ptr<server> m_server;
    };
}
