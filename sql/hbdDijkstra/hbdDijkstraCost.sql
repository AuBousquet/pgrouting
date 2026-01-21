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


----------------------
-- pgr_hbd_dijkstra_cost
----------------------

--v4.0
-- ONE TO ONE
CREATE FUNCTION pgr_hbd_dijkstra_cost(
    TEXT,   -- edges_sql (required)
    TEXT,   -- vertices_sql (required)
    BIGINT, -- from_vid (required)
    BIGINT, -- to_vid (required)

    directed BOOLEAN DEFAULT true,

    OUT start_vid BIGINT,
    OUT end_vid BIGINT,
    OUT agg_cost FLOAT)
RETURNS SETOF RECORD AS
$BODY$
    SELECT a.start_vid, a.end_vid, a.agg_cost
    FROM _pgr_hbd_dijkstra(_pgr_get_statement($1), _pgr_get_statement($2), ARRAY[$3]::BIGINT[], ARRAY[$4]::BIGINT[], $5, true) AS a;
$BODY$
LANGUAGE sql VOLATILE STRICT
COST 100
ROWS 1000;

COMMENT ON FUNCTION pgr_hbd_dijkstra_cost(TEXT, TEXT, BIGINT, BIGINT, BOOLEAN)
IS 'pgr_hbd_dijkstra_cost(One to One)
- Parameters:
  - Edges SQL with columns: id, source, target, cost [,reverse_cost]
  - Vertices SQL with columns: id, order
  - From vertex identifier
  - To vertex identifier
- Optional Parameters
  - directed := true
- Documentation:
  - ${PROJECT_DOC_LINK}/pgr_hbd_dijkstra_cost.html
';

-- ONE TO MANY
CREATE FUNCTION pgr_hbd_dijkstra_cost(
    TEXT,     -- edges_sql (required)
    TEXT,     -- vertices_sql (required)
    BIGINT,   -- from_vid (required)
    ANYARRAY, -- to_vids (required)

    directed BOOLEAN DEFAULT true,

    OUT start_vid BIGINT,
    OUT end_vid BIGINT,
    OUT agg_cost FLOAT)
RETURNS SETOF RECORD AS
$BODY$
    SELECT a.start_vid, a.end_vid, a.agg_cost
    FROM _pgr_hbd_dijkstra(_pgr_get_statement($1), _pgr_get_statement($2), ARRAY[$3]::BIGINT[], $4::BIGINT[], $5, true) as a;
$BODY$
LANGUAGE sql VOLATILE STRICT
COST 100
ROWS 1000;

COMMENT ON FUNCTION pgr_hbd_dijkstra_cost(TEXT, TEXT, BIGINT, ANYARRAY, BOOLEAN)
IS 'pgr_hbd_dijkstra_cost(One to Many)
- Parameters:
  - Edges SQL with columns: id, source, target, cost [,reverse_cost]
  - Vertices SQL with columns: id, order
  - From vertex identifier
  - To ARRAY[vertices identifiers]
- Optional Parameters
  - directed := true
- Documentation:
  - ${PROJECT_DOC_LINK}/pgr_hbd_dijkstra_cost.html
';

-- MANY TO ONE
CREATE FUNCTION pgr_hbd_dijkstra_cost(
    TEXT,     -- edges_sql (required)
    TEXT,     -- vertices_sql (required)
    ANYARRAY, -- from_vids (required)
    BIGINT,   -- to_vid (required)

    directed BOOLEAN DEFAULT true,

    OUT start_vid BIGINT,
    OUT end_vid BIGINT,
    OUT agg_cost FLOAT)
RETURNS SETOF RECORD AS
$BODY$
    SELECT a.start_vid, a.end_vid, a.agg_cost
    FROM _pgr_hbd_dijkstra(_pgr_get_statement($1), _pgr_get_statement($2), $2::BIGINT[], ARRAY[$3]::BIGINT[], $4, true) as a;
$BODY$
LANGUAGE sql VOLATILE STRICT
COST 100
ROWS 1000;

COMMENT ON FUNCTION pgr_hbd_dijkstra_cost(TEXT, TEXT, ANYARRAY, BIGINT, BOOLEAN)
IS 'pgr_hbd_dijkstra_cost(Many to One)
- Parameters:
  - Edges SQL with columns: id, source, target, cost [,reverse_cost]
  - Vertices SQL with columns: id, order
  - From ARRAY[vertices identifiers]
  - To vertex identifier
- Optional Parameters
  - directed := true
- Documentation:
  - ${PROJECT_DOC_LINK}/pgr_hbd_dijkstra_cost.html
';

-- MANY TO MANY
CREATE FUNCTION pgr_hbd_dijkstra_cost(
    TEXT,     -- edges_sql (required)
    TEXT,     -- vertices_sql (required)
    ANYARRAY, -- from_vids (required)
    ANYARRAY, -- to_vids (required)

    directed BOOLEAN DEFAULT true,

    OUT start_vid BIGINT,
    OUT end_vid BIGINT,
    OUT agg_cost FLOAT)
RETURNS SETOF RECORD AS
$BODY$
    SELECT a.start_vid, a.end_vid, a.agg_cost
    FROM _pgr_hbd_dijkstra(_pgr_get_statement($1), _pgr_get_statement($2), $3::BIGINT[], $4::BIGINT[], directed, true) as a;
$BODY$
LANGUAGE SQL VOLATILE STRICT
COST 100
ROWS 1000;

COMMENT ON FUNCTION pgr_hbd_dijkstra_cost(TEXT, TEXT, ANYARRAY, ANYARRAY, BOOLEAN)
IS 'pgr_bdDijkstraCost(Many to Many)
- Parameters:
  - Edges SQL with columns: id, source, target, cost [,reverse_cost]
  - Vertices SQL with columns: id, order
  - From ARRAY[vertices identifiers]
  - To ARRAY[vertices identifiers]
- Optional Parameters
  - directed := true
- Documentation:
  - ${PROJECT_DOC_LINK}/pgr_hbd_dijkstra_cost.html
';

-- COMBINATIONS
CREATE FUNCTION pgr_hbd_dijkstra_cost(
    TEXT,     -- edges_sql (required)
    TEXT,     -- vertices_sql (required)
    TEXT,     -- combinations_sql (required)

    directed BOOLEAN DEFAULT true,

    OUT start_vid BIGINT,
    OUT end_vid BIGINT,
    OUT agg_cost FLOAT)
RETURNS SETOF RECORD AS
$BODY$
    SELECT a.start_vid, a.end_vid, a.agg_cost
    FROM _pgr_bdDijkstra(_pgr_get_statement($1), _pgr_get_statement($2), directed, true) as a;
$BODY$
LANGUAGE SQL VOLATILE STRICT
COST 100
ROWS 1000;

COMMENT ON FUNCTION pgr_hbd_dijkstra_cost(TEXT, TEXT, TEXT, BOOLEAN)
IS 'pgr_bdDijkstraCost(Combinations)
- Parameters:
  - Edges SQL with columns: id, source, target, cost [,reverse_cost]
  - Vertices SQL with columns: id, order
  - Combinations SQL with columns: source, target
- Optional Parameters
  - directed := true
- Documentation:
  - ${PROJECT_DOC_LINK}/pgr_hbd_dijkstra_cost.html
';
