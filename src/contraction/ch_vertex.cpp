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

// Accessors
const Identifiers<int64_t>& CH_vertex::get_contracted_vertices() const {
    return m_contracted_vertices;
}

Identifiers<int64_t>& CH_vertex::get_contracted_vertices() {
    return m_contracted_vertices;
}

void CH_vertex::set_contracted_vertices(Identifiers<int64_t>& contracted_vertices_ids) {
    m_contracted_vertices = contracted_vertices_ids;
}

// Other member functions
void CH_vertex::cp_members(const CH_vertex &other) {
    this->id = other.id;
    this->m_contracted_vertices = other.get_contracted_vertices();
}

bool CH_vertex::has_contracted_vertices() const {
    if (m_contracted_vertices.size() == 0)
        return false;
    return true;
}

/*!
    @brief adds an ID of another vertex, contracted in this one
    @param [in] vid vertex ID
*/
void CH_vertex::add_contracted_vertex(const CH_vertex& v) {
    m_contracted_vertices += v.id;
    m_contracted_vertices += v.get_contracted_vertices();
}

void CH_vertex::add_contracted_vertices_id(const Identifiers<int64_t>& vertices_ids) {
    m_contracted_vertices += vertices_ids;
}

void CH_vertex::clear_contracted_vertices()
{
    m_contracted_vertices.clear();
}

// Friend function
std::ostream& operator << (std::ostream& os, const CH_vertex& v) {
    os << "{id: " << v.id << ",\t"
       << "contracted vertices: "
       << v.get_contracted_vertices()
       << "}";
    
    return os;
}

}  // namespace pgrouting
