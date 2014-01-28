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

#include "manager.hpp"
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <list>

namespace nlp {
    class listener {
    public:
        listener() = default;
        listener(listener const &) = delete;
        listener & operator=(listener const &) = delete;
    private:
        sf::TcpListener listen;
        std::unique_ptr<sf::TcpSocket> next_socket;
    };
    class listener_manager {
    public:
        listener_manager() = default;
        listener_manager(listener_manager const &) = delete;
        listener_manager & operator=(listener_manager const &) = delete;
        void add_listener(uint16_t port, std::function<std::unique_ptr<packet_handler>()> func) {

        }
    private:
        std::list<listener> listeners;
    };
    void manager::add_listener(uint16_t port, std::function<std::unique_ptr<packet_handler>()> func) {
        if (!lmanager) {
            lmanager = std::make_unique<listener_manager>();
        }
        lmanager->add_listener(port, func);
    }
}