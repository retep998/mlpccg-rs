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
#include "packet_handler.hpp"
#include "manager.hpp"

#pragma warning(push, 1)
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <iostream>
#include <iomanip>
#pragma warning(pop)

namespace nlp {
    connection::connection(std::function<ptr<packet_handler>(ptr<packet_handler>)> p_func,
                           std::unique_ptr<sf::TcpSocket> p_socket, ptr<manager> p_manager) :
        m_socket{std::move(p_socket)},
        m_manager{p_manager} {
        m_receive = p_func(this);
        m_socket->setBlocking(false);
    }
    connection::~connection() {}
    sf::Socket & connection::get_socket() const {
        return *m_socket;
    }
    void connection::update() {
        if (is_dead()) {
            return;
        }
        sf::Packet packet;
        for (;;) {
            auto err = m_socket->receive(packet);
            if (err == sf::Socket::Status::Disconnected) {
                kill();
                return;
            } else if (err == sf::Socket::Status::Error) {
                kill();
                return;
            } else if (err == sf::Socket::Status::Done) {
                m_receive->handle(packet);
            } else {
                return;
            }
        }
    }
    bool connection::is_dead() const {
        return m_dead;
    }
    void connection::handle(sf::Packet & p) {
        if (is_dead()) {
            return;
        }
        m_socket->send(p);
    }
    void connection::kill() {
        if (!is_dead()) {
            m_dead = true;
            m_receive->kill();
            m_manager->kill(this);
        }
    }
}
