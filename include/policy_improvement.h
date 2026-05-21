/**
 * @file policy_improvement.h
 * @brief Definition of interfaces for policy improvement algorithms
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#ifndef POLICY_IMPROVEMENT_H
#define POLICY_IMPROVEMENT_H

#include <stdint.h>

#include "MDP.h"

uint32_t policy_improvement_solve_MDP(MDP *mdp);

void policy_improvement_discount_factor_print_results(MDP *mdp);
uint32_t policy_improvement_discount_factor_solve_MDP(MDP *mdp);

#endif