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

#include "framed_stream.hpp"
#include "framed_stream_impl.hpp"
#include "packet.hpp"

#pragma warning(push, 1)
#include <uv.h>
#pragma warning(pop)

namespace nlp {
    //framed_stream
    void framed_stream::write(packet p_packet) const {
        p_packet.compute_size();
        m_impl->m_stream.write(std::move(p_packet.get()));
    }
    void framed_stream::read(std::function<void(packet)> p_func) {
        m_impl->read(std::move(p_func));
    }
    framed_stream framed_stream::create(uv::stream p_stream) {
        auto && a = std::shared_ptr<impl>{new impl{p_stream}};
        return{std::move(a)};
    }
    framed_stream::framed_stream(std::shared_ptr<impl> p_impl) :
        m_impl{std::move(p_impl)} {}
    //framed_stream::impl
    framed_stream::impl::impl(uv::stream p_stream) :
        m_stream{std::move(p_stream)} {}
    void framed_stream::impl::read(std::function<void(packet)> p_func) {
        m_read_callback = std::move(p_func);
        m_stream.read([this](std::vector<char> const & p_data) {
            std::copy(p_data.cbegin(), p_data.cend(), std::back_inserter(m_data));
            if (m_data.size() >= sizeof(uint32_t)) {
                auto size = ::ntohl(reinterpret_cast<uint32_t &>(m_data[0]));
                if (m_data.size() >= size + sizeof(uint32_t)) {
                    m_read_callback({m_data.begin() + sizeof(uint32_t), m_data.begin() + size + sizeof(uint32_t)});
                    m_data.erase(m_data.begin(), m_data.begin() + size + sizeof(uint32_t));
                }
            }
        });
    }
}
