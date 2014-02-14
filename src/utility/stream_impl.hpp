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
#include "handle_impl.hpp"

#pragma warning(push, 1)
#include <uv.h>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        class stream::impl : public handle::impl{
        public:
            impl() = delete;
            impl(impl const &) = delete;
            impl(impl &&) = delete;
            ~impl() = default;
            impl & operator=(impl const &) = delete;
            impl & operator=(impl &&) = delete;
            uv_handle_t * get_handle() override;
            virtual uv_stream_t * get_stream() = 0;
            void listen(int, uv_connection_cb);
        protected:
            impl(std::shared_ptr<loop::impl>);
        };
    }
}
