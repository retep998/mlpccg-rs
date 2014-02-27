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
#include "stream.hpp"
#include "loop.hpp"

namespace nlp {
    namespace uv {
        class tty final : public stream {
        public:
            class impl;
            tty() = default;
            tty(tty const &) = default;
            tty(tty &&) = default;
            ~tty() = default;
            tty & operator=(tty const &) = default;
            tty & operator=(tty &&) = default;
            impl * operator->() const;
            static tty create(loop const &);
        protected:
        };
    }
}
