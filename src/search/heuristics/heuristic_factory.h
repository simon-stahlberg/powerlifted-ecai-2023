#ifndef SEARCH_HEURISTIC_FACTORY_H
#define SEARCH_HEURISTIC_FACTORY_H

#include <string>

#include "../options.h"

class Task;
class Heuristic;

/**
 * @brief Factory class to generate corresponding heuristic object
 */
class HeuristicFactory {
public:
    static Heuristic *create(const Options &opt, const Task &task);
};

#endif //SEARCH_HEURISTIC_FACTORY_H
