/*PGR-GNU*****************************************************************
File: bdDijkstra_driver.cpp

Generated with Template by:
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


#include <sstream>
#include <deque>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <set>


#include "cpp_common/combinations.hpp"
#include "cpp_common/pgdata_getters.hpp"
#include "cpp_common/alloc.hpp"
#include "cpp_common/assert.hpp"
#include "cpp_common/ch_graphs.hpp"
#include "cpp_common/combinations.hpp"
#include "cpp_common/pgdata_getters.hpp"
#include "cpp_common/to_postgres.hpp"
#include "drivers/hbdDijkstra/hbdDijkstra_driver.h"


void pgr_hbd_dijkstra(
        char *edges_sql,
        char *vertices_sql,
        char *combinations_sql,
        ArrayType *starts,
        ArrayType *ends,
        bool directed,
        bool only_cost,
        Path_rt **return_tuples,
        size_t *return_count,
        char **log_msg,
        char **notice_msg,
        char **err_msg) {
    using pgrouting::Path;
    using pgrouting::pgr_alloc;
    using pgrouting::to_pg_msg;
    using pgrouting::pgr_free;
    using pgrouting::utilities::get_combinations;

    std::ostringstream log;
    std::ostringstream err;
    std::ostringstream notice;
    char *hint = nullptr;

    try {
        pgassert(!(*log_msg));
        pgassert(!(*notice_msg));
        pgassert(!(*err_msg));
        pgassert(!(*return_tuples));
        pgassert(*return_count == 0);

        // origins and destinations extraction
        hint = combinations_sql;
        auto combinations = get_combinations(combinations_sql, starts, ends, true);
        hint = nullptr;

        if (combinations.empty() && combinations_sql) {
            *notice_msg = to_pg_msg("No (source, target) pairs found");
            *log_msg = to_pg_msg(combinations_sql);
            return;
        }

        // Edges creation
        hint = edges_sql;
        auto edges = pgrouting::pgget::get_edges(std::string(edges_sql), true, false);

        if (edges.empty()) {
            *notice_msg = to_pg_msg("No edges found");
            *log_msg = hint ? to_pg_msg(hint) : to_pg_msg(log);
            return;
        }
        hint = nullptr;

        // Vertices creation
        hint = vertices_sql;
        auto vertices =
            pgrouting::pgget::get_ordered_vertices(std::string(vertices_sql));

        if (vertices.empty()) {
            *notice_msg = to_pg_msg("No vertices found");
            *log_msg = hint ? to_pg_msg(hint) : to_pg_msg(log);
            return;
        }
        hint = nullptr;

        // Graph creation and shortest path search
        std::deque<Path> paths;

        if (directed) {
            pgrouting::graph::CHDirectedGraph graph;
            graph.insert_edges(edges);
            graph.cp_vertices_order(vertices);
            paths =
                detail::perform_hbd_dijkstra
                    <typename pgrouting::graph::CHDirectedGraph>(
                    graph, combinations, only_cost, log);
        } else {
            pgrouting::graph::CHUniqueUndirectedGraph graph;
            graph.insert_edges(edges);
            graph.cp_vertices_order(vertices);
            paths =
                detail::perform_hbd_dijkstra
                    <typename pgrouting::graph::CHUniqueUndirectedGraph>(
                    graph, combinations, only_cost, log);
        }
        auto count = count_tuples(paths);

        if (count == 0) {
            (*return_tuples) = NULL;
            (*return_count) = 0;
            notice << "No paths found";
            *log_msg = to_pg_msg(notice);
            return;
        }

        (*return_tuples) = pgr_alloc(count, (*return_tuples));
        (*return_count) = (collapse_paths(return_tuples, paths));

        pgassert(err.str().empty());
        *log_msg = log.str().empty()?
            *log_msg :
            to_pg_msg(log);
        *notice_msg = notice.str().empty()?
            *notice_msg :
            to_pg_msg(notice);
    } catch (AssertFailedException &except) {
        (*return_tuples) = pgr_free(*return_tuples);
        (*return_count) = 0;
        err << except.what();
        *err_msg = to_pg_msg(err);
        *log_msg = to_pg_msg(log);
    } catch (const std::string &ex) {
        *err_msg = to_pg_msg(ex);
        *log_msg = hint? to_pg_msg(hint) : to_pg_msg(log);
    } catch (std::exception &except) {
        (*return_tuples) = pgr_free(*return_tuples);
        (*return_count) = 0;
        err << except.what();
        *err_msg = to_pg_msg(err);
        *log_msg = to_pg_msg(log);
    } catch(...) {
        (*return_tuples) = pgr_free(*return_tuples);
        (*return_count) = 0;
        err << "Caught unknown exception!";
        *err_msg = to_pg_msg(err);
        *log_msg = to_pg_msg(log);
    }
}
