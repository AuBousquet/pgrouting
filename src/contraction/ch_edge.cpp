/*PGR-GNU*****************************************************************
File: ch_edge.cpp

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

#include "contraction/ch_edge.hpp"

namespace pgrouting {

    void CH_edge::clear_contracted_vertices() {
        m_contracted_vertices.clear();
    }

    int64_t CH_edge::get_id() {
        return id;
    }

    int64_t CH_edge::get_source() {
        return source;
    }

    int64_t CH_edge::get_target() {
        return target;
    }

    double CH_edge::get_cost() {
        return cost;
    }

    void CH_edge::cp_members(CH_edge& other) {
        cost = other.cost;
        id = other.id;
        source = other.source;
        target = other.target;
        m_contracted_vertices += other.get_contracted_vertices();
    }

    void CH_edge::set_contracted_vertices(Identifiers<int64_t>& contracted_vertices_ids) {
        m_contracted_vertices = contracted_vertices_ids;
    }

    Identifiers<int64_t>& CH_edge::get_contracted_vertices() {
        return m_contracted_vertices;
    }

    std::set<int64_t>& CH_edge::get_contracted_vertices_() {
        return m_contracted_vertices.get_ids();
    }

    void CH_edge::add_contracted_vertex(CH_vertex& v) {
        m_contracted_vertices += v.get_id();
        m_contracted_vertices += v.get_contracted_vertices();
    }

    void CH_edge::add_contracted_vertices_from_edge(CH_edge& e) {
        if (e.has_contracted_vertices())
            m_contracted_vertices += e.get_contracted_vertices();
    }

    void CH_edge::add_contracted_vertices(Identifiers<int64_t>& ids) {
        m_contracted_vertices += ids;
    }

    bool CH_edge::has_contracted_vertices() {
        return !m_contracted_vertices.empty();
    }

    std::ostream& operator <<(std::ostream& os, CH_edge& e) {
        os << "{id: " << e.id << ",\t"
            << "source: " << e.source << ",\t"
            << "target: " << e.target << ",\t"
            << "cost: " << e.cost << ",\t"
            << "contracted vertices: "
            << e.get_contracted_vertices()
            << "}";
        return os;
    }

}  // namespace pgrouting
