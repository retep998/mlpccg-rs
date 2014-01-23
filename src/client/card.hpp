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
#include "ptr.hpp"

namespace nlp {
    class card {
    public:
        enum class type {
            mane,
            pfriend,
            event,
            problem,
            resource,
            troublemaker
        };
    private:
        class data {
        public:
            virtual type get_type() const = 0;
        private:
        };
        class data_mane final : public data {
        public:
            type get_type() const override {
                return type::mane;
            }
        private:
        };
    public:
        card() = default;
        card(card const &) = default;
        card & operator=(card const &) = default;
        explicit operator bool() const {
            return !!m_data;
        }
        bool operator==(card const & o) const {
            return m_data == o.m_data;
        }
        type get_type() const {
            return m_data->get_type();
        }
    private:
        ptr<data> m_data = nullptr;
    };
}
