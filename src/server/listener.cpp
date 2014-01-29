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

#include "listener.hpp"
#include "connection.hpp"
#include <SFML/Network/IpAddress.hpp>
#include <iostream>

namespace nlp {
    listener::iterator::iterator(iterator && o) : listen{o.listen}, next{std::move(o.next)} {}
    listener::iterator::iterator(ptr<listener> listen, bool pop) : listen{listen} {
        if (pop) {
            next = listen->get_next();
        }
    }
    listener::iterator::~iterator() {}
    bool listener::iterator::operator!=(iterator const & o) const {
        return next != o.next;
    }
    listener::iterator & listener::iterator::operator++() {
        next = listen->get_next();
        return *this;
    }
    std::unique_ptr<connection> listener::iterator::operator*() {
        return std::move(next);
    }
    listener::listener(std::unique_ptr<sf::TcpListener> && listen, recv_handler_creator && func) : listen{std::move(listen)}, func{std::move(func)} {}
    listener::~listener() {}
    std::unique_ptr<listener> listener::create(uint16_t port, recv_handler_creator && func) {
        auto listen = std::make_unique<sf::TcpListener>();
        auto err = listen->listen(port);
        if (err != sf::Socket::Status::Done) {
            std::cerr << "Failed to listen to port " << port << std::endl;
            return nullptr;
        }
        std::cout << "Listening on port " << port << std::endl;
        listen->setBlocking(false);
        return std::make_unique<listener>(std::move(listen), std::move(func));
    }
    listener::iterator listener::begin() {
        return{this, true};
    }
    listener::iterator listener::end() {
        return{this, false};
    }
    sf::Socket & listener::get_socket() const {
        return *listen;
    }
    std::unique_ptr<connection> listener::get_next() {
        if (!socket) {
            socket = std::make_unique<sf::TcpSocket>();
        }
        auto err = listen->accept(*socket);
        if (err != sf::Socket::Status::Done) {
            return nullptr;
        }
        std::cout << "Accepted connection from " << socket->getRemoteAddress() << std::endl;
        auto next = std::make_unique<connection>(func, std::move(socket));
        return next;
    }
}
