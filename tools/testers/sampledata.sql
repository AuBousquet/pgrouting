------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
--              SAMPLE DATA
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
-- CopyRight(c) pgRouting developers
-- Creative Commons Attribution-Share Alike 3.0 License : https://creativecommons.org/licenses/by-sa/3.0/

DROP TABLE IF EXISTS edges;
DROP TABLE IF EXISTS vertices;
DROP table if exists pointsOfInterest;
DROP TABLE IF EXISTS old_restrictions;
DROP TABLE IF EXISTS restrictions;
DROP TABLE IF EXISTS combinations;
DROP TABLE IF EXISTS vehicles;
DROP TABLE IF EXISTS orders;


/* --EDGE TABLE CREATE start */
CREATE TABLE edges (
    id BIGSERIAL PRIMARY KEY,
    source BIGINT,
    target BIGINT,
    cost FLOAT,
    reverse_cost FLOAT,
    capacity BIGINT,
    reverse_capacity BIGINT,
    x1 FLOAT,
    y1 FLOAT,
    x2 FLOAT,
    y2 FLOAT,
    geom geometry
);
/* --EDGE TABLE CREATE end */
/* --EDGE TABLE ADD DATA start */
INSERT INTO edges (
    cost, reverse_cost,
    capacity, reverse_capacity, geom) VALUES
( 1,  1,  80, 130,   ST_MakeLine(ST_POINT(2, 0), ST_POINT(2, 1))),
(-1,  1,  -1, 100,   ST_MakeLine(ST_POINT(2, 1), ST_POINT(3, 1))),
(-1,  1,  -1, 130,   ST_MakeLine(ST_POINT(3, 1), ST_POINT(4, 1))),
( 1,  1, 100,  50,   ST_MakeLine(ST_POINT(2, 1), ST_POINT(2, 2))),
( 1, -1, 130,  -1,   ST_MakeLine(ST_POINT(3, 1), ST_POINT(3, 2))),
( 1,  1,  50, 100,   ST_MakeLine(ST_POINT(0, 2), ST_POINT(1, 2))),
( 1,  1,  50, 130,   ST_MakeLine(ST_POINT(1, 2), ST_POINT(2, 2))),
( 1,  1, 100, 130,   ST_MakeLine(ST_POINT(2, 2), ST_POINT(3, 2))),
( 1,  1, 130,  80,   ST_MakeLine(ST_POINT(3, 2), ST_POINT(4, 2))),
( 1,  1, 130,  50,   ST_MakeLine(ST_POINT(2, 2), ST_POINT(2, 3))),
( 1, -1, 130,  -1,   ST_MakeLine(ST_POINT(3, 2), ST_POINT(3, 3))),
( 1, -1, 100,  -1,   ST_MakeLine(ST_POINT(2, 3), ST_POINT(3, 3))),
( 1, -1, 100,  -1,   ST_MakeLine(ST_POINT(3, 3), ST_POINT(4, 3))),
( 1,  1,  80, 130,   ST_MakeLine(ST_POINT(2, 3), ST_POINT(2, 4))),
( 1,  1,  80,  50,   ST_MakeLine(ST_POINT(4, 2), ST_POINT(4, 3))),
( 1,  1,  80,  80,   ST_MakeLine(ST_POINT(4, 1), ST_POINT(4, 2))),
( 1,  1, 130, 100,   ST_MakeLine(ST_POINT(0.5, 3.5), ST_POINT(1.999999999999, 3.5))),
( 1,  1,  50, 130,   ST_MakeLine(ST_POINT(3.5, 2.3), ST_POINT(3.5, 4)));
/* --EDGE TABLE ADD DATA end */

/* -- q1 */
SELECT * INTO vertices
FROM pgr_extractVertices('SELECT id, geom FROM edges ORDER BY id');
/* -- q1-1 */
CREATE SEQUENCE vertices_id_seq;
ALTER TABLE vertices ALTER COLUMN id SET DEFAULT nextval('vertices_id_seq');
ALTER SEQUENCE vertices_id_seq OWNED BY vertices.id;
SELECT setval('vertices_id_seq', (SELECT coalesce(max(id)) FROM vertices));
/* -- q1-2 */
\d vertices
/* -- q2 */
SELECT * FROM vertices;
/* -- q3 */
/* -- set the source information */
UPDATE edges AS e
SET source = v.id, x1 = x, y1 = y
FROM vertices AS v
WHERE ST_StartPoint(e.geom) = v.geom;

/* -- set the target information */
UPDATE edges AS e
SET target = v.id, x2 = x, y2 = y
FROM vertices AS v
WHERE ST_EndPoint(e.geom) = v.geom;
/* -- q4 */
SELECT id, source, target
FROM edges ORDER BY id;
/* -- q5 */

/* --POINTS CREATE start */

/* -- p1 */
CREATE TABLE pointsOfInterest(
    pid BIGSERIAL PRIMARY KEY,
    edge_id BIGINT,
    side CHAR,
    fraction FLOAT,
    geom geometry);
/* -- p2 */
INSERT INTO pointsOfInterest (edge_id, side, fraction, geom) VALUES
(1, 'l' , 0.4, ST_POINT(1.8, 0.4)),
(15, 'r' , 0.4, ST_POINT(4.2, 2.4)),
(12, 'l' , 0.6, ST_POINT(2.6, 3.2)),
(6, 'r' , 0.3, ST_POINT(0.3, 1.8)),
(5, 'l' , 0.8, ST_POINT(2.9, 1.8)),
(4, 'b' , 0.7, ST_POINT(2.2, 1.7));
/* -- p3 */
/* --POINTS CREATE end */

/* --COMBINATIONS CREATE start */
/* -- c1 */
CREATE TABLE combinations (
    source BIGINT,
    target BIGINT
);
/* -- c2 */
INSERT INTO combinations (
    source, target) VALUES
(5, 6),
(5, 10),
(6, 5),
(6, 15),
(6, 14);
/* -- c3 */
SELECT * FROM combinations;
/* -- c4 */
/* --COMBINATIONS CREATE end */

/* --RESTRICTIONS CREATE start */
/* -- r1 */
CREATE TABLE restrictions (
    id SERIAL PRIMARY KEY,
    path BIGINT[],
    cost FLOAT
);
/* -- r2 */
INSERT INTO restrictions (path, cost) VALUES
(ARRAY[4, 7], 100),
(ARRAY[8, 11], 100),
(ARRAY[7, 10], 100),
(ARRAY[3, 5, 9], 4),
(ARRAY[9, 16], 100);
/* -- r3 */
SELECT * FROM restrictions;
/* -- r4 */
/* --RESTRICTIONS CREATE end */


/* --VEHICLES TABLE START */

CREATE TABLE vehicles (
  id BIGSERIAL PRIMARY KEY,
  capacity FLOAT,
  start_node_id BIGINT,
  start_x FLOAT,
  start_y FLOAT,
  start_open FLOAT,
  start_close FLOAT
);

INSERT INTO vehicles
(start_node_id, start_x,  start_y,  start_open,  start_close, capacity) VALUES
(            11,       3,        2,           0,           50,       50),
(            11,       3,        2,           0,           50,       50);

/* --VEHICLES TABLE END */



/* --ORDERS TABLE START */
CREATE TABLE orders (
    id BIGSERIAL PRIMARY KEY,
    demand FLOAT,
    /* the pickups */
    p_node_id BIGINT,
    p_x FLOAT,
    p_y FLOAT,
    p_open FLOAT,
    p_close FLOAT,
    p_service FLOAT,
    /* the deliveries */
    d_node_id BIGINT,
    d_x FLOAT,
    d_y FLOAT,
    d_open FLOAT,
    d_close FLOAT,
    d_service FLOAT
);


INSERT INTO orders
(demand,
    p_node_id,  p_x, p_y,  p_open,  p_close,  p_service,
    d_node_id,  d_x, d_y,  d_open,  d_close,  d_service) VALUES
(10,
            10,    3,   1,      2,         10,          3,
            3,    1,   2,      6,         15,          3),
(20,
            16,    4,   2,      4,         15,          2,
            15,    4,   1,      6,         20,          3),
(30,
            7,    2,   2,      2,         10,          3,
           12,    3,   3,      3,         20,          3);


/* --ORDERS TABLE END */


CREATE TABLE old_restrictions (
    rid BIGINT NOT NULL,
    to_cost FLOAT,
    target_id BIGINT,
    via_path TEXT
);
/* --rest01 */
INSERT INTO old_restrictions (rid, to_cost, target_id, via_path) VALUES
(1, 100,  7,  '4'),
(1, 100, 11,  '8'),
(1, 100, 10,  '7'),
(2,   4,  9,  '5, 3'),
(3, 100,  9, '16');



------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
--              SAMPLE DATA
------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------
-- CopyRight(c) pgRouting developers
-- Creative Commons Attribution-Share Alike 3.0 License : https://creativecommons.org/licenses/by-sa/3.0/

DROP TABLE IF EXISTS edges_2;
DROP TABLE IF EXISTS vertices_2;

create table vertices_2 (
    id integer primary key,
    x double precision,
    y double precision,
    geom Geometry(Point)
);

create table edges_2 (
    id bigserial primary key,
    source bigint,
    target bigint,
    cost float,
    reverse_cost float,
    geom geometry(Linestring)
);


/*---------------------------------------------------------------

 1_______11_____9_____7
 | \      \    / \   /|
 |  \      \  /   \ / |
 |   3______10_____8  |
 |  /  \   / |      \ |
 | /    \ /  |       \|
 2/______4___5________6



A-K = 1-11 = cost 3 ;
K-I = 11-9 = cost 6 ;
A-C = 1-3 = cost 5 ;
A-B = 1-2 = cost 3 ;
B-D = 2-4 = cost 5 ;
B-C = 2-3 = cost 3 ;
C-D = 3-4 = cost 2 ;
D-E = 4-5 = cost 7 ;
E-F = 5-6 = cost 6 ;
C-J = 3-10 = cost 2 ;
J-D = 10-4 = cost 4 ;
J-E = 10-5 = cost 3 ;
J-K = 10-11 = cost 3 ;
J-I = 10-9 = cost 4 ;
J-H = 10-8 = cost 2 ;
H-F = 8-6 = cost 2 ;
H-G = 8-7 = cost 3 ;
G-F = 7-6 = cost 4 ;
I-H = 9-8 = cost 3 ;
I-G = 9-7 = cost 5 ;


1(0,2)
2(0,0)
3(0.5,1)
4(1,0)
5(1.5,0)
6(3,0)
7(3,1)
8(2.5,1)
9(2,2)
10(1.5,1)
11(1,2)


*/

insert into edges_2 (cost, reverse_cost, geom) 
values
    ( 3, 3, ST_MakeLine(ST_Point(0,2), ST_Point(1,2)) ), --1-11
    ( 6, 6, ST_MakeLine(ST_Point(1,2), ST_Point(2,2)) ), --11-9
    ( 5, 5, ST_MakeLine(ST_Point(0,2), ST_Point(0.5,1)) ), --1-3
    ( 3, 3, ST_MakeLine(ST_Point(0,2), ST_Point(0,0)) ), --1-2
    ( 5, 5, ST_MakeLine(ST_Point(0,0), ST_Point(1,0)) ), --2-4
    ( 3, 3, ST_MakeLine(ST_Point(0,0), ST_Point(0.5,1)) ), --2-3
    ( 2, 2, ST_MakeLine(ST_Point(0.5,1), ST_Point(1,0)) ), --3-4
    ( 7, 7, ST_MakeLine(ST_Point(1,0), ST_Point(1.5,0)) ), --4-5
    ( 6, 6, ST_MakeLine(ST_Point(1.5,0), ST_Point(3,0)) ), --5-6
    ( 2, 2, ST_MakeLine(ST_Point(0.5,1), ST_Point(1.5,1)) ), --3-10
    ( 4, 4, ST_MakeLine(ST_Point(1.5,1), ST_Point(1,0)) ), --10-4
    ( 3, 3, ST_MakeLine(ST_Point(1.5,1), ST_Point(1.5,0)) ), --10-5
    ( 3, 3, ST_MakeLine(ST_Point(1.5,1), ST_Point(1,2)) ), --10-11
    ( 4, 4, ST_MakeLine(ST_Point(1.5,1), ST_Point(2,2)) ), --10-9
    ( 2, 2, ST_MakeLine(ST_Point(1.5,1), ST_Point(2.5,1)) ), --10-8
    ( 2, 2, ST_MakeLine(ST_Point(2.5,1), ST_Point(3,0)) ), --8-6
    ( 3, 3, ST_MakeLine(ST_Point(2.5,1), ST_Point(3,1)) ), --8-7
    ( 4, 4, ST_MakeLine(ST_Point(3,1), ST_Point(3,0)) ), --7-6
    ( 5, 5, ST_MakeLine(ST_Point(2,2), ST_Point(3,1)) ), --9-7
    ( 3, 3, ST_MakeLine(ST_Point(2,2), ST_Point(2.5,1)) ) --9-8
;

/* -- q1 */
SELECT * INTO vertices_2
FROM pgr_extractVertices('SELECT id, geom FROM edges_2 ORDER BY id');

/* -- q1-1 */
CREATE SEQUENCE vertices_2_id_seq;

ALTER TABLE vertices_2
ALTER COLUMN id SET DEFAULT nextval('vertices_2_id_seq');

ALTER SEQUENCE vertices_2_id_seq
OWNED BY vertices_2.id;

SELECT setval('vertices_2_id_seq', (SELECT coalesce(max(id)) FROM vertices_2));

/* -- q1-2 */
\d vertices_2

/* -- q2 */
SELECT *
FROM vertices_2;

/* -- q3 */
/* -- set the source information */
UPDATE edges_2 AS e
SET
    source = v.id,
    x1 = x,
    y1 = y
FROM vertices_2 AS v
WHERE ST_StartPoint(e.geom) = v.geom;

/* -- set the target information */
UPDATE edges_2 AS e
SET 
    target = v.id,
    x2 = x,
    y2 = y
FROM vertices_2 AS v
WHERE ST_EndPoint(e.geom) = v.geom;

/* -- q4 */
SELECT id, source, target
FROM edges_2
ORDER BY id;
/* -- q5 */

