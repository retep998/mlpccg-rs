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
#include <utility/ptr.hpp>

#pragma warning(push, 1)
#include <memory>
#include <functional>
#pragma warning(pop)

namespace sf {
    class TcpListener;
    class TcpSocket;
    class Socket;
}
namespace nlp {
    class connection;
    class packet_handler;
    class manager;
    class listener final {
    public:
        listener() = delete;
        listener(listener const &) = delete;
        listener(uint16_t, std::function<ptr<packet_handler>(ptr<packet_handler>)>, ptr<manager>);
        ~listener();
        listener & operator=(listener const &) = delete;
        sf::Socket & get_socket() const;
        std::unique_ptr<connection> get_next();
    private:
        std::unique_ptr<sf::TcpListener> m_listen;
        std::unique_ptr<sf::TcpSocket> m_socket;
        std::function<ptr<packet_handler>(ptr<packet_handler>)> m_func;
        ptr<manager> m_manager;
    };
}
