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

#include <utility/tty.hpp>
#include <utility/loop.hpp>
#include <SFML/Graphics/Image.hpp>
#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <codecvt>
#include <map>
#include <set>
#include <cmath>

namespace nlp {
    namespace ascii {
        using char8_t = unsigned char;
        using u8string = std::basic_string<char8_t>;
        struct color {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            bool operator==(color const & p_other) const {
                return r == p_other.r && g == p_other.g && b == p_other.b;
            }
            bool operator<(color const & p_other) const {
                return r == p_other.r ? g == p_other.g ? b < p_other.b : g < p_other.g : r < p_other.r;
            }
        };
        struct entry {
            color col;
            uint8_t fg;
            uint8_t bg;
            char16_t ch;
        };
        struct coverage {
            unsigned cover;
            char16_t ch;
        };
        unsigned conwidth{};
        unsigned const cwidth{6};
        unsigned const cheight{8};
        std::string const chars_name{"assets/chars" + std::to_string(cwidth) + "x" + std::to_string(cheight) + ".png"};
        std::array<color, 0x10> const colors{{{0x00, 0x00, 0x00}, {0x80, 0x00, 0x00}, {0x00, 0x80, 0x00}, {0x80, 0x80, 0x00}, {0x00, 0x00, 0x80}, {0x80, 0x00, 0x80}, {0x00, 0x80, 0x80}, {0xc0, 0xc0, 0xc0}, {0x80, 0x80, 0x80}, {0xff, 0x00, 0x00}, {0x00, 0xff, 0x00}, {0xff, 0xff, 0x00}, {0x00, 0x00, 0xff}, {0xff, 0x00, 0xff}, {0x00, 0xff, 0xff}, {0xff, 0xff, 0xff}}};
        std::array<char16_t, 222> const wchars{{0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f, 0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7, 0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5, 0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9, 0x00ff, 0x00d6, 0x00dc, 0x00a2, 0x00a3, 0x00a5, 0x20a7, 0x0192, 0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba, 0x00bf, 0x2310, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb, 0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556, 0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510, 0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f, 0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567, 0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b, 0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580, 0x03b1, 0x00df, 0x0393, 0x03c0, 0x03a3, 0x03c3, 0x00b5, 0x03c4, 0x03a6, 0x0398, 0x03a9, 0x03b4, 0x221e, 0x03c6, 0x03b5, 0x2229, 0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248, 0x00b0, 0x2219, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x25a0}};
        std::vector<coverage> coverages{};
        std::array<std::array<std::array<uint16_t, 0x100>, 0x100>, 0x100> grid{};
        std::vector<entry> entries{};
        std::set<color> taken_colors{};
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> codecvt{};
        void calc_coverage() {
            auto && in = sf::Image{};
            in.loadFromFile(chars_name);
            auto index = unsigned{0};
            for (auto & c : wchars) {
                auto sum = unsigned{0};
                for (auto x = unsigned{0}; x < cwidth; ++x) {
                    for (auto y = unsigned{0}; y < cheight; ++y) {
                        if (in.getPixel(index + x, y) != sf::Color::Black) {
                            ++sum;
                        }
                    }
                }
                coverages.push_back({sum, c});
                index += cwidth;
            }
        }
        void clear_grid() {
            for (auto & x : grid) {
                for (auto & y : x) {
                    y.fill(std::numeric_limits<uint16_t>::max());
                }
            }
        }
        double gamma_decode(uint8_t x) {
            return std::pow(x * (1. / 255.), 2.2);
        }
        uint8_t gamma_encode(double x) {
            return static_cast<uint8_t>(std::pow(x, 1. / 2.2) * 255.);
        }
        void calc_combos() {
            auto all_colors = std::initializer_list<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
            auto total_mult = 1. / (cwidth * cheight);
            for (auto & c : coverages) {
                auto ratio = c.cover * total_mult;
                auto iratio = 1. - ratio;
                for (auto & x : all_colors) {
                    auto & fg = colors[x];
                    for (auto & y : all_colors) {
                        auto & bg = colors[y];
                        auto && combine = color{gamma_encode(gamma_decode(fg.r) * ratio + gamma_decode(bg.r) * iratio), gamma_encode(gamma_decode(fg.g) * ratio + gamma_decode(bg.g) * iratio), gamma_encode(gamma_decode(fg.b) * ratio + gamma_decode(bg.b) * iratio)};
                        if (taken_colors.find(combine) == taken_colors.end()) {
                            entries.push_back({combine, x, y, c.ch});
                            taken_colors.insert(combine);
                        }
                    }
                }
            }
        }
        void prep() {
            calc_coverage();
            clear_grid();
            calc_combos();
        }
        entry & get_grid(color const & c) {
            auto & r = grid[c.r][c.g][c.b];
            if (r == std::numeric_limits<uint16_t>::max()) {
                auto bestd = std::numeric_limits<int>::max();
                auto besti = uint16_t{};
                for (auto i = uint16_t{0}; i < entries.size(); ++i) {
                    auto & e = entries[i];
                    auto & opp = e.col;
                    auto diff = std::abs(c.r - opp.r) + std::abs(c.g - opp.g) + std::abs(c.b - opp.b);
                    if (diff < bestd) {
                        bestd = diff;
                        besti = i;
                    }
                }
                r = besti;
            }
            return entries[r];
        }
        void print() {
            auto && wstr = std::u16string{wchars.cbegin(), wchars.cend()};
            auto && u8chars = codecvt.to_bytes(wstr);
            auto && my_loop = loop{};
            auto && my_tty = tty{my_loop};
            my_tty << u8chars << '\n';
        }
        void convert(std::string name) {
            auto && in = sf::Image{};
            if (!in.loadFromFile(name)) {
                return;
            }
            auto && out = std::ofstream("assets/motd.txt", std::ios::binary);
            auto imgwidth = in.getSize().x;
            auto imgheight = in.getSize().y;
            auto xratio = static_cast<double>(imgwidth) / conwidth;
            auto yratio = xratio * cheight / cwidth;
            auto conheight = static_cast<unsigned>(imgheight / yratio);
            for (auto y = unsigned{0}; y < conheight; ++y) {
                auto yt = static_cast<unsigned>(y * yratio), yb = static_cast<unsigned>((y + 1) * yratio);
                for (auto x = unsigned{0}; x < conwidth; ++x) {
                    auto xt = static_cast<unsigned>(x * xratio), xb = static_cast<unsigned>((x + 1) * xratio);
                    auto r = 0., g = 0., b = 0.;
                    for (auto yy = yt; yy < yb; ++yy) {
                        for (auto xx = xt; xx < xb; ++xx) {
                            auto && c = in.getPixel(xx, yy);
                            r += gamma_decode(c.r) * c.a;
                            g += gamma_decode(c.g) * c.a;
                            b += gamma_decode(c.b) * c.a;
                        }
                    }
                    auto mult = 1. / ((xb - xt) * (yb - yt) * 255);
                    auto && combine = color{gamma_encode(r * mult), gamma_encode(g * mult), gamma_encode(b * mult)};
                    auto & e = get_grid(combine);
                    out << "\x1b[" << (e.fg & 0x8 ? "1" : "21");
                    out << ";" << (e.bg & 0x8 ? "5" : "25");
                    out << ";" << static_cast<int>((e.fg & 0x7) + 30);
                    out << ";" << static_cast<int>((e.bg & 0x7) + 40);
                    out << "m" << codecvt.to_bytes(e.ch);
                }
                out << '\n';
            }
        }
        void debug() {
            auto && out = std::ofstream("assets/motd.txt", std::ios::binary);
            auto && convert = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{};
            for (auto i = unsigned{0}; i < 0x100; ++i) {
                auto && combine = color{static_cast<uint8_t>(i), static_cast<uint8_t>(i), static_cast<uint8_t>(i)};
                auto & e = get_grid(combine);
                out << "\x1b[" << (e.fg & 0x8 ? "1" : "21");
                out << ";" << (e.bg & 0x8 ? "5" : "25");
                out << ";" << static_cast<int>((e.fg & 0x7) + 30);
                out << ";" << static_cast<int>((e.bg & 0x7) + 40);
                out << "m" << codecvt.to_bytes(e.ch);
            }
        }
    }
}

int main(int argc, char ** argv) {
    auto args = std::vector<std::string>{argv, argv + argc};
    nlp::ascii::prep();
    nlp::ascii::debug();
    if (args.size() > 1) {
        std::cin >> nlp::ascii::conwidth;
        nlp::ascii::convert(args[1]);
    }
    std::cout << "Press enter to continue" << std::endl;
    std::cin.get();
}
