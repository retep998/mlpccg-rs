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
#include "tcp.hpp"

namespace nlp {
    class packet;
    class framed_stream final {
    public:
        class impl;
        framed_stream() = default;
        framed_stream(framed_stream const &) = default;
        framed_stream(framed_stream &&) = default;
        ~framed_stream() = default;
        framed_stream & operator=(framed_stream const &) = default;
        framed_stream & operator=(framed_stream &&) = default;
        void write(packet) const;
        void read(std::function<void(packet)>);
        uv::stream const & get() const;
        static framed_stream create(uv::stream);
    protected:
        framed_stream(std::shared_ptr<impl>);
        std::shared_ptr<impl> m_impl;
    };
}
