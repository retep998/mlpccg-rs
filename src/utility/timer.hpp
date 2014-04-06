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
#include <functional>
#include <chrono>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        class timer final : public handle {
        public:
            class impl;
            timer() = default;
            timer(timer const &) = default;
            timer(timer &&) = default;
            ~timer() = default;
            timer & operator=(timer const &) = default;
            timer & operator=(timer &&) = default;
            impl * operator->() const;
            static timer create(loop const &);
            void start(std::function<void(void)>, std::chrono::milliseconds, std::chrono::milliseconds);
            void stop();
            void again();
            void set_repeat(std::chrono::milliseconds);
            std::chrono::milliseconds get_repeat();
        protected:
        };
    }
}
