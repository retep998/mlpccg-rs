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
#include <chrono>
#include <cmath>
#ifdef NLP_CLANG
    #include <boost/filesystem.hpp>
#else
    #include <filesystem>
#endif
#include <regex>
#include <functional>
#ifdef NLP_CLANG
namespace sys = boost::filesystem;
#else
namespace sys = std::tr2::sys;
#endif

namespace nlp {
    namespace ascii {
        using char8_t = unsigned char;
        using u8string = std::basic_string<char8_t>;
        struct color {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
        struct color_double {
            double r;
            double g;
            double b;
            color_double operator*(double const & p_mult) const {
                return{r * p_mult, g * p_mult, b * p_mult};
            }
            color_double operator+(color_double const & p_other) const {
                return{r + p_other.r, g + p_other.g, b + p_other.b};
            }
            color_double operator-(color_double const & p_other) const {
                return{r - p_other.r, g - p_other.g, b - p_other.b};
            }
            bool operator<(color_double const & p_other) const {
                return r == p_other.r ? g == p_other.g ? b < p_other.b : g < p_other.g : r < p_other.r;
            }
            color_double & operator+=(color_double const & p_other) {
                r += p_other.r;
                g += p_other.g;
                b += p_other.b;
                return *this;
            }
            double luminance() const {
                return 0.2126 * r + 0.7152 * g + 0.0722 * b;
            }
            color_double abs() const {
                return{std::abs(r), std::abs(g), std::abs(b)};
            }
        };
        struct entry {
            color_double col;
            double diff;
            char16_t ch;
            uint8_t fg;
            uint8_t bg;
        };
        struct coverage {
            unsigned cover;
            char16_t ch;
        };
        enum dithering {
            none,
            sierra,
            floydsteinberg,
            atkinson,
        };
        dithering dither{dithering::none};
        double const gamma_val{2.4};
        double const gamma_mult{255.};
        double const gamma_a{0.055};
        double const gamma_factor{12.92};
        double const gamma_decode_threshhold{0.04045};
        double const gamma_encode_threshhold{0.0031308};
        unsigned conwidth{};
        unsigned const cwidth{6};
        unsigned const cheight{8};
        double smoothness{};
        sys::path asset_path{};
        sys::path const chars_name{"chars" + std::to_string(cwidth) + "x" + std::to_string(cheight) + ".png"};
        std::array<color, 0x10> const colors{{{0x00, 0x00, 0x00}, {0x80, 0x00, 0x00}, {0x00, 0x80, 0x00}, {0x80, 0x80, 0x00}, {0x00, 0x00, 0x80}, {0x80, 0x00, 0x80}, {0x00, 0x80, 0x80}, {0xc0, 0xc0, 0xc0}, {0x80, 0x80, 0x80}, {0xff, 0x00, 0x00}, {0x00, 0xff, 0x00}, {0xff, 0xff, 0x00}, {0x00, 0x00, 0xff}, {0xff, 0x00, 0xff}, {0x00, 0xff, 0xff}, {0xff, 0xff, 0xff}}};
        std::array<char16_t, 222> const wchars{{0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f, 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7, 0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5, 0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9, 0x00ff, 0x00d6, 0x00dc, 0x00a2, 0x00a3, 0x00a5, 0x20a7, 0x0192, 0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba, 0x00bf, 0x2310, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb, 0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556, 0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510, 0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f, 0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567, 0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b, 0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580, 0x03b1, 0x00df, 0x0393, 0x03c0, 0x03a3, 0x03c3, 0x00b5, 0x03c4, 0x03a6, 0x0398, 0x03a9, 0x03b4, 0x221e, 0x03c6, 0x03b5, 0x2229, 0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248, 0x00b0, 0x2219, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x25a0}};
        std::vector<coverage> coverages{};
        std::array<std::array<std::array<uint16_t, 0x100>, 0x100>, 0x100> grid{};
        std::vector<entry> entries{};
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> codecvt{};
        void calc_coverage() {
            auto && in = sf::Image{};
            in.loadFromFile((asset_path / chars_name).string());
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
                if (std::none_of(coverages.cbegin(), coverages.cend(), [&sum](coverage const & c) {
                    return c.cover == sum;
                })) {
                    coverages.push_back({sum, c});
                }
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
        double gamma_decode(double p_val) {
            if (p_val <= gamma_decode_threshhold) {
                return p_val / gamma_mult / gamma_factor;
            } else {
                return std::pow((p_val / gamma_mult + gamma_a) / (1 + gamma_a), gamma_val);
            }
        }
        double gamma_encode(double p_val) {
            if (p_val <= gamma_encode_threshhold) {
                return std::min(255., std::max(0., p_val * gamma_factor * gamma_mult));
            } else {
                return std::min(255., std::max(0., ((1 + gamma_a) * std::pow(p_val, 1 / gamma_val) - gamma_a) * gamma_mult));
            }
        }
        color_double gamma(color const & p_color) {
            return{gamma_decode(p_color.r), gamma_decode(p_color.g), gamma_decode(p_color.b)};
        }
        color gamma(color_double const & p_color) {
            return{static_cast<uint8_t>(gamma_encode(p_color.r)), static_cast<uint8_t>(gamma_encode(p_color.g)), static_cast<uint8_t>(gamma_encode(p_color.b))};
        }
        color_double gamma(sf::Color const & p_color) {
            auto const mult = p_color.a / gamma_mult;
            return{gamma_decode(p_color.r * mult), gamma_decode(p_color.g * mult), gamma_decode(p_color.b * mult)};
        }
        void calc_combos() {//std::array<uint8_t, 16>{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
            auto const && fg_colors = std::array<uint8_t, 16>{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
            auto const && bg_colors = std::array<uint8_t, 16>{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};
            auto const total_mult = 1. / (cwidth * cheight);
            for (auto & c : coverages) {
                auto const ratio = c.cover * total_mult;
                auto const iratio = 1. - ratio;
                for (auto x : fg_colors) {
                    auto & fg = colors[x];
                    auto && fgg = gamma(fg);
                    auto const lf = fgg.luminance();
                    for (auto y : bg_colors) {
                        auto & bg = colors[y];
                        auto && bgg = gamma(bg);
                        auto const bf = bgg.luminance();
                        auto const diff = std::abs(lf - bf);
                        auto && combine = gamma(fg) * ratio + gamma(bg) * iratio;
                        entries.push_back({combine, diff, c.ch, x, y});
                    }
                }
            }
        }
        void prep() {
            calc_coverage();
            clear_grid();
            calc_combos();
        }
        entry & get_grid(color_double const & p_color) {
            auto && gc = gamma(p_color);
            auto & r = grid[gc.r][gc.g][gc.b];
            if (r == std::numeric_limits<uint16_t>::max()) {
                auto bestd = std::numeric_limits<double>::max();
                auto besti = uint16_t{};
                for (auto i = uint16_t{0}; i < entries.size(); ++i) {
                    auto & e = entries[i];
                    auto & opp = e.col;
                    auto diff = (p_color - opp).abs().luminance() + e.diff * smoothness;
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
            //auto && wstr = std::u16string{wchars.cbegin(), wchars.cend()};
            //auto && u8chars = codecvt.to_bytes(wstr);
            //auto && my_loop = loop{};
            //auto && my_tty = tty{my_loop};
            //my_tty << u8chars << '\n';
        }
        void display_results() {
            //auto && my_tty = tty{loop::get_default()};
            //auto && in = std::ifstream{asset_path / sys::path{"motd.txt"}, std::ios::binary};
            //auto && line = std::string{};
            //std::getline(in, line, '\0');
            //my_tty << line;
            //my_tty << tty::clear;
            //std::cin.get();
        }
        void convert(std::string p_name) {
            auto && in = sf::Image{};
            if (!in.loadFromFile(p_name)) {
                return;
            }
            auto && out = std::ofstream((asset_path / sys::path{"motd.txt"}).string(), std::ios::binary);
            auto imgwidth = in.getSize().x;
            auto imgheight = in.getSize().y;
            auto xratio = static_cast<double>(imgwidth) / conwidth;
            auto yratio = xratio * cheight / cwidth;
            auto conheight = static_cast<unsigned>(imgheight / yratio);
            auto shrunk = std::vector<color_double>{};
            auto const total_pixels = conwidth * conheight;
            shrunk.resize(total_pixels);
            for (auto y = unsigned{0}; y < conheight; ++y) {
                auto yt = static_cast<unsigned>(y * yratio), yb = static_cast<unsigned>((y + 1) * yratio);
                for (auto x = unsigned{0}; x < conwidth; ++x) {
                    auto xt = static_cast<unsigned>(x * xratio), xb = static_cast<unsigned>((x + 1) * xratio);
                    auto sum = color_double{};
                    for (auto yy = yt; yy < yb; ++yy) {
                        for (auto xx = xt; xx < xb; ++xx) {
                            auto && c = in.getPixel(xx, yy);
                            sum += gamma(c);
                        }
                    }
                    auto mult = 1. / ((xb - xt) * (yb - yt));
                    shrunk[y * conwidth + x] = sum * mult;
                }
            }
            auto adjust = [&total_pixels, &shrunk](unsigned const & x, unsigned const & y, color_double const & err, double const & mult) {
                auto const index = y * conwidth + x;
                if (index < total_pixels) {
                    shrunk[index] += err * mult;
                }
            };
            auto lfg = uint8_t{}, lbg = uint8_t{};
            for (auto y = unsigned{0}; y < conheight; ++y) {
                for (auto x = unsigned{0}; x < conwidth; ++x) {
                    auto & c = shrunk[y * conwidth + x];
                    auto & e = get_grid(c);
                    auto && err = c - e.col;
                    switch (dither) {
                    case dithering::floydsteinberg:{
                        adjust(x + 1, y + 0, err, 7. / 16.);
                        adjust(x - 1, y + 1, err, 3. / 16.);
                        adjust(x + 0, y + 1, err, 5. / 16.);
                        adjust(x + 1, y + 1, err, 1. / 16.);
                    } break;
                    case dithering::sierra:{
                        adjust(x + 1, y + 0, err, 5. / 32.);
                        adjust(x + 2, y + 0, err, 3. / 32.);
                        adjust(x + 2, y + 1, err, 2. / 32.);
                        adjust(x - 1, y + 1, err, 4. / 32.);
                        adjust(x - 0, y + 1, err, 5. / 32.);
                        adjust(x + 1, y + 1, err, 4. / 32.);
                        adjust(x + 2, y + 1, err, 2. / 32.);
                        adjust(x - 1, y + 2, err, 2. / 32.);
                        adjust(x + 0, y + 2, err, 3. / 32.);
                        adjust(x + 1, y + 2, err, 2. / 32.);
                    } break;
                    case dithering::atkinson:{
                        adjust(x + 1, y + 0, err, 1. / 8.);
                        adjust(x + 2, y + 0, err, 1. / 8.);
                        adjust(x - 1, y + 1, err, 1. / 8.);
                        adjust(x + 0, y + 1, err, 1. / 8.);
                        adjust(x + 1, y + 1, err, 1. / 8.);
                        adjust(x + 0, y + 2, err, 1. / 8.);
                    } break;
                    default:;
                    }
                    if (e.fg != lfg || e.bg != lbg) {
                        lfg = e.fg;
                        lbg = e.bg;
                        out << "\x1b[" << (e.fg & 0x8 ? "1" : "21");
                        out << ";" << (e.bg & 0x8 ? "5" : "25");
                        out << ";" << static_cast<int>((e.fg & 0x7) + 30);
                        out << ";" << static_cast<int>((e.bg & 0x7) + 40) << "m";
                    }
                    out << codecvt.to_bytes(e.ch);
                }
                out << '\n';
            }
            out << std::flush;
            display_results();
        }
        void load_config() {
            auto && file = std::ifstream((asset_path / sys::path{"ascii.cfg"}).string());
            auto && line = std::string{};
            auto && mapping = std::map<std::string, std::function<void(std::string const &)>>{};
            mapping["dithering"] = [](std::string const & str) {
                if (str == "floydsteinberg") {
                    dither = dithering::floydsteinberg;
                } else if (str == "sierra") {
                    dither = dithering::sierra;
                } else if (str == "atkinson") {
                    dither = dithering::atkinson;
                } else {
                    dither = dithering::none;
                }
            };
            mapping["smoothness"] = [](std::string const & str) {
                smoothness = std::stod(str);
            };
            mapping["width"] = [](std::string const & str) {
                conwidth = static_cast<unsigned>(std::stoul(str));
            };
            auto && regex = std::regex{"(.*)=(.*)", std::regex_constants::ECMAScript};
            while (std::getline(file, line)) {
                auto m = std::smatch{};
                std::regex_match(line, m, regex);
                auto & f = mapping[m[1]];
                if (f) {
                    f(m[2]);
                }
            }
        }
        void do_stuff(std::vector<std::string> p_args) {
            asset_path = sys::path{p_args[0]}.remove_filename() / sys::path{"assets"};
            load_config();
            if (p_args.size() > 1) {
                auto t1 = std::chrono::high_resolution_clock::now();
                nlp::ascii::prep();
                nlp::ascii::convert(p_args[1]);
                auto t2 = std::chrono::high_resolution_clock::now();
            } else {
                nlp::ascii::print();
            }
        }
    }
}

int main(int p_argc, char ** p_argv) {
    nlp::ascii::do_stuff({p_argv, p_argv + p_argc});
}
