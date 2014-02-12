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

#pragma warning(push, 1)
#include <memory>
#pragma warning(pop)

namespace nlp {
    class loop;
    class listener {
    public:
        class impl;
        listener() = default;
        listener(listener const &) = default;
        listener(listener &&) = default;
        ~listener();
        listener & operator=(listener const &) = default;
        listener & operator=(listener &&) = default;
        void open(loop const &, uint16_t);
    private:
        std::shared_ptr<impl> m_impl;
    };
}