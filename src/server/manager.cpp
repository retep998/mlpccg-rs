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
#include <utility/ptr.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <list>
#include <stdexcept>
#include <iostream>

namespace nlp {
    class connection {
    public:
        connection() = delete;
        connection(connection const &) = delete;
        connection & operator=(connection const &) = delete;
    private:
    };
    class listener {
    public:
        class iterator {
        public:
            iterator() = default;
            iterator(iterator const &) = default;
            iterator(ptr<listener> listen) : listen(listen) {}
            iterator & operator=(iterator const &) = default;
        private:
            ptr<listener> listen;
        };
        listener() = delete;
        listener(listener const &) = delete;
        listener(std::unique_ptr<sf::TcpListener> && listen, packet_handler_creator && func) : listen(std::move(listen)), func(std::move(func)), socket(std::make_unique<sf::TcpSocket>()) {}
        listener & operator=(listener const &) = delete;
        static std::unique_ptr<listener> create(uint16_t port, packet_handler_creator && func) {
            auto listen = std::make_unique<sf::TcpListener>();
            auto err = listen->listen(port);
            if (err != sf::Socket::Status::Done) {
                std::cerr << "Failed to listen to port " << port << std::endl;
                return nullptr;
            }
            listen->setBlocking(false);
            return std::make_unique<listener>(std::move(listen), std::move(func));
        }
        iterator begin() {
            return{this};
        }
        iterator end() {
            return{};
        }
        sf::Socket & get_socket() {
            return *listen;
        }
    protected:
        void get_next() {
            auto err = listen->accept(*socket);
            if (err == sf::Socket::Status::Done) {
                return;
            }
            return;
        }
    private:
        std::unique_ptr<sf::TcpListener> listen;
        std::unique_ptr<sf::TcpSocket> socket;
        packet_handler_creator func;
    };
    class manager::impl {
    public:
        void add_listener(uint16_t port, packet_handler_creator && func) {
            if (auto listen = listener::create(port, std::move(func))) {
                listen_select.add(listen->get_socket());
                listeners.emplace_back(std::move(listen));
            }
            float a = 5, b = 6;
            auto c = a * b;
        }
        void update() {
            for (auto & listen : listeners) {

            }
        }
    private:
        std::vector<std::unique_ptr<listener>> listeners;
        sf::SocketSelector listen_select;
    };
    manager::manager() : m_data(std::make_unique<impl>()) {}
    void manager::add_listener(uint16_t port, packet_handler_creator && func) {
        m_data->add_listener(port, std::move(func));
    }
    void manager::update() {
        m_data->update();
    }
}