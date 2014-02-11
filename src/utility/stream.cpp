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

#include "stream.hpp"
#include "check.hpp"
#include "stream_impl.hpp"
#include <uv.h>
#include <cassert>
#include <vector>

namespace nlp {
    namespace uv {
        stream::impl::impl(std::shared_ptr<loop::impl> p_loop) :
            handle::impl{p_loop} {}
        class stream::writer final {
        public:
            writer(std::string const & p_str) :
                m_data{p_str.cbegin(), p_str.cend()} {
                m_buf.len = static_cast<unsigned>(m_data.size());
                m_buf.base = m_data.data();
                m_write.data = this;
            }
            static void callback(uv_write_t * p_write, int p_status) {
                assert(p_write);
                assert(p_status == 0);
                auto w = reinterpret_cast<writer *>(p_write->data);
                assert(w);
                delete w;
            }
        protected:
            std::vector<char> m_data;
            uv_buf_t m_buf;
            uv_write_t m_write;
            friend stream;
        };
        void stream::write(std::string const & p_str) {
            auto i = get_impl();
            assert(i);
            auto s = i->get_stream();
            assert(s);
            auto w = std::make_unique<writer>(p_str);
            check(uv_write(&w->m_write, s, &w->m_buf, unsigned{1}, &writer::callback));
            w.release();
        }
        stream::stream(std::shared_ptr<impl> p_impl) :
            handle{std::static_pointer_cast<handle::impl>(p_impl)} {}
        std::shared_ptr<stream::impl> stream::get_impl() const {
            return std::static_pointer_cast<stream::impl>(m_impl);
        }
    }
}
