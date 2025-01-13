/*PGR-GNU*****************************************************************
File: edge_xy_t.hpp

Copyright (c) 2017 pgRouting developers
Mail: project@pgrouting.org

Oslandia - Aurélie Bousquet - 2024
Mail: aurelie.bousquet@oslandia.com / contact@oslandia.com

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
/*! @file */

#ifndef INCLUDE_CPP_COMMON_ORDERED_VERTEX_T_HPP_
#define INCLUDE_CPP_COMMON_ORDERED_VERTEX_T_HPP_
#pragma once

#include <cstdint>

struct OrderedVertex_t {
    int64_t id;
    int64_t order;
};

#endif  // INCLUDE_CPP_COMMON_ORDERED_VERTEX_T_HPP_
