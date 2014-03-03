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

#include "player.hpp"
#include "game.hpp"
#include "server.hpp"

#pragma warning(push, 1)
#include <map>
#include <set>
#include <sstream>
#include <iomanip>
#include <iostream>
#pragma warning(pop)

namespace nlp {
    player::player(uint32_t p_id, server & p_server, uv::stream p_stream) :
        m_id{p_id},
        m_name{default_name()},
        m_server{p_server},
        m_timer{uv::timer::create(m_server.loop())},
        m_stream{framed_stream::create(p_stream)} {
        p_stream.eof([this] {
            kill();
        });
        std::cout << io::time << m_name << " has joined" << std::endl;
        send_id();
        send_player_joined({});
        send_player_joined(this);
        send_game_created({});
        m_server.for_player([&](player & p_player) {
            p_player.send_player_joined(this);
        });
        m_timer.start([&] {
            send_ping();
        }, {}, {});
        m_stream.read([&](packet p_packet) {
            handle(std::move(p_packet));
        });
    }
    uint32_t player::get_id() const {
        return m_id;
    }
    std::string const & player::get_name() const {
        return m_name;
    }
    std::string player::default_name() const {
        std::ostringstream ss;
        ss << "Pony" << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << m_id;
        return ss.str();
    }
    void player::kill() {
        std::cout << io::time << m_name << " has left" << std::endl;
        m_server.destroy_player(m_id);
    }
    void player::handle(packet p_packet) {
        auto opcode = uint16_t{};
        p_packet >> opcode;
        switch (opcode) {
        case 0x0001: {
            uint32_t id{};
            p_packet >> id;
            send_pong(id);
        } break;
        case 0x0002: {
            auto id = uint32_t{};
            p_packet >> id;
            if (m_ping_id && id == m_ping_id) {
                m_ping_id = {0};
                m_timer.start([&] {
                    send_ping();
                }, std::chrono::seconds{30}, {});
            }
        } break;
        case 0x0003: {
            auto new_name = std::string{};
            p_packet >> new_name;
            if (new_name.length() > 20) {
                new_name.resize(20);
            } else if (new_name.empty()) {
                new_name = default_name();
            }
            std::cout << io::time << io::strip(m_name) << " renamed to " << io::strip(new_name) << std::endl;
            m_name.swap(new_name);
            m_server.for_player([&](player & p_player) {
                p_player.send_player_joined(this);
            });
        } break;
        case 0x0007: {
            if (m_game) {
                m_game->remove_player(this);
            }
            auto game_name = std::string{};
            p_packet >> game_name;
            m_server.create_game(game_name).add_player(this);
            std::cout << io::time << m_name << " created game " << game_name << std::endl;
        } break;
        case 0x0008: {
            if (m_game) {
                m_game->remove_player(this);
            }
            auto id = uint32_t{};
            p_packet >> id;
            if (auto g = m_server.get_game(id)) {
                g->add_player(this);
                std::cout << io::time << m_name << " joined game " << g->get_name() << std::endl;
            }
        } break;
        case 0x000A: {
            auto message = std::string{};
            p_packet >> message;
            std::cout << io::time << m_name << " globally says " << message << std::endl;
            m_server.for_player([&](player & p_player) {
                p_player.send_global_chat(this, message);
            });
        } break;
        case 0x0010: {
            auto message = std::string{};
            p_packet >> message;
            if (m_game) {
                std::cout << io::time << m_name << " game says " << message << std::endl;
                m_game->for_player([&](player & p_player) {
                    p_player.send_game_chat(this, message);
                });
            }
        } break;
        case 0x0012: {
            auto id = uint32_t{};
            p_packet >> id;
            auto message = std::string{};
            p_packet >> message;
            if (auto target = m_server.get_player(id)) {
                std::cout << io::time << m_name << " private to " << target->get_name() <<
                    " says " << message << std::endl;
                target->send_private_chat(this, message);
            } else {
                std::cout << io::time << id << " does not exist!" << std::endl;
            }
        } break;
        default: break;
        }
    }
    void player::start(uint16_t p_opcode) {
        m_packet = packet{};
        m_packet << p_opcode;
    }
    void player::send() {
        m_stream.write(std::move(m_packet));
    }
    void player::send_ping() {
        start(0x0001);
        auto dist = std::uniform_int_distribution<uint32_t>{1, std::numeric_limits<uint32_t>::max()};
        m_ping_id = dist(m_server.rng());
        m_packet << m_ping_id;
        m_timer.start([this] {
            kill();
        }, std::chrono::seconds{10}, {});
        send();
    }
    void player::send_pong(uint32_t p_id) {
        start(0x0002);
        m_packet << p_id;
        send();
    }
    void player::send_id() {
        start(0x0004);
        m_packet << m_id;
        send();
    }
    void player::send_game_deleted(ptr<game> p_game) {
        start(0x0005);
        if (p_game) {
            m_packet << uint32_t{1} << p_game->get_id();
        } else {
            m_packet << uint32_t{0};
        }
        send();
    }
    void player::send_game_created(ptr<game> p_game) {
        start(0x0006);
        if (p_game) {
            m_packet << uint32_t{1} << p_game->get_id() << p_game->get_name();
        } else {
            m_packet << m_server.total_games();
            m_server.for_game([&](game & p_game) {
                m_packet << p_game.get_id() << p_game.get_name();
            });
        }
        send();
    }
    void player::send_game_joined(ptr<game> p_game) {
        start(0x0009);
        if (p_game) {
            m_packet << p_game->get_id();
        } else {
            m_packet << uint32_t{0};
        }
        m_game = p_game;
        send();
    }
    void player::send_player_joined(ptr<player> p_player) {
        start(0x000B);
        if (p_player) {
            m_packet << uint32_t{1} << p_player->get_id() << p_player->get_name();
        } else {
            m_packet << m_server.total_players();
            m_server.for_player([&](player & p_player) {
                m_packet << p_player.get_id() << p_player.get_name();
            });
        }
        send();
    }
    void player::send_player_joined_game(ptr<player> p_player) {
        start(0x000C);
        if (p_player) {
            m_packet << uint32_t{1} << p_player->get_id();
        } else if (m_game) {
            m_packet << m_game->total_players();
            m_game->for_player([&](player & p_player) {
                m_packet << p_player.get_id();
            });
        } else {
            m_packet << uint32_t{0};
        }
        send();
    }
    void player::send_player_left_game(ptr<player> p_player) {
        start(0x000D);
        if (p_player) {
            m_packet << uint32_t{1} << p_player->get_id();
        } else {
            m_packet << uint32_t{0};
        }
        send();
    }
    void player::send_global_chat(ptr<player> p_player, std::string const & p_message) {
        start(0x000E);
        if (p_player) {
            m_packet << uint32_t{1} << p_player->get_id() << p_message;
        } else {
            m_packet << uint32_t{0};
        }
        send();
    }
    void player::send_player_left(ptr<player> p_player) {
        start(0x000F);
        if (p_player) {
            m_packet << uint32_t{1} << p_player->get_id();
        } else {
            m_packet << m_server.total_players();
            m_server.for_player([&](player & p_player) {
                m_packet << p_player.get_id();
            });
        }
        send();
    }
    void player::send_game_chat(ptr<player> p_player, std::string const & p_message) {
        start(0x0011);
        if (p_player) {
            m_packet << uint32_t{1} << p_player->get_id() << p_message;
        } else {
            m_packet << uint32_t{0};
        }
        send();
    }
    void player::send_private_chat(ptr<player> p_player, std::string const & p_message) {
        start(0x0013);
        if (p_player) {
            m_packet << uint32_t{1} << p_player->get_id() << p_message;
        } else {
            m_packet << uint32_t{0};
        }
        send();
    }
}
