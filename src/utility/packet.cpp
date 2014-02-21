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
#include <sstream>
#include <iostream>
#include <iomanip>
#include <iterator>
#pragma warning(pop)

namespace nlp {
    packet::packet(std::vector<char>::const_iterator first, std::vector<char>::const_iterator last) :
        m_data{first, last} {
        auto ss = std::ostringstream{};
        ss << std::hex << std::setfill('0') << std::uppercase;
        for (auto c : m_data) {
            ss << std::setw(2) << static_cast<unsigned>(static_cast<unsigned char>(c)) << ' ';
        }
        std::cerr << "In: " << ss.str() << std::endl;;
    }
    void packet::compute_size() {
        if (m_data.size() > std::numeric_limits<uint32_t>::max()) {
            throw std::runtime_error{"Packet too big!"};
        }
        auto size = static_cast<uint32_t>(m_data.size() - 4);
        if (size < sizeof(uint32_t)) {
            throw std::runtime_error{"Bad packet size"};
        }
        size = ::htonl(size);
        reinterpret_cast<uint32_t &>(m_data[0]) = size;
        auto ss = std::ostringstream{};
        ss << std::hex << std::setfill('0') << std::uppercase;
        for (auto c : m_data) {
            ss << std::setw(2) << static_cast<unsigned>(static_cast<unsigned char>(c)) << ' ';
        }
        std::cerr << "Out: " << ss.str() << std::endl;
    }
    std::vector<char> & packet::get() {
        return m_data;
    }
    packet & packet::operator>>(uint16_t & p_val) {
        read(&p_val, sizeof(p_val));
        p_val = ::ntohs(p_val);
        return *this;
    }
    packet & packet::operator>>(uint32_t & p_val) {
        read(&p_val, sizeof(p_val));
        p_val = ::ntohl(p_val);
        return *this;
    }
    packet & packet::operator>>(std::string & p_val) {
        auto size = uint32_t{};
        *this >> size;
        p_val.resize(size);
        read(const_cast<char *>(p_val.data()), size);
        return *this;
    }
    packet & packet::operator<<(uint16_t p_val) {
        p_val = ::htons(p_val);
        write(&p_val, sizeof(p_val));
        return *this;
    }
    packet & packet::operator<<(uint32_t p_val) {
        p_val = ::htonl(p_val);
        write(&p_val, sizeof(p_val));
        return *this;
    }
    packet & packet::operator<<(std::string const & p_val) {
        auto size = static_cast<uint32_t>(p_val.size());
        *this << size;
        write(p_val.data(), size);
        return *this;
    }
    void packet::read(void * p_val, size_t p_size) {
        if (m_read_pos + p_size <= m_data.size()) {
            auto val = reinterpret_cast<char *>(p_val);
            std::copy(m_data.begin() + static_cast<ptrdiff_t>(m_read_pos),
                      m_data.begin() + static_cast<ptrdiff_t>(m_read_pos + p_size), val);
            m_read_pos += p_size;
        } else {
            m_read_pos = m_data.size();
        }
    }
    void packet::write(void const * p_val, size_t p_size) {
        auto val = reinterpret_cast<char const *>(p_val);
        std::copy(val, val + p_size, std::back_inserter(m_data));
    }
}
