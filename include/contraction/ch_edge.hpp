/*PGR-GNU*****************************************************************
File: ch_edge.hpp

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

#ifndef INCLUDE_CPP_COMMON_CH_EDGE_HPP_
#define INCLUDE_CPP_COMMON_CH_EDGE_HPP_
#pragma once

#include <iostream>
#include <sstream>
#include "contraction/ch_vertex.hpp"

namespace pgrouting {

class CH_edge {
    public:
        CH_edge() = default;

        CH_edge(int64_t eid, int64_t source, int64_t target, double cost):
            id(eid), 
            source(source),
            target(target), 
            cost(cost) 
        {}

        int64_t get_id();
        int64_t get_source();
        int64_t get_target();
        void cp_members(CH_edge &);

        void set_contracted_vertices(Identifiers<int64_t>&);
        Identifiers<int64_t>& get_contracted_vertices();
        std::set<int64_t>& get_contracted_vertices_();

        void add_contracted_vertex(CH_vertex &);
        void add_contracted_vertices_from_edge(CH_edge &);
        void add_contracted_vertices(Identifiers<int64_t>&);

        bool has_contracted_vertices();
        void clear_contracted_vertices();

        friend std::ostream& operator << (std::ostream&, CH_edge&);

    public:
        int64_t id;
        int64_t source;
        int64_t target;
        double cost;

    private:
        Identifiers<int64_t> m_contracted_vertices;
};

}  // namespace pgrouting

#endif  // INCLUDE_CPP_COMMON_CH_EDGE_HPP_
