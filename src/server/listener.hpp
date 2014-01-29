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

#pragma once
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <utility/ptr.hpp>
#include <memory>
#include <functional>

namespace nlp {
    class connection;
    class recv_handler;
    class send_handler;
    using recv_handler_creator = std::function<std::unique_ptr<recv_handler>(ptr<send_handler>)>;
    class listener final {
    public:
        class iterator final {
        public:
            iterator() = delete;
            iterator(iterator const &) = delete;
            iterator(iterator && o);
            iterator(ptr<listener> listen, bool pop);
            ~iterator();
            iterator & operator=(iterator const &) = delete;
            bool operator!=(iterator const & o) const;
            iterator & operator++();
            std::unique_ptr<connection> operator*();
        private:
            ptr<listener> listen;
            std::unique_ptr<connection> next;
        };
        listener() = delete;
        listener(listener const &) = delete;
        listener(std::unique_ptr<sf::TcpListener> && listen, recv_handler_creator && func);
        ~listener();
        listener & operator=(listener const &) = delete;
        static std::unique_ptr<listener> create(uint16_t port, recv_handler_creator && func);
        iterator begin();
        iterator end();
        sf::Socket & get_socket() const;
    protected:
        std::unique_ptr<connection> get_next();
    private:
        std::unique_ptr<sf::TcpListener> listen;
        std::unique_ptr<sf::TcpSocket> socket;
        recv_handler_creator func;
    };
}
