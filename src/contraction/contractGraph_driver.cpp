/*PGR-GNU*****************************************************************
File: contractGraph_driver.cpp

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
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

#include "drivers/contraction/contractGraph_driver.h"

#include <algorithm>
#include <deque>
#include <sstream>
#include <string>
#include <vector>

#include "cpp_common/pgdata_getters.hpp"
#include "contraction/ch_graph.hpp"
#include "contraction/contract.hpp"
#include "contraction/contractionGraph.hpp"

#include "c_types/contracted_rt.h"
#include "cpp_common/identifiers.hpp"
#include "cpp_common/alloc.hpp"

namespace {

template <typename G>
void process_contraction(
    G &graph,
    const std::vector< Edge_t > &edges,
    const std::vector< int64_t > &forbidden_vertices,
    const std::vector< int64_t > &methods_sequence,
    int64_t max_cycles
) {
    graph.insert_edges(edges);
    pgrouting::Identifiers<typename G::V> forbid_vertices;
    for (const auto &vertex : forbidden_vertices) {
        if (graph.has_vertex(vertex)) {
            forbid_vertices += graph.get_V(vertex);
        }
    }
    /*
    * Function call to get the contracted graph
    */
    using Contract = pgrouting::contraction::Pgr_contract<G>;
    Contract result(
        graph,
        forbid_vertices,
        methods_sequence,
        max_cycles);
}

template <typename G>
void get_postgres_result(
    G &graph,
    contracted_rt **return_tuples,
    size_t *count
)
{
    using pgrouting::pgr_alloc;

    auto modified_vertices(graph.get_modified_vertices());
    auto shortcut_edges(graph.get_shortcuts());

    (*count) = modified_vertices.size() + shortcut_edges.size();
    (*return_tuples) = pgr_alloc((*count), (*return_tuples));
    size_t sequence = 0;

    for (const auto id : modified_vertices) {
        auto v = graph.get_V(id);
        int64_t* contracted_vertices = NULL;
        auto vids = graph[v].get_contracted_vertices();

        contracted_vertices = pgr_alloc(vids.size(), contracted_vertices);

        int count = 0;
        for (const auto id : vids) {
            contracted_vertices[count++] = id;
        }
        (*return_tuples)[sequence] = {
            const_cast<char*>("v"),
            id,
            contracted_vertices,
            -1, 
            -1, 
            -1.00, 
            count
        };
        ++sequence;
    }

    int64_t eid = 0;
    for (auto e : shortcut_edges) {
        auto edge = graph[e];
        int64_t* contracted_vertices = NULL;

        const auto vids(edge.get_contracted_vertices());
        pgassert(!vids.empty());

        contracted_vertices = pgr_alloc(vids.size(), contracted_vertices);
        int count = 0;
        for (const auto vid : vids) {
            contracted_vertices[count++] = vid;
        }
        (*return_tuples)[sequence] = {
            const_cast<char*>("e"),
            --eid,
            contracted_vertices, 
            edge.source, 
            edge.target, 
            edge.cost,
            count
        };
        ++sequence;
    }
}

}  // namespace



void
pgr_do_contractGraph(
        char *edges_sql,
        ArrayType* forbidden,
        ArrayType* order,
        int64_t max_cycles,
        bool directed,
        contracted_rt **return_tuples,
        size_t *return_count,
        char **log_msg,
        char **notice_msg,
        char **err_msg
    ) 
{
    using pgrouting::pgr_alloc;
    using pgrouting::pgr_msg;
    using pgrouting::pgr_free;
    using pgrouting::pgget::get_intArray;
    using pgrouting::pgget::get_edges;

    std::ostringstream log;
    std::ostringstream notice;
    std::ostringstream err;
    char *hint = nullptr;

    try {
        pgassert(max_cycles != 0);
        pgassert(!(*log_msg));
        pgassert(!(*notice_msg));
        pgassert(!(*err_msg));
        pgassert(!(*return_tuples));
        pgassert(*return_count == 0);

        hint = edges_sql;
        auto edges = get_edges(std::string(edges_sql), true, false);
        if (edges.empty()) {
            *notice_msg = pgr_msg("No edges found");
            *log_msg = hint? pgr_msg(hint) : pgr_msg(log.str().c_str());
            return;
        }
        hint = nullptr;

        auto forbid = get_intArray(forbidden, true);
        auto ordering = get_intArray(order, false);

        for (const auto kind : ordering) {
            *log_msg = pgr_msg(log.str().c_str());
            if (!pgrouting::contraction::is_valid_contraction(static_cast<int>(kind))) {
                *err_msg = pgr_msg("Invalid contraction type found");
                *log_msg = pgr_msg(log.str().c_str());
                return;
            }
        }

        if (directed) {
            using DirectedGraph = pgrouting::graph::CHDirectedGraph;
            DirectedGraph digraph;

            process_contraction (
                digraph, 
                edges, 
                forbid, 
                ordering,
                max_cycles);

            get_postgres_result(
                digraph,
                return_tuples,
                return_count
            );

        } else {

            using UndirectedGraph = pgrouting::graph::CHUndirectedGraph;
            UndirectedGraph undigraph;
            process_contraction (
                undigraph, 
                edges, 
                forbid, 
                ordering,
                max_cycles);

            get_postgres_result(
                undigraph,
                return_tuples,
                return_count
            );

        }

        *log_msg = log.str().empty()?
            *log_msg :
            pgr_msg(log.str().c_str());

        *notice_msg = notice.str().empty()?
            *notice_msg :
            pgr_msg(notice.str().c_str());
        
    } catch (AssertFailedException &except) {
        (*return_tuples) = pgr_free(*return_tuples);
        (*return_count) = 0;
        err << except.what();
        *err_msg = pgr_msg(err.str().c_str());
        *log_msg = pgr_msg(log.str().c_str());
    } catch (const std::string &ex) {
        *err_msg = pgr_msg(ex.c_str());
        *log_msg = hint? pgr_msg(hint) : pgr_msg(log.str().c_str());
    } catch (std::exception &except) {
        (*return_tuples) = pgr_free(*return_tuples);
        (*return_count) = 0;
        err << except.what();
        *err_msg = pgr_msg(err.str().c_str());
        *log_msg = pgr_msg(log.str().c_str());
    } catch(...) {
        (*return_tuples) = pgr_free(*return_tuples);
        (*return_count) = 0;
        err << "contractGraph_driver : caught unknown exception!";
        *err_msg = pgr_msg(err.str().c_str());
        *log_msg = pgr_msg(log.str().c_str());
    }
}
