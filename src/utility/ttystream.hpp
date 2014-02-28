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
}
