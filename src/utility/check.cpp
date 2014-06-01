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

#include "check.hpp"

#pragma warning(push, 1)
#include <uv.h>
#include <stdexcept>
#include <string>
#include <iostream>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        __declspec(noinline)
        void fail(int p_err) {
            std::cerr << "ERROR: " << ::uv_err_name(p_err);
            std::cerr << ": " << ::uv_strerror(p_err) << std::endl;
            std::abort();
        }
        void check(int p_err) {
            if (p_err < 0) {
                fail(p_err);
            }
        }
    }
}
