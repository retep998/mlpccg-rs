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
#include "framed_stream.hpp"

namespace nlp {
    class framed_stream::impl final {
    public:
        impl() = delete;
        impl(impl const &) = delete;
        impl(impl &&) = delete;
        ~impl() = default;
        impl & operator=(impl const &) = delete;
        impl & operator=(impl &&) = delete;
    protected:
        void read(std::function<void(packet)>);
        impl(uv::stream);
        uv::stream m_stream;
        std::function<void(packet)> m_read_callback;
        std::vector<char> m_data;
        friend framed_stream;
    };
}