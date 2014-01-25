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

#include "client.hpp"
#include "../connection.hpp"
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <chrono>
#include <thread>

namespace nlp {
    namespace client {
        void run() {
            std::unique_ptr<sf::TcpSocket> socket = std::make_unique<sf::TcpSocket>();
            socket->connect("127.0.0.1", 273, sf::seconds(3));
            connection conn(std::move(socket));
            sf::RenderWindow window;
            window.create(sf::VideoMode(800, 600), "NoLifePony");
            while (window.isOpen()) {
                sf::Event e;
                while (window.pollEvent(e)) switch (e.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                default:;
                }
                conn.update();
                window.clear(conn.is_disconnected() ? sf::Color::Red : sf::Color::Green);
                window.display();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
}
