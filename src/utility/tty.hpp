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

#pragma once
#include <string>
#include <cstdint>
#include <memory>

struct uv_tty_s;
namespace nlp {
    class loop;
    class tty {
    public:
        enum style {
            clear = 0,
            fgbright = 1,
            bgbright = 5,
            fgdark = 21,
            bgdark = 25,
            fgblack = 30,
            fgred = 31,
            fggreen = 32,
            fgyellow = 33,
            fgblue = 34,
            fgmagenta = 35,
            fgcyan = 36,
            fgwhite = 37,
            bgblack = 40,
            bgred = 41,
            bggreen = 42,
            bgyellow = 43,
            bgblue = 44,
            bgmagenta = 45,
            bgcyan = 46,
            bgwhite = 47,
        };
        tty() = delete;
        tty(tty const &) = delete;
        tty(tty &&) = delete;
        tty(loop const &);
        ~tty() noexcept(false);
        tty & operator=(tty const &) = delete;
        tty & operator=(tty &&) = delete;
        tty & operator<<(std::string const &);
        tty & operator<<(style const &);
        tty & operator<<(char const &);
        tty & set(std::initializer_list<style> const &);
        tty & write(std::string const &);
    private:
        std::unique_ptr<uv_tty_s> m_tty;
    };
}
