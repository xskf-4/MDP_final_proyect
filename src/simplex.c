/**
 * @file simplex.c
 * @brief Implementation of simplex method
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <matrix_.h>

#include "simplex.h"

// inner structs
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


// Inner interfaces

static int LPP_matrix_form_clone(LPP_matrix_form *LPP_dest, LPP_matrix_form *LPP_src) {
    if(LPP_dest == NULL || LPP_src == NULL)
        return 1;
    
    size_t i;
    size_t n_variables = (LPP_src->A).columns, n_constrains = (LPP_src->A).rows;
    matrix_clone(&(LPP_dest->A), &(LPP_src->A));
    matrix_clone(&(LPP_dest->b), &(LPP_src->b));
    matrix_clone(&(LPP_dest->c), &(LPP_src->c));
    matrix_clone(&(LPP_dest->x), &(LPP_src->x));

    for(i = 0; i < n_variables; i++)
        (LPP_dest->variable_type_)[i] = (LPP_src->variable_type_)[i];

    for(i = 0; i < n_constrains; i++)
        (LPP_dest->constrain_type_)[i] = (LPP_src->constrain_type_)[i];

    (LPP_dest->type) = (LPP_src->type);

    return 0;
}

static int LPP_matrix_standard_form_init(LPP_matrix_standard_form *LPP, size_t n_variables, size_t n_constrains) {
    size_t n_variables_NB = n_variables - n_constrains;
    // init A
    if(matrix_init(&(LPP->A), n_constrains, n_variables) != 0)
        return 1;
    
    // init b & xB
    if(matrix_init_column_vectors(n_constrains, 2, &(LPP->b), &(LPP->xB)) != 0)
        return 1;
    
    // init c
    if(matrix_init(&(LPP->c), 2, n_variables) != 0)
        return 1;

    // init B & invB & AB
    if(matrix_init_square_matrices(n_constrains, 2, &(LPP->B), &(LPP->inv_B)) != 0)
        return 1;

    // init cB
    if(matrix_init(&(LPP->cB), 2, n_constrains) != 0)
        return 1;
    
    // init cNB
    if(matrix_init(&(LPP->cNB), 2, n_variables_NB) != 0)
        return 1;

    // init ANB
    if(matrix_init(&(LPP->ANB), n_constrains, n_variables_NB) != 0)
        return 1;

    // init zNB
    if(matrix_init(&(LPP->zNB), 2, n_variables_NB) != 0)
        return 1;

    // init z
    if(matrix_init(&(LPP->z), 2, 1) != 0)
        return 1;

    LPP->x_B = (int *)malloc(sizeof(int) * n_variables);
    LPP->x_NB = (int *)malloc(sizeof(int) * n_variables);

    if((LPP->x_B) == NULL)
        return 1;

    if((LPP->x_NB) == NULL)
        return 1;

    memset(LPP->x_B, -1, sizeof(int) * n_variables);
    memset(LPP->x_NB, -1, sizeof(int) * n_variables);

    return 0;
}

static int LPP_matrix_standard_form_destroy(LPP_matrix_standard_form *LPP) {
    matrix_destroy_matrices(3, &(LPP->A), &(LPP->b), &(LPP->c));
    matrix_destroy_matrices(4, &(LPP->B), &(LPP->inv_B), &(LPP->cB), &(LPP->xB));
    matrix_destroy_matrices(3, &(LPP->cNB), &(LPP->zNB), &(LPP->ANB));
    free(LPP->x_B);
    free(LPP->x_NB);
    return 0;
}

static int LPP_matrix_form_to_standard_form(LPP_matrix_standard_form *LPP_dest, LPP_matrix_form *LPP_src) {
    if(LPP_dest == NULL)
        return 1;

    // Set n_variables and n_constrains for standard form
    size_t i;
    size_t n_constrains = (LPP_src->A).rows, n_variables = (LPP_src->A).columns;

    LPP_matrix_form LPP_copy;
    LPP_matrix_form_init(&LPP_copy, n_variables, n_constrains);
    LPP_matrix_form_clone(&LPP_copy, LPP_src);

    // Set MIN to MAX
    if(LPP_copy.type == MIN) {
        matrix_negate(&(LPP_copy.c), &(LPP_copy.c));
        LPP_copy.type = MAX;
    }

    for(i = 0; i < n_variables; i++) {
        if(LPP_copy.variable_type_[i] == NON_POSITIVE_VARIABLE) {
            matrix_scale_column(&(LPP_copy.A), i, -1.0);
            matrix_scale_column(&(LPP_copy.c), i, -1.0);
            LPP_copy.variable_type_[i] = NON_NEGATIVE_VARIABLE;
        }
    }

    for(i = 0; i < n_constrains; i++) {
        if((LPP_copy.b).values[i][0] < 0.0) {
            matrix_scale_row(&(LPP_copy.A), i, -1.0);
            matrix_scale_row(&(LPP_copy.b), i, -1.0);
            switch (LPP_copy.constrain_type_[i]) {
                case LESS_THAN_OR_EQUAL_CONSTRAIN:
                    LPP_copy.constrain_type_[i] = GREATER_THAN_OR_EQUAL_CONSTRAIN;
                    break;
                case GREATER_THAN_OR_EQUAL_CONSTRAIN:
                    LPP_copy.constrain_type_[i] = LESS_THAN_OR_EQUAL_CONSTRAIN;
                    break;
                default:
                    break;
            }
        }
    }

    // Set standard form
    for(i = 0; i < (LPP_copy.A).columns; i++) {
        if(LPP_copy.variable_type_[i] == FREE_VARIABLE)
            n_variables ++;
    }

    for(i = 0; i < (LPP_copy.A).rows; i++) {
        switch(LPP_copy.constrain_type_[i]) {
            case EQUAL_CONSTRAIN:
            case LESS_THAN_OR_EQUAL_CONSTRAIN:
                n_variables++;
                break;
            case GREATER_THAN_OR_EQUAL_CONSTRAIN:
                n_variables += 2;
                break;
            default:
                return 1;
        }
    }

    LPP_matrix_standard_form_init(LPP_dest, n_variables, n_constrains);

    // Set b
    matrix_clone(&(LPP_dest->b), &(LPP_copy.b));
    // Set A & c
    size_t l = 0, k = 0;
    for(i = 0; i < (LPP_copy.A).columns; i++) {
        (LPP_dest->c).values[1][l] = (LPP_copy.c).values[0][i];
        matrix_clone_column(&(LPP_dest->A), &(LPP_copy.A), l, i);
        (LPP_dest->x_NB)[l] = k;
        if(LPP_copy.variable_type_[i] == FREE_VARIABLE) {
            l++;
            k++;
            (LPP_dest->c).values[1][l] = (LPP_copy.c).values[0][i];
            matrix_clone_column(&(LPP_dest->A), &(LPP_copy.A), l, i);
            matrix_scale_column(&(LPP_dest->c), l, -1.0);
            matrix_scale_column(&(LPP_dest->A), l, -1.0);
            (LPP_dest->x_NB)[l] = k;
        }
        l++;
        k++;
    }

    for(i = 0; i < n_constrains; i++) {
        switch(LPP_copy.constrain_type_[i]) {
            case GREATER_THAN_OR_EQUAL_CONSTRAIN:
                (LPP_dest->A).values[i][l] = -1.0;
                (LPP_dest->x_NB)[l] = k;
                k++;
                break;
            case LESS_THAN_OR_EQUAL_CONSTRAIN:
                (LPP_dest->A).values[i][l] = 1.0;
                (LPP_dest->x_B)[l] = i;
                break;
            default:
                continue;
        }
        l++;
    }

    for(i = 0; i < n_constrains; i++) {
        switch(LPP_copy.constrain_type_[i]) {
            case GREATER_THAN_OR_EQUAL_CONSTRAIN:
            case EQUAL_CONSTRAIN:
                (LPP_dest->A).values[i][l] = 1.0;
                (LPP_dest->c).values[0][l] = -1.0;
                (LPP_dest->x_B)[l] = i;
                break;
            default:
                continue;
        }
        l++;
    }

    LPP_matrix_form_destroy(&LPP_copy);
    return 0;
}

static int LPP_matrix_standard_form_set_matrices(LPP_matrix_standard_form *LPP) {
    if(LPP == NULL)
        return 1;

    size_t i;
    uint32_t n_variables = (LPP->A).columns;

    // Basic matrices (B - matrices)
    for(i = 0; i < n_variables; i++) {
        if((LPP->x_B)[i] == -1)
            continue;
        
        // Set AB
        matrix_clone_column(&(LPP->B), &(LPP->A), (LPP->x_B)[i], i);
        // Set cB
        matrix_clone_column(&(LPP->cB), &(LPP->c), (LPP->x_B)[i], i);
    }

    // Non Basic matrices (NB - matrices)
    for(i = 0; i < n_variables; i++) {
        if((LPP->x_NB)[i] == -1)
            continue;

        // Set ANB
        matrix_clone_column(&(LPP->ANB), &(LPP->A), (LPP->x_NB)[i], i);
        // Set cNB
        matrix_clone_column(&(LPP->cNB), &(LPP->c), (LPP->x_NB)[i], i);
    }

    // Set xB
    matrix_multiplication(&(LPP->xB), &(LPP->inv_B), &(LPP->b));

    // Set z
    matrix_multiplication(&(LPP->z), &(LPP->cB), &(LPP->xB));

    return 0;
}

static int big_M_comparison(M_point x, M_point y) {
    // x(aM+b) < y(aM+b)
    if(x.a < y.a - SIMPLEX_EPSILON)
        return 1;

    if(fabs(x.a-y.a) < SIMPLEX_EPSILON && x.b < y.b - SIMPLEX_EPSILON)
        return 1;

    return 0;
}

static int LPP_matrix_standard_form_find_entry_variable(LPP_matrix_standard_form *LPP, size_t *entry_index) {
    size_t i;
    Matrix Aux;
    // set zNB
    // inv_B * ANB
    matrix_init(&Aux, (LPP->inv_B).rows, (LPP->ANB).columns);
    matrix_multiplication(&Aux, &(LPP->inv_B), &(LPP->ANB));
    // cB * Aux
    matrix_multiplication(&(LPP->zNB), &(LPP->cB), &Aux);
    // zNB - cNB
    matrix_subtraction(&(LPP->zNB), &(LPP->zNB), &(LPP->cNB));

    // get entry_index
    M_point p;
    M_point min_p = {0.0, 0.0};

    *entry_index = -1;
    for(i = 0; i < (LPP->A).columns; i++) {
        if((LPP->x_NB)[i] == -1)
            continue;

        p.a = (LPP->zNB).values[0][(LPP->x_NB)[i]];
        p.b = (LPP->zNB).values[1][(LPP->x_NB)[i]];
        // p < min_p
        if(big_M_comparison(p, min_p)) {
            *entry_index = i;
            break;
        }
    }
    matrix_destroy(&Aux);

    if(*entry_index == (size_t )-1)
        return 1;

    return 0;
}

static int LPP_matrix_standard_form_set_inv_B(LPP_matrix_standard_form *LPP, Matrix y_i, size_t leaving_index) {
    Matrix E, new_inv_B;
    matrix_init_square_matrices((LPP->inv_B).rows, 2, &E, &new_inv_B);
    matrix_identity(&E);
    matrix_clone_column(&E, &y_i, (LPP->x_B)[leaving_index], 0);

    size_t i;
    double pivot = y_i.values[(LPP->x_B)[leaving_index]][0];
    for(i = 0; i < E.rows; i++) {
        if((E.values)[i][(LPP->x_B)[leaving_index]] == 0)
            continue;
            
        (E.values)[i][(LPP->x_B)[leaving_index]] /= pivot;
        (E.values)[i][(LPP->x_B)[leaving_index]] *= -1.0;
        if(i == (size_t )(LPP->x_B)[leaving_index])
            (E.values)[i][(LPP->x_B)[leaving_index]] = 1 / pivot;
    }

    matrix_multiplication(&new_inv_B, &E, &(LPP->inv_B));
    matrix_clone(&(LPP->inv_B), &new_inv_B);

    matrix_destroy_matrices(2, &E, &new_inv_B);
    return 0;
}

static int LPP_matrix_standard_form_find_leaving_variable(LPP_matrix_standard_form *LPP, size_t *leaving_index, size_t entry_index) {
    size_t i;
    Matrix y_i, A_i;
    matrix_init_column_vectors((LPP->b).rows, 2, &y_i, &A_i);
    matrix_clone_column(&A_i, &(LPP->A), 0, entry_index);
    matrix_multiplication(&y_i, &(LPP->inv_B), &A_i);
    
    double min_t = INFINITY, t;
    // Find leaving variable
    for(i = 0; i < y_i.rows; i++) {
        if(y_i.values[i][0] <= SIMPLEX_EPSILON)
            continue;
    
        t = (LPP->xB).values[i][0] / y_i.values[i][0];

        if(t < min_t - SIMPLEX_EPSILON)
            min_t = t;
    }

    *leaving_index = -1;
    for(i = 0; i < (LPP->A).columns; i++) {
        if((LPP->x_B)[i] == -1)
            continue;

        if(y_i.values[(LPP->x_B)[i]][0] < + SIMPLEX_EPSILON)
            continue;

        t = (LPP->xB).values[(LPP->x_B)[i]][0] / y_i.values[(LPP->x_B)[i]][0];
        if(fabs(t - min_t) < SIMPLEX_EPSILON) {
            *leaving_index = i;
            break;
        }
    }

    LPP_matrix_standard_form_set_inv_B(LPP, y_i, *leaving_index);
    matrix_destroy_matrices(2, &y_i, &A_i);
    if(*leaving_index == (size_t )-1)
        return 1;

    return 0;
}

static int LPP_matrix_standard_form_set_new_solution(LPP_matrix_standard_form *LPP) {
    size_t entry_index, leaving_index;

    /* Set entry variable */
    if(LPP_matrix_standard_form_find_entry_variable(LPP, &entry_index) != 0)
        return 1;

    /* Set leaving variable */
    if(LPP_matrix_standard_form_find_leaving_variable(LPP, &leaving_index, entry_index) != 0)
        return 2;

    // swap x_B[leavingindex] <-> x_NB[entry_index]
    int aux = (LPP->x_B)[leaving_index];
    (LPP->x_NB)[leaving_index] = (LPP->x_NB)[entry_index];
    (LPP->x_B)[entry_index] = aux;
    (LPP->x_NB)[entry_index] = -1;
    (LPP->x_B)[leaving_index] = -1;

    return 0;
}

static int Simplex_method(LPP_matrix_standard_form *LPP) {
    size_t i;
    int value;
    // Init solution
    LPP_matrix_standard_form_set_matrices(LPP);
    matrix_clone(&(LPP->inv_B), &(LPP->B));

    i = 0;
    while (1) {
        LPP_matrix_standard_form_set_matrices(LPP);

        value = LPP_matrix_standard_form_set_new_solution(LPP);
        if(value == 1) // Entry variable not found (Optimal solution)
            break;
        if(value == 2)// Leaving variable not found (Inf solution)
            break;
        if(i > MAX_ITERATIONS) {
            value = 3;
            break;
        }

        i++;
    }
    
    return value;
}

static int LPP_matrix_form_set_solution_type(LPP_matrix_form *LPP_dest, LPP_matrix_standard_form LPP_src, int value) {
    if(fabs((LPP_src.z).values[0][0]) > SIMPLEX_EPSILON && (value == 1 || value == 3)) {
        LPP_dest->solution_type = NON_FEASIBLE_SOLUTION;
        return 0;
    }
    
    if(value == 2) {
        LPP_dest->solution_type = NON_BOUNDED_SOLUTION;
        return 0;
    }
    
    if(value == 3) {
        LPP_dest->solution_type = MAX_ITERATION;
        return 0;
    }
    
    LPP_dest->solution_type = FEASIBLE_SOLUTION;

    return 0;
}

static int LPP_matrix_form_set_solution(LPP_matrix_form *LPP_dest, LPP_matrix_standard_form *LPP_src) {
    size_t i, l = 0;
    for(i = 0; i < (LPP_dest->x).rows; i++) {
        switch(LPP_dest->variable_type_[i]) {
            case NON_NEGATIVE_VARIABLE:
                if((LPP_src->x_B)[l] == -1) {
                    (LPP_dest->x).values[i][0] = 0.0;
                }
                else {
                    (LPP_dest->x).values[i][0] = (LPP_src->xB).values[(LPP_src->x_B)[l]][0];
                }
                break;
            case NON_POSITIVE_VARIABLE:
                if((LPP_src->x_B)[l] == -1) {
                    (LPP_dest->x).values[i][0] = 0.0;
                }
                else {
                    (LPP_dest->x).values[i][0] = -(LPP_src->xB).values[(LPP_src->x_B)[l]][0];
                }
                break;
            case FREE_VARIABLE:
                if((LPP_src->x_B)[l] == -1) {
                    (LPP_dest->x).values[i][0] += 0.0;
                }
                else {
                    (LPP_dest->x).values[i][0] += (LPP_src->xB).values[(LPP_src->x_B)[l]][0];
                }
                l++;
                if((LPP_src->x_B)[l] == -1) {
                    (LPP_dest->x).values[i][0] -= 0.0;
                }
                else {
                    (LPP_dest->x).values[i][0] -= (LPP_src->xB).values[(LPP_src->x_B)[l]][0];
                }
            default:
                break;
        }
        l++;
    }

    (LPP_dest->z) = (LPP_src->z).values[1][0];

    if(LPP_dest->type == MIN)
        (LPP_dest->z) *= -1.0;

    return 0;
}

// Public interfaces

int Simplex_solve(LPP_matrix_form *LPP) {
    if(LPP == NULL)
        return 0;

    LPP_matrix_standard_form LPP_std;

    LPP_matrix_form_to_standard_form(&LPP_std, LPP);

    // Simplex method
    int value = Simplex_method(&LPP_std);
    
    /* Translate solution */
    LPP_matrix_form_set_solution_type(LPP, LPP_std, value);

    LPP_matrix_form_set_solution(LPP, &LPP_std);

    LPP_matrix_standard_form_destroy(&LPP_std);
    return 1;
}

int LPP_matrix_form_init(LPP_matrix_form *LPP, size_t n_variables, size_t n_constrains) {
    if(matrix_init(&(LPP->A), n_constrains, n_variables) != 0)
        return 1;
    
    if(matrix_init_column_vector(&(LPP->b), n_constrains) != 0)
        return 1;
    
    if(matrix_init_column_vector(&(LPP->x), n_variables) != 0)
        return 1;

    if(matrix_init_row_vector(&(LPP->c), n_variables) != 0)
        return 1;

    (LPP->constrain_type_) = (Constrain_type *)malloc(sizeof(Constrain_type) * n_constrains);
    (LPP->variable_type_) = (Variable_type *)malloc(sizeof(Variable_type) * n_variables);

    if(LPP->constrain_type_ == NULL)
        return 1;

    if(LPP->variable_type_ == NULL)
        return 1;

    memset(LPP->constrain_type_, NONE_CONSTRAIN, sizeof(Constrain_type) * n_constrains);
    memset(LPP->variable_type_, NONE_VARIABLE, sizeof(Variable_type) * n_variables);
    LPP->type = NONE;
    LPP->solution_type = FEASIBLE_SOLUTION;
    return 0;
}

int LPP_matrix_form_destroy(LPP_matrix_form *LPP) {
    matrix_destroy_matrices(4, &(LPP->A), &(LPP->b), &(LPP->c), &(LPP->x));
    free(LPP->constrain_type_);
    free(LPP->variable_type_);
    return 0;
}
