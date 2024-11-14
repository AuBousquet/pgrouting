/*PGR-GNU*****************************************************************
File: contract.hpp

Generated with Template by:
Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

Function's developer:
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

#ifndef INCLUDE_CONTRACTION_CONTRACT_HPP_
#define INCLUDE_CONTRACTION_CONTRACT_HPP_
#pragma once

#include <deque>
#include <vector>
#include "cpp_common/assert.hpp"

#include "contraction/contractionGraph.hpp"
#include "contraction/ch_graphs.hpp"
#include "contraction/linearContraction.hpp"
#include "contraction/deadEndContraction.hpp"
#include "contraction/contractionsHierarchy.hpp"

namespace pgrouting {
namespace contraction {

bool is_valid_contraction(int number);

template < class G >
class Pgr_contract {
    typedef typename G::V V;

public:
    Pgr_contract(
        G &graph,
        Identifiers<V> forbidden_vertices,
        std::vector<int64_t> contraction_methods,
        int64_t max_cycles,
        std::ostringstream &log,
        std::ostringstream &err
    ) 
    {
        std::deque<int64_t> contraction_methods_deque;
        //  push -1 to indicate the start of the queue
        contraction_methods_deque.push_back(-1);
        contraction_methods_deque.insert(
            contraction_methods_deque.end(),
            contraction_methods.begin(), 
            contraction_methods.end()
        );
        log << "Contraction methods queue prepared" << std::endl;

        for (int64_t i = 0; i < max_cycles; ++i) {
            log << "Contraction cycle number: " << i+1 << std::endl;
            int64_t front = contraction_methods_deque.front();
            contraction_methods_deque.pop_front();
            contraction_methods_deque.push_back(front);

            auto kind = contraction_methods_deque.front();
            while (kind != -1) {
                perform_one_contraction_cycle(
                    graph, 
                    kind, 
                    forbidden_vertices,
                    log,
                    err
                );
                contraction_methods_deque.pop_front();
                contraction_methods_deque.push_back(front);
                kind = contraction_methods_deque.front();
            }
        }
    }

private:
    void perform_one_contraction_cycle(
        G &graph,
        int64_t kind,
        Identifiers<V> &forbidden_vertices,
        std::ostringstream &log,
        std::ostringstream &err
    )
    {
        switch (kind) {
            case -1:
                pgassert(false);
                break;

            case 1:
                log << "Dead-end contractions" << std::endl;
                perform_dead_end_contraction(graph, forbidden_vertices, log, err);
                break;

            case 2:
                log << "Linear contractions" << std::endl;
                perform_linear_contraction(graph, forbidden_vertices, log, err);
                break;

            case 3:
                log << "Contractions hierarchy" << std::endl;
                perform_contractions_hierarchy(graph, forbidden_vertices, log, err);
                break;
            
            default:
                pgassert(false);
                break;
        }
    }

    void perform_dead_end_contraction(
        G &graph,
        Identifiers<V> forbidden_vertices,
        std::ostringstream &log,
        std::ostringstream &err
    )
    {
        Pgr_deadend<G> deadendContractor;
        deadendContractor.setForbiddenVertices(forbidden_vertices);

        deadendContractor.calculateVertices(graph);
        try {
            deadendContractor.doContraction(graph);
        }
        catch ( ... ) {
            throw;
        }
    }

    void perform_linear_contraction(
        G &graph,
        Identifiers<V>& forbidden_vertices,
        std::ostringstream &log,
        std::ostringstream &err
    ) 
    {
        Pgr_linear<G> linearContractor;
        try {
            linearContractor(graph, forbidden_vertices);
        }
        catch ( ... ) {
            throw;
        }
    }

    void perform_contractions_hierarchy(
        G &graph,
        Identifiers<V> forbidden_vertices,
        std::ostringstream &log,
        std::ostringstream &err
    ) 
    {
        Pgr_contractionsHierarchy<G> CH;
        CH.setForbiddenVertices(forbidden_vertices);

        try {
            CH.doContraction(graph, log, err);
        }
        catch ( ... ) {
            throw;
        }
    }
};

}  // namespace contraction
}  // namespace pgrouting

#endif  // INCLUDE_CONTRACTION_CONTRACT_HPP_
