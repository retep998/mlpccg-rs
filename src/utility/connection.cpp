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

#include "connection.hpp"
#include <iostream>

namespace nlp {
    std::ostream & report(std::unique_ptr<sf::TcpSocket> & socket) {
        std::cout << "[" << socket->getRemoteAddress() << ":" << socket->getRemotePort() << "] ";
        return std::cout;
    }
    connection::connection(std::unique_ptr<sf::TcpSocket> && ptr) : socket(std::move(ptr)) {
        socket->setBlocking(false);
        report(socket) << "Connected." << std::endl;
    }
    void connection::update() {
        if (disconnected)
            return;
        sf::Packet p;
        auto err = socket->receive(p);
        switch (err) {
        case sf::Socket::Status::Disconnected:
            report(socket) << "Disconnected." << std::endl;
            disconnected = true;
            break;
        case sf::Socket::Status::Done:
            report(socket) << "Received packet." << std::endl;
            //Handle packet
            break;
        case sf::Socket::Status::Error:
            report(socket) << "Error." << std::endl;
            disconnected = true;
            break;
        case sf::Socket::Status::NotReady:
            break;
        }
    }
    bool connection::is_disconnected() {
        return disconnected;
    }
}
