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

#include "experiment.hpp"
#include <utility/loop.hpp>
#include <utility/tty.hpp>

#pragma warning(push, 1)
#include <cstdint>
#include <vector>
#include <fstream>
#pragma warning(pop)

namespace nlp {
    void experiment() {
        auto tty = uv::tty::create(uv::loop::create());
        auto in = std::ifstream{"assets/motd.txt", std::ios::binary};
        auto line = std::string{};
        std::getline(in, line, '\0');
        tty.write(line);
        tty.write("\x1b[0m");
    }
}
