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

#include "manager.hpp"
#include "connection.hpp"
#include "listener.hpp"
#include <utility/ptr.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <list>
#include <iostream>
#include <chrono>

namespace nlp {
    manager::manager() :
        m_select{std::make_unique<sf::SocketSelector>()} {}
    manager::~manager() {}
    void manager::add_listener(uint16_t p_port, std::function<std::unique_ptr<recv_handler>(ptr<send_handler>)> && p_func) {
        if (auto listen = listener::create(p_port, std::move(p_func))) {
            m_select->add(listen->get_socket());
            m_listeners.emplace_back(std::move(listen));
        }
    }
    void manager::update() {
        if (m_select->wait(sf::seconds(1))) {
            for (auto & listen : m_listeners) {
                if (m_select->isReady(listen->get_socket())) {
                    for (auto && next : *listen) {
                        m_select->add(next->get_socket());
                        m_connections.emplace_back(std::move(next));
                    }
                }
            }
            for (auto & c : m_connections) {
                if (m_select->isReady(c->get_socket())) {
                    c->update();
                }
            }
        }
        auto now = std::chrono::steady_clock::now();
        if (now - m_last_update > std::chrono::seconds{5}) {
            m_last_update = now;
            for (auto & c : m_connections) {
                c->recv_update();
            }
            for (auto & c : m_connections) {
                if (c->is_disconnected()) {
                    m_select->remove(c->get_socket());
                }
            }
            m_connections.remove_if([](std::unique_ptr<connection> const & c) {
                return c->is_disconnected();
            });
        }
    }
}
