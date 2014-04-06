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

#include "loop_impl.hpp"
#include "check.hpp"

#pragma warning(push, 1)
#include <stdexcept>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        void loop_interface::run() {
            check(uv_run(impl().get(), UV_RUN_DEFAULT));
        }
        loop_impl & loop_interface::impl() {
            return static_cast<loop_impl &>(*this);
        }
        uv_loop_t * loop_impl::get() {
            return &m_loop;
        }
        loop loop_init() {
            auto ptr = std::make_unique<loop_impl>();
            check(uv_loop_init(ptr->get()));
            return{ptr.release(), [](loop_impl * p_impl) {
                std::unique_ptr<loop_impl> ptr{p_impl};
                check(uv_run(p_impl->get(), UV_RUN_NOWAIT));
                if (uv_loop_alive(p_impl->get())) {
                    throw std::runtime_error{"Trying to destroy loop that is still alive"};
                }
                check(uv_loop_close(p_impl->get()));
            }};
        }
    }
}
