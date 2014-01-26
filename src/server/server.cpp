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

#include "server.hpp"
#include "player.hpp"
#include <SFML/Network.hpp>
#include <vector>
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <chrono>
#include <list>
#include <algorithm>

namespace nlp {
    namespace server {
        std::list<player> players;
        void run() {
            std::cout << "NoLifePony server." << std::endl;
            player::init();
            sf::TcpListener listener;
            listener.listen(273);
            listener.setBlocking(false);
            auto next_socket = std::make_unique<sf::TcpSocket>();
            auto last_update = std::chrono::steady_clock::now();
            for (;;) {
                if (listener.accept(*next_socket) == sf::Socket::Done) {
                    players.emplace_back(std::move(next_socket));
                    next_socket = std::make_unique<sf::TcpSocket>();
                }
                for (auto && p : players)
                    p.update();
                players.remove_if([](player & p) {
                    if (p.is_disconnected()) {
                        return true;
                    }
                    return false;
                });
                auto now = std::chrono::steady_clock::now();
                if (now - last_update > std::chrono::seconds(5)) {
                    last_update = now;
                    std::cout << "================Update================" << std::endl;
                    std::cout << "Total: " << players.size() << std::endl;
                    std::map<std::string, size_t> counts;
                    for (auto & p : players) {
                        ++counts[p.get_address()];
                    }
                    for (auto & c : counts) {
                        std::cout << c.first << ": " << c.second << std::endl;
                    }
                    connection::print_counts();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
}
