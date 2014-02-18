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
#include "packet_handler.hpp"
#include <utility/ptr.hpp>

#pragma warning(push, 1)
#include <memory>
#include <functional>
#pragma warning(pop)

namespace sf {
    class TcpSocket;
    class Packet;
    class Socket;
}
namespace nlp {
    class manager;
    class connection final : private packet_handler {
    public:
        connection() = delete;
        connection(connection const &) = delete;
        connection(std::function<ptr<packet_handler>(ptr<packet_handler>)>, std::unique_ptr<sf::TcpSocket>,
                   ptr<manager>);
        ~connection();
        connection & operator=(connection const &) = delete;
        sf::Socket & get_socket() const;
        void update();
        bool is_dead() const;
    private:
        void handle(sf::Packet &) override;
        void kill() override;
        ptr<packet_handler> m_receive;
        std::unique_ptr<sf::TcpSocket> m_socket;
        ptr<manager> m_manager;
        bool m_dead = false;
    };

}
