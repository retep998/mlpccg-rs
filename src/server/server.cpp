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

#include "server.hpp"
#include "player.hpp"
#include <SFML/Network.hpp>
#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <chrono>

namespace nlp {
    namespace server {
        std::vector<player> players;
        void run() {
            std::cout << "Starting NoLifePony server" << std::endl;
            sf::TcpListener listener;
            listener.listen(273);
            listener.setBlocking(false);
            std::unique_ptr<sf::TcpSocket> next_socket = std::make_unique<sf::TcpSocket>();
            for (;;) {
                if (listener.accept(*next_socket) == sf::Socket::Done) {
                    std::cout << "Accepted connection from " << next_socket->getRemoteAddress() << ":" << next_socket->getRemotePort() << std::endl;
                    players.emplace_back(next_socket);
                    next_socket = std::make_unique<sf::TcpSocket>();
                }
                for (auto && p : players) {

                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            std::cout << "Shutting down NoLifePony server" << std::endl;
        }
    }
}