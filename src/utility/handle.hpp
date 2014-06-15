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
#include "loop.hpp"

#pragma warning(push, 1)
#include <memory>
#pragma warning(pop)

namespace nlp {
namespace uv {
class handle;
using handle_t = std::shared_ptr<handle>;
class handle {
public:
    bool is_active() const;

protected:
private:
    void close(::uv_close_cb);
    bool has_ref() const;
    bool is_closing() const;
    void ref();
    void unref();
};
}
}
