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

#include "experiment.hpp"
#include <utility/loop.hpp>
#include <utility/tty.hpp>
#include <utility/tcp.hpp>
#include <utility/ip.hpp>
#include <utility/packet.hpp>
#include <utility/framed_stream.hpp>
#include <utility/timer.hpp>

#pragma warning(push, 1)
#include <cstdint>
#include <vector>
#include <fstream>
#include <sstream>
#include <list>
#include <iomanip>
#include <algorithm>
#pragma warning(pop)

namespace nlp {
    void experiment() {
        auto connections = std::list<framed_stream>{};
        auto loop = uv::loop::create();
        auto out = uv::tty::create(loop);
        auto in = std::ifstream{"assets/motd.txt", std::ios::binary};
        auto line = std::string{};
        auto listener = uv::tcp::create(loop);
        std::getline(in, line, '\0');
        out.write(line);
        out.write("\x1b[0m");
        listener.bind(uv::ip::create("0.0.0.0", 273));
        listener.listen([&](uv::stream p_stream) {
            auto && frame = framed_stream::create(p_stream);
            connections.emplace_back(frame);
            auto it = --connections.cend();
#pragma warning(push)
#pragma warning(disable: 5026 5027)
            p_stream.eof([&, it] {
                connections.erase(it);
                out.write("Disconnected!\n");
            });
#pragma warning(pop)
            frame.read([&out](packet p_packet) {
                out.write("Got packet!\n");
                auto ss = std::ostringstream{};
                ss << std::hex << std::setfill('0') << std::uppercase;
                for (auto c : p_packet.get()) {
                    ss << std::setw(2) << static_cast<unsigned>(static_cast<unsigned char>(c)) << ' ';
                }
                ss << '\n';
                out.write(ss.str());
            });
            out.write("Got connection!\n");
        }, 0x100);
        loop.run();
        return;
    }
}
