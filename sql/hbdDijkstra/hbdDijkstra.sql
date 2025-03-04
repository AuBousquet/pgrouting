/*PGR-GNU*****************************************************************

Copyright (c) 2017 pgRouting developers
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


-------------------
-- pgr_hbd_dijkstra
-------------------
--v4.0
CREATE FUNCTION pgr_hbd_dijkstra(
    TEXT,     -- edges_sql (required)
    TEXT,     -- vertices_sql (required)
    TEXT,     -- combinations_sql (required)

    directed BOOLEAN DEFAULT TRUE,

    OUT seq INTEGER,
    OUT path_seq INTEGER,
    OUT start_vid BIGINT,
    OUT end_vid BIGINT,
    OUT node BIGINT,
    OUT edge BIGINT,
    OUT cost FLOAT,
    OUT agg_cost FLOAT)
RETURNS SETOF RECORD AS
$BODY$
    SELECT seq, path_seq, start_vid, end_vid, node, edge, cost, agg_cost
    FROM _pgr_hbd_dijkstra(_pgr_get_statement($1), _pgr_get_statement($2), directed, false);
$BODY$
LANGUAGE SQL VOLATILE STRICT
COST 100
ROWS 1000;


-- COMMENTS
COMMENT ON FUNCTION pgr_hbd_dijkstra(TEXT, TEXT, TEXT, BOOLEAN)
IS 'pgr_hbd_dijkstra(Combinations)
- Parameters:
  - Edges SQL with columns: id, source, target, cost [,reverse_cost]
  - Vertices SQL with columns: id, order
  - Combinations SQL with columns: source, target
- Optional Parameters
  - directed := true
- Documentation:
  - ${PROJECT_DOC_LINK}/pgr_hbd_dijkstra.html
';
