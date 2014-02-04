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
#include <uv.h>
#include <memory>
#include <iostream>

namespace nlp {
    uv_loop_t;
    tty::tty(loop const & p_loop) : m_tty{std::make_unique<uv_tty_t>()} {
        if (uv_tty_init(p_loop.get(), m_tty.get(), 1, false)) {
            throw std::runtime_error{"Failed to initialize TTY!"};
        }
        if (uv_guess_handle(1) != UV_TTY) {
            throw std::runtime_error{"Terminal is not TTY!"};
        }
    }
    tty::~tty() noexcept(false) {
        if (uv_tty_reset_mode()) {
            throw std::runtime_error{"Failed to reset TTY mode!"};
        }
    }
    tty & tty::operator<<(std::string const & p_str) {
        std::cout << p_str;
        return *this;
    }
    tty & tty::operator<<(char const & p_char) {
        std::cout << p_char;
        return *this;
    }
    tty & tty::operator<<(style const & p_style) {
        std::cout << std::flush;
        auto str = "\x1b[" + std::to_string(static_cast<int>(p_style)) + "m";
        auto write = uv_write_t{};
        auto buf = uv_buf_t{};
        buf.base = const_cast<char *>(str.c_str());
        buf.len = static_cast<decltype(buf.len)>(str.size());
        if (uv_write(&write, reinterpret_cast<uv_stream_t *>(m_tty.get()), &buf, 1, nullptr)) {
            throw std::runtime_error{"Failed to write data!"};
        }
        return *this;
    }
    tty & tty::set(std::initializer_list<style> const & p_styles) {
        std::cout << std::flush;
        auto str = std::string{"\x1b["};
        for (auto it = p_styles.begin(); it != p_styles.end(); ++it) {
            if (it != p_styles.begin()) {
                str.push_back(';');
            }
            str.append(std::to_string(static_cast<int>(*it)));
        }
        str.push_back('m');
        auto write = uv_write_t{};
        auto buf = uv_buf_t{};
        buf.base = const_cast<char *>(str.c_str());
        buf.len = static_cast<decltype(buf.len)>(str.size());
        if (uv_write(&write, reinterpret_cast<uv_stream_t *>(m_tty.get()), &buf, 1, nullptr)) {
            throw std::runtime_error{"Failed to write data!"};
        }
        return *this;
    }
}
