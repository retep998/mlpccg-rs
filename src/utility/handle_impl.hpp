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
#include "loop.hpp"

#pragma warning(push, 1)
#include <uv.h>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        template <typename T>
        struct handle_deleter {
            void operator()(T * p_handle) {
                ::uv_close(p_handle->handle::get(), [](::uv_handle_t * p_handle) {
                    delete static_cast<handle_impl *>(p_handle->data);
                });
                p_handle->m_loop.reset();
            }
        };
        struct handle_impl : virtual handle_interface {
            ::uv_handle_t * get();
        protected:
            void init();
        private:
            ::uv_handle_t * m_handle;
            loop m_loop;
        };

    }
}
