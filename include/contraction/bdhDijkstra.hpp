/*PGR-GNU*****************************************************************
File: bdhDijkstra.hpp

Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
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

#ifndef INCLUDE_CONTRACTION_BDH_DIJKSTRA_HPP_
#define INCLUDE_CONTRACTION_BDH_DIJKSTRA_HPP_
#pragma once

#include <string>
#include <queue>
#include <utility>
#include <vector>
#include <limits>
#include <functional>

#include "cpp_common/bidirectional.hpp"
#include "cpp_common/basePath_SSEC.hpp"
#include "contraction/contractionGraph.hpp"

namespace pgrouting {
namespace contraction {

template < typename G >
class Pgr_bdhDijkstra : public Pgr_bidirectional<G> {
    typedef typename Pgr_bidirectional<G>::V V;
    typedef typename Pgr_bidirectional<G>::E E;
    typedef typename Pgr_bidirectional<G>::Cost_Vertex_pair Cost_Vertex_pair;

    using Pgr_bidirectional<G>::graph;
    using Pgr_bidirectional<G>::m_log;
    using Pgr_bidirectional<G>::v_source;
    using Pgr_bidirectional<G>::v_target;

    using Pgr_bidirectional<G>::backward_predecessor;
    using Pgr_bidirectional<G>::backward_queue;
    using Pgr_bidirectional<G>::backward_finished;
    using Pgr_bidirectional<G>::backward_cost;
    using Pgr_bidirectional<G>::backward_edge;

    using Pgr_bidirectional<G>::forward_predecessor;
    using Pgr_bidirectional<G>::forward_queue;
    using Pgr_bidirectional<G>::forward_finished;
    using Pgr_bidirectional<G>::forward_cost;
    using Pgr_bidirectional<G>::forward_edge;

    using Pgr_bidirectional<G>::bidirectional;

 public:
    explicit Pgr_bdhDijkstra(G &pgraph):
        Pgr_bidirectional<G>(pgraph) {
            m_log << "pgr_bdhDijkstra constructor\n";
        }

    ~Pgr_bdhDijkstra() = default;

    Path pgr_bdhDijkstra(V start_vertex, V end_vertex, bool only_cost) {
        m_log << "pgr_bdhDijkstra\n";
        v_source = start_vertex;
        v_target = end_vertex;

        return bidirectional(only_cost);
    }

    using Pgr_bidirectional<G>::log;
    using Pgr_bidirectional<G>::clean_log;

 private:
    void explore_forward(const Cost_Vertex_pair &node) {
        typename G::EO_i out, out_end;

        auto current_cost = node.first;
        auto current_node = node.second;

        for (boost::tie(out, out_end) = out_edges(current_node, graph.graph);
                out != out_end; ++out) {
            auto edge_cost = graph[*out].cost;
            auto next_node = graph.adjacent(current_node, *out);

            if (forward_finished[next_node]) continue;

            if (edge_cost + current_cost < forward_cost[next_node]) {
                forward_cost[next_node] = edge_cost + current_cost;
                forward_predecessor[next_node] = current_node;
                forward_edge[next_node] = graph[*out].id;
                forward_queue.push({forward_cost[next_node], next_node});
            }
        }
        forward_finished[current_node] = true;
    }

    void explore_backward(const Cost_Vertex_pair &node) {
        typename G::EI_i in, in_end;

        auto current_cost = node.first;
        auto current_node = node.second;

        for (boost::tie(in, in_end) = in_edges(current_node, graph.graph);
                in != in_end; ++in) {
            auto edge_cost = graph[*in].cost;
            auto next_node = graph.adjacent(current_node, *in);

            if (backward_finished[next_node]) continue;

            if (edge_cost + current_cost < backward_cost[next_node]) {
                backward_cost[next_node] = edge_cost + current_cost;
                backward_predecessor[next_node] = current_node;
                backward_edge[next_node] = graph[*in].id;
                backward_queue.push({backward_cost[next_node], next_node});
            }
        }
        backward_finished[current_node] = true;
    }
};

}  // namespace contraction
}  // namespace pgrouting

#endif  // INCLUDE_CONTRACTION_BDH_DIJKSTRA_HPP_
