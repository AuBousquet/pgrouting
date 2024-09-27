/*PGR-GNU*****************************************************************
File: ch_vertex.cpp

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

#include "contraction/ch_vertex.hpp"

#include <algorithm>
#include <vector>

namespace pgrouting {

int64_t CH_vertex::get_id() {
    return id;
}

double CH_vertex::get_metric() {
    return metric;
}

int64_t CH_vertex::get_vertex_order() {
    return vertex_order;
}

void CH_vertex::set_id(int64_t vid) {
    id = vid;
}

void CH_vertex::set_metric(double m) {
    metric=m;
}

void CH_vertex::set_vertex_order(int64_t order) {
    vertex_order=order;
}

void CH_vertex::cp_members(const CH_vertex &other) {
    this->id = other.id;
    this->m_contracted_vertices = other.get_contracted_vertices();
}

const Identifiers<int64_t>& CH_vertex::get_contracted_vertices() const {
    return m_contracted_vertices;
}

Identifiers<int64_t>& CH_vertex::get_contracted_vertices() {
    return m_contracted_vertices;
}

bool CH_vertex::has_contracted_vertices() const {
    if (m_contracted_vertices.size() == 0)
        return false;
    return true;
}

void CH_vertex::add_contracted_vertex(CH_vertex& v) {
    m_contracted_vertices += v.id;
    m_contracted_vertices += v.get_contracted_vertices();
}

void CH_vertex::add_contracted_vertex_id(int64_t vid) {
    m_contracted_vertices += vid;
}

std::ostream& operator <<(std::ostream& os, const CH_vertex& v) {
    os << "{id: " << v.id << ",\t"
     << "contracted vertices: "
     << v.get_contracted_vertices()
     << "}";
    return os;
}

}  // namespace pgrouting
