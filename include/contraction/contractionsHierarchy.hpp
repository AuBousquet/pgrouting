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
    typedef typename std::pair< int64_t, V > V_p;
    typedef typename std::priority_queue< V_p, std::vector<V_p>, std::greater<V_p> > PQ;

public:
    void setForbiddenVertices(Identifiers<V> forbidden_vertices) {
        forbiddenVertices = forbidden_vertices;
    }

    double find_pmax(
        G &graph, 
        V u, 
        V v, 
        Identifiers<V> out_vertices, 
        std::ostringstream &log,
        std::ostringstream &err
    ) 
    {
        double p_max = 0., c;
        E e, f;
        bool found_e, found_f;
        
        log << std::endl << "pmax calculation from (" << graph[u].id << ", " << graph[v].id << ")" << std::endl;
        boost::tie(e, found_e) = boost::edge(u, v, graph.graph);
        if (found_e) {
            for (V w : out_vertices) {
                log << "  to node w = " << graph[w].id;
                boost::tie(f, found_f) = boost::edge(v, w, graph.graph);
                if (found_f) {
                    c = graph[e].cost + graph[f].cost;
                    log << ", costs : " << graph[e].cost << " + " << graph[f].cost << std::endl;
                    if (c > p_max) p_max = c;
                }
            }
        }
        else 
            err << "Edge (" << graph[u].id << ", " << graph[v].id << ") not found" << std::endl;
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
        std::ostringstream &log,
        std::ostringstream &err
    )
    {
        Identifiers<V> adjacent_in_vertices = graph.find_adjacent_in_vertices(v);
        Identifiers<V> adjacent_out_vertices = graph.find_adjacent_out_vertices(v);

        int shortcuts = 0;
        int old_edges = adjacent_in_vertices.size() + adjacent_out_vertices.size();
        log << std::endl << ">> Contraction of node " << graph[v].id;
        
        while (!adjacent_in_vertices.empty()) {
            try {
                V u = adjacent_in_vertices.front();
                adjacent_in_vertices.pop_front();
                std::vector<V> predecessors(graph.num_vertices());
                std::vector<double> distances(graph.num_vertices());
                const std::set<V> goals = adjacent_out_vertices.get_ids(); 
                V_i out_i, out_end;

                // Calculation of p_max
                double p_max = find_pmax(graph, u, v, adjacent_out_vertices, log, err);
                if ( p_max > 0 ) {                    
                    // Launch of a shortest paths query from u to all nodes with distance less than p_max
                    log << "  found p_max = " << p_max;
                    boost::dijkstra_shortest_paths(
                        graph.graph, 
                        u,
                        boost::predecessor_map(&predecessors[0])
                        .weight_map(get(&G::G_T_E::cost, graph.graph))
                        .distance_map(&distances[0])
                        .distance_inf(std::numeric_limits<double>::infinity())
                    );
                    log << "  first labelling done for node " << graph[v].id << std::endl;

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
                                if (distances[w] < c) { 
                                    // The shortest path from u to w was by v
                                    if (graph.process_shortcut_(u, v, w, simulation, distances[w], log)) {
                                        shortcuts++;
                                    }
                                }
                            }
                        }
                    }
                } // p_max > 0
            } catch ( boost::exception const &except ) {
                log << std::endl << "Vertex contraction exception: " << dynamic_cast<std::exception const &>(except).what() << std::endl;
            } catch ( ... ) {
                log << std::endl << "Unknown vertex contraction error" << std::endl;
            }
        }
        
        if (!simulation) {
            for (auto &w : adjacent_out_vertices) {
                boost::remove_edge(v, w, graph.graph);
            }
            for (auto &u : adjacent_in_vertices) {
                boost::remove_edge(u, v, graph.graph);
            }
            (graph[v]).clear_contracted_vertices(); 
        }

        if (shortcuts > 0) 
            log << shortcuts << " shortcuts created, " << old_edges << " old edges, edge difference = " << shortcuts - old_edges << ".";
        log << std::endl;

        return shortcuts - old_edges;
    }

    void doContraction(
        G &graph, 
        std::ostringstream &log,
        std::ostringstream &err
    )
    {
        // First iteration over vertices
        G contracted_graph=graph;
        // Fill the priority queue with a first search
        log << "Do contraction" << std::endl;

        PQ minPQ;
        BGL_FORALL_VERTICES_T(v, graph.graph, B_G) {
            if (!forbiddenVertices.has(v)) {
                minPQ.push(std::make_pair(process_vertex_contraction(graph, v, false, log, err), v));
            }
        }

        // Second iteration: lazy heuristics 
        // The graph is reinitialized
        while (!minPQ.empty()) {
            std::pair< int64_t, V > ordered_vertex = minPQ.top();
            minPQ.pop();
            int64_t corrected_order = process_vertex_contraction(graph, ordered_vertex.second, true, log, err);            
            log << "Initial order " << ordered_vertex.first << ", new order " << corrected_order << ", vertex: " << graph[ordered_vertex.second].id << ", min value of the queue: " << minPQ.top().first << std::endl;

            if (minPQ.top().first < corrected_order) {
                log << "Vertex reinserted" << std::endl;
                minPQ.push(std::make_pair(corrected_order, ordered_vertex.second));
            }
            else {
                std::pair< int64_t, V > contracted_vertex;
                contracted_vertex.first = process_vertex_contraction(contracted_graph, ordered_vertex.second, false, log, err);
                contracted_vertex.second = ordered_vertex.second;
                log << "Vertex endly contracted" << std::endl;
                priority_queue.push(contracted_vertex);
            }
        }

        E_i e, e_end;
        for (boost::tie(e, e_end) = edges(graph.graph); e != e_end; ++e) {
            log << graph.graph[*e].id << std::endl;
        }
        BGL_FORALL_EDGES_T(e_, graph.graph, B_G) {
            log << graph.graph[e_].id << std::endl;
        }
        
    }

private:
    PQ priority_queue;
    Identifiers<V> forbiddenVertices;
};

}  // namespace contraction
}  // namespace pgrouting

#endif  // INCLUDE_CONTRACTION_PGR_LINEARCONTRACTION_HPP_
