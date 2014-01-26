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
#include <functional>

namespace nlp {
    std::vector<connection::handler> handlers;
    std::ostream & connection::report() {
        std::cout << "[" << socket->getRemoteAddress() << ":" << socket->getRemotePort() << "] ";
        return std::cout;
    }
    connection::connection(std::unique_ptr<sf::TcpSocket> && ptr) : socket(std::move(ptr)) {
        socket->setBlocking(false);
        report() << "Connected." << std::endl;
    }
    void connection::update() {
        if (disconnected)
            return;
        auto now = std::chrono::steady_clock::now();
        if (now - last_ping > std::chrono::seconds(10)) {
            send_ping();
        }
        if (now - last_pong > std::chrono::seconds(20)) {
            report() << "Timed out." << std::endl;
            disconnected = true;
            return;
        }
        sf::Packet p;
        auto err = socket->receive(p);
        switch (err) {
        case sf::Socket::Status::Disconnected:
            report() << "Disconnected." << std::endl;
            disconnected = true;
            break;
        case sf::Socket::Status::Done: {
            uint16_t opcode;
            p >> opcode;
            if (opcode >= handlers.size()) {
                report() << "Invalid opcode: " << opcode << std::endl;
                disconnected = true;
                break;
            }
            auto & f = handlers[opcode];
            if (!f) {
                report() << "Invalid opcode: " << opcode << std::endl;
                disconnected = true;
                break;
            }
            (this->*f)(p);
            break;
        }
        case sf::Socket::Status::Error:
            report() << "Error." << std::endl;
            disconnected = true;
            break;
        case sf::Socket::Status::NotReady:
            break;
        }
    }
    bool connection::is_disconnected() {
        return disconnected;
    }
    void connection::add_handler(size_t opcode, handler func) {
        if (opcode >= handlers.size())
            handlers.resize(opcode + 1);
        handlers[opcode] = func;
    }
    void connection::init() {
        add_handler(0x0001, &connection::handle_ping);
        add_handler(0x0002, &connection::handle_pong);
    }
    void connection::handle_ping(sf::Packet &) {
        send_pong();
    }
    void connection::handle_pong(sf::Packet &) {
        last_pong = std::chrono::steady_clock::now();
        ping_time = last_pong - last_ping;
        report() << "Ping: " << std::chrono::duration_cast<std::chrono::milliseconds>(ping_time).count() << "ms" << std::endl;
    }
    void connection::send_ping() {
        last_ping = std::chrono::steady_clock::now();
        sf::Packet p;
        p << uint16_t(0x0001);
        socket->send(p);
    }
    void connection::send_pong() {
        sf::Packet p;
        p << uint16_t(0x0002);
        socket->send(p);
    }
}
