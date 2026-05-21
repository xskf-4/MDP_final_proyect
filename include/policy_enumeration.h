/**
 * @file policy_enumeration.h
 * @brief Definition of interface for policy enumeration algorithm
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#ifndef POLICY_ENUMERATION_H
#define POLICY_ENUMERATION_H

#include <stdint.h>

#include "MDP.h"

uint32_t policy_enumeration_solve_MDP(MDP *mdp);

#endif