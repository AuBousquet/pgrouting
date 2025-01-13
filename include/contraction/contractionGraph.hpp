/*PGR-GNU*****************************************************************
File: contractionGraph.hpp

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developers:
Copyright (c) 2016 Rohith Reddy
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

#ifndef INCLUDE_CONTRACTION_CONTRACTIONGRAPH_HPP_
#define INCLUDE_CONTRACTION_CONTRACTIONGRAPH_HPP_
#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>

#include <boost/graph/iteration_macros.hpp>

#include "cpp_common/base_graph.hpp"
#include "contraction/ch_vertex.hpp"
#include "contraction/ch_edge.hpp"

namespace pgrouting {
namespace graph {

template <class G, bool t_directed>
class contractionGraph :
        public Pgr_base_graph<G, CH_vertex, CH_edge, t_directed> {
 public:
    using V = typename boost::graph_traits<G>::vertex_descriptor;
    using E = typename boost::graph_traits<G>::edge_descriptor;
    using EO_i = typename boost::graph_traits<G>::out_edge_iterator;
    using EI_i = typename boost::graph_traits<G>::in_edge_iterator;
    using E_i = typename boost::graph_traits<G>::edge_iterator;
    using V_p = typename std::pair<double, V>;
    using PQ = typename std::priority_queue<V_p, std::vector<V_p>,
            std::greater<V_p>>;

    // Constructors
    /*!
        Prepares the _graph_ to be of type *directed*
    */
    explicit contractionGraph<G, t_directed>() :
        Pgr_base_graph<G, CH_vertex, CH_edge, t_directed>() {
        min_edge_id = 0;
    }

    int64_t get_next_id() {
        return --min_edge_id;
    }

    Identifiers<V> get_forbidden_vertices() {
        return forbidden_vertices;
    }

    /*!
        @brief for C calls: to get the metric of a node, directly from the graph
        @param [in] v vertex_descriptor
        @return int64_t: the value of the metric for node v
    */
    double get_vertex_metric(int64_t vertex_id) {
        return (this->graph[this->vertices_map[vertex_id]]).get_metric();
    }

    /*!
        @brief for C calls: to get the order of a node, directly from the graph
        @param [in] v vertex_descriptor
        @return int64_t: the order of node v
    */
    int64_t get_vertex_order(int64_t vertex_id) {
        return (this->graph[this->vertices_map[vertex_id]]).get_vertex_order();
    }

    void set_forbidden_vertices(
        Identifiers<V> m_forbidden_vertices) {
        forbidden_vertices = m_forbidden_vertices;
    }

    /*!
        @brief defines the metric and hierarchy at the level of the nodes,
        from a given priority queue
        @param [in] PQ priority_queue
        @return void
    */
    void set_vertices_metric_and_hierarchy(
            PQ priority_queue,
            std::ostringstream &log) {
        int64_t i = 0;
        while (!priority_queue.empty()) {
            i++;
            std::pair<double, V> ordered_vertex = priority_queue.top();
            priority_queue.pop();

            (this->graph[ordered_vertex.second]).set_metric(
                ordered_vertex.first);
            (this->graph[ordered_vertex.second]).set_vertex_order(i);

            log << "(" << ordered_vertex.first << ", "
                << (this->graph[ordered_vertex.second]).id
                << ")" << std::endl;
            log << " metric = " << get_vertex_metric(
                    (this->graph[ordered_vertex.second]).id)
                << " order = " << get_vertex_order(
                    (this->graph[ordered_vertex.second]).id)
                << std::endl;
        }
    }

    // Other member functions
    /*!
        @brief add a shortcut to the graph during contraction
        @param [in] edge of type *CH_edge* is to be added
        @param [in] u vertex
        @param [in] v vertex
        u -> w -> v
        u -> v
        edge (u, v) is a new edge e
        contracted_vertices = w + contracted vertices
    */
    void add_shortcut(const CH_edge &edge, V u, V v) {
        bool inserted;
        E e;
        if (edge.cost < 0)
            return;
        boost::tie(e, inserted) = boost::add_edge(u, v, this->graph);
        this->graph[e] = edge;
    }

    /*!
        @brief copies shortcuts and modified vertices from another graph
        @result void
    */
    void copy_shortcuts(
        std::vector<pgrouting::CH_edge> &shortcuts,
        std::ostringstream &log) {
        for (auto it = shortcuts.begin(); it != shortcuts.end(); it++) {
            V u, v;
            u = this->vertices_map[it->source];
            v = this->vertices_map[it->target];
            log << "Shortcut " << it->id << "("
                << it->source << ", " << it->target
                << ")" << std::endl;
            add_shortcut(*it, u, v);
        }
    }

    /*!
        @brief get the vertex descriptors of adjacent vertices of *v*
        @param [in] v vertex_descriptor
        @return Identifiers<V>: The set of vertex descriptors adjacent
        to the given vertex *v*
    */
    Identifiers<V> find_adjacent_vertices(V v) const {
        Identifiers<V> adjacent_vertices;
        for (const auto &e : boost::make_iterator_range(
                out_edges(v, this->graph)))
            adjacent_vertices += this->adjacent(v, e);

        for (const auto &e : boost::make_iterator_range(
                in_edges(v, this->graph)))

            adjacent_vertices += this->adjacent(v, e);

        return adjacent_vertices;
    }

    /*!
        @brief get the vertex descriptors of adjacent vertices of *v*
        @param [in] v vertex_descriptor
        @return Identifiers<V>: The set of out vertex descriptors adjacent to the given vertex *v*
    */
    Identifiers<V> find_adjacent_out_vertices(V v) const {
        Identifiers<V> adjacent_vertices;

        for (const auto &out : boost::make_iterator_range(
                out_edges(v, this->graph)))
            adjacent_vertices += this->adjacent(v, out);

        return adjacent_vertices;
    }

    /*!
        @brief get the vertex descriptors of adjacent vertices of *v*
        @param [in] v vertex_descriptor
        @return Identifiers<V>: The set of in vertex descriptors adjacent to the given vertex *v*
    */
    Identifiers<V> find_adjacent_in_vertices(V v) const {
        Identifiers<V> adjacent_vertices;

        for (const auto &in : boost::make_iterator_range(
                in_edges(v, this->graph)))
            adjacent_vertices += this->adjacent(v, in);

        return adjacent_vertices;
    }

    /*!
        @brief vertices with at least one contracted vertex
        @result The vids Identifiers with at least one contracted vertex
    */
    std::vector<E> get_shortcuts() {
        std::vector<E> eids;
        for (const auto &e:
            boost::make_iterator_range(edges(this->graph))) {
            if (this->graph[e].id < 0) {
                eids.push_back(e);
                pgassert(!(this->graph[e]).get_contracted_vertices().empty());
            } else {
                pgassert((this->graph[e]).get_contracted_vertices().empty());
            }
        }
        std::sort(
            eids.begin(),
            eids.end(),
            [&](E lhs, E rhs) {
                return -1 * ((this->graph)[lhs]).id
                        < -1 * ((this->graph)[rhs]).id;
            });

        return eids;
    }

    /*!
        @brief vertices with at least one contracted vertex
        @result The vids Identifiers with at least one contracted vertex
    */
    Identifiers<int64_t> get_modified_vertices() {
        Identifiers<int64_t> vids;
        for (const auto &v :
                boost::make_iterator_range(boost::vertices(this->graph))) {
            if ((this->graph[v].vertex_order > 0)
            || ((this->graph[v]).has_contracted_vertices())) {
                vids += (this->graph[v]).id;
            }
        }

        return vids;
    }

    /*!
        @brief get the edge with minimum cost between two vertices
        @param [in] u vertex_descriptor of source vertex
        @param [in] v vertex_descriptor of target vertex
        @return E: The edge descriptor of the edge with minimum cost
    */
    std::tuple<CH_edge, bool> get_min_cost_edge(V u, V v) {
        Identifiers<int64_t> contracted_vertices;
        double min_cost = (std::numeric_limits<double>::max)();
        bool found = false;
        CH_edge edge;

        if (this->is_directed()) {
            for (const auto &e:
                boost::make_iterator_range(out_edges(u, this->graph))) {
                if (target(e, this->graph) == v) {
                    contracted_vertices +=
                        (this->graph[e]).get_contracted_vertices();
                    if ((this->graph[e]).cost < min_cost) {
                        min_cost = (this->graph[e]).cost;
                        found = true;
                        edge = this->graph[e];
                    }
                }
            }
            // To follow the principles presented
            // for linear contraction in "issue_1002.pg" test 3
            edge.set_contracted_vertices(contracted_vertices);

            return std::make_tuple(edge, found);
        }

        pgassert(this->is_undirected());
        for (const auto &e : boost::make_iterator_range(
            out_edges(u, this->graph))) {
            if (this->adjacent(u, e) == v) {
                contracted_vertices +=
                    (this->graph[e]).get_contracted_vertices();
                if ((this->graph[e]).cost < min_cost) {
                    min_cost = (this->graph[e]).cost;
                    found = true;
                    edge = this->graph[e];
                }
            }
        }
        return std::make_tuple(edge, found);
    }

    /*!
        @brief tests if the edges sequence (u, v), (v, w) exists in the graph
    */
    bool has_u_v_w(V u, V v, V w) const {
        return boost::edge(u, v, this->graph).second
            && boost::edge(v, w, this->graph).second;
    }

    /*!
        @brief tests if v is in the middle of two edges
        with no possible bifurcation in v
    */
    bool is_linear(V v) {
        // Checking adjacent vertices constraint
        auto adjacent_vertices = find_adjacent_vertices(v);
        if (adjacent_vertices.size() == 2) {
            // Checking u - v - w
            V u = adjacent_vertices.front();
            adjacent_vertices.pop_front();
            V w = adjacent_vertices.front();
            adjacent_vertices.pop_front();
            if (is_shortcut_possible(u, v, w)) {
                return true;
            }
            return false;
        }
        return false;
    }

    /*
        Possibility of a shortcut from left vertex to right vertex
        *v* should be a linear vertex
        u <-> v -> w: v not considered linear
        @dot
        graph G {
        graph [rankdir=LR];
        subgraph cluster0 {
        node [shape=point,height=0.2,style=filled,color=black];
        style=filled;
        color=lightgrey;
        a0; a1; a2;
        label = "rest of graph";
        }
        v [color=green];
        v -- left;
        v -- right;
        u -- a0;
        w -- a1;
        }
        @enddot
    */
    bool is_shortcut_possible(V u, V v, V w) {
        if (u == v || v == w || u == w)
            return false;
        pgassert(u != v);
        pgassert(v != w);
        pgassert(u != w);
        if (this->is_undirected()) {
            /*
            * u - v - w
            */
            return has_u_v_w(u, v, w);
        }
        pgassert(this->is_directed());
        return
            /*
             * u <-> v <-> w
             */
            (has_u_v_w(u, v, w) && has_u_v_w(w, v, u))
            /*
             * u -> v -> w
             */
            ||
            (has_u_v_w(u, v, w) && !(boost::edge(v, u, this->graph).second
            || boost::edge(w, v, this->graph).second))
            /*
             * u <- v <- w
             */
            ||
            (has_u_v_w(w, v, u) && !(boost::edge(v, w, this->graph).second
            || boost::edge(u, v, this->graph).second));
    }

    /*!
        @brief builds the shortcut information and adds it
        during contraction or afterwards to copy them to the source graph
        @param [in] u origin node of the shortcut
        @param [in] v shortcuted node
        @param [in] w destination node of the shortcut
        *
        * u ----e1{v1}----> v ----e2{v2}----> w
        *
        * e1: min cost edge from u to v
        * e2: min cost edge from v to w
        *
        * result:
        * u ---{v+v1+v2}---> w
        *
        *
    */
    CH_edge process_shortcut(V u, V v, V w) {
        auto e1 = get_min_cost_edge(u, v);
        auto e2 = get_min_cost_edge(v, w);

        double cost = std::numeric_limits<double>::max();
        if (std::get<1>(e1) && std::get<1>(e2))
            cost = std::get<0>(e1).cost + std::get<0>(e2).cost;

        // Create shortcut
        CH_edge shortcut(
            get_next_id(),
            (this->graph[u]).get_id(),
            (this->graph[w]).get_id(),
            cost);

        shortcut.add_contracted_vertex(this->graph[v]);
        shortcut.add_contracted_vertices_from_edge(std::get<0>(e1));
        shortcut.add_contracted_vertices_from_edge(std::get<0>(e2));

        // Add shortcut in the current graph (to go on the process)
        add_shortcut(shortcut, u, w);

        // Return shortcut to store and reuse in the final graph
        return shortcut;
    }

    /*!
        @brief print the graph with contracted vertices of all vertices and edges
    */
    friend std::ostream &operator<<(
            std::ostream &os, const contractionGraph &g) {
        EO_i out, out_end;
        for (const auto &vi : boost::make_iterator_range(vertices(g.graph))) {
            if ((*vi) >= g.num_vertices())
                break;
            os << g.graph[*vi].id << "(" << (*vi) << ")"
                << g.graph[*vi].get_contracted_vertices() << std::endl;
            os << " out_edges_of(" << g.graph[*vi].id << "):";
            for (const auto &out : out_edges(*vi, g.graph)) {
                os << " " << g.graph[*out].id
                    << "=(" << g.graph[g.source(*out)].id
                    << ", " << g.graph[g.target(*out)].id << ") = "
                    << g.graph[*out].cost << "\t";
            }
            os << std::endl;
        }
        return os;
    }

 private:
    int64_t min_edge_id;
    Identifiers<V> forbidden_vertices;
};
}  // namespace graph
}  // namespace pgrouting

#endif  // INCLUDE_CONTRACTION_CONTRACTIONGRAPH_HPP_
