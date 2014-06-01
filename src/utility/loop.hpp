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
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <uv.h>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        struct loop;
        using loop_t = std::shared_ptr<loop>;
        struct loop final {
            void run();
            void run_once();
            void run_nowait();
            bool alive() const;
            void stop();
            void update_time();
            std::chrono::milliseconds now();
            void walk(std::function<void(handle)>);
            static loop_t make();
        private:
            loop();
            void close();
            ::uv_loop_t * get();
            ::uv_loop_t const * get() const;
            static void destroy(loop *);
            ::uv_loop_t m_loop;
        };
    }
}
