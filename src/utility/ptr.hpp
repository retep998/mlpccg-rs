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
    class ptr {
    public:
        ptr() = default;
        ptr(ptr const &) = default;
        ptr(std::unique_ptr<T> const & o) : m_ptr{o.get()} {}
        ptr(T & o) : m_ptr{&o} {}
        ptr(nullptr_t) {}
        ptr(T * o) : m_ptr{o} {}
        template <typename U>
        ptr(ptr<U> const & o) : m_ptr(o.get()) {}
        ptr & operator=(ptr const &) = default;
        explicit operator bool() const {
            return m_ptr != nullptr;
        }
        T * operator->() const {
            return m_ptr;
        }
        T & operator*() const {
            return *m_ptr;
        }
        bool operator==(ptr const & o) const {
            return m_ptr == o.m_ptr;
        }
        bool operator!=(ptr const & o) const {
            return m_ptr != o.m_ptr;
        }
        T * get() const {
            return m_ptr;
        }
    private:
        T * m_ptr{nullptr};
    };
}
