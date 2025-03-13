/*PGR-GNU*****************************************************************

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

-----------------------------
-- pgr_bdDijkstraCostMatrix
-----------------------------

CREATE FUNCTION pgr_hbd_dijkstra_cost_matrix(
    TEXT,     -- edges_sql (required)
    TEXT,     -- vertices_sql (required)
    ANYARRAY, -- vids (required)

    directed BOOLEAN DEFAULT true,

    OUT start_vid BIGINT,
    OUT end_vid BIGINT,
    OUT agg_cost FLOAT)
RETURNS SETOF RECORD AS
$BODY$
    SELECT a.start_vid, a.end_vid, a.agg_cost
    FROM _pgr_hbd_dijkstra(_pgr_get_statement($1), _pgr_get_statement($1), $2::BIGINT[], $2::BIGINT[], $3, true) a;
$BODY$
LANGUAGE SQL VOLATILE
COST 100
ROWS 1000;

-- COMMENT

COMMENT ON FUNCTION pgr_hbd_dijkstra_cost_matrix(TEXT, TEXT, ANYARRAY, BOOLEAN)
IS 'pgr_hbd_dijkstra_cost_matrix
- Parameters:
    - Edges SQL with columns: id, source, target, cost [,reverse_cost]
    - Vertices SQL with columns: id, order
    - Vertices SQL with columns: id, order
    - ARRAY [vertices identifiers]
- Optional Parameters
    - directed := true
- Documentation:
    - ${PROJECT_DOC_LINK}/pgr_hbd_dijkstra_cost_matrix.html
';
