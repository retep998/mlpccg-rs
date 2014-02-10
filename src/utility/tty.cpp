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
#include "loop.hpp"
#include "check.hpp"
#include <uv.h>
#include <memory>
#include <algorithm>

namespace nlp {
    void callback_close_tty(uv_handle_t * p_handle) {
        delete p_handle;
    }
    void callback_write_tty(uv_write_t * p_write, int p_status) {
        check(p_status);
        delete p_write;
    }
    tty::tty(loop const & p_loop) : m_tty{std::make_unique<uv_tty_t>()} {
        check(uv_tty_init(p_loop.get(), m_tty.get(), 1, false));
    }
    tty::~tty() {
        uv_close(reinterpret_cast<uv_handle_t *>(m_tty.release()), callback_close_tty);
    }
    tty & tty::operator<<(std::string const & p_str) {
        auto write = std::make_unique<uv_write_t>();
        auto buf = uv_buf_t{};
        buf.base = const_cast<char *>(p_str.c_str());
        buf.len = static_cast<decltype(buf.len)>(p_str.size());
        check(uv_write(write.release(), reinterpret_cast<uv_stream_t *>(m_tty.get()), &buf, 1, callback_write_tty));
        return *this;
    }
    tty & tty::operator<<(char const & p_char) {
        auto write = std::make_unique<uv_write_t>();
        auto buf = uv_buf_t{};
        buf.base = const_cast<char *>(&p_char);
        buf.len = static_cast<decltype(buf.len)>(sizeof(p_char));
        check(uv_write(write.release(), reinterpret_cast<uv_stream_t *>(m_tty.get()), &buf, 1, callback_write_tty));
        return *this;
    }
    tty & tty::operator<<(style const & p_style) {
        auto str = "\x1b[" + std::to_string(static_cast<int>(p_style)) + "m";
        auto write = std::make_unique<uv_write_t>();
        auto buf = uv_buf_t{};
        buf.base = const_cast<char *>(str.c_str());
        buf.len = static_cast<decltype(buf.len)>(str.size());
        check(uv_write(write.release(), reinterpret_cast<uv_stream_t *>(m_tty.get()), &buf, 1, callback_write_tty));
        return *this;
    }
    tty & tty::set(std::initializer_list<style> const & p_styles) {
        auto str = std::string{"\x1b["};
        for (auto it = p_styles.begin(); it != p_styles.end(); ++it) {
            if (it != p_styles.begin()) {
                str.push_back(';');
            }
            str.append(std::to_string(static_cast<int>(*it)));
        }
        str.push_back('m');
        auto write = std::make_unique<uv_write_t>();
        auto buf = uv_buf_t{};
        buf.base = const_cast<char *>(str.c_str());
        buf.len = static_cast<decltype(buf.len)>(str.size());
        check(uv_write(write.release(), reinterpret_cast<uv_stream_t *>(m_tty.get()), &buf, 1, callback_write_tty));
        return *this;
    }
    std::string tty::strip(std::string p_str) {
        for (auto & c : p_str) {
            if (c == '\x1b') {
                c = '\0';
            }
        }
        return p_str;
    }
}
