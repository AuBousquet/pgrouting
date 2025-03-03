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

#include <functional>
#include <limits>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "cpp_common/basePath_SSEC.hpp"
#include "cpp_common/bidirectional.hpp"
#include "cpp_common/contractionGraph.hpp"

namespace pgrouting {
namespace bidirectional {

class Pgr_hbdDijkstra : public Pgr_bidirectional<Pgr_contractionGraph> {
  using V = typename Pgr_bidirectional<Pgr_contractionGraph>::V;
  using E = typename Pgr_bidirectional<Pgr_contractionGraph>::E;
  using Cost_Vertex_pair =
      typename Pgr_bidirectional<Pgr_contractionGraph>::Cost_Vertex_pair;

  using Pgr_bidirectional<Pgr_contractionGraph>::graph;
  using Pgr_bidirectional<Pgr_contractionGraph>::m_log;
  using Pgr_bidirectional<Pgr_contractionGraph>::v_source;
  using Pgr_bidirectional<Pgr_contractionGraph>::v_target;

  using Pgr_bidirectional<Pgr_contractionGraph>::backward_predecessor;
  using Pgr_bidirectional<Pgr_contractionGraph>::backward_queue;
  using Pgr_bidirectional<Pgr_contractionGraph>::backward_finished;
  using Pgr_bidirectional<Pgr_contractionGraph>::backward_cost;
  using Pgr_bidirectional<Pgr_contractionGraph>::backward_edge;

  using Pgr_bidirectional<Pgr_contractionGraph>::forward_predecessor;
  using Pgr_bidirectional<Pgr_contractionGraph>::forward_queue;
  using Pgr_bidirectional<Pgr_contractionGraph>::forward_finished;
  using Pgr_bidirectional<Pgr_contractionGraph>::forward_cost;
  using Pgr_bidirectional<Pgr_contractionGraph>::forward_edge;

  // shortest path function
  using Pgr_bidirectional<Pgr_contractionGraph>::bidirectional;

 public:
  explicit Pgr_hbdDijkstra(G &pgraph) : Pgr_bidirectional<G>(pgraph) {
    m_log << "pgr_hbdDijkstra constructor\n";
  }

  ~Pgr_hbdDijkstra() = default;

  Path pgr_hbdDijkstra(V start_vertex, V end_vertex, bool only_cost) {
    m_log << "pgr_hbdDijkstra\n";
    v_source = start_vertex;
    v_target = end_vertex;

    return bidirectional(only_cost);
  }

  using Pgr_bidirectional<Pgr_contractionGraph>::log;
  using Pgr_bidirectional<Pgr_contractionGraph>::clean_log;

 private:
  void explore_forward(const Cost_Vertex_pair &node) {
    typename G::EO_i out, out_end;

    auto current_cost = node.first;
    auto current_node = node.second;

    for (const auto &out : boost::make_iterator_range(
             graph.graph.find_adjacent_up_vertices(current_node))) {
      auto edge_cost = graph[out].cost;
      auto next_node = graph.adjacent(current_node, out);

      if (forward_finished[next_node]) continue;

      if (edge_cost + current_cost < forward_cost[next_node]) {
        forward_cost[next_node] = edge_cost + current_cost;
        forward_predecessor[next_node] = current_node;
        forward_edge[next_node] = graph[out].id;
        forward_queue.push({forward_cost[next_node], next_node});
      }
    }
    forward_finished[current_node] = true;
  }

  void explore_backward(const Cost_Vertex_pair &node) {
    typename G::EI_i in, in_end;

    auto current_cost = node.first;
    auto current_node = node.second;

    for (const auto &in : boost::make_iterator_range(
             graph.graph.find_adjacent_down_vertices(current_node))) {
      auto edge_cost = graph[in].cost;
      auto next_node = graph.adjacent(current_node, in);

      if (backward_finished[next_node]) continue;

      if (edge_cost + current_cost < backward_cost[next_node]) {
        backward_cost[next_node] = edge_cost + current_cost;
        backward_predecessor[next_node] = current_node;
        backward_edge[next_node] = graph[in].id;
        backward_queue.push({backward_cost[next_node], next_node});
      }
    }
    backward_finished[current_node] = true;
  }
};

}  // namespace bidirectional
}  // namespace pgrouting

#endif  // INCLUDE_HBDDIJKSTRA_HBDDIJKSTRA_HPP_
