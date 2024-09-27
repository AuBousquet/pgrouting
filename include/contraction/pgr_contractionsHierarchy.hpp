/*PGR-GNU*****************************************************************
File: pgr_contractionsHierarchy.hpp

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

#ifndef INCLUDE_CONTRACTION_PGR_CONTRACTIONSHIERARCHY_HPP_
#define INCLUDE_CONTRACTION_PGR_CONTRACTIONSHIERARCHY_HPP_
#pragma once


#include <queue>
#include <functional>
#include <string>
#include <vector>
#include <utility>

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/filtered_graph.hpp>

#include "cpp_common/identifiers.hpp"
#include "cpp_common/pgr_alloc.hpp"

#include "c_common/e_report.h"

#include "visitors/dijkstra_visitors.hpp"



namespace pgrouting {
namespace contraction {

template < class G >
class Pgr_hierarchy {
 private:
     typedef typename G::V V;
     typedef typename G::E E;
     typedef typename G::V_i V_i;
     typedef typename G::E_i E_i;
     typedef typename G::B_G B_G;
     typedef typename std::pair< int64_t, V > V_p;
     typedef typename std::priority_queue< V_p, std::vector<V_p>, std::less<V_p> > PQ;


 public:
     void setForbiddenVertices(
            Identifiers<V> forbidden_vertices) {
        forbiddenVertices = forbidden_vertices;
     }

     Pgr_hierarchy():min_edge_id(0) {}


 private:
     int64_t get_next_id() {
         return --min_edge_id;
     }

 public:
     std::string log() const {
        return m_log.str();
     }

     void clean_log() {
        m_log.clear();
     }  

     void process_shortcut(G &graph, V u, V v, V w) {
         auto e1 = graph.get_min_cost_edge(u, v);
         auto e2 = graph.get_min_cost_edge(v, w);

         if (std::get<2>(e1) && std::get<2>(e2)) {
             auto contracted_vertices = std::get<1>(e1) + std::get<1>(e2);
             double cost = std::get<0>(e1) + std::get<0>(e2);
             contracted_vertices += graph[v].id;
             contracted_vertices += graph[v].contracted_vertices();

             // Create shortcut
             CH_edge shortcut(
                     get_next_id(),
                     graph[u].id,
                     graph[w].id,
                     cost);
             shortcut.contracted_vertices() = contracted_vertices;

             graph.add_shortcut(shortcut, u, w);
         }
     }

     double find_pmax(G &graph, V v, V u, Identifiers<V> out_vertices) {
         double p_max = 0.;
         E e, f;
         bool found_e, found_f;
         double c;

         for (V w : out_vertices) {
             boost::tie(e, found_e) = boost::edge(u, v, graph.graph);
             boost::tie(f, found_f) = boost::edge(v, w, graph.graph);
             if (found_e && found_f) {
                 c = graph[e].cost + graph[f].cost;
                 if (c > p_max) {
                     p_max = c;
                 }
             }
         }
         return p_max;
     }

     int64_t process_vertex_contraction(G &graph, V v) {
         Identifiers<V> adjacent_in_vertices = graph.find_adjacent_in_vertices(v);
         Identifiers<V> adjacent_out_vertices = graph.find_adjacent_out_vertices(v);
                  
         int shortcuts = 0;
         int old_edges = adjacent_in_vertices.size() + adjacent_out_vertices.size();
         
         while (!adjacent_in_vertices.empty()) {
             V u = adjacent_in_vertices.front();
             adjacent_in_vertices.pop_front();

             try {
                std::vector<V> predecessors(graph.num_vertices());
                std::vector<double> distances(graph.num_vertices());
                std::deque<V> nodesInDistance;
                V_i out_i, out_end;

                // Calculation of p_max
                double p_max;
                p_max = find_pmax(graph, v, u, adjacent_out_vertices);

                // Launch of a shortest paths query from u to all nodes with distance less than p_max
                boost::dijkstra_shortest_paths(
                    graph.graph,
                    u,
                    boost::predecessor_map(&predecessors[0])
                        .weight_map(get(&G::G_T_E::cost, graph.graph))
                        .distance_map(&distances[0])
                        .visitor(visitors::dijkstra_one_goal_visitor<V>(v))
                );

                /* abort in case of an interruption occurs (e.g. the query is being cancelled) */
                CHECK_FOR_INTERRUPTS();
                
                // Create a shortcut, for each w, when c(u, v) + c(u, w) > c(shortest path from u to v)
                for (const auto &w : adjacent_out_vertices) {
                    E e, f;
                    bool found_e, found_f;
                    double c;
                    boost::tie(e, found_e) = boost::edge(u, v, graph.graph);
                    boost::tie(f, found_f) = boost::edge(v, w, graph.graph);
                    if (found_e && found_f) {
                        c = graph[e].cost + graph[f].cost;
                        if (distances[w] < c) {
                            process_shortcut(graph, u, v, w);
                            shortcuts++;
                        }
                    }
                }
             }
             catch ( std::exception &except ) {
                 err << "Dijkstra shortest path query " << except.what() << std::endl;
             }
         }
         return shortcuts - old_edges;
     }

     void doContraction(G &graph) {
         // Fill the priority queue with a first search
         std::ostringstream err;
         err << "Do contraction" << std::endl;
         BGL_FORALL_VERTICES_T(v, graph.graph, B_G) {
             if (!forbiddenVertices.has(v)) {
                 minPQ.push(std::make_pair(process_vertex_contraction(graph, v), v));
             }
         }

         while (!minPQ.empty()) {
             std::pair< int64_t, V > ordered_vertex = minPQ.top();
             V current_vertex;
             int64_t corrected_order = process_vertex_contraction(graph, current_vertex);
             
             /* abort in case of an interruption occurs (e.g. the query is being cancelled) */
             CHECK_FOR_INTERRUPTS();

             // take next element
             minPQ.pop();
             ordered_vertex = minPQ.top();
             // if the next element has a smaller order than the corrected current one
             // then it is put back in the priority queue
             // otherwise, it is considered as permanently contracted
             if (corrected_order > ordered_vertex.first) {
                 minPQ.push(std::make_pair(corrected_order, current_vertex));
             }
         }
     }

 private:
     PQ minPQ;
     mutable std::ostringstream m_log;
     Identifiers<V> forbiddenVertices;
     int64_t min_edge_id;
};

}  // namespace contraction
}  // namespace pgrouting

#endif  // INCLUDE_CONTRACTION_PGR_LINEARCONTRACTION_HPP_
