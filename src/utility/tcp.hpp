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
        class tcp final : public stream {
        public:
            class impl;
            tcp() = default;
            tcp(tcp const &) = default;
            tcp(tcp &&) = default;
            ~tcp() = default;
            tcp & operator=(tcp const &) = default;
            tcp & operator=(tcp &&) = default;
            static tcp listen(loop const &, std::string const &, uint16_t);
        protected:
            tcp(std::shared_ptr<impl>);
            std::shared_ptr<impl> get_impl() const;
        };
    }
}
