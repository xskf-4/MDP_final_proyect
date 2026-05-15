#ifndef MATRIX_H
#define MATRIX_H

// Struct for matrices
typedef struct {
    double **values;
    int rows;
    int columns;
} Matrix;

/*
    Public interfaces
*/
int matrix_init(Matrix *M, int n_rows, int n_columns);
int matrix_init_matrices(int n_rows, int n_columns, int n_matrices, ...);
int matrix_destroy(Matrix *M);
int matrix_destroy_matrices(int n_matrices, ...);

int matrix_is_init(Matrix *M);
int matrix_is_symmetrical(Matrix *M);
int matrix_is_diagonally_dominant(Matrix *M);
int matrix_is_invertible(Matrix *M);

int matrix_swap_rows(Matrix *M, int n_row1, int n_row2);
int matrix_swap_columns(Matrix *M, int n_column1, int n_column2);
int matrix_scale_row(Matrix *M, int n_row, double escalar);
int matrix_scale_column(Matrix *M, int n_column, double escalar);
int matrix_elementary_row_op1(Matrix *M, int n_row_dest, int n_row_src, double scalar);
int matrix_clone_column(Matrix *M_dest, Matrix *M_src, int i, int j);

int matrix_submatrix(Matrix *M_dest, Matrix *M_src, int n_row_i, int n_row_f, int n_column_i, int n_column_f);

int matrix_random(Matrix *M);

int matrix_gaussian_determinant(Matrix *M, double *det);
int matrix_transpose(Matrix *M_dest, Matrix *M_src);
int matrix_identity(Matrix *M);
int matrix_zero(Matrix *M);
int matrix_clone(Matrix *M_dest, Matrix *M_src);
int matrix_inverse(Matrix *M_dest, Matrix *M_src);
int matrix_entrywise_operation(Matrix *M_dest, Matrix *M_x, Matrix *M_y, double (operation)(double x, double y));
int matrix_negate(Matrix *M_dest, Matrix *M_src);
int matrix_multiplication(Matrix *M_dest, Matrix *M_x, Matrix *M_y);

int matrix_euclidean_norm(Matrix *M, double *norm);
int matrix_sum_norm(Matrix *M, double *norm);
int matrix_max_norm(Matrix *M, double *norm);

double add(double x, double y);
double subtract(double x, double y);

/*
    Inner interfaces
*/
static int is_in_range(int n,int l_low, int l_upp);

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