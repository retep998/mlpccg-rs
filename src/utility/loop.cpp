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

#pragma warning(push, 1)
#include <stdexcept>
#include <iostream>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        void loop::run() {
            check(::uv_run(get(), ::UV_RUN_DEFAULT));
        }
        void loop::run_once() {
            check(::uv_run(get(), ::UV_RUN_ONCE));
        }
        void loop::run_nowait() {
            check(::uv_run(get(), ::UV_RUN_NOWAIT));
        }
        bool loop::alive() const {
            return ::uv_loop_alive(get()) != 0;
        }
        void loop::stop() {
            ::uv_stop(get());
        }
        void loop::update_time() {
            ::uv_update_time(get());
        }
        std::chrono::milliseconds loop::now() {
            return std::chrono::milliseconds{::uv_now(get())};
        }
        void loop::walk(std::function<void(handle)> p_func) {
            uv_walk_cb;
            ::uv_walk(get(), [](uv_handle_t * p_handle, void * p_arg) {
                auto & func = *static_cast<std::function<void(handle)> *>(p_arg);
                auto && hand = handle::from_ptr(p_handle);
            }, &p_func);
        }
        loop::loop() {
            check(::uv_loop_init(get()));
        }
        ::uv_loop_t * loop::get() {
            return &m_loop;
        }
        ::uv_loop_t const * loop::get() const {
            return &m_loop;
        }
        void loop::destroy(loop * p_impl) {
            std::unique_ptr<loop> ptr{p_impl};
            //Run the loop once more to finish any active requests
            ptr->run();
            if (ptr->alive()) {
                std::cerr << "ERROR: Loop is still alive" << std::endl;
                std::abort();
            }
            check(::uv_loop_close(ptr->get()));
        }
        loop make_loop() {
            return{new loop_impl{}, destroy_loop};
        }
    }
}
