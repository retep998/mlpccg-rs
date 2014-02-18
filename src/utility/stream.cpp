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
        stream::impl * stream::operator->() const {
            return static_cast<impl *>(m_impl.get());
        }
        void stream::listen(std::function<void(stream)> p_func, int p_backlog) const {
            (*this)->m_listen_callback = std::move(p_func);
            check(::uv_listen(&(*this)->get_stream(), p_backlog, [](uv_stream_t * p_stream, int p_status) {
                auto && a = static_cast<stream::impl *>(static_cast<handle::impl *>(p_stream->data));
                check(p_status);
                auto && b = a->clone();
                check(::uv_accept(&a->get_stream(), &b->get_stream()));
                a->m_listen_callback(std::move(b));
            }));
        }
        void stream::read_start(std::function<void(std::vector<char> const &)> p_func) const {
            (*this)->m_read_callback = std::move(p_func);
            check(::uv_read_start(&(*this)->get_stream(),[](uv_handle_t * p_handle, size_t p_size, uv_buf_t * p_buf) {
                auto && a = static_cast<stream::impl *>(static_cast<handle::impl *>(p_handle->data));
                a->m_read_buf.resize(p_size);
                *p_buf = ::uv_buf_init(a->m_read_buf.data(), static_cast<unsigned>(a->m_read_buf.size()));
            }, [](uv_stream_t * p_stream, ssize_t p_size, uv_buf_t const *) {
                auto && a = static_cast<stream::impl *>(static_cast<handle::impl *>(p_stream->data));
                if (p_size < 0) switch (p_size) {
                case UV__EOF:
                case UV__ECONNRESET:
                    a->m_eof_callback();
                    return;
                default:
                    check(static_cast<int>(p_size));
                }
                a->m_read_buf.resize(static_cast<std::vector<char>::size_type>(p_size));
                a->m_read_callback(a->m_read_buf);
            }));
        }
        void stream::read_stop() const {
            check(::uv_read_stop(&(*this)->get_stream()));
        }
        void stream::write(std::string const & p_data) const {
            write(std::vector<char>{p_data.cbegin(), p_data.cend()});
        }
        void stream::write(std::vector<char> p_data) const {
            new writer{std::move(p_data), *this};
        }
        void stream::eof(std::function<void()> p_func) const {
            (*this)->m_eof_callback = std::move(p_func);
        }
        //stream::impl
        uv_handle_t & stream::impl::get_handle() {
            return reinterpret_cast<uv_handle_t &>(get_stream());
        }
        stream::impl::impl(loop const & p_loop) :
            handle::impl{p_loop} {}
        //stream::writer
        stream::writer::writer(std::vector<char> && p_data, stream const & p_stream) :
            m_data{std::move(p_data)} {
            m_buf = ::uv_buf_init(m_data.data(), static_cast<unsigned>(m_data.size()));
            m_write.data = this;
            check(::uv_write(&m_write, &p_stream->get_stream(), &m_buf, unsigned{1},
                [](uv_write_t * p_write, int p_status) {
                auto && a = std::unique_ptr<writer>{static_cast<writer *>(p_write->data)};
                check(p_status);
            }));
        }
    }
}
