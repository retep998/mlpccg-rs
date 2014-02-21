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
#include "connection.hpp"

#pragma warning(push, 1)
#include <SFML/Graphics.hpp>
#include <atomic>
#pragma warning(pop)

namespace nlp {
    class context : public connection {
    public:
        context() = delete;
        context(context const &) = delete;
        context(context &&) = delete;
        ~context() = default;
        context & operator=(context const &) = delete;
        context & operator=(context &&) = delete;
        class manager {
        public:
            manager() = default;
            manager(manager const &) = delete;
            manager(manager &&) = delete;
            ~manager();
            manager & operator=(manager const &) = delete;
            manager & operator=(manager &&) = delete;
            void update();
            void render(sf::RenderWindow &);
            std::unique_ptr<context> const & get() {
                return current;
            }
        private:
            std::unique_ptr<sf::TcpSocket> socket;
            std::unique_ptr<context> current;
            std::atomic_bool connected;
        };
        static void init() {
            connection::init();
        }
        context(std::unique_ptr<sf::TcpSocket> && ptr) : connection(std::move(ptr)) {}
    private:
        void render(sf::RenderWindow &) {

        }
    };
}
