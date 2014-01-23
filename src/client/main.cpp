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

#include "card.hpp"
#include "deck.hpp"
#include "field.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

using namespace nlp;

int main() {
    deck d;
    if (d.is_valid())
        std::cout << "test" << std::endl;
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
        window.clear(sf::Color(255, 128, 192));
        window.display();
    }
}
