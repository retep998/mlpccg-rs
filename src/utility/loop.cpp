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
#include "loop_deleter.hpp"
#include "check.hpp"

#pragma warning(push, 1)
#include <stdexcept>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        //loop
        std::shared_ptr<loop::impl> const & loop::operator->() const {
            return m_impl;
        }
        void loop::run() const {
            check(uv_run(m_impl->m_loop, UV_RUN_DEFAULT));
        }
        loop loop::create() {
            auto && a = loop{};
            a.m_impl = std::shared_ptr<impl>{new impl{}, deleter{}};
            return a;
        }
        //loop::impl
        uv_loop_t * loop::impl::get() {
            return m_loop;
        }
        loop::impl::impl() :
            m_loop{uv_loop_new()} {}
        //loop::deleter
        void loop::deleter::operator()(impl * p_impl) {
            auto && a = std::unique_ptr<impl>{p_impl};//Guarantee that p_impl gets destroyed even if an exception is thrown
            check(uv_run(p_impl->m_loop, UV_RUN_ONCE));//One final run to deal with all the handle close events
            if (uv_loop_alive(p_impl->m_loop)) {
                throw std::runtime_error{"ERROR: Trying to destroy loop when it is still alive!"};
            }
            uv_loop_delete(p_impl->m_loop);
        }
    }
}
