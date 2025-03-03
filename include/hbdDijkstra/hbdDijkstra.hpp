/*PGR-GNU*****************************************************************
File: bdDijkstra.hpp

Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) Aurélie Bousquet - 2025
Mail: aurelie.bousquet at oslandia.com

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


#ifndef INCLUDE_HBDDIJKSTRA_HBDDIJKSTRA_HPP_
#define INCLUDE_HBDDIJKSTRA_HBDDIJKSTRA_HPP_
#pragma once

#include <string>
#include <queue>
#include <utility>
#include <vector>
#include <limits>
#include <functional>

#include "cpp_common/bidirectional.hpp"
#include "cpp_common/basePath_SSEC.hpp"

namespace pgrouting {
namespace bidirectional {

template < typename G >
class Pgr_hbdDijkstra : public Pgr_bidirectional<G> {
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
    using Pgr_bidirectional<G>::log;
    using Pgr_bidirectional<G>::clean_log;

    using Pgr_bidirectional<G>::bidirectional;

 public:
    explicit Pgr_hbdDijkstra(G &pgraph):
        Pgr_bidirectional<G>(pgraph) {
            m_log << "pgr_hbdDijkstra constructor\n";
    }

    ~Pgr_hbdDijkstra() = default;

    Path path_search(V start_vertex, V end_vertex, bool only_cost) {
        m_log << "pgr_hbdDijkstra\n";
        v_source = start_vertex;
        v_target = end_vertex;

        return bidirectional(only_cost);
    }

 private:
    /*! @brief explores the graph forward
        (from the origin to the destination)
        from a given node
        @param [in] node pair of (double, vertex_descriptor)
    */
    void explore_forward(
        const Cost_Vertex_pair &node) {
        typename G::EO_i out, out_end;

        auto current_cost = node.first;
        auto current_node = node.second;

        for (const auto &out : boost::make_iterator_range(
                graph.find_adjacent_up_vertices(current_node))) {
            auto edge_cost =
                graph[boost::edge(current_node, out, graph.graph).first].cost;
            if (forward_finished[out]) continue;

            if (edge_cost + current_cost < forward_cost[out]) {
                forward_cost[out] = edge_cost + current_cost;
                forward_predecessor[out] = current_node;
                forward_edge[out] = graph[out].id;
                forward_queue.push({forward_cost[out], out});
            }
        }
        forward_finished[current_node] = true;
    }

    /*!
        @brief explores the graph backward
        (from the destination to the origin)
        from a given node
        @param [in] node pair of (double, vertex_descriptor)
    */
    void explore_backward(
        const Cost_Vertex_pair &node) {
        typename G::EI_i in, in_end;
        auto current_cost = node.first;
        auto current_node = node.second;

        for (const auto &in : boost::make_iterator_range(
                graph.find_adjacent_down_vertices(current_node))) {
            auto edge_cost =
                graph[boost::edge(in, current_node, graph.graph).first].cost;
            if (backward_finished[in]) continue;

            if (edge_cost + current_cost < backward_cost[in]) {
                backward_cost[in] = edge_cost + current_cost;
                backward_predecessor[in] = current_node;
                backward_edge[in] = graph[in].id;
                backward_queue.push({backward_cost[in], in});
            }
        }
        backward_finished[current_node] = true;
    }
};

}  // namespace bidirectional
}  // namespace pgrouting

#endif  // INCLUDE_HBDDIJKSTRA_BDDIJKSTRA_HPP_
