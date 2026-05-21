/**
 * @file value_iteration.h
 * @brief Definition of interface for value iteration algorithm
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#ifndef VALUE_ITERATION_H
#define VALUE_ITERATION_H

#include <stdint.h>

#include "MDP.h"

uint32_t value_iteration_solve_MDP(MDP *mdp);

#endif