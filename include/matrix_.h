/**
 * @file matrix_.h
 * @brief Definition of uint32_terfaces, macros and structure of matrices
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#ifndef MATRIX_H
#define MATRIX_H

#include <stdint.h>

// Struct for matrices
typedef struct {
    double **values;
    uint32_t rows;
    uint32_t columns;
} Matrix;

/*
    Public uint32_terfaces
*/
uint32_t matrix_init(Matrix *M, uint32_t n_rows, uint32_t n_columns);
uint32_t matrix_init_matrices(uint32_t n_rows, uint32_t n_columns, uint32_t n_matrices, ...);
uint32_t matrix_destroy(Matrix *M);
uint32_t matrix_destroy_matrices(uint32_t n_matrices, ...);

uint32_t matrix_is_init(Matrix *M);
uint32_t matrix_is_symmetrical(Matrix *M);
uint32_t matrix_is_diagonally_dominant(Matrix *M);
uint32_t matrix_is_invertible(Matrix *M);

uint32_t matrix_swap_rows(Matrix *M, uint32_t n_row1, uint32_t n_row2);
uint32_t matrix_swap_columns(Matrix *M, uint32_t n_column1, uint32_t n_column2);
uint32_t matrix_scale_row(Matrix *M, uint32_t n_row, double escalar);
uint32_t matrix_scale_column(Matrix *M, uint32_t n_column, double escalar);
uint32_t matrix_elementary_row_op1(Matrix *M, uint32_t n_row_dest, uint32_t n_row_src, double scalar);
uint32_t matrix_clone_column(Matrix *M_dest, Matrix *M_src, uint32_t i, uint32_t j);

uint32_t matrix_submatrix(Matrix *M_dest, Matrix *M_src, uint32_t n_row_i, uint32_t n_row_f, uint32_t n_column_i, uint32_t n_column_f);

uint32_t matrix_random(Matrix *M);

uint32_t matrix_gaussian_determinant(Matrix *M, double *det);
uint32_t matrix_transpose(Matrix *M_dest, Matrix *M_src);
uint32_t matrix_identity(Matrix *M);
uint32_t matrix_zero(Matrix *M);
uint32_t matrix_clone(Matrix *M_dest, Matrix *M_src);
uint32_t matrix_inverse(Matrix *M_dest, Matrix *M_src);
uint32_t matrix_entrywise_operation(Matrix *M_dest, Matrix *M_x, Matrix *M_y, double (operation)(double x, double y));
uint32_t matrix_negate(Matrix *M_dest, Matrix *M_src);
uint32_t matrix_multiplication(Matrix *M_dest, Matrix *M_x, Matrix *M_y);

uint32_t matrix_euclidean_norm(Matrix *M, double *norm);
uint32_t matrix_sum_norm(Matrix *M, double *norm);
uint32_t matrix_max_norm(Matrix *M, double *norm);

double add(double x, double y);
double subtract(double x, double y);

/*
    Macros
*/
#define matrix_rows(M) ((M)->rows)
#define matrix_columns(M) ((M)->columns)
#define matrix_is_square(M) ((M)->columns == (M)->rows ? 1 : 0)
#define matrix_have_same_dimentions(M_x, M_y) (((M_x)->columns == (M_y)->columns && (M_x)->rows == (M_y)->rows) ? 1 : 0)
#define matrix_is_column_vector(vector) ((((vector)->rows) > 1 && ((vector)->columns) == 1) ? 1 : 0)
#define matrix_is_row_vector(vector) ((((vector)->rows) == 1 && ((vector)->columns) > 1) ? 1 : 0)

#define matrix_init_column_vector(M, dimension) matrix_init((M), dimension, 1)
#define matrix_init_row_vector(M, dimension) matrix_init((M), 1, dimension)
#define matrix_init_square_matrix(M, dimension) matrix_init((M), dimension, dimension)

#define matrix_init_column_vectors(dimension, n_matrices, ...) matrix_init_matrices(dimension, 1, n_matrices, __VA_ARGS__)
#define matrix_init_row_vectors(dimension, n_matrices, ...) matrix_init_matrices(1, dimension, n_matrices, __VA_ARGS__)
#define matrix_init_square_matrices(dimension, n_matrices, ...) matrix_init_matrices(dimension, dimension, n_matrices, __VA_ARGS__)

#define matrix_addition(M_dest, M_x, M_y) matrix_entrywise_operation(M_dest, M_x, M_y, add)
#define matrix_subtraction(M_dest, M_x, M_y) matrix_entrywise_operation(M_dest, M_x, M_y, subtract)

#endif 