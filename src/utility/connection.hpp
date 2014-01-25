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
#include <SFML/Network.hpp>
#include <memory>
#include <chrono>

namespace nlp {
    class connection {
    public:
        bool is_disconnected();
    protected:
        connection(std::unique_ptr<sf::TcpSocket> && ptr);
        void update();
    private:
        std::unique_ptr<sf::TcpSocket> socket;
        std::chrono::steady_clock::time_point last_ping = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point last_pong = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration ping_time = std::chrono::seconds(0);
        bool disconnected = false;
    };
}
