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
#include "context.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <list>

namespace nlp {
    namespace client {
        void run() {
            std::list<sf::TcpSocket> sockets;
            sf::Packet in;
            sf::Packet out;
            out << uint16_t(2);
            for (;;) {
                sockets.emplace_back();
                sockets.back().setBlocking(false);
                sockets.back().connect("72.19.121.29", 273);
                for (auto & s : sockets) {
                    auto err = s.receive(in);
                    if (err == sf::Socket::Status::Done) {
                        uint16_t op;
                        in >> op;
                        if (op == 1) {
                            s.send(out);
                        }
                    }
                }
                //std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            context::init();
            context::manager current;
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
                current.update();
                current.render(window);
                window.display();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
}
