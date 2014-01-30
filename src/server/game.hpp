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
    class game final : public std::enable_shared_from_this<game> {
    public:
        game(std::string name);
        ~game();
        uint32_t get_id() const;
        static std::shared_ptr<game> create(std::string name);
        static std::shared_ptr<game> get(uint32_t id);
        static std::map<uint32_t, ptr<game>> games;
        std::string const & get_name() const;
    private:
        std::shared_ptr<game> make_shared();
        std::set<ptr<player>> players;
        ptr<player> player_one, player_two;
        std::string name;
        uint32_t id;
    };
}
