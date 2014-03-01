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

#include "ttystream.hpp"
#include "tty.hpp"

#pragma warning(push, 1)
#include <iostream>
#include <string>
#include <iomanip>
#pragma warning(pop)

namespace nlp {
    class ttybuf : public std::streambuf {
    public:
        ttybuf() = delete;
        ttybuf(uv::loop const & p_loop) :
            m_tty{uv::tty::create(p_loop)},
            m_old{std::cout.rdbuf()} {
            std::cout.rdbuf(this);
        }
        ttybuf(ttybuf const &) = delete;
        ttybuf(ttybuf &&) = delete;
        ~ttybuf() {
            std::cout.rdbuf(m_old);
        }
        ttybuf & operator=(ttybuf const &) = delete;
        ttybuf & operator=(ttybuf &&) = delete;
        int_type overflow(int_type p_char) override {
            if (!traits_type::eq_int_type(traits_type::eof(), p_char)) {
                m_buf.push_back(traits_type::to_char_type(p_char));
            }
            return{0};
        }
        int sync() override {
            m_tty.write(m_buf.cbegin(), m_buf.cend());
            m_buf.clear();
            return{0};
        }
        std::streamsize xsputn(char_type const * p_str, std::streamsize p_length) override {
            if (p_length < 0) {
                throw std::invalid_argument{"Length cannot be negative"};
            }
            m_buf.append(p_str, static_cast<std::string::size_type>(p_length));
            return p_length;
        }
    private:
        uv::tty m_tty;
        std::string m_buf;
        std::streambuf * m_old;
    };
    class ttystream::impl {
    public:
        impl() = delete;
        impl(uv::loop const & p_loop) :
            m_out{p_loop} {}
        impl(impl const &) = delete;
        impl(impl &&) = delete;
        ~impl() = default;
        impl & operator=(impl const &) = delete;
        impl & operator=(impl &&) = delete;
    private:
        ttybuf m_out;
    };
    ttystream::ttystream(uv::loop const & p_loop) :
        m_impl{std::make_unique<impl>(p_loop)} {}
    ttystream::~ttystream() = default;
    void action::operator()(std::ostream & p_stream) const {
        return m_func(p_stream);
    }
    std::ostream & operator<<(std::ostream & p_stream, action const & p_action) {
        p_action(p_stream);
        return p_stream;
    }
    std::ostream & operator<<(std::ostream & p_stream, style const & p_style) {
        return p_stream << "\x1b[" << static_cast<int>(p_style) << "m";
    }
    action strip(std::string const & p_str) {
        return{[&](std::ostream & p_stream) {
            for (auto const & c : p_str) {
                if (c < 0 || c >= 0x20) {
                    p_stream.put(c);
                }
            }
        }};
    }
    action quote(std::string const & p_str) {
        return{[&](std::ostream & p_stream) {
            p_stream << '"' << strip(p_str) << '"';
        }};
    }
    std::ostream & time(std::ostream & p_stream) {
        auto t = std::time(nullptr);
        auto tm = std::localtime(&t);
        p_stream << style::fgyellow << style::fgbright << std::put_time(tm, "[%H:%M:%S] ") << style::fgwhite;
        return p_stream;
    }
}
