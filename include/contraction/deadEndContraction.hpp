/*PGR-GNU*****************************************************************
File: deadEndContraction.hpp

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

#ifndef INCLUDE_CONTRACTION_DEADENDCONTRACTION_HPP_
#define INCLUDE_CONTRACTION_DEADENDCONTRACTION_HPP_
#pragma once


#include <functional>
#include <queue>
#include <vector>

#include <boost/graph/adjacency_list.hpp>

#include "cpp_common/identifiers.hpp"
#include "cpp_common/interruption.hpp"

namespace pgrouting {
namespace contraction {

template < class G >
class Pgr_deadend : public Pgr_messages {
 private:
    using V = typename G::V;
    using E = typename G::E;

    Identifiers<V> deadend_vertices;

 public:
    Pgr_deadend() = default;

    void calculate_vertices(G &graph) {
        for (const auto &v : boost::make_iterator_range(vertices(graph.graph))) {
            if (is_dead_end(graph, v) && !graph.get_forbidden_vertices().has(v)) {
                deadend_vertices += v;
            }
        }
    }

    bool is_dead_end(G &graph, V v) {
        if (graph.is_undirected()) {
            return graph.find_adjacent_vertices(v).size() == 1;
        }

        pgassert(graph.is_directed());

        return graph.find_adjacent_vertices(v).size() == 1
            || (graph.in_degree(v) > 0 && graph.out_degree(v) == 0);
    }

    void do_contraction(G &graph) {
        calculate_vertices(graph);

        while (!deadend_vertices.empty()) {
            V v = deadend_vertices.front();
            deadend_vertices -= v;
            pgassert(is_dead_end(graph, v));

            Identifiers<V> local;
            for (auto u : graph.find_adjacent_vertices(v)) {
                /*
                 * u{v1} --{v2}-> v{v3}
                 *
                 * u{v1 + v + v2 + v3}     v{}
                 */
                const auto& e = graph.get_min_cost_edge(u, v);
                graph[u].get_contracted_vertices() +=
                    std::get<0>(e).get_contracted_vertices();
                graph[u].add_contracted_vertex(graph[v]);

                deadend_vertices -= v;
                local += u;
            }

            graph[v].clear_contracted_vertices();
            boost::clear_vertex(v, graph.graph);

            /* abort in case of an interruption occurs
            (e.g. the query is being cancelled) */
            CHECK_FOR_INTERRUPTS();

            for (const auto &u : local) {
                if (is_dead_end(graph, u) && !graph.get_forbidden_vertices().has(u)) {
                    deadend_vertices += u;
                } else {
                    deadend_vertices -= u;
                }
            }
        }
    }
};

}  // namespace contraction
}  // namespace pgrouting

#endif  // INCLUDE_CONTRACTION_DEADENDCONTRACTION_HPP_
