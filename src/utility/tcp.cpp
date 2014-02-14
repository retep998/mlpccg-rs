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
#include "check.hpp"

#pragma warning(push, 1)
#include <iostream>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        //tcp
        tcp tcp::listen(loop const & p_loop, std::string const & p_ip, uint16_t p_port) {
            auto && a = sockaddr_in{};
            check(uv_ip4_addr(p_ip.c_str(), p_port, &a));
            auto && b = std::shared_ptr<impl>{new impl{p_loop.get_impl(), reinterpret_cast<sockaddr const &>(a)}, deleter{}};
            return{b};
        }
        tcp::tcp(std::shared_ptr<impl> p_impl) :
            stream{std::static_pointer_cast<stream::impl>(p_impl)} {}
        //tcp::impl
        uv_stream_t * tcp::impl::get_stream() {
            return reinterpret_cast<uv_stream_t *>(&m_tcp);
        }
        tcp::impl::impl(std::shared_ptr<loop::impl> p_loop, sockaddr const & p_addr) :
            stream::impl{p_loop} {
            check(uv_tcp_init(p_loop->get(), &m_tcp));
            m_tcp.data = static_cast<handle::impl *>(this);
            check(uv_tcp_bind(&m_tcp, &p_addr, 0));
            check(uv_listen(get_stream(), 128, [](uv_stream_t *, int p_status) {
                check(p_status);
                std::cerr << "Received connection!" << std::endl;
            }));
        }
    }
}
