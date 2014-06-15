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
loop::loop() {
    check(::uv_loop_init(get()));
}
bool loop::alive() const {
    return ::uv_loop_alive(get()) != 0;
}
void loop::close() {
    check(::uv_loop_close(get()));
}
std::chrono::milliseconds loop::now() {
    return std::chrono::milliseconds{::uv_now(get())};
}
void loop::run() {
    check(::uv_run(get(), ::UV_RUN_DEFAULT));
}
void loop::run_once() {
    check(::uv_run(get(), ::UV_RUN_ONCE));
}
void loop::run_nowait() {
    check(::uv_run(get(), ::UV_RUN_NOWAIT));
}
void loop::stop() {
    ::uv_stop(get());
}
void loop::update_time() {
    ::uv_update_time(get());
}
void loop::walk(std::function<void(handle)> p_func) {
    m_walk_func = std::move(p_func);
    auto helper = [](uv_handle_t *, void *) {
        // TODO
        // m_walk_func(handle::from_ptr(p_handle));
    };
    ::uv_walk(get(), helper, nullptr);
}
::uv_loop_t *loop::get() {
    return &m_loop;
}
::uv_loop_t const *loop::get() const {
    return &m_loop;
}
void loop::destroy(loop *p_impl) {
    // Guarantee pointer is deleted when this function ends
    std::unique_ptr<loop> ptr{p_impl};
    // Run the loop once more to finish any active requests
    ptr->run();
    if (ptr->alive()) {
        std::cerr << "ERROR: Loop is still alive" << std::endl;
        std::abort();
    }
    ptr->close();
}
loop_t loop::make() {
    return {new loop{}, destroy};
}
}
}
