/*PGR-GNU*****************************************************************
File: withPoints_ksp.c

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
Copyright (c) 2015 Celia Virginia Vergara Castillo
Mail: vicky at erosion.dev

Copyright (c) 2023 Abhinav Jain
Mail: this.abhinav at gmail.com

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

#include <stdbool.h>
#include "c_common/postgres_connection.h"

#include "c_types/path_rt.h"
#include "c_common/time_msg.h"
#include "c_common/e_report.h"

#include "c_common/pgdata_getters.h"

#include "drivers/withPoints/get_new_queries.h"
#include "drivers/yen/withPoints_ksp_driver.h"
#include "c_common/debug_macro.h"

PGDLLEXPORT Datum _pgr_withpointsksp(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(_pgr_withpointsksp);

static
void
process(
        char* edges_sql,
        char* points_sql,
        char* combinations_sql,
        ArrayType *starts,
        ArrayType *ends,

        int64_t* start_pid,
        int64_t* end_pid,

        int p_k,
        char *driving_side,

        bool directed,
        bool heap_paths,
        bool details,

        Path_rt **result_tuples,
        size_t *result_count) {
    if (p_k < 0) {
        return;
    }

    size_t k = (size_t)p_k;

    if (start_pid) {
        driving_side[0] = (char) tolower(driving_side[0]);
        if (!((driving_side[0] == 'r')
                    || (driving_side[0] == 'l'))) {
            driving_side[0] = 'b';
        }
    } else {
        driving_side[0] = (char) tolower(driving_side[0]);
        if (directed) {
            if (!((driving_side[0] == 'r') || (driving_side[0] == 'l'))) {
                throw_error("Invalid value of 'driving side'", "Valid values are for directed graph are: 'r', 'l'");
                return;
            }
        } else if (!(driving_side[0] == 'b')) {
            throw_error("Invalid value of 'driving side'", "Valid values are for undirected graph is: 'b'");
            return;
        }
    }

    pgr_SPI_connect();

    char* log_msg = NULL;
    char* notice_msg = NULL;
    char* err_msg = NULL;

    Point_on_edge_t *points = NULL;
    size_t total_points = 0;
    pgr_get_points(points_sql, &points, &total_points, &err_msg);
    throw_error(err_msg, points_sql);

    char *edges_of_points_query = NULL;
    char *edges_no_points_query = NULL;
    get_new_queries(
            edges_sql, points_sql,
            &edges_of_points_query,
            &edges_no_points_query);

    Edge_t *edges_of_points = NULL;
    size_t total_edges_of_points = 0;
    pgr_get_edges(edges_of_points_query, &edges_of_points, &total_edges_of_points, true, false, &err_msg);
    throw_error(err_msg, edges_of_points_query);

    Edge_t *edges = NULL;
    size_t total_edges = 0;
    pgr_get_edges(edges_no_points_query, &edges, &total_edges, true, false, &err_msg);
    throw_error(err_msg, edges_no_points_query);

    int64_t* start_pidsArr = NULL;
    size_t size_start_pidsArr = 0;
    int64_t* end_pidsArr = NULL;
    size_t size_end_pidsArr = 0;
    II_t_rt *combinationsArr = NULL;
    size_t total_combinations = 0;
    if (start_pid && end_pid) {
        start_pidsArr = start_pid; size_start_pidsArr = 1;
        end_pidsArr = end_pid; size_end_pidsArr = 1;
    } else if (starts && ends) {
        start_pidsArr = pgr_get_bigIntArray(&size_start_pidsArr, starts, false, &err_msg);
        throw_error(err_msg, "While getting start pids");
        end_pidsArr = pgr_get_bigIntArray(&size_end_pidsArr, ends, false, &err_msg);
        throw_error(err_msg, "While getting end pids");
    } else if (combinations_sql) {
        pgr_get_combinations(combinations_sql, &combinationsArr, &total_combinations, &err_msg);
        throw_error(err_msg, combinations_sql);
    }

    pfree(edges_of_points_query);
    pfree(edges_no_points_query);

    if ((total_edges + total_edges_of_points) == 0) {
        if (end_pidsArr) pfree(end_pidsArr);
        if (start_pidsArr) pfree(start_pidsArr);
        if (combinationsArr) pfree(combinationsArr);
        pgr_SPI_finish();
        return;
    }

    if (total_combinations == 0 && (size_start_pidsArr== 0 || size_end_pidsArr == 0)) {
        if (edges) pfree(edges);
        pgr_SPI_finish();
        return;
    }

    clock_t start_t = clock();

    pgr_do_withPointsKsp(
            edges,           total_edges,
            points,          total_points,
            edges_of_points, total_edges_of_points,
            combinationsArr, total_combinations,
            start_pidsArr,   size_start_pidsArr,
            end_pidsArr,     size_end_pidsArr,

            k,

            directed,
            heap_paths,
            driving_side[0],
            details,

            result_tuples,
            result_count,

            &log_msg,
            &notice_msg,
            &err_msg);
    time_msg(" processing withPointsKSP", start_t, clock());

    if (err_msg && (*result_tuples)) {
        pfree(*result_tuples);
        (*result_tuples) = NULL;
        (*result_count) = 0;
    }

    pgr_global_report(log_msg, notice_msg, err_msg);

    if (log_msg) pfree(log_msg);
    if (notice_msg) pfree(notice_msg);
    if (err_msg) pfree(err_msg);
    if (edges) pfree(edges);
    if (start_pid && end_pid) {
        start_pidsArr = NULL;
        end_pidsArr = NULL;
    }
    if (start_pidsArr) pfree(start_pidsArr);
    if (end_pidsArr) pfree(end_pidsArr);
    if (combinationsArr) pfree(combinationsArr);

    pgr_SPI_finish();
}




PGDLLEXPORT Datum _pgr_withpointsksp(PG_FUNCTION_ARGS) {
    FuncCallContext     *funcctx;
    TupleDesc            tuple_desc;

    Path_rt  *result_tuples = 0;
    size_t result_count = 0;

    if (SRF_IS_FIRSTCALL()) {
        MemoryContext   oldcontext;
        funcctx = SRF_FIRSTCALL_INIT();
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        if (PG_NARGS() == 10) {
            process(
                text_to_cstring(PG_GETARG_TEXT_P(0)),
                text_to_cstring(PG_GETARG_TEXT_P(1)),
                NULL,
                PG_GETARG_ARRAYTYPE_P(2),
                PG_GETARG_ARRAYTYPE_P(3),
                NULL, NULL,
                PG_GETARG_INT32(4),
                text_to_cstring(PG_GETARG_TEXT_P(5)),
                PG_GETARG_BOOL(6),
                PG_GETARG_BOOL(7),
                PG_GETARG_BOOL(8),
                &result_tuples,
                &result_count);
        } else if (PG_NARGS() == 8) {
            process(
                text_to_cstring(PG_GETARG_TEXT_P(0)),
                text_to_cstring(PG_GETARG_TEXT_P(1)),
                text_to_cstring(PG_GETARG_TEXT_P(2)),
                NULL, NULL,
                NULL, NULL,
                PG_GETARG_INT32(3),
                text_to_cstring(PG_GETARG_TEXT_P(4)),
                PG_GETARG_BOOL(5),
                PG_GETARG_BOOL(6),
                PG_GETARG_BOOL(7),
                &result_tuples,
                &result_count);
        } else if (PG_NARGS() == 9) {
            /* this is for the old signature */
            int64_t departure = PG_GETARG_INT64(2);
            int64_t destination = PG_GETARG_INT64(3);

            process(
                    text_to_cstring(PG_GETARG_TEXT_P(0)),
                    text_to_cstring(PG_GETARG_TEXT_P(1)),
                    NULL,
                    NULL, NULL,
                    &departure,
                    &destination,
                    PG_GETARG_INT32(4),
                    text_to_cstring(PG_GETARG_TEXT_P(7)),
                    PG_GETARG_BOOL(5),
                    PG_GETARG_BOOL(6),
                    PG_GETARG_BOOL(8),
                    &result_tuples,
                    &result_count);
        }


        funcctx->max_calls = result_count;

        funcctx->user_fctx = result_tuples;
        if (get_call_result_type(fcinfo, NULL, &tuple_desc)
                != TYPEFUNC_COMPOSITE)
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("function returning record called in context "
                         "that cannot accept type record")));

        funcctx->tuple_desc = tuple_desc;
        MemoryContextSwitchTo(oldcontext);
    }

    funcctx = SRF_PERCALL_SETUP();
    tuple_desc = funcctx->tuple_desc;
    result_tuples = (Path_rt*) funcctx->user_fctx;

    if (funcctx->call_cntr < funcctx->max_calls) {
        HeapTuple    tuple;
        Datum        result;
        Datum        *values;
        bool*        nulls;

        size_t n = (PG_NARGS() == 9)? 7 : 9;

        values = palloc(n * sizeof(Datum));
        nulls = palloc(n * sizeof(bool));

        size_t i;
        for (i = 0; i < n; ++i) {
            nulls[i] = false;
        }

        int64_t path_id = 1;
        if (funcctx->call_cntr != 0) {
            if (result_tuples[funcctx->call_cntr - 1].edge == -1) {
                path_id = result_tuples[funcctx->call_cntr - 1].start_id + 1;
            } else {
                path_id = result_tuples[funcctx->call_cntr - 1].start_id;
            }
        }

        values[0] = Int32GetDatum((int32_t)funcctx->call_cntr + 1);
        values[1] = Int32GetDatum((int32_t)path_id);
        values[2] = Int32GetDatum(result_tuples[funcctx->call_cntr].seq);
        if (PG_NARGS() != 9) {
            values[3] = Int64GetDatum(result_tuples[funcctx->call_cntr].start_id);
            values[4] = Int64GetDatum(result_tuples[funcctx->call_cntr].end_id);
        }
        values[n - 4] = Int64GetDatum(result_tuples[funcctx->call_cntr].node);
        values[n - 3] = Int64GetDatum(result_tuples[funcctx->call_cntr].edge);
        values[n - 2] = Float8GetDatum(result_tuples[funcctx->call_cntr].cost);
        values[n - 1] = Float8GetDatum(result_tuples[funcctx->call_cntr].agg_cost);

        result_tuples[funcctx->call_cntr].start_id = path_id;

        tuple = heap_form_tuple(tuple_desc, values, nulls);
        result = HeapTupleGetDatum(tuple);
        SRF_RETURN_NEXT(funcctx, result);
    } else {
        SRF_RETURN_DONE(funcctx);
    }
}
