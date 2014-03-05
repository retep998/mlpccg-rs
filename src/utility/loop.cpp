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

#include "loop.hpp"
#include "loop_impl.hpp"
#include "check.hpp"

#pragma warning(push, 1)
#include <stdexcept>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        //loop
        loop::loop(init_t const &) :
            m_impl{std::shared_ptr<impl>{new impl{}, [](impl * p_impl) {
            std::unique_ptr<impl> a{p_impl};
            check(uv_run(p_impl->get(), UV_RUN_NOWAIT));
            if (uv_loop_alive(p_impl->get())) {
                throw std::runtime_error{"Loop is still alive"};
            }
            check(uv_loop_close(p_impl->get()));
        }}} {}
        std::shared_ptr<loop::impl> const & loop::operator->() const {
            return m_impl;
        }
        void loop::run() const {
            check(uv_run(m_impl->get(), UV_RUN_DEFAULT));
        }
        //loop::impl
        uv_loop_t * loop::impl::get() {
            return &m_loop;
        }
        loop::impl::impl() {
            check(uv_loop_init(&m_loop));
        }
    }
}
