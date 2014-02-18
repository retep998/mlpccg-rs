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

#include "ip_impl.hpp"
#include "check.hpp"

namespace nlp {
    namespace uv {
        //ip
        ip::impl * ip::operator->() const {
            return m_impl.get();
        }
        ip ip::create(std::string const & p_ip, uint16_t p_port) {
            auto && a = ip{};
            a.m_impl = std::shared_ptr<impl>{new impl{p_ip, p_port}};
            return a;
        }
        //ip::impl
        sockaddr const & ip::impl::get() const {
            return reinterpret_cast<sockaddr const &>(m_addr);
        }
        ip::impl::impl(std::string const & p_ip, uint16_t p_port) {
            check(uv_ip4_addr(p_ip.c_str(), p_port, &m_addr));
        }
    }
}
