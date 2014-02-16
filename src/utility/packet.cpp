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

#include "packet.hpp"

#pragma warning(push, 1)
#include <uv.h>
#include <cstdint>
#pragma warning(pop)

namespace nlp {
    packet::packet(std::vector<char>::const_iterator first, std::vector<char>::const_iterator last) :
        m_data{first, last} {}
    void packet::compute_size() {
        auto size = static_cast<uint32_t>(m_data.size());
        if (size < sizeof(uint32_t)) {
            throw std::runtime_error{"Bad packet size"};
        }
        size = ::htonl(size);
        reinterpret_cast<uint32_t &>(m_data[0]) = size;
    }
    std::vector<char> & packet::get() {
        return m_data;
    }
}
