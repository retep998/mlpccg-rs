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
#include "loop.hpp"

#pragma warning(push, 1)
#include <iosfwd>
#include <string>
#include <functional>
#pragma warning(pop)

namespace nlp {
    class ttystream {
    public:
        class impl;
        ttystream() = delete;
        ttystream(uv::loop const &);
        ttystream(ttystream const &) = delete;
        ttystream(ttystream &&) = delete;
        ~ttystream();
        ttystream & operator=(ttystream const &) = delete;
        ttystream & operator=(ttystream &&) = delete;
    private:
        std::unique_ptr<impl> m_impl;
    };
    namespace io {
        enum class style {
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
        struct action {
            void operator()(std::ostream & p_stream) const;
            std::function<void(std::ostream &)> m_func;
        };
        std::ostream & operator<<(std::ostream &, action const &);
        std::ostream & operator<<(std::ostream &, style const &);
        action strip(std::string const &);
        action quote(std::string const &);
        action name(std::string const &);
        std::ostream & time(std::ostream &);
    }
}
