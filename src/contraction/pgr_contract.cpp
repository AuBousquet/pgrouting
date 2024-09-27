/*PGR-GNU*****************************************************************
File: pgr_contract.cpp

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2016 Rohith Reddy
Mail:

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 ********************************************************************PGR-GNU*/

#include "contraction/pgr_contract.hpp"

namespace pgrouting {
namespace contraction {

/*! 
  @brief Tests if the required contraction method is valid
  @result Boolean, true if valid, false otherwise
*/
bool is_valid_contraction(int number) {    
    switch (number) {
        case 1: // dead end
        case 2: // linear
        case 3: // hierarchy
            return true;
            break;
        default:
            return false;
            break;
    }
}

}  // namespace contraction
}  // namespace pgrouting
