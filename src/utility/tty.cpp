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
#include "loop_impl.hpp"
#include "check.hpp"
#include "stream_impl.hpp"
#include <uv.h>
#include <memory>
#include <algorithm>
#include <cassert>
#include <iostream>

namespace nlp {
    namespace uv {
        class tty::impl final : public stream::impl{
        public:
            uv_stream_t * get_stream() override {
                return reinterpret_cast<uv_stream_t *>(&m_tty);
            }
        protected:
            impl(std::shared_ptr<loop::impl> p_loop) : stream::impl{p_loop} {
                check(uv_tty_init(m_loop->get(), &m_tty, 1, false));
                m_tty.data = this;
            }
            uv_tty_t m_tty;
            friend tty;
        };
        class tty::deleter final {
        public:
            static void callback(uv_handle_t * p_handle) {
                assert(p_handle);
                auto a = reinterpret_cast<impl *>(p_handle->data);
                assert(a);
                delete a;
                std::cerr << "Deleted tty";
            }
            void operator()(impl * p_impl) {
                p_impl->m_loop.reset();
                uv_close(p_impl->get_handle(), &callback);
            }
        };
        tty tty::create(loop const & p_loop) {
            auto i = std::shared_ptr<impl>{new impl(p_loop.get_impl()), deleter{}};
            return{i};
        }
        tty::tty(std::shared_ptr<impl> p_impl) :
            stream{std::static_pointer_cast<stream::impl>(p_impl)}{

        }
        std::shared_ptr<tty::impl> tty::get_impl() const {
            return std::static_pointer_cast<tty::impl>(m_impl);
        }
    }
}
