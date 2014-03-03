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

#include "tty.hpp"
#include "tty_impl.hpp"
#include "handle_deleter.hpp"
#include "loop_impl.hpp"
#include "check.hpp"

namespace nlp {
    namespace uv {
        //tty
        tty::impl * tty::operator->() const {
            return static_cast<impl *>(m_impl.get());
        }
        tty tty::create(loop const & p_loop) {
            auto && a = tty{};
            a.m_impl = std::shared_ptr<impl>{new impl{p_loop}, deleter{}};
            return a;
        }
        //tty::impl
        tty::impl::~impl() {
            check(uv_tty_reset_mode());
        }
        uv_stream_t & tty::impl::get_stream() {
            return reinterpret_cast<uv_stream_t &>(m_tty);
        }
        stream tty::impl::clone() const {
            return create(m_loop);
        }
        tty::impl::impl(loop const & p_loop) :
            stream::impl{p_loop} {
            check(uv_tty_init(&m_loop->get(), &m_tty, 1, false));
            m_tty.data = static_cast<handle::impl *>(this);
        }
    }
}
