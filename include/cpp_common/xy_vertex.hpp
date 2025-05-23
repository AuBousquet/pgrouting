/*PGR-GNU*****************************************************************
File: xy_vertex.hpp

Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

 Copyright (c) 2015 Celia Virginia Vergara Castillo
 vicky_vergara@hotmail.com

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

#ifndef INCLUDE_CPP_COMMON_XY_VERTEX_HPP_
#define INCLUDE_CPP_COMMON_XY_VERTEX_HPP_
#pragma once


#include <vector>
#include <cstdint>

#include "cpp_common/bpoint.hpp"
#include "cpp_common/edge_xy_t.hpp"


namespace pgrouting {

class XY_vertex {
 public:
  XY_vertex() = default;
  XY_vertex(const int64_t _id, double _x, double _y) :
      id(_id), point(_x, _y) {
      }

  XY_vertex(const Edge_xy_t &other, bool is_source) :
      id(is_source? other.source : other.target),
      point(is_source? Bpoint(other.x1, other.y1) : Bpoint(other.x2, other.y2))
      {}


  double x() const {return point.x();}
  double y() const {return point.y();}

  inline void cp_members(const XY_vertex &other) {
      this->id = other.id;
      this->point = other.point;
  }

  friend std::ostream& operator<<(std::ostream& log, const XY_vertex &v);
  bool operator==(const XY_vertex &rhs) const;

 public:
  int64_t id;
  Bpoint point;
};

size_t
check_vertices(std::vector < XY_vertex > vertices);

std::vector < XY_vertex >
extract_vertices(
    const Edge_xy_t *data_edges, size_t count);

std::vector < XY_vertex >
extract_vertices(
    const std::vector <Edge_xy_t > &data_edges);


}  // namespace pgrouting

#endif  // INCLUDE_CPP_COMMON_XY_VERTEX_HPP_
