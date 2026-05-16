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

typedef struct {
    Matrix A;
    Matrix b;
    Matrix c;
    Matrix B;
    Matrix inv_B;
    Matrix cB;
    Matrix xB;
    Matrix ANB;
    Matrix cNB;
    Matrix zNB;
    Matrix z;
    int *x_B;
    int *x_NB;
} LPP_matrix_standard_form;

typedef struct {
    double a;
    double b;
} M_point;

// Public interfaces

int Simplex_solve(LPP_matrix_form *LPP);
int LPP_matrix_form_init(LPP_matrix_form *LPP, size_t n_variables, size_t n_constrains);
int LPP_matrix_form_destroy(LPP_matrix_form *LPP);

// Inner interfaces

static int LPP_matrix_form_clone(LPP_matrix_form *LPP_dest, LPP_matrix_form *LPP_src);

static int LPP_matrix_standard_form_init(LPP_matrix_standard_form *LPP, size_t n_variables, size_t n_constrains);
static int LPP_matrix_standard_form_destroy(LPP_matrix_standard_form *LPP);

static int LPP_matrix_form_to_standard_form(LPP_matrix_standard_form *LPP_dest, LPP_matrix_form *LPP_src);
static int LPP_matrix_standard_form_set_matrices(LPP_matrix_standard_form *LPP);

static int big_M_comparison(M_point x, M_point y);
static int LPP_matrix_standard_form_find_entry_variable(LPP_matrix_standard_form *LPP, size_t *entry_index);
static int LPP_matrix_standard_form_set_inv_B(LPP_matrix_standard_form *LPP, Matrix y_i, size_t entry_index, size_t leaving_index);
static int LPP_matrix_standard_form_find_leaving_variable(LPP_matrix_standard_form *LPP, size_t *leaving_index, size_t entry_index);
static int LPP_matrix_standard_form_set_new_solution(LPP_matrix_standard_form *LPP);

static int Simplex_method(LPP_matrix_standard_form *LPP);
static int LPP_matrix_form_set_solution_type(LPP_matrix_form *LPP_dest, LPP_matrix_standard_form LPP_src, int value);
static int LPP_matrix_form_set_solution(LPP_matrix_form *LPP_dest, LPP_matrix_standard_form *LPP_src);

#endif