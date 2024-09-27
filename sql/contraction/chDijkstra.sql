/*PGR-GNU*****************************************************************

Copyright (c) Oslandia
Mail: contact@oslandia.com

Copyright (c) Aurélie Bousquet
mail: aurelie.bousquet@oslandia.com

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
-- pgr_chDijkstra
-------------------


-- ONE TO ONE
--v2.6
CREATE FUNCTION pgr_chDijkstra(
    TEXT,   -- edges_sql (required)
    BIGINT, -- from_vid
    BIGINT, -- to_vid

    directed BOOLEAN DEFAULT true,

    OUT seq INTEGER,
    OUT path_seq INTEGER,
    OUT node BIGINT,
    OUT edge BIGINT,
    OUT cost FLOAT,
    OUT agg_cost FLOAT)
RETURNS SETOF RECORD AS
$BODY$
    SELECT seq, path_seq, node, edge, cost, agg_cost
    FROM _pgr_chDijkstra(_pgr_get_statement($1), ARRAY[$2]::BIGINT[], ARRAY[$3]::BIGINT[], $4, false);
$BODY$
LANGUAGE sql VOLATILE STRICT
COST 100
ROWS 1000;


-- ONE TO MANY
--v2.6
CREATE FUNCTION pgr_chDijkstra(
    TEXT,    -- edges_sql (required)
    BIGINT,   -- from_vid (required)
    ANYARRAY, -- to_vids (required)

    directed BOOLEAN DEFAULT TRUE,

    OUT seq INTEGER,
    OUT path_seq INTEGER,
    OUT end_vid BIGINT,
    OUT node BIGINT,
    OUT edge BIGINT,
    OUT cost FLOAT,
    OUT agg_cost FLOAT)
RETURNS SETOF RECORD AS
$BODY$
    SELECT seq, path_seq, end_vid, node, edge, cost, agg_cost
    FROM _pgr_chDijkstra(_pgr_get_statement($1), ARRAY[$2]::BIGINT[], $3::BIGINT[], $4, false);
$BODY$
LANGUAGE sql VOLATILE STRICT
COST 100
ROWS 1000;


-- MANY TO ONE
--v2.6
CREATE FUNCTION pgr_chDijkstra(
    TEXT,    -- edges_sql (required)
    ANYARRAY, -- from_vids (required)
    BIGINT,   -- to_vid (required)

    directed BOOLEAN DEFAULT TRUE,

    OUT seq INTEGER,
    OUT path_seq INTEGER,
    OUT start_vid BIGINT,
    OUT node BIGINT,
    OUT edge BIGINT,
    OUT cost FLOAT,
    OUT agg_cost FLOAT)
RETURNS SETOF RECORD AS
$BODY$
    SELECT seq, path_seq, start_vid, node, edge, cost, agg_cost
    FROM _pgr_chDijkstra(_pgr_get_statement($1), $2::BIGINT[], ARRAY[$3]::BIGINT[], $4, false);
$BODY$
LANGUAGE sql VOLATILE STRICT
COST 100
ROWS 1000;



-- MANY TO MANY
--v2.6
CREATE FUNCTION pgr_chDijkstra(
    TEXT,     -- edges_sql (required)
    ANYARRAY, -- from_vids (required)
    ANYARRAY, -- to_vids (required)

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
    FROM _pgr_chDijkstra(_pgr_get_statement($1), $2::BIGINT[], $3::BIGINT[], directed, false);
$BODY$
LANGUAGE SQL VOLATILE STRICT
COST 100
ROWS 1000;


-- COMBINATIONS
--v3.2
CREATE FUNCTION pgr_chDijkstra(
    TEXT,     -- edges_sql (required)
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
    FROM _pgr_chDijkstra(_pgr_get_statement($1), _pgr_get_statement($2), directed, false);
$BODY$
LANGUAGE SQL VOLATILE STRICT
COST 100
ROWS 1000;


-- COMMENTS

COMMENT ON FUNCTION pgr_chDijkstra(TEXT, BIGINT, BIGINT, BOOLEAN)
IS 'pgr_chDijkstra(One to One)
- Parameters:
  - edges SQL with columns: id, source, target, source_order, target_order, cost [,reverse_cost]
  - From vertex identifier
  - To vertex identifier
- Optional Parameters:
  - directed := true
- Documentation:
  - ${PROJECT_DOC_LINK}/pgr_chDijkstra.html
';

COMMENT ON FUNCTION pgr_chDijkstra(TEXT, BIGINT, ANYARRAY, BOOLEAN)
IS 'pgr_chDijkstra(One to Many)
- Parameters:
  - Edges SQL with columns: id, source, target, source_order, target_order, cost [,reverse_cost]
  - From vertex identifier
  - To ARRAY[vertices identifiers]
- Optional Parameters
  - directed := true
- Documentation:
  - ${PROJECT_DOC_LINK}/pgr_chDijkstra.html
';

COMMENT ON FUNCTION pgr_chDijkstra(TEXT, ANYARRAY, BIGINT, BOOLEAN)
IS 'pgr_chDijkstra(Many to One)
- Parameters:
  - Edges SQL with columns: id, source, target, source_order, target_order, cost [,reverse_cost]
  - From ARRAY[vertices identifiers]
  - To vertex identifier
- Optional Parameters
  - directed := true
- Documentation:
  - ${PROJECT_DOC_LINK}/pgr_chDijkstra.html
';

COMMENT ON FUNCTION pgr_chDijkstra(TEXT, ANYARRAY, ANYARRAY, BOOLEAN)
IS 'pgr_chDijkstra(Many to Many)
- Parameters:
  - Edges SQL with columns: id, source, target, source_order, target_order, cost [,reverse_cost]
  - From ARRAY[vertices identifiers]
  - To ARRAY[vertices identifiers]
- Optional Parameters
  - directed := true
- Documentation:
  - ${PROJECT_DOC_LINK}/pgr_chDijkstra.html
';

COMMENT ON FUNCTION pgr_chDijkstra(TEXT, TEXT, BOOLEAN)
IS 'pgr_chDijkstra(Combinations)
- Parameters:
  - Edges SQL with columns: id, source, target, source_order, target_order, cost [,reverse_cost]
  - Combinations SQL with columns: source, target
- Optional Parameters
  - directed := true
- Documentation:
  - ${PROJECT_DOC_LINK}/pgr_chDijkstra.html
';
