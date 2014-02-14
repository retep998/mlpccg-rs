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
#include "stream_impl.hpp"
#include "stream_writer.hpp"
#include "check.hpp"

namespace nlp {
    namespace uv {
        //stream
        void stream::write(std::string const & p_str) {
            new writer{p_str, get_impl()->get_stream()};
        }
        stream::stream(std::shared_ptr<impl> p_impl) :
            handle{std::static_pointer_cast<handle::impl>(p_impl)} {}
        std::shared_ptr<stream::impl> stream::get_impl() const {
            return std::static_pointer_cast<stream::impl>(m_impl);
        }
        //stream::impl
        uv_handle_t * stream::impl::get_handle() {
            return reinterpret_cast<uv_handle_t *>(get_stream());
        }
        void stream::impl::listen(int p_backlog, uv_connection_cb p_callback) {
            check(uv_listen(get_stream(), p_backlog, p_callback));
        }
        stream::impl::impl(std::shared_ptr<loop::impl> p_loop) :
            handle::impl{std::move(p_loop)} {}
        //stream::writer
        stream::writer::writer(std::string const & p_str, uv_stream_t * p_stream) :
            m_data{p_str.cbegin(), p_str.cend()} {
            m_buf.len = static_cast<unsigned>(m_data.size());
            m_buf.base = m_data.data();
            m_write.data = this;
            check(uv_write(&m_write, p_stream, &m_buf, unsigned{1}, [](uv_write_t * p_write, int p_status) {
                check(p_status);
                delete reinterpret_cast<writer *>(p_write->data);
            }));
        }
    }
}
