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

namespace nlp {
    uv_loop_s * loop::get() const {
        return m_loop;
    }
    void loop::run_default() const {
        check(m_loop);
        check(uv_run(m_loop, UV_RUN_DEFAULT));
    }
    void loop::run_once() const {
        check(m_loop);
        check(uv_run(m_loop, UV_RUN_ONCE));
    }
    void loop::run_nowait() const {
        check(m_loop);
        check(uv_run(m_loop, UV_RUN_NOWAIT));
    }
    loop loop::create() {
        return{uv_loop_new()};
    }
    loop loop::get_default() {
        return{uv_default_loop()};
    }
    loop::loop(uv_loop_t * p_loop) :
        m_loop{p_loop} {}
}
