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
#include "card.hpp"

#pragma warning(push, 1)
#include <vector>
#include <array>
#include <algorithm>
#pragma warning(pop)

namespace nlp {
    class deck {
    public:
        bool is_valid() const {
            return is_mane_valid() && is_problems_valid() && is_regulars_valid();
        }
        bool is_mane_valid() const {
            return m_mane && m_mane.is_mane();
        }
        bool is_problems_valid() const {
            return m_problems.size() == 10 && std::all_of(m_problems.cbegin(),m_problems.cend(),
                                                          [this](card const & c) {
                return c && c.is_problem() && std::count(m_problems.cbegin(), m_problems.cend(), c) <= 2;
            });
        }
        bool is_regulars_valid() const {
            return m_problems.size() >= 45 && std::all_of(m_problems.cbegin(), m_problems.cend(),
                                                          [this](card const & c) {
                return c && c.is_regular() && std::count(m_problems.cbegin(), m_problems.cend(), c) <= 3;
            });
        }
    private:
        card m_mane;
        std::vector<card> m_problems;
        std::vector<card> m_regulars;
    };
}
