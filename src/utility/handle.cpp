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
#include "loop_impl.hpp"

namespace nlp {
namespace uv {
void handle_impl::init() {
    get()->data = this;
}
handle make_handle(std::unique_ptr<handle_impl> p_handle) {
    p_handle->get()->data = p_handle.get();
    return {p_handle.release(), [](handle_impl *p_handle) {}};
}
}
}
