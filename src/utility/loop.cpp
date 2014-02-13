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

namespace nlp {
    namespace uv {
        //loop
        void loop::run_default() const {
            check(uv_run(m_impl->m_loop, UV_RUN_DEFAULT));
        }
        void loop::run_once() const {
            check(uv_run(m_impl->m_loop, UV_RUN_ONCE));
        }
        void loop::run_nowait() const {
            check(uv_run(m_impl->m_loop, UV_RUN_NOWAIT));
        }
        loop loop::create() {
            auto && i = std::shared_ptr<impl>{new impl{uv_loop_new(), true}, deleter{}};
            return{i};
        }
        loop loop::get_default() {
            static auto && def = std::shared_ptr<impl>{new impl{uv_default_loop(), false}, deleter{}};
            return{def};
        }
        std::shared_ptr<loop::impl> const & loop::get_impl() const {
            return m_impl;
        }

        loop::loop(std::shared_ptr<impl> p_impl) :
            m_impl{std::move(p_impl)} {}
        //loop::impl
        uv_loop_t * loop::impl::get() const {
            return m_loop;
        }
        loop::impl::impl(uv_loop_t * p_loop, bool p_owned) :
            m_loop{p_loop},
            m_owned{p_owned} {}
        //loop::deleter
        void loop::deleter::operator()(impl * p_impl) {
            check(uv_run(p_impl->m_loop, UV_RUN_DEFAULT));
            if (p_impl->m_owned) {
                uv_loop_delete(p_impl->m_loop);
            }
            delete p_impl;
        }
    }
}
