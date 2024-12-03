/*PGR-GNU*****************************************************************
File: ch_graph.hpp

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developers:
Copyright (c) 2016 Rohith Reddy
Mail:

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

#ifndef INCLUDE_CONTRACTION_CH_GRAPH_HPP_
#define INCLUDE_CONTRACTION_CH_GRAPH_HPP_
#pragma once


#include <algorithm>
#include <limits>
#include <vector>

#include "contraction/contractionGraph.hpp"
#include "cpp_common/base_graph.hpp"
#include "contraction/ch_vertex.hpp"
#include "contraction/ch_edge.hpp"


namespace pgrouting {
namespace graph {

using CHUndirectedGraph =  contractionGraph < boost::adjacency_list < boost::listS, boost::vecS, boost::undirectedS, CH_vertex, CH_edge >, false >;
using CHDirectedGraph = contractionGraph < boost::adjacency_list < boost::listS, boost::vecS, boost::bidirectionalS, CH_vertex, CH_edge >, true >;

}  // namespace graph
}  // namespace pgrouting

#endif  // INCLUDE_CONTRACTION_CH_GRAPH_HPP_
