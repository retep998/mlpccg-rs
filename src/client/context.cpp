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

#include "context.hpp"

#pragma warning(push, 1)
#include <thread>
#pragma warning(pop)

namespace nlp {
    std::unique_ptr<sf::Font> font;
    context::manager::~manager() {
        font.reset();
    }
    void context::manager::update() {
        if (!current) {
            if (!socket) {
                connected = false;
                socket = std::make_unique<sf::TcpSocket>();
                std::thread([this] {
                    while (socket->connect("127.0.0.1", 273, sf::seconds(10)) != sf::Socket::Status::Done);
                    connected = true;
                }).detach();
            } else if (connected) {
                current = std::make_unique<context>(std::move(socket));
            }
        } else {
            current->update();
            if (current->is_disconnected()) {
                current.reset();
            }
        }
    }
    void context::manager::render(sf::RenderWindow & window) {
        if (!font) {
            font = std::make_unique<sf::Font>();
            font->loadFromFile("assets/font.ttf");
        }
        if (!current) {
            sf::Text t;
            t.setCharacterSize(64);
            t.setString("Offline");
            t.setColor(sf::Color::Red);
            t.setPosition(32, 0);
            t.setFont(*font);
            window.draw(t);
        } else {
            sf::Text t;
            t.setCharacterSize(64);
            t.setString("Online");
            t.setColor(sf::Color::Green);
            t.setPosition(32, 0);
            t.setFont(*font);
            window.draw(t);
        }
    }
}
