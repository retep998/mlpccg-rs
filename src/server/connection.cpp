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
#include "recv_handler.hpp"
#include <utility/format.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <iostream>

namespace nlp {
    connection::connection(recv_handler_creator const & func, std::unique_ptr<sf::TcpSocket> && sock) : socket{std::move(sock)} {
        recv = func(this);
        socket->setBlocking(false);
    }
    connection::~connection() {}
    sf::Socket & connection::get_socket() const {
        return *socket;
    }
    void connection::update() {
        if (disconnected) {
            return;
        }
        for (;;) {
            auto err = socket->receive(packet);
            if (err == sf::Socket::Status::Error || err == sf::Socket::Status::Disconnected) {
                disconnect();
                return;
            } else if (err == sf::Socket::Status::Done) {
                recv->recv(packet);
            } else {
                return;
            }
        }
    }
    void connection::recv_update() {
        if (disconnected) {
            return;
        }
        recv->update();
    }
    void connection::send(sf::Packet & p) {
        socket->send(p);
    }
    void connection::disconnect() {
        disconnected = true;
    }
    bool connection::is_disconnected() const {
        return disconnected;
    }
}
