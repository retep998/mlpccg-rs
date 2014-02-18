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
#include "loop.hpp"
#include "ip.hpp"

#pragma warning(push, 1)
#include <functional>
#include <chrono>
#pragma warning(pop)

namespace nlp {
    namespace uv {
        class tcp final : public stream {
        public:
            class impl;
            tcp() = default;
            tcp(tcp const &) = default;
            tcp(tcp &&) = default;
            ~tcp() = default;
            tcp & operator=(tcp const &) = default;
            tcp & operator=(tcp &&) = default;
            impl * operator->() const;
            static tcp create(loop const &);
            void bind(ip const &);
            void nodelay(bool);
            void keepalive(bool, std::chrono::seconds);
            void simultaneous_accepts(bool);
            ip getsockname();
            ip getpeername();
            void connect(ip const &, std::function<void()>);
        protected:
        };
    }
}
