..
   ****************************************************************************
    pgRouting Manual
    Copyright(c) pgRouting Contributors

    This documentation is licensed under a Creative Commons Attribution-Share
    Alike 3.0 License: https://creativecommons.org/licenses/by-sa/3.0/
   ****************************************************************************

.. index::
   single: Topology Family ; pgr_analyzeGraph - Deprecated since 3.8.0
   single: analyzeGraph - Deprecated since 3.8.0

|

``pgr_analyzeGraph`` -- Deprecated since 3.8.0
===============================================================================

``pgr_analyzeGraph`` — Analyzes the network topology.

.. rubric:: Availability

* Version 3.8.0

  * Deprecated function.

* Version 2.0.0

  * Official function.

.. include:: migration.rst
   :start-after: migrate_pgr_analyzeGraph_start
   :end-before: migrate_pgr_analyzeGraph_end

Description
-------------------------------------------------------------------------------

The function returns:

- ``OK`` after the analysis has finished.
- ``FAIL`` when the analysis was not completed due to an error.

.. admonition:: \ \
   :class: signatures

   | pgr_analyzeGraph(**edge_table**, **tolerance**, [**options**])
   | **options:** ``[the_geom, id, source, target, rows_where]``

   | RETURNS ``VARCHAR``


.. rubric:: Prerequisites

The edge table to be analyzed must contain a source column and a target column
filled with id's of the vertices of the segments and the corresponding vertices
table <edge_table>_vertices_pgr that stores the vertices information.

Parameters
-------------------------------------------------------------------------------

The analyze graph function accepts the following parameters:

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
:rows_where: ``text`` Condition to select a subset or rows. Default value is
             ``true`` to indicate all rows.

The function returns:

- ``OK`` after the analysis has finished.

  * Uses the vertices table: <edge_table>_vertices_pgr.
  * Fills completely the ``cnt`` and ``chk`` columns of the vertices table.
  * Returns the analysis of the section of the network defined by ``rows_where``

- ``FAIL`` when the analysis was not completed due to an error.

  * The vertices table is not found.
  * A required column of the Network table is not found or is not of the appropriate type.
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
      vertex as incoming.
:eout: ``integer`` Number of vertices in the edge_table that reference this
       vertex as outgoing.
:the_geom: ``geometry`` Point geometry of the vertex.

Usage when the edge table's columns MATCH the default values:
...............................................................................

.. rubric:: The simplest way to use pgr_analyzeGraph is:

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q1
   :end-before: -- q1.1

.. rubric:: Arguments are given in the order described in the parameters:

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q3
   :end-before: -- q3.1

We get the same result as the simplest way to use the function.

.. warning:: An error would occur when

   the arguments are not given in the appropriate order:

   In this example, the column ``id`` of the table ``mytable``
   is passed to the function as the geometry column, and the geometry column
   ``the_geom`` is passed to the function as the id column.

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q5
   :end-before: -- q5.1


.. rubric:: When using the named notation

The order of the parameters do not matter:

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q6
   :end-before: -- q6.1

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q7
   :end-before: -- q7.1

Parameters defined with a default value can be omitted, as long as the value
matches the default:

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q8
   :end-before: -- q8.1

.. rubric:: Selecting rows using rows_where parameter

Selecting rows based on the id. Displays the analysis a the section of the
network.

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q9
   :end-before: -- q9.1

Selecting the rows where the geometry is near the geometry of row with ``id`` =
5

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q10
   :end-before: -- q10.1

Selecting the rows where the geometry is near the geometry of the row with
``gid`` =100 of the table ``othertable``.

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q11
   :end-before: -- q11.1

Usage when the edge table's columns DO NOT MATCH the default values:
...............................................................................

For the following table

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q12
   :end-before: -- q12.1

.. rubric:: Using positional notation:

The arguments need to be given in the order described in the parameters:

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q13
   :end-before: -- q13.1

.. warning:: An error would occur when
   the arguments are not given in the appropriate order: In this example, the
   column ``gid`` of the table ``mytable`` is passed to the function as the
   geometry column, and the geometry column ``mygeom`` is passed to the function
   as the id column.

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q14
   :end-before: -- q14.1

.. rubric:: When using the named notation

The order of the parameters do not matter:

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q15
   :end-before: -- q15.1

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q16
   :end-before: -- q16.1

In this scenario omitting a parameter would create an error because the default
values for the column names do not match the column names of the table.

.. rubric:: Selecting rows using rows_where parameter

Selecting rows based on the id.

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q17
   :end-before: -- q17.1

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q18
   :end-before: -- q18.1

Selecting the rows WHERE the geometry is near the geometry of row with ``id`` =5 .

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q19
   :end-before: -- q19.1

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q20
   :end-before: -- q20.1

Selecting the rows WHERE the geometry is near the place='myhouse' of the table
``othertable``. (note the use of quote_literal)

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q21
   :end-before: -- q21.1

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q22
   :end-before: -- q22.1

Additional Examples
-------------------------------------------------------------------------------

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q23
   :end-before: -- q23.1

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q24
   :end-before: -- q24.1

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q25
   :end-before: -- q25.1

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q26
   :end-before: -- q26.1

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q27
   :end-before: -- q27.1

.. literalinclude:: analyzeGraph.queries
   :start-after: -- q28
   :end-before: -- q28.1

See Also
-------------------------------------------------------------------------------

* :doc:`sampledata`
* :doc:`topology-functions`
* :doc:`pgr_analyzeOneWay`
* :doc:`pgr_nodeNetwork` to create nodes to a not noded edge table.

.. rubric:: Indices and tables

* :ref:`genindex`
* :ref:`search`
