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
#include "check.hpp"
#include <uv.h>
#include <cassert>

namespace nlp {
    namespace uv {
        class loop::impl : public std::enable_shared_from_this<impl>{
        public:
        private:
            impl(uv_loop_t * p_loop, bool p_owned) :
                m_loop{p_loop},
                m_owned{p_owned} {}
            uv_loop_t * m_loop;
            bool m_owned;
            friend loop;
        };
        class loop::deleter {
        public:
            void operator()(impl * p_ptr) {
                if (p_ptr->m_owned) {
                    uv_loop_delete(p_ptr->m_loop);
                }
                delete p_ptr;
            }
        };
        void loop::run_default() const {
            assert(m_impl);
            check(uv_run(m_impl->m_loop, UV_RUN_DEFAULT));
        }
        void loop::run_once() const {
            assert(m_impl);
            check(uv_run(m_impl->m_loop, UV_RUN_ONCE));
        }
        void loop::run_nowait() const {
            assert(m_impl);
            check(uv_run(m_impl->m_loop, UV_RUN_NOWAIT));
        }
        loop loop::create() {
            auto i = std::shared_ptr<impl>{new impl(uv_loop_new(), true), deleter{}};
            return{i};
        }
        loop loop::get_default() {
            static auto def = std::shared_ptr<impl>{new impl(uv_default_loop(), false), deleter{}};
            return{def};
        }
        loop::loop(std::shared_ptr<impl> p_impl) :
            m_impl{p_impl} {
            assert(m_impl);
        }
    }
}
