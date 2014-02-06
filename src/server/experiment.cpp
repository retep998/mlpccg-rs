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
#include <iostream>
#include <cstdint>
#include <vector>
#include <fstream>

namespace nlp {
    void experiment() {
        auto && my_loop = loop{};
        auto && my_tty = tty{my_loop};
        auto && in = std::ifstream("assets/motd.txt", std::ios::binary);
        auto && line = std::string{};
        std::getline(in, line, '\0');
        my_tty.write(line);
        my_tty << tty::clear;
        std::cin.get();
    }
}
