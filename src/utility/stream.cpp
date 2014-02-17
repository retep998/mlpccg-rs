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

#pragma warning(push, 1)
#include <algorithm>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        //stream
        void stream::write(std::string const & p_data) const {
            write(std::vector<char>{p_data.cbegin(), p_data.cend()});
        }
        void stream::write(std::vector<char> p_data) const {
            new writer{std::move(p_data), &get()->get_stream()};
        }
        void stream::read(std::function<void(std::vector<char> const &)> p_func) const {
            get()->m_read_callback = std::move(p_func);
            check(uv_read_start(&get()->get_stream(), [](uv_handle_t * p_handle, size_t p_size, uv_buf_t * p_buf) {
                auto && a = static_cast<stream::impl *>(static_cast<handle::impl *>(p_handle->data));
                a->m_read_buf.resize(p_size);
                *p_buf = uv_buf_init(a->m_read_buf.data(), static_cast<unsigned>(a->m_read_buf.size()));
            }, [](uv_stream_t * p_stream, ssize_t p_size, uv_buf_t const *) {
                auto && a = static_cast<stream::impl *>(static_cast<handle::impl *>(p_stream->data));
                if (p_size < 0) switch (p_size) {
                case UV__EOF:
                case UV__ECONNRESET:
                    if (a->m_disconnect_callback) {
                        a->m_disconnect_callback();
                    }
                    return;
                default:
                    check(static_cast<int>(p_size));
                }
                a->m_read_buf.resize(static_cast<std::vector<char>::size_type>(p_size));
                a->m_read_callback(a->m_read_buf);
            }));
        }
        void stream::disconnect(std::function<void()> p_func) const {
            get()->m_disconnect_callback = std::move(p_func);
        }
        std::shared_ptr<stream::impl> stream::get() const {
            return std::static_pointer_cast<stream::impl>(m_impl);
        }
        //stream::impl
        uv_handle_t & stream::impl::get_handle() {
            return reinterpret_cast<uv_handle_t &>(get_stream());
        }
        void stream::impl::listen(int p_backlog, uv_connection_cb p_callback) {
            check(uv_listen(&get_stream(), p_backlog, p_callback));
        }
        stream::impl::impl(loop const & p_loop) :
            handle::impl{p_loop} {}
        //stream::writer
        stream::writer::writer(std::vector<char> && p_data, uv_stream_t * p_stream) :
            m_data{std::move(p_data)} {
            m_buf = uv_buf_init(m_data.data(), static_cast<unsigned>(m_data.size()));
            m_write.data = this;
            check(uv_write(&m_write, p_stream, &m_buf, unsigned{1}, [](uv_write_t * p_write, int p_status) {
                check(p_status);
                delete static_cast<writer *>(p_write->data);
            }));
        }
    }
}
