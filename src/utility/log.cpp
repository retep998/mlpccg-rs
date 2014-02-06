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

#include "log.hpp"
#include <sstream>
#include <ctime>
#include <iomanip>

namespace nlp {
    block::block(std::string p_str) : m_str{std::move(p_str)} {}
    std::string & block::value() {
        return m_str;
    }
    std::string const & block::value() const {
        return m_str;
    }
    log::log() {
    }
    log::log(log && p_other) {
        p_other.m_owned = false;
    }
    log::~log() {
        std::cout << std::endl;
    }
    log && log::operator<<(block && p_block) && {
        throw;
    }
}
