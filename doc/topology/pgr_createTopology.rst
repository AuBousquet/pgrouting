..
   ****************************************************************************
    pgRouting Manual
    Copyright(c) pgRouting Contributors

    This documentation is licensed under a Creative Commons Attribution-Share
    Alike 3.0 License: https://creativecommons.org/licenses/by-sa/3.0/
   ****************************************************************************

.. index::
   single: Topology Family ; pgr_createTopology - Deprecated since v3.8.0
   single: createTopology - Deprecated since v3.8.0

|

``pgr_createTopology`` - Deprecated since v3.8.0
===============================================================================

``pgr_createTopology`` — Builds a network topology based on the geometry
information.

.. rubric:: Availability

* Version 3.8.0

  * Deprecated function.

* Version 2.0.0

  * Official function.
  * Renamed from version 1.x

.. include:: migration.rst
   :start-after: migrate_pgr_createTopology_start
   :end-before: migrate_pgr_createTopology_end

Description
-------------------------------------------------------------------------------

The function returns:

- ``OK`` after the network topology has been built and the vertices table
  created.
- ``FAIL`` when the network topology was not built due to an error.

|Boost| Boost Graph Inside

Signatures
-------------------------------------------------------------------------------

.. admonition:: \ \
   :class: signatures

   | pgr_createTopology(edge_table, tolerance, [**options**])
   | **options:** ``[the_geom, id, source, target, rows_where, clean]``

   | RETURNS ``VARCHAR``

Parameters
-------------------------------------------------------------------------------

The topology creation function accepts the following parameters:

:edge_table: ``text`` Network table name. (may contain the schema name as well)
:tolerance: ``float8`` Snapping tolerance of disconnected edges. (in projection
            unit)
:the_geom: ``text`` Geometry column name of the network table. Default value is
           ``the_geom``.
:id: ``text`` Primary key column name of the network table. Default value is
     ``id``.
:source: ``text`` Source column name of the network table. Default value is
         ``source``.
:target: ``text`` Target column name of the network table. Default value is
         ``target``.
:rows_where: ``text`` Condition to SELECT a subset or rows. Default value is
             ``true`` to indicate all rows that where ``source`` or ``target``
             have a null value, otherwise the condition is used.
:clean: ``boolean`` Clean any previous topology. Default value is ``false``.

.. warning::

    The ``edge_table`` will be affected

    - The ``source`` column values will change.
    - The ``target`` column values will change.

      - An index will be created, if it doesn't exists,
        to speed up the process to the following columns:

        * ``id``
        * ``the_geom``
        * ``source``
        * ``target``

The function returns:

- ``OK`` after the network topology has been built.

  * Creates a vertices table: <edge_table>_vertices_pgr.
  * Fills ``id`` and ``the_geom`` columns of the vertices table.
  * Fills the source and target columns of the edge table referencing the ``id``
    of the vertices table.


- ``FAIL`` when the network topology was not built due to an error:

  * A required column of the Network table is not found or is not of the
    appropriate type.
  * The condition is not well formed.
  * The names of source , target or id are the same.
  * The SRID of the geometry could not be determined.

.. rubric:: The Vertices Table

The structure of the vertices table is:

:id: ``bigint`` Identifier of the vertex.
:cnt: ``integer`` Number of vertices in the edge_table that reference this
      vertex.
:chk: ``integer`` Indicator that the vertex might have a problem.
:ein: ``integer`` Number of vertices in the edge_table that reference this
      vertex AS incoming.
:eout: ``integer`` Number of vertices in the edge_table that reference this
       vertex AS outgoing.
:the_geom: ``geometry`` Point geometry of the vertex.

Usage when the edge table's columns MATCH the default values:
...............................................................................

The simplest way to use pgr_createTopology is:

.. literalinclude:: createTopology.queries
   :start-after: --q1
   :end-before: --q1.1

.. rubric:: When the arguments are given in the order described in the
   parameters:

We get the sameresult as the simplest way to use the function.

.. literalinclude:: createTopology.queries
   :start-after: --q2
   :end-before: --q2.1

.. warning::
    An error would occur when the arguments are not given in the appropriate
    order: In this example, the column ``id`` of the table ``ege_table`` is
    passed to the function as the geometry column, and the geometry column
    ``the_geom`` is passed to the function as the id column.

    .. literalinclude:: createTopology.queries
       :start-after: --q3
       :end-before: --q3.1

.. rubric:: When using the named notation

Parameters defined with a default value can be omitted, as long as the value
matches the default And The order of the parameters would not matter.

.. literalinclude:: createTopology.queries
   :start-after: --q4
   :end-before: --q4.1

.. literalinclude:: createTopology.queries
   :start-after: --q5
   :end-before: --q5.1

.. literalinclude:: createTopology.queries
   :start-after: --q6
   :end-before: --q6.1

.. rubric:: Selecting rows using rows_where parameter

Selecting rows based on the id.

.. literalinclude:: createTopology.queries
   :start-after: --q7
   :end-before: --q7.1

Selecting the rows where the geometry is near the geometry of row with ``id =
5``.

.. literalinclude:: createTopology.queries
   :start-after: --q8
   :end-before: --q8.1

Selecting the rows where the geometry is near the geometry of the row with
``gid`` =100 of the table ``othertable``.

.. literalinclude:: createTopology.queries
   :start-after: --q9
   :end-before: --q9.1

Usage when the edge table's columns DO NOT MATCH the default values:
...............................................................................

For the following table

.. literalinclude:: createTopology.queries
   :start-after: --q10
   :end-before: --q10.1

.. rubric:: Using positional notation:

The arguments need to be given in the order described in the parameters.

Note that this example uses clean flag. So it recreates the whole vertices
table.

.. literalinclude:: createTopology.queries
   :start-after: --q11
   :end-before: --q11.1

.. warning::
   An error would occur when the arguments are not given in the appropiriate
   order: In this example, the column ``gid`` of the table ``mytable`` is passed
   to the function AS the geometry column, and the geometry column ``mygeom`` is
   passed to the function AS the id column.

    .. literalinclude:: createTopology.queries
       :start-after: --q12
       :end-before: --q12.1

.. rubric:: When using the named notation

In this scenario omitting a parameter would create an error because the default
values for the column names do not match the column names of the table.
The order of the parameters do not matter:

.. literalinclude:: createTopology.queries
   :start-after: --q13
   :end-before: --q13.1

.. literalinclude:: createTopology.queries
   :start-after: --q14
   :end-before: --q14.1

.. rubric:: Selecting rows using rows_where parameter

Based on id:

.. literalinclude:: createTopology.queries
   :start-after: --q15
   :end-before: --q15.1

Selecting the rows where the geometry is near the geometry of the row with
``gid`` =100 of the table ``othertable``.

.. literalinclude:: createTopology.queries
   :start-after: --q16
   :end-before: --q16.1

Additional Examples
-------------------------------------------------------------------------------

.. contents::
   :local:


Create a routing topology
...............................................................................

An alternate method to create a routing topology use :doc:`pgr_extractVertices`

.. include:: pgr_extractVertices.rst
   :start-after: create_routing_topology_start
   :end-before: create_routing_topology_end


With full output
...............................................................................

This example start a clean topology, with 5 edges, and then its incremented to
the rest of the edges.

.. literalinclude:: createTopology.queries
   :start-after: --q17
   :end-before: --q17.1

See Also
-------------------------------------------------------------------------------

* :doc:`sampledata`
* :doc:`topology-functions`

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`

