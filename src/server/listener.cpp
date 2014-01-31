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
#include <utility/format.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <iostream>

namespace nlp {
    listener::listener(uint16_t p_port, std::function<ptr<packet_handler>(ptr<packet_handler>)> p_func, ptr<manager> p_manager) :
        m_listen{std::make_unique<sf::TcpListener>()},
        m_func{std::move(p_func)},
        m_manager{p_manager} {
        auto err = m_listen->listen(p_port);
        if (err != sf::Socket::Status::Done) {
            throw std::runtime_error{"Failed to listen to port " + std::to_string(p_port)};
        }
        std::cout << time() << "Listening on port " << p_port << "." << std::endl;
        m_listen->setBlocking(false);
    }
    listener::~listener() {}
    sf::Socket & listener::get_socket() const {
        return *m_listen;
    }
    std::unique_ptr<connection> listener::get_next() {
        if (!m_socket) {
            m_socket = std::make_unique<sf::TcpSocket>();
        }
        auto err = m_listen->accept(*m_socket);
        if (err != sf::Socket::Status::Done) {
            return nullptr;
        }
        auto next = std::make_unique<connection>(m_func, std::move(m_socket), m_manager);
        return next;
    }
}
