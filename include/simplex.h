/**
 * @file simplex.h
 * @brief Definition of interfaces, macros and structures of the simplex method
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#ifndef SIMPLEX_H
#define SIMPLEX_H

#include <matrix_.h>

#define SIMPLEX_EPSILON 1e-12
#define MAX_ITERATIONS 100

typedef enum {
    MAX = 0,
    MIN,
    NONE
} LPP_type;

typedef enum {
    FEASIBLE_SOLUTION = 0,
    NON_FEASIBLE_SOLUTION,
    NON_BOUNDED_SOLUTION,
    MAX_ITERATION
} Solution_type;

typedef enum {
    NON_NEGATIVE_VARIABLE = 0,
    NON_POSITIVE_VARIABLE,
    FREE_VARIABLE,
    NONE_VARIABLE
} Variable_type;

typedef enum {
    LESS_THAN_OR_EQUAL_CONSTRAIN = 0,
    GREATER_THAN_OR_EQUAL_CONSTRAIN,
    EQUAL_CONSTRAIN,
    NONE_CONSTRAIN
} Constrain_type;

typedef struct {
    Matrix A;
    Matrix b;
    Matrix c;
    Matrix x;
    double z;
    Constrain_type *constrain_type_;
    Variable_type *variable_type_;
    LPP_type type;
    Solution_type solution_type;
} LPP_matrix_form;


// Public interfaces

int Simplex_solve(LPP_matrix_form *LPP);
int LPP_matrix_form_init(LPP_matrix_form *LPP, size_t n_variables, size_t n_constrains);
int LPP_matrix_form_destroy(LPP_matrix_form *LPP);

#endif