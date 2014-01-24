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
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

namespace nlp {
    namespace client {
        void run() {
            sf::TcpSocket socket;
            bool success = false;
            if (socket.connect("127.0.0.1", 273, sf::seconds(3)) == sf::Socket::Done)
                success = true;
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
                window.clear(success ? sf::Color::Green : sf::Color::Red);
                window.display();
            }

        }
    }
}