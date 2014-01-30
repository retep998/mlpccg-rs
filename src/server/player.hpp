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
#include "recv_handler.hpp"
#include <utility/ptr.hpp>
#include <SFML/Network/Packet.hpp>
#include <chrono>

namespace nlp {
    class game;
    class send_handler;
    class player final : public recv_handler {
    public:
        player(ptr<send_handler> send);
        ~player();
        uint32_t get_id() const;
        static ptr<player> get(uint32_t);
        std::string const & get_name() const;
    private:
        void recv(sf::Packet &);
        void update();
        ptr<send_handler> send;
        std::chrono::steady_clock::time_point last_ping{std::chrono::steady_clock::now()};
        uint32_t last_ping_id{};
        sf::Packet packet;
        std::string nickname;
        uint32_t id;
        std::shared_ptr<game> current_game;
    };
}
