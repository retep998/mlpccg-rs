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
#include "send_handler.hpp"
#include <utility/ptr.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <memory>
#include <functional>

namespace nlp {
    class recv_handler;
    class send_handler;
    using recv_handler_creator = std::function<std::unique_ptr<recv_handler>(ptr<send_handler>)>;
    class connection final : private send_handler {
    public:
        connection() = delete;
        connection(connection const &) = delete;
        connection(recv_handler_creator const & func, std::unique_ptr<sf::TcpSocket> && sock);
        ~connection();
        connection & operator=(connection const &) = delete;
        sf::Socket & get_socket() const;
        bool update();
        bool recv_update();
    private:
        void send(sf::Packet &) override;
        void disconnect() override;
        std::unique_ptr<recv_handler> recv;
        std::unique_ptr<sf::TcpSocket> socket;
        sf::Packet packet;
        bool disconnected = false;
    };

}
