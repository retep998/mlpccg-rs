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
#include <iostream>
#include <string>

namespace nlp {
    class block {
    public:
        block() = delete;
        block(block const &) = delete;
        block(block &&) = delete;
        block(std::string);
        block & operator=(block const &) = delete;
        block & operator=(block &&) = delete;
        std::string & value();
        std::string const & value() const;
    private:
        std::string m_str;
    };
    class log {
    public:
        log();
        log(log const &) = delete;
        log(log &&);
        ~log();
        log & operator=(log const &) = delete;
        log & operator=(log &&) = delete;
        template <typename T>
        log && operator<<(T && p_value) && {
            std::cout << p_value;
            return std::move(*this);
        }
        log && operator<<(block &&) && ;
    private:
        bool m_owned = true;
    };
}
