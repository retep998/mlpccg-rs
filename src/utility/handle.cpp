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

#include "handle.hpp"
#include "handle_impl.hpp"
#include "handle_deleter.hpp"
#include "loop.hpp"

#pragma warning(push, 1)
#include <iostream>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        //handle
        handle::handle(std::shared_ptr<impl> p_impl) :
            m_impl{std::move(p_impl)} {}
        //handle::impl
        handle::impl::impl(std::shared_ptr<loop::impl> p_loop) :
            m_loop{std::move(p_loop)} {}
        //handle::deleter
        void handle::deleter::operator()(impl * p_impl) const {
            uv_close(p_impl->get_handle(), [](uv_handle_t * p_handle) {
                delete reinterpret_cast<impl *>(p_handle->data);
                std::cerr << "Handle deleted!" << std::endl;
            });
            p_impl->m_loop.reset();
        }
    }
}
