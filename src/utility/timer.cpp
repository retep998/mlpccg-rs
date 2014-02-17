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

#include "timer_impl.hpp"
#include "loop_impl.hpp"
#include "handle_deleter.hpp"
#include "check.hpp"

namespace nlp {
    namespace uv {
        //timer
        void timer::start(std::function<void(void)> p_callback, std::chrono::milliseconds p_timeout, std::chrono::milliseconds p_repeat) {
            get()->m_callback = std::move(p_callback);
            check(uv_timer_start(&get()->m_timer, [](uv_timer_t * p_timer, int p_status) {
                check(p_status);
                auto a = static_cast<timer::impl *>(static_cast<handle::impl *>(p_timer->data));
                a->m_callback();
            }, static_cast<uint64_t>(p_timeout.count()), static_cast<uint64_t>(p_repeat.count())));
        }
        timer timer::create(loop const & p_loop) {
            auto && a = timer{};
            a.m_impl = std::shared_ptr<impl>{new impl{p_loop}, deleter{}};
            return{std::move(a)};
        }
        std::shared_ptr<timer::impl> timer::get() {
            return std::static_pointer_cast<impl>(m_impl);
        }
        //timer::impl
        uv_handle_t & timer::impl::get_handle() {
            return reinterpret_cast<uv_handle_t &>(m_timer);
        }
        timer::impl::impl(loop const & p_loop) :
            handle::impl{p_loop} {
            check(uv_timer_init(m_loop->get(), &m_timer));
            m_timer.data = static_cast<handle::impl *>(this);
        }
    }
}
