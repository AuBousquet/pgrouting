/*PGR-GNU*****************************************************************
File: ch_vertex.cpp

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
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

#include "contraction/ch_vertex.hpp"

#include <algorithm>
#include <vector>

namespace pgrouting {

// Constructors
CH_vertex::CH_vertex() {
    vertex_order = -1;
    metric = -1;
}

// Accessors
void CH_vertex::set_id(int64_t vid) {
    id = vid;
}

void CH_vertex::set_metric(int64_t m) {
    metric = m;
}

void CH_vertex::set_vertex_order(int64_t order) {
    vertex_order = order;
}

void CH_vertex::set_contracted_vertices(Identifiers<int64_t>& contracted_vertices_ids) {
    contracted_vertices = contracted_vertices_ids;
}

int64_t CH_vertex::get_id() {
    return id;
}

double CH_vertex::get_metric() {
    return metric;
}

int64_t CH_vertex::get_vertex_order() {
    return vertex_order;
}

const Identifiers<int64_t>& CH_vertex::get_contracted_vertices() const {
    return contracted_vertices;
}

Identifiers<int64_t>& CH_vertex::get_contracted_vertices() {
    return contracted_vertices;
}

/*!
    @brief copies members 
    @param [in] other CH_vertex used to copy the member values from
*/
void CH_vertex::cp_members(const CH_vertex &other) {
    this->id = other.id;
    this->contracted_vertices = other.get_contracted_vertices();
}

// Other member functions
bool CH_vertex::has_contracted_vertices() const {
    if (contracted_vertices.size() == 0)
        return false;
    return true;
}

/*!
    @brief adds an ID of another vertex, contracted in this one
    @param [in] vid vertex ID
*/
void CH_vertex::add_contracted_vertex(const CH_vertex& v) {
    contracted_vertices += v.id;
    contracted_vertices += v.get_contracted_vertices();
}

void CH_vertex::add_contracted_vertex_id(int64_t vid) {
    contracted_vertices += vid;
}

void CH_vertex::add_contracted_vertices_id(const Identifiers<int64_t>& vertices_ids) {
    contracted_vertices += vertices_ids;
}

void CH_vertex::clear_contracted_vertices() {
    contracted_vertices.clear();
}

std::ostream& operator <<(std::ostream& os, const CH_vertex& v) {
    os << "{id: " << v.id << ",\t"
       << "contracted vertices: "
       << v.get_contracted_vertices()
       << "}";
    return os;
}

}  // namespace pgrouting
