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
#include <utility/ptr.hpp>

namespace nlp {
    class card {
    public:
        class data;
        enum class type {
            mane,
            pfriend,
            event,
            problem,
            resource,
            troublemaker
        };
        card() = default;
        card(card const &) = default;
        //card(card &&) = default;
        card & operator=(card const &) = default;
        //card & operator=(card &&) = default;
        explicit operator bool() const;
        bool operator==(card const & o) const;
        type get_type() const;
        bool is_mane() const;
        bool is_problem() const;
        //Returns whether the card is a regular card, aka a card that goes directly in the deck, and not separately as a problem or mane.
        bool is_regular() const;
    private:
        ptr<data> m_data;
    };
}
