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

namespace nlp {
    class card {
        enum class type {
            mane,
            pfriend,
            event,

        };
        class data {
        public:
            virtual type get_type() = 0;
        private:
        };
        class data_mane final : public data {
        public:
            type get_type() override {
                return type::mane;
            }
        private:
        };
    public:
        card() = default;
        card(card const &) = default;
        card & operator=(card const &) = default;
        explicit operator bool() {
            return m_data != nullptr;
        }
        type get_type() {
            if (!m_data)
                throw std::runtime_error("Null pointer exception!");
        }
    private:
        data * m_data = nullptr;
    };
}
