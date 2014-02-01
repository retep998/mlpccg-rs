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
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <iostream>
#include <iomanip>

namespace nlp {
    connection::connection(std::function<ptr<packet_handler>(ptr<packet_handler>)> p_func, std::unique_ptr<sf::TcpSocket> p_socket, ptr<manager> p_manager) :
        m_socket{std::move(p_socket)},
        m_packet{std::make_unique<sf::Packet>()},
        m_manager{p_manager} {
        m_receive = p_func(this);
        m_socket->setBlocking(false);
    }
    connection::~connection() {}
    sf::Socket & connection::get_socket() const {
        return *m_socket;
    }
    void connection::update() {
        if (is_disconnected()) {
            return;
        }
        for (;;) {
            auto err = m_socket->receive(*m_packet);
            if (err == sf::Socket::Status::Error || err == sf::Socket::Status::Disconnected) {
                disconnect();
                return;
            } else if (err == sf::Socket::Status::Done) {
                auto a = reinterpret_cast<unsigned char const *>(m_packet->getData());
                auto b = a + m_packet->getDataSize();
                std::cout << m_socket->getRemoteAddress() << ":" << m_socket->getRemotePort() << " receive ";
                for (auto i = a; i != b; ++i) {
                    std::cout << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(*i) << ' ';
                }
                std::cout << std::endl;
                m_receive->handle(*m_packet);
            } else {
                return;
            }
        }
    }
    bool connection::is_disconnected() const {
        return m_disconnected;
    }
    void connection::handle(sf::Packet & p) {
        auto a = reinterpret_cast<unsigned char const *>(p.getData());
        auto b = a + p.getDataSize();
        std::cout << m_socket->getRemoteAddress() << ":" << m_socket->getRemotePort() << " send ";
        for (auto i = a; i != b; ++i) {
            std::cout << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(*i) << ' ';
        }
        std::cout << std::endl;
        auto err = m_socket->send(p);
        if (err != sf::Socket::Status::Done) {
            std::cout << m_socket->getRemoteAddress() << ":" << m_socket->getRemotePort() << " SEND FAILURE" << std::endl;;
        }
    }
    void connection::disconnect() {
        if (!is_disconnected()) {
            m_disconnected = true;
            m_receive->disconnect();
            m_manager->disconnect(this);
        }
    }
}
