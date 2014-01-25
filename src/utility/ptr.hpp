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
#include <stdexcept>
#include <memory>

namespace nlp {
    //Non-owning pointer with null pointer checking
    template <typename T>
    class ptr {
    public:
        ptr() = default;
        ptr(ptr const &) = default;
        //ptr(ptr &&) = default;//Enable with VS 2014
        ptr & operator=(ptr const &) = default;
        //ptr & operator=(ptr &&) = default;//Enable with VS 2014
        ptr(std::unique_ptr<T> const & o) : m_ptr(o.get()) {}
        ptr(T & o) : m_ptr(&o) {}
        ptr(nullptr_t) : m_ptr(nullptr) {}
        explicit operator bool() const {
            return m_ptr != nullptr;
        }
        T * operator->() const {
            if (m_ptr == nullptr)
                throw std::runtime_error("Null pointer exception!");
            return m_ptr;
        }
        T & operator*() const {
            if (m_ptr == nullptr)
                throw std::runtime_error("Null pointer exception!");
            return *m_ptr;
        }
        bool operator==(ptr const & o) const {
            return m_ptr == o.m_ptr;
        }
    private:
        T * m_ptr = nullptr;
    };
}
