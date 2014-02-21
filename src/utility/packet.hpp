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

#pragma warning(push, 1)
#include <vector>
#include <cstdint>
#pragma warning(pop)

namespace nlp {
    class packet {
    public:
        packet() = default;
        packet(packet const &) = default;
        packet(packet &&) = default;
        packet(std::vector<char>::const_iterator, std::vector<char>::const_iterator);
        ~packet() = default;
        packet & operator=(packet const &) = default;
        packet & operator=(packet &&) = default;
        void compute_size();
        std::vector<char> & get();
        packet & operator>>(int8_t &);
        packet & operator>>(uint8_t &);
        packet & operator>>(int16_t &);
        packet & operator>>(uint16_t &);
        packet & operator>>(int32_t &);
        packet & operator>>(uint32_t &);
        packet & operator>>(int64_t &);
        packet & operator>>(uint64_t &);
        packet & operator>>(std::string &);
        packet & operator<<(int8_t);
        packet & operator<<(uint8_t);
        packet & operator<<(int16_t);
        packet & operator<<(uint16_t);
        packet & operator<<(int32_t);
        packet & operator<<(uint32_t);
        packet & operator<<(int64_t);
        packet & operator<<(uint64_t);
        packet & operator<<(std::string const &);
    protected:
        void read(void *, size_t);
        void write(void const *, size_t);
        std::vector<char> m_data{4};
        size_t m_read_pos{4};
    };
}
