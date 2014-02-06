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
#include <memory>

namespace nlp {
    //Non-owning pointer with null pointer checking
    template <typename T>
    class ptr final {
    public:
        ptr() = default;
        ptr(ptr const &) = default;
        ptr(ptr &&) = default;
        ptr(std::unique_ptr<T> const & p_o) : m_ptr{p_o.get()} {}
        ptr(T & p_o) : m_ptr{&p_o} {}
        ptr(std::nullptr_t) {}
        ptr(T * p_o) : m_ptr{p_o} {}
        template <typename U>
        ptr(ptr<U> const & p_o) : m_ptr(p_o.get()) {}
        ptr & operator=(ptr const &) = default;
        ptr & operator=(ptr &&) = default;
        bool operator==(ptr const & p_o) const {
            return m_ptr == p_o.m_ptr;
        }
        bool operator!=(ptr const & p_o) const {
            return m_ptr != p_o.m_ptr;
        }
        bool operator<(ptr const & p_o) const {
            return m_ptr < p_o.m_ptr;
        }
        bool operator>(ptr const & p_o) const {
            return m_ptr > p_o.m_ptr;
        }
        bool operator<=(ptr const & p_o) const {
            return m_ptr <= p_o.m_ptr;
        }
        bool operator>=(ptr const & p_o) const {
            return m_ptr >= p_o.m_ptr;
        }
        explicit operator bool() const {
            return m_ptr != nullptr;
        }
        T * operator->() const {
            return m_ptr;
        }
        T & operator*() const {
            return *m_ptr;
        }
        T * get() const {
            return m_ptr;
        }
    private:
        T * m_ptr{nullptr};
    };
}
