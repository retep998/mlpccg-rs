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
#include "packet_handler.hpp"
#include <utility/ptr.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <list>
#include <stdexcept>
#include <iostream>
#include <chrono>

namespace nlp {
    class connection final : private packet_handler {
    public:
        connection() = delete;
        connection(connection const &) = delete;
        connection(recv_handler_creator const & func, std::unique_ptr<sf::TcpSocket> && socket) : socket(std::move(socket)) {
            recv = func(this);
            socket->setBlocking(false);
        }
        connection & operator=(connection const &) = delete;
        void handle(sf::Packet &) override {

        }
        sf::Socket & get_socket() {
            return *socket;
        }
        void update() {

        }
    private:
        std::unique_ptr<packet_handler> recv;
        std::unique_ptr<sf::TcpSocket> socket;
    };
    class listener final {
    public:
        class iterator final {
        public:
            iterator() = delete;
            iterator(iterator const &) = delete;
            iterator(iterator && o) : listen(o.listen), next(std::move(o.next)) {}
            iterator(ptr<listener> listen, bool pop) : listen(listen) {
                if (pop) {
                    next = listen->get_next();
                }
            }
            iterator & operator=(iterator const &) = delete;
            bool operator!=(iterator const & o) const {
                return next != o.next;
            }
            iterator & operator++() {
                next = listen->get_next();
                return *this;
            }
            std::unique_ptr<connection> operator*() {
                return std::move(next);
            }
        private:
            ptr<listener> listen;
            std::unique_ptr<connection> next;
        };
        listener() = delete;
        listener(listener const &) = delete;
        listener(std::unique_ptr<sf::TcpListener> && listen, recv_handler_creator && func) : listen(std::move(listen)), func(std::move(func)) {}
        listener & operator=(listener const &) = delete;
        static std::unique_ptr<listener> create(uint16_t port, recv_handler_creator && func) {
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
        iterator begin() {
            return{this, true};
        }
        iterator end() {
            return{this, false};
        }
        sf::Socket & get_socket() {
            return *listen;
        }
    protected:
        std::unique_ptr<connection> get_next() {
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
    private:
        std::unique_ptr<sf::TcpListener> listen;
        std::unique_ptr<sf::TcpSocket> socket;
        recv_handler_creator func;
    };
    class manager::impl final {
    public:
        impl() = default;
        impl(impl const &) = delete;
        impl & operator=(impl const &) = delete;
        void add_listener(uint16_t port, recv_handler_creator && func) {
            if (auto listen = listener::create(port, std::move(func))) {
                select.add(listen->get_socket());
                listeners.emplace_back(std::move(listen));
            }
        }
        void update() {
            if (select.wait(sf::seconds(1))) {
                for (auto & listen : listeners) {
                    if (select.isReady(listen->get_socket())) {
                        for (auto && next : *listen) {
                            select.add(next->get_socket());
                            connections.emplace_back(std::move(next));
                        }
                    }
                }
                for (auto & connect : connections) {
                    if (select.isReady(connect->get_socket())) {
                        connect->update();
                    }
                }
            }
        }
    private:
        std::vector<std::unique_ptr<listener>> listeners;
        std::vector<std::unique_ptr<connection>> connections;
        sf::SocketSelector select;
        std::chrono::steady_clock::time_point last_clear = std::chrono::steady_clock::now();
    };
    manager::manager() : m_data(std::make_unique<impl>()) {}
    manager::~manager() {}
    void manager::add_listener(uint16_t port, recv_handler_creator && func) {
        m_data->add_listener(port, std::move(func));
    }
    void manager::update() {
        m_data->update();
    }
}