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
#include <memory>
#include <functional>
#include <cstdint>

namespace nlp {
    class packet_handler;
    class listener_manager;
    class connection_manager;
    class manager final {
    public:
        manager() = default;
        manager(manager const &) = delete;
        manager & operator=(manager const &) = delete;
        template <typename T>
        void listen(uint16_t port) {
            add_listener(port, [] {
                return std::make_unique<T>();
            });
        }
    private:
        void add_listener(uint16_t, std::function<std::unique_ptr<packet_handler>()>);
        std::unique_ptr<listener_manager> lmanager;
        std::unique_ptr<connection_manager> cmanager;
    };
}