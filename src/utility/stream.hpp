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
#include "handle.hpp"

#pragma warning(push, 1)
#include <string>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        class stream : public handle {
        public:
            class impl;
            class deleter;
            class writer;
            stream() = default;
            stream(stream const &) = default;
            stream(stream &&) = default;
            ~stream() = default;
            stream & operator=(stream const &) = default;
            stream & operator=(stream &&) = default;
            void write(std::string const &);
        protected:
            stream(std::shared_ptr<impl>);
            std::shared_ptr<impl> get_impl() const;
        };
    }
}
