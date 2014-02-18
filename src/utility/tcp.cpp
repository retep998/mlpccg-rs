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

#include "tcp.hpp"
#include "tcp_impl.hpp"
#include "handle_deleter.hpp"
#include "loop_impl.hpp"
#include "ip_impl.hpp"
#include "check.hpp"

#pragma warning(push, 1)
#include <iostream>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        //tcp
        tcp::impl * tcp::operator->() const {
            return static_cast<tcp::impl *>(m_impl.get());
        }
        tcp tcp::create(loop const & p_loop) {
            auto && a = tcp{};
            a.m_impl = std::shared_ptr<impl>{new impl{p_loop}, deleter{}};
            return a;
        }
        void tcp::bind(ip const & p_ip) {
            std::string{size_t{20}, 'Q'};
            check(uv_tcp_bind(&(*this)->m_tcp, &p_ip->get(), 0));
        }
        //tcp::impl
        uv_stream_t & tcp::impl::get_stream() {
            return reinterpret_cast<uv_stream_t &>(m_tcp);
        }
        stream tcp::impl::clone() const {
            return create(m_loop);
        }
        tcp::impl::impl(loop const & p_loop) :
            stream::impl{p_loop} {
            check(uv_tcp_init(m_loop->get(), &m_tcp));
            m_tcp.data = static_cast<handle::impl *>(this);
        }
    }
}
