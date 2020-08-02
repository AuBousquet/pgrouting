\i setup.sql

SELECT plan(2);



-- 0 edge, 0 vertex tests

PREPARE q1 AS
SELECT id, source, target, cost, reverse_cost
FROM edge_table
WHERE id > 18;

-- Graph is empty - it has 0 edge and 0 vertex
SELECT is_empty('q1', 'q1: Graph with 0 edge and 0 vertex');

-- 0 edge, 0 vertex tests

PREPARE isPlanar1 AS
SELECT *
FROM pgr_isPlanar(
    'q1'
);

SELECT set_eq('isPlanar1',$$VALUES('f'::bool) $$, '2: Graph with 0 edge and 0 vertex -> Empty row is returned');


SELECT * FROM finish();
ROLLBACK;