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

#pragma warning(push, 1)
#include <SFML/Network/SocketSelector.hpp>
#include <iostream>
#pragma warning(pop)

namespace nlp {
    manager::manager() :
        m_select{std::make_unique<sf::SocketSelector>()} {}
    manager::~manager() {}
    void manager::add_listener(uint16_t p_port, std::function<ptr<packet_handler>(ptr<packet_handler>)> p_func) {
        try {
            auto listen = std::make_unique<listener>(p_port, std::move(p_func), this);
            m_select->add(listen->get_socket());
            m_listeners.emplace_back(std::move(listen));
        } catch (std::exception const &) {
        }
    }
    void manager::update() {
        if (m_select->wait(sf::seconds(1))) {
            for (auto & listen : m_listeners) {
                if (m_select->isReady(listen->get_socket())) {
                    for (;;) {
                        auto next = listen->get_next();
                        if (!next) {
                            break;
                        }
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
    }
    void manager::collect() {
        m_connections.remove_if([](auto & conn) {
            return conn->is_dead();
        });
    }
    void manager::kill(ptr<connection> p_conn) {
        m_select->remove(p_conn->get_socket());
    }
}
