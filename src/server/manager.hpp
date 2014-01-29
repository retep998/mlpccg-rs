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
#include <utility/ptr.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <memory>
#include <functional>
#include <cstdint>
#include <list>
#include <chrono>

namespace nlp {
    class listener;
    class connection;
    class recv_handler;
    class send_handler;
    using recv_handler_creator = std::function<std::unique_ptr<recv_handler>(ptr<send_handler>)>;
    class manager final {
    public:
        manager();
        manager(manager const &) = delete;
        ~manager();
        manager & operator=(manager const &) = delete;
        template <typename T>
        void listen(uint16_t port) {
            add_listener(port, [](ptr<send_handler> p) {
                return std::make_unique<T>(p);
            });
        }
        void update();
    private:
        void add_listener(uint16_t port, recv_handler_creator && func);
        std::vector<std::unique_ptr<listener>> listeners;
        std::list<std::unique_ptr<connection>> connections;
        sf::SocketSelector select;
        std::chrono::steady_clock::time_point last_update{std::chrono::steady_clock::now()};
    };
}
