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
#include <utility/rng.hpp>
#include <map>

namespace nlp {
    std::map<uint32_t, ptr<game>> game::games;
    std::shared_ptr<game> game::create(std::string name) {
        return std::make_shared<game>(name);
    }
    std::shared_ptr<game> game::get(uint32_t id) {
        auto it = games.find(id);
        if (it != games.end()) {
            return it->second->make_shared();
        }
        return{nullptr};
    }
    uint32_t game::total() {
        return static_cast<uint32_t>(games.size());
    }
    game::game(std::string name) : name{name} {
        std::uniform_int_distribution<uint32_t> dist{std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max()};
        do {
            id = dist(rng);
        } while (games.find(id) != games.end());
        games.emplace(id, this);
    }
    game::~game() {
        games.erase(id);
    }
    uint32_t game::get_id() const {
        return id;
    }
    std::shared_ptr<game> game::make_shared() {
        return std::shared_ptr<game>{this};
    }
    std::string const & game::get_name() const {
        return name;
    }
}
