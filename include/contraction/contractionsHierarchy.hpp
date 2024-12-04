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
#include "cpp_common/alloc.hpp"
#include "cpp_common/messages.hpp"

#include "c_common/e_report.h"

#include "visitors/dijkstra_visitors.hpp"

namespace pgrouting {
namespace contraction {

template < class G >
class Pgr_contractionsHierarchy : public Pgr_messages {
private:
    typedef typename G::V V;
    typedef typename G::E E;
    typedef typename G::V_i V_i;
    typedef typename G::E_i E_i;
    typedef typename G::B_G B_G;
    typedef typename G::PQ PQ;

public:
    double find_pmax(
        G &graph, 
        V u, 
        V v, 
        Identifiers<V> out_vertices
    ) 
    {
        double p_max, c;
        E e, f;
        bool found_e, found_f;        
        boost::tie(e, found_e) = boost::edge(u, v, graph.graph);
        if (found_e) {
            for (V w : out_vertices) {
                boost::tie(f, found_f) = boost::edge(v, w, graph.graph);
                if (found_f) {
                    c = graph[e].cost + graph[f].cost;
                    if (c > p_max) p_max = c;
                }
            }
        }
        return p_max;
    }

    /*! 
    @brief contracts vertex *v* 
    @param [in] G graph
    @param [in] v vertex_descriptor
    @param [in] log log output
    @param [in] err err output
    @return contraction order: the node-contraction associated metrics
    */
    int64_t process_vertex_contraction(
        G &graph, 
        V v, 
        bool simulation,
        std::vector<CH_edge> &shortcuts,
        std::ostringstream &log
    )
    {
        Identifiers<V> adjacent_in_vertices = graph.find_adjacent_in_vertices(v);
        Identifiers<V> adjacent_out_vertices = graph.find_adjacent_out_vertices(v);
        int64_t old_edges = static_cast<int64_t>(adjacent_in_vertices.size()) + static_cast<int64_t>(adjacent_out_vertices.size());
        int64_t shortcuts_number = 0;
        log << std::endl << ">> Contraction of node " << graph[v].id << std::endl;
        log << num_vertices(graph.graph) << " vertices and " << num_edges(graph.graph) << " edges " << std::endl;
        while (!adjacent_in_vertices.empty()) {
            V u = adjacent_in_vertices.front();
            adjacent_in_vertices.pop_front();
            std::vector<V> predecessors(graph.num_vertices());
            std::vector<double> distances(graph.num_vertices());
            const std::set<V> goals = adjacent_out_vertices.get_ids(); 
            V_i out_i, out_end;
            log << "  >> from " << graph[u].id << std::endl;
            // Calculation of p_max
            int64_t p_max = find_pmax(graph, u, v, adjacent_out_vertices);
            if ( p_max > 0 ) {                    
                // Launch of a shortest paths query from u to all nodes with distance less than p_max
                log << "    p_max = " << p_max << std::endl;
                std::set<int64_t> reached_vertices_ids;
                try {
                    boost::dijkstra_shortest_paths(
                        graph.graph, 
                        u,
                        boost::predecessor_map(&predecessors[0])
                        .weight_map(get(&G::G_T_E::cost, graph.graph))
                        .distance_map(&distances[0])
                        .distance_inf(std::numeric_limits<double>::infinity())
                        .visitor(pgrouting::visitors::dijkstra_max_distance_visitor<V>(p_max, distances, reached_vertices_ids, log))
                    );
                }
                catch ( pgrouting::max_dist_reached & ) {
                    log << "    PgRouting exception during labelling!" << std::endl;
                    log << "    >>> Labelling interrupted because max distance is reached. " << std::endl; 
                    log << "    >>> Number of labelled vertices: " << reached_vertices_ids.size() << std::endl;
                    Identifiers<int64_t> r;
                    r.set_ids(reached_vertices_ids);
                    log << "    >>> Reached vertices: " << r << std::endl;
                }
                catch ( boost::exception const &except ) {
                    log << std::endl << "Boost exception during vertex contraction! " << dynamic_cast<std::exception const &>(except).what() << std::endl;
                }
                catch ( ... ) {
                    log << "    Unknown exception during labelling!" << std::endl; 
                }
                /* abort in case of an interruption occurs (e.g. the query is being cancelled) */
                CHECK_FOR_INTERRUPTS();                    
                // Create a shortcut, for each w, when c(u, v) + c(u, w) > cost(shortest path from u to v)
                for (const auto &w : adjacent_out_vertices) {
                    E e, f, g;
                    bool found_e, found_f, found_g;
                    double c;                        
                    if ( u != w ) {
                        boost::tie(e, found_e) = boost::edge(u, v, graph.graph);
                        boost::tie(f, found_f) = boost::edge(v, w, graph.graph);
                        boost::tie(g, found_g) = boost::edge(u, w, graph.graph);
                        if (found_e && found_f && (!found_g || (found_g && (distances[w] < graph[g].cost)))) {
                            c = graph[e].cost + graph[f].cost;
                            if ((predecessors[w]==v) && (predecessors[v]==u) && (distances[w] == c) && (graph.is_shortcut_possible(u, v, w))) { 
                                if (!simulation) {
                                    pgrouting::CH_edge ch_e = graph.process_shortcut_from_shortest_path(u, v, w, distances[w], log);
                                    shortcuts.push_back(ch_e);
                                }
                                shortcuts_number++;
                            }
                        }
                    }
                }
            }
        }
        if (!simulation) {
            for (auto &w : adjacent_out_vertices)
                boost::remove_edge(v, w, graph.graph);
            for (auto &u : graph.find_adjacent_in_vertices(v))
                boost::remove_edge(u, v, graph.graph);
            (graph[v]).clear_contracted_vertices();
            log << "  Size of the graph after contraction: " << num_vertices(graph.graph) << " vertices and " << num_edges(graph.graph) << " edges" << std::endl;
            log << "  " << shortcuts_number << " shortcuts created, " << old_edges << " old edges" << std::endl;
        }
        log << "  Metric: edge difference = " << shortcuts_number - old_edges << std::endl;
        return static_cast<double>(shortcuts_number - old_edges);
    }

    void do_contraction(
        G &graph, 
        std::ostringstream &log
    )
    {
        // First iteration over vertices
        G graph_copy=graph;
        std::vector<pgrouting::CH_edge> shortcuts;

        // Fill the priority queue with a first search
        log << "Do contraction" << std::endl;
        log << std::endl << ">>>> FIRST LABELLING" << std::endl;
        PQ minPQ;
        BGL_FORALL_VERTICES_T(v, graph.graph, B_G) {
            if (!(graph.get_forbidden_vertices()).has(v)) {
                minPQ.push(std::make_pair(process_vertex_contraction(graph, v, false, shortcuts, log), v));
            }
        }

        log << std::endl << ">>>> SECOND LABELLING" << std::endl;
        shortcuts.clear();
        graph = graph_copy;
        
        // Second iteration: lazy heuristics 
        // The graph is reinitialized
        while (!minPQ.empty()) {
            std::pair< int64_t, V > ordered_vertex = minPQ.top();
            minPQ.pop();
            int64_t corrected_metric = process_vertex_contraction(graph_copy, ordered_vertex.second, true, shortcuts, log);       
            log << "  Vertex: " << graph[ordered_vertex.second].id << ", min value of the queue: " << minPQ.top().first << std::endl;
            log << "  Lazy non-destructive simulation: initial order " << ordered_vertex.first << ", new order " << corrected_metric << std::endl;
            
            if (minPQ.top().first < corrected_metric) {
                log << "   Vertex reinserted in the queue" << std::endl;
                minPQ.push(std::make_pair(corrected_metric, ordered_vertex.second));
            }
            else {
                std::pair< int64_t, V > contracted_vertex;
                V u = graph_copy.vertices_map[graph[ordered_vertex.second].id];
                contracted_vertex.first = process_vertex_contraction(graph_copy, u, false, shortcuts, log);
                log << "  Vertex endly contracted in the queue" << std::endl;
                contracted_vertex.second = ordered_vertex.second;
                priorityQueue.push(contracted_vertex);
            }
        }
        log << std::endl << "Copy shortcuts" << std::endl;
        graph.copy_shortcuts(shortcuts, log);
        log << std::endl << "Priority queue: " << std::endl;
        graph.set_vertices_metric_and_hierarchy(priorityQueue, log);
    }

private:
    PQ priorityQueue;
};

}  // namespace contraction
}  // namespace pgrouting

#endif  // INCLUDE_CONTRACTION_PGR_CONTRACTIONSHIERARCHY_HPP_
