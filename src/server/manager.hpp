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
#include <memory>
#include <functional>
#include <cstdint>
#include <list>
#include <chrono>
#include <vector>

namespace sf {
    class SocketSelector;
}
namespace nlp {
    class listener;
    class connection;
    class packet_handler;
    class manager final {
    public:
        manager();
        manager(manager const &) = delete;
        ~manager();
        manager & operator=(manager const &) = delete;
        void add_listener(uint16_t, std::function<ptr<packet_handler>(ptr<packet_handler>)>);
        void update();
        void collect();
        void disconnect(ptr<connection>);
    private:
        std::vector<std::unique_ptr<listener>> m_listeners;
        std::list<std::unique_ptr<connection>> m_connections;
        std::unique_ptr<sf::SocketSelector> m_select;
    };
}
