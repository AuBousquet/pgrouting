/*PGR-GNU*****************************************************************
File: linearContraction.hpp

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

#ifndef INCLUDE_CONTRACTION_LINEARCONTRACTION_HPP_
#define INCLUDE_CONTRACTION_LINEARCONTRACTION_HPP_
#pragma once


#include <functional>
#include <queue>
#include <vector>

#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "contraction/ch_edge.hpp"
#include "contraction/contractionGraph.hpp"
#include "cpp_common/identifiers.hpp"
#include "cpp_common/messages.hpp"


namespace pgrouting {
namespace contraction {

template < class G >
class Pgr_linear : public Pgr_messages {
 public:
    using V = typename G::V;
    using V_i = typename G::V_i;
    using B_G = typename G::B_G;

    // Constructor
    Pgr_linear() = default;

    // Other member functions
    void calculate_vertices(G &graph) {
        linearVertices.clear();
        for (const auto &v : boost::make_iterator_range(vertices(graph.graph))) {
            if (is_contractible(graph, v)) {
                linearVertices += v;
            }
        }
    }

    void do_contraction(G &graph) {
        calculate_vertices(graph);

        while (!linearVertices.empty()) {
            V v = linearVertices.front();
            linearVertices -= v;
            pgassert(is_contractible(graph, v));
            contract_node(graph, v);
        }
    }

    bool is_contractible(G &graph, V v) {
        return graph.is_linear(v) && !graph.get_forbidden_vertices().has(v);
    }

    void contract_node(G &graph, V v) {
        pgassert(is_contractible(graph, v));

        Identifiers<V> adjacent_vertices =
            graph.find_adjacent_vertices(v);
        pgassert(adjacent_vertices.size() == 2);

        V u = adjacent_vertices.front();
        adjacent_vertices.pop_front();
        V w = adjacent_vertices.front();
        adjacent_vertices.pop_front();

        pgassert(v != u);
        pgassert(v != w);
        pgassert(u != w);

        if (graph.is_directed()) {
            /*
            *  u --> v --> w
            */
            graph.process_shortcut(u, v, w);
            /*
            *  w --> v --> u
            */
            graph.process_shortcut(w, v, u);

        } else {
            pgassert(graph.is_undirected());
            /*
            * u - v - w
            */
            graph.process_shortcut(u, v, w);
        }

        graph[v].get_contracted_vertices().clear();
        boost::clear_vertex(v, graph.graph);
        linearVertices -= v;

        if (is_contractible(graph, u)) {
            contract_node(graph, u);
        } else {
            linearVertices -= u;
        }
        if (is_contractible(graph, w)) {
            contract_node(graph, w);
        } else {
            linearVertices -= w;
        }
    }

 private:
    Identifiers<V> linearVertices;
};

}  // namespace contraction
}  // namespace pgrouting

#endif  // INCLUDE_CONTRACTION_LINEARCONTRACTION_HPP_
