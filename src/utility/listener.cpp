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

#include "listener.hpp"
#include "loop.hpp"

#pragma warning(push, 1)
#include <uv.h>
#include <stdexcept>
#include <iostream>
#pragma warning(pop)


namespace nlp {
    class listener::impl {
    public:
        impl() = delete;
        impl(impl const &) = delete;
        impl(impl &&) = delete;
        ~impl() = default;
        impl & operator=(impl const &) = delete;
        impl & operator=(impl &&) = delete;
        static void close(uv_handle_t *);
    private:
        std::shared_ptr<uv_tcp_t> m_tcp;
    };
    void listener::impl::close(uv_handle_t * p_handle) {
        auto handle = reinterpret_cast<listener::impl *>(p_handle);
    }
    //listener::listener(loop const & p_loop, uint16_t p_port) :
    //    m_tcp{std::make_unique<stream>()} {
    //    if (!uv_tcp_init(p_loop.get(), m_tcp.get())) {
    //        throw std::runtime_error{"Failed to initialize TCP handle"};
    //    }
    //    auto addr = sockaddr_in{};
    //    if (!uv_ip4_addr("0.0.0.0", p_port, &addr)) {
    //        throw std::runtime_error{"Failed to obtain IP address"};
    //    }
    //    if (!uv_tcp_bind(m_tcp.get(), reinterpret_cast<sockaddr *>(&addr), {})) {
    //        throw std::runtime_error{"Failed to bind TCP handle"};
    //    }
    //    if (!uv_listen(reinterpret_cast<uv_stream_t *>(m_tcp.get()), 0x100, callback)) {
    //        throw std::runtime_error{"Failed to listen"};
    //    }
    //}
    listener::~listener() {}
}