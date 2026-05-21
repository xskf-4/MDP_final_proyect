/**
 * @file MDP.h
 * @brief Definition of interfaces, macros and structures for MDPs
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#ifndef MDP_H
#define MDP_H

#include <stdint.h>

#include "list.h"
#include "matrix_.h"

/*
    Struct for Policies
*/
typedef struct {
    uint32_t *decision_by_state_;
    double expected_average_cost;
    Matrix M;
    Matrix stationary_vector;
    uint8_t is_valid;
} Policy;

/*
    Enum for type of system
*/
typedef enum {
    COSTS_SYSTEM = 0,
    REWARDS_SYSTEM,
    NONE_SYSTEM
} system_type;

/*
    Struct for MDPs
*/
typedef struct {
    uint32_t n_states;
    uint32_t n_decisions;
    uint8_t is_valid;
    Matrix *transition_matrix_;
    Matrix costs_matrix;
    List *valid_decision_states_lists; // valid decision k states lists (uint32_t)
    List *valid_state_decisions_lists; // valid state i decisions lists (uint32_t)
    Policy optimal_policy;
    system_type type;
} MDP;

/*
    Public Interfaces
*/

// Policy interfaces

uint32_t Policy_init(Policy *P, uint32_t n_states);
void Policy_clone(Policy *dest, Policy *src);
uint32_t Policy_are_equal(Policy p1, Policy p2);
uint32_t Policy_set_from_array(Policy *P, uint32_t *src, MDP mdp);
uint32_t Policy_get_stationary_vector(Policy *P, Matrix_system *S);
uint32_t Policy_get_expected_average_cost(Policy *P, Matrix *Costs);

// MDP interfaces

void MDP_set_labels(char *labels[2], MDP mdp);
void Policy_print(Policy P, char *s[2]);
void Policy_destroy(Policy *P);
uint32_t MDP_init(MDP *mdp, uint32_t n_states, uint32_t n_decisions);
void MDP_set_NULL(MDP *mdp);
void MDP_destroy(MDP *mdp);
void MDP_set_lists(MDP *mdp);
void MDP_print(MDP mdp);
void MDP_check(MDP *mdp);

#endif