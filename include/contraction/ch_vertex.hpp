/*PGR-GNU*****************************************************************
File: ch_vertex.hpp

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developers:
Copyright (c) 2016 Rohith Reddy
Mail:
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
#ifndef INCLUDE_CONTRACTION_CH_VERTEX_HPP_
#define INCLUDE_CONTRACTION_CH_VERTEX_HPP_

#include <iostream>
#include <sstream>
#include <vector>

#include "cpp_common/edge_t.hpp"
#include "cpp_common/identifiers.hpp"

namespace pgrouting {
class CH_vertex {
 private:
    // Attributes
    Identifiers<int64_t> contracted_vertices;
  
 public:
    int64_t id;
    int64_t vertex_order;
    int64_t metric;

    // Constructors
    CH_vertex();
    CH_vertex(const CH_vertex &) = default;
    CH_vertex(const Edge_t &other, bool is_source):
        id(is_source ? other.source : other.target) {}

    // Accessors
    void set_contracted_vertices(Identifiers<int64_t>&);
    void set_id(int64_t);
    void set_metric(int64_t);
    void set_vertex_order(int64_t);

    int64_t get_id();
    double get_metric();
    int64_t get_vertex_order();
    const Identifiers<int64_t>& get_contracted_vertices() const;
    Identifiers<int64_t>& get_contracted_vertices();
    
    void cp_members(const CH_vertex &other);

    // Other member functions
    void add_contracted_vertex(const CH_vertex&);
    void add_contracted_vertex_id(int64_t vid);
    void add_contracted_vertices_id(const Identifiers<int64_t>&);
    bool has_contracted_vertices() const;
    void clear_contracted_vertices();

    friend std::ostream& operator << (std::ostream&, const CH_vertex&);
};

size_t
check_vertices(std::vector<CH_vertex> vertices);
}  // namespace pgrouting

#endif  // INCLUDE_CONTRACTION_CH_VERTEX_HPP_
