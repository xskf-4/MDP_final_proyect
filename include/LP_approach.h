/**
 * @file LP_approach.h
 * @brief Definition of interface for Linear Programming approach algorithm
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#ifndef LP_APPROACH_H
#define LP_APPROACH_H

#include <stdint.h>

#include "MDP.h"

uint32_t LP_approach_solve_MDP(MDP *mdp);

#endif