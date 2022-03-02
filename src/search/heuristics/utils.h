#ifndef SEARCH_HEURISTICS_UTILS_H_
#define SEARCH_HEURISTICS_UTILS_H_

#include "../task.h"

#include "../datalog/grounder/weighted_grounder.h"

datalog::Datalog initialize_datalog(const Task &task, datalog::AnnotationGenerator annotation_generator);

std::vector<datalog::Fact> get_datalog_facts_from_state(const DBState &s, const Task &task);

#endif //SEARCH_HEURISTICS_UTILS_H_
