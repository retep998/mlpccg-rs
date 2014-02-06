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

#include "client.hpp"
#include "context.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <list>
#ifdef NLP_CLANG
    #include <boost/filesystem.hpp>
#else
    #include <filesystem>
#endif
#include <vector>
#include <algorithm>
#include <random>

#ifdef NLP_CLANG
namespace sys = boost::filesystem;
#else
namespace sys = std::tr2::sys;
#endif

namespace nlp {
    namespace client {
        void run() {
            std::vector<sys::path> paths;
            for (auto it = sys::directory_iterator("assets/backgrounds"); it != sys::directory_iterator(); ++it) {
                paths.push_back(it->path());
            }
            std::random_device rand;
            std::mt19937 engine(rand());
            std::shuffle(paths.begin(), paths.end(), engine);
            sf::Texture image;
            image.setSmooth(true);
            bool success = false;
            for (auto & p : paths) {
                if (image.loadFromFile(p.string())) {
                    success = true;
                    break;
                }
            }
            if (!success)
                exit(1);
            auto isize = image.getSize();
            auto iratio = 1.f * isize.y / isize.x;
            sf::Sprite sprite;
            sprite.setTexture(image);
            sprite.setOrigin(isize.x / 2.f, isize.y / 2.f);
            context::init();
            context::manager current;
            sf::RenderWindow window;
            window.create(sf::VideoMode(960, 540), "NoLifePony");
            while (window.isOpen()) {
                sf::Event e;
                while (window.pollEvent(e)) switch (e.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                default:;
                }
                auto wsize = window.getSize();
                sf::View view;
                view.reset(sf::FloatRect(0, 0, static_cast<float>(wsize.x), static_cast<float>(wsize.y)));
                auto wratio = 1.f * wsize.y / wsize.x;
                if (wratio > iratio) {
                    sprite.setScale(wratio / iratio * wsize.x / isize.x, 1.f * wsize.y / isize.y);
                } else {
                    sprite.setScale(1.f * wsize.x / isize.x, iratio / wratio * wsize.y / isize.y);
                }
                sprite.setPosition(wsize.x / 2.f, wsize.y / 2.f);
                window.setView(view);
                current.update();
                window.clear(sf::Color::Black);
                window.draw(sprite);
                current.render(window);
                window.display();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
}
