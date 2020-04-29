#ifndef SEARCH_INVERSE_ORDERED_JOIN_SUCCESSOR_H
#define SEARCH_INVERSE_ORDERED_JOIN_SUCCESSOR_H

#include "generic_join_successor.h"

/**
 * This class implements a full join program ordering joins by its arity.
 * Predicates with higher arity are joined first.
 *
 * @see generic_join_successor.h
 */

class InverseOrderedJoinSuccessorGenerator : public GenericJoinSuccessor {
public:
    explicit InverseOrderedJoinSuccessorGenerator(Task &task) : GenericJoinSuccessor(task) {};

    std::vector<Table>
    parse_precond_into_join_program(const std::vector<Atom> &precond,
                                    const DBState &state) override;

};


#endif //SEARCH_INVERSE_ORDERED_JOIN_SUCCESSOR_H
