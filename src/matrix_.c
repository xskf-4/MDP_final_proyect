/**
 * @file matrix_.c
 * @brief Implementation of uint32_terfaces of matrices
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>

#include "matrix_.h"

// Inner functions
static uint32_t is_in_range(uint32_t n,uint32_t l_low, uint32_t l_upp) {
    if(l_low <= n && n<=l_upp)
        return 1;

    return 0;
}

/*
    Initialize the matrix
*/
uint32_t matrix_init(Matrix *M, uint32_t n_rows, uint32_t n_columns) {
    if(n_rows < 1 || n_columns < 1)
        return 1;

    uint32_t i;

    M->values = (double **)malloc(sizeof(double *) * n_rows);

    if(M->values == NULL)
        return 1;

    for(i = 0; i < n_rows; i++) {
        (M->values)[i] = (double *)malloc(sizeof(double ) * n_columns);
        if((M->values)[i] == NULL)
            return 1;
    }
    
    M->rows = n_rows;
    M->columns = n_columns;

    matrix_zero(M);
    return 0;
}

/*
    Initialize multiple matrices
*/
uint32_t matrix_init_matrices(uint32_t n_rows, uint32_t n_columns, uint32_t n_matrices, ...) {
    va_list matrices;
    uint32_t i;
    Matrix *M_i;

    va_start(matrices, n_matrices);

    for(i = 0; i < n_matrices; i++) {
        M_i = va_arg(matrices, Matrix *);
        if(matrix_init(M_i, n_rows, n_columns)) {
            return 1;
        }
    }
    
    va_end(matrices);
    return 0;
}

/*
    Destroying the matrix
*/
uint32_t matrix_destroy(Matrix *M) {
    if(!matrix_is_init(M))
        return 1;
    
    uint32_t i;

    for(i = 0; i < M->rows; i++)
        free((M->values)[i]);

    free(M->values);

    M->values = NULL;
    M->columns = 0;
    M->rows = 0;
    return 0;
}

/*
    Destroying multiple matrices
*/
uint32_t matrix_destroy_matrices(uint32_t n_matrices, ...) {
    va_list matrices;
    uint32_t i;
    Matrix *M_i;

    va_start(matrices, n_matrices);

    for(i = 0; i < n_matrices; i++) {
        M_i = va_arg(matrices, Matrix *);
        if(matrix_destroy(M_i)) {
            return 1;
        }
    }

    va_end(matrices);
    return 0;
}

/*
    Returns 1 if the matrix is inizialized, 0 otherwise
*/
uint32_t matrix_is_init(Matrix *M) {
    if(M == NULL)
        return 0;

    if(M->values == NULL)
        return 0;

    return 1;
}

/*
    Returns 1 if the matrix is symmetric, 0 otherwise
*/
uint32_t matrix_is_symmetrical(Matrix *M) {
    if(!matrix_is_init(M))
        return 0;

    if(!matrix_is_square(M))
        return 0;

    uint32_t i, j;

    for(i = 0; i < M->rows; i++) {
        for(j = i+1; j < M->columns; j++) {
            if((M->values)[i][j] != (M->values)[j][i])
                return 0;
        }
    }

    return 1;
}

/*
    Returns 1 if the matrix is diagonally dominant
*/
uint32_t matrix_is_diagonally_dominant(Matrix *M) {
    if(!matrix_is_init(M))
        return 0;

    if(!matrix_is_square(M))
        return 0;

    uint32_t i, j;
    double aux[2];

    for(i = 0; i < M->rows; i++) {
        aux[0] = 0;
        aux[1] = 0;
        for(j = 0; j < M->columns; j++) {
            if(i == j)
                continue;
            
            aux[0] += fabsl((M->values)[i][j]);
            aux[1] += fabsl((M->values)[j][i]);
        }
        if(aux[0] >= (M->values)[i][i] || aux[1] >= (M->values)[i][i])
            return 0;
    }
    return 1;
}

/*
    Returns 1 if the matrix determinant is 0
*/
uint32_t matrix_is_invertible(Matrix *M) {
    double determinante;

    matrix_gaussian_determinant(M, &determinante);

    if(determinante == 0)
        return 0;

    return 1;
}

/*
    Swaps the rows according to the given indices
*/
uint32_t matrix_swap_rows(Matrix *M, uint32_t n_row1, uint32_t n_row2) {
    if(!matrix_is_init(M))
        return 1;

    if(n_row1 >= M->rows || n_row2 >= M->rows)
        return 1;

    if(n_row1 == n_row2)
        return 0;

    double *aux;

    aux = M->values[n_row1];
    M->values[n_row1] = M->values[n_row2];
    M->values[n_row2] = aux;
    return 0;
}

/*
    Swaps the columns according to the given indices
*/
uint32_t matrix_swap_columns(Matrix *M, uint32_t n_column1, uint32_t n_column2) {
    if(n_column1 >= M->columns || n_column2 >= M->columns)
        return 1;

    double aux;
    uint32_t i;

    for(i = 0; i < M->rows; i++) {
        aux = (M->values)[i][n_column1];
        (M->values)[i][n_column1] = (M->values)[i][n_column2];
        (M->values)[i][n_column2] = aux;
    }
    return 0;
}

/*
    Multiplies a row of a matrix by a scalar
*/
uint32_t matrix_scale_row(Matrix *M, uint32_t n_row, double escalar) {
    uint32_t i;
    if(!matrix_is_init(M))
        return 1;

    if(n_row >= M->rows)
        return 1;

    for(i = 0; i < M->columns; i++)
        (M->values)[n_row][i] *= escalar;

    return 0;
}

/*
    Multiplies a column of a matrix by a scalar
*/
uint32_t matrix_scale_column(Matrix *M, uint32_t n_column, double escalar) {
    uint32_t i;
    if(!matrix_is_init(M))
        return 1;

    if(n_column >= M->columns)
        return 1;

    for(i = 0; i < M->rows; i++)
        (M->values)[i][n_column] *= escalar;

    return 0;
}

/*
    Adds the src row, multiplied by a scalar, to the dest row
*/
uint32_t matrix_elementary_row_op1(Matrix *M, uint32_t n_row_dest, uint32_t n_row_src, double escalar) {
    uint32_t i;
    if(!matrix_is_init(M))
        return 1;

    if(!is_in_range(n_row_dest, 0, M->rows - 1))
        return 1;
    
    if(!is_in_range(n_row_src, 0, M->rows - 1))
        return 1;

    for(i = 0; i < M->columns; i++)
        (M->values)[n_row_dest][i] += (M->values)[n_row_src][i] * escalar;

    return 0;
}

/*
    Clones column j from M_src uint32_to column i from M_dest
*/
uint32_t matrix_clone_column(Matrix *M_dest, Matrix *M_src, uint32_t i, uint32_t j) {
    if(i > M_dest->columns)
        return 1;

    if(j > M_src->columns)
        return 1;

    if(M_dest->rows != M_src->rows)
        return 1;

    size_t x;
    for(x = 0; x < M_dest->rows; x++)
        (M_dest->values)[x][i] = (M_src->values)[x][j];

    return 0;
}

/*
    Returns the submatrix produced
*/
uint32_t matrix_submatrix(Matrix *M_dest, Matrix *M_src, uint32_t n_row_i, uint32_t n_row_f, uint32_t n_column_i, uint32_t n_column_f) {
    uint32_t i, j;
    if(!(matrix_is_init(M_dest) && matrix_is_init(M_src)))
        return 1;

    if(M_dest->columns - 1 != n_column_f - n_column_i || M_dest->rows - 1 != n_row_f - n_row_i)
        return 1;

    if(n_row_i > n_row_f || n_column_i > n_column_f)
        return 1;

    if(!(is_in_range(n_row_i, 0, M_src->rows - 1) && is_in_range(n_row_f, 0, M_src->rows - 1) && is_in_range(n_column_i, 0, M_src->columns - 1) && is_in_range(n_column_f, 0, M_src->columns - 1)))
        return 1;

    for(i = 0; i < M_dest->rows; i++)
        for(j = 0; j < M_dest->columns; j++)
            (M_dest->values)[i][j] = (M_src->values)[n_row_i+i][n_column_i+j];

    return 0;
}

/*
    Assigns random values to the entries of the given matrix
*/
uint32_t matrix_random(Matrix *M) {
    if(!matrix_is_init(M))
        return 1;

    uint32_t i, j;

    for(i = 0; i < M->rows; i++) {
        for(j = 0; j < M->columns; j++) {
            (M->values)[i][j] = rand() % 21;
            if(rand() % 2)
                (M->values)[i][j] = -(M->values)[i][j];
        }
    }

    return 0;   
}

/*
    Returns the determinant of the given matrix
*/
uint32_t matrix_gaussian_determinant(Matrix *M, double *det) {
    if(!matrix_is_init(M))
        return 1;

    if(!matrix_is_square(M))
        return 1;

    uint32_t i, j;
    double aux;
    Matrix A;

    matrix_init(&A, M->rows, M->columns);
    matrix_clone(&A, M);

    *det = 1;
    for(i = 0; i < M->columns; i++) {
        j = i+1;
        while ((A.values)[i][i] == 0) {
            if(j == M->rows) {
                *det = 0;
                return 0;
            }
            matrix_swap_rows(&A, i, j);
            *det *= -1;
            j++;
        }
        
        for(j = i+1; j < M->rows; j++) {
            aux = - (A.values)[j][i] / (A.values)[i][i];
            matrix_elementary_row_op1(&A, j, i, aux);
        }
    }
    
    for(i = 0; i < M->rows; i++)
        *det *= (A.values)[i][i];

    matrix_destroy(&A);
    return 0;
}

/*
    Transposes the given matrix
*/
uint32_t matrix_transpose(Matrix *M_dest, Matrix *M_src) {
    uint32_t i, j;
    Matrix A;
    if(!(matrix_is_init(M_src) && matrix_is_init(M_dest)))
        return 1;

    if((M_src->rows)!=(M_dest->columns) || (M_src->columns)!=(M_dest->rows))
        return 1;

    matrix_init(&A, (M_src->rows), (M_src->columns));
    matrix_clone(&A, M_src);

    for(i = 0; i < A.rows; i++) {
        for(j = 0; j < A.columns; j++) {
            (M_dest->values)[j][i]=(A.values)[i][j];
        }
    }

    matrix_destroy(&A);
    return 0;
}

/*
    Sets the given matrix to the identity matrix
*/
uint32_t matrix_identity(Matrix *M) {
    uint32_t i, j;

    if(!matrix_is_init(M) || !matrix_is_square(M))
        return 1;

    for(i = 0; i < M->rows; i++) {
        for(j = 0; j < M->columns; j++) {
            if(i == j)
                (M->values)[i][j] = 1;
            else
                (M->values)[i][j] = 0;
        }
    }
    return 0;
}

/*
    Sets the matrix entries to 0
*/
uint32_t matrix_zero(Matrix *M) {
    uint32_t i, j;
    if(!matrix_is_init(M))
        return 1;

    for(i = 0; i < M->rows; i++)
        for(j = 0; j < M->columns; j++)
            (M->values)[i][j] = 0;

    return 0;
}

/*
    Copies the values from one matrix to another
*/
uint32_t matrix_clone(Matrix *M_dest, Matrix *M_src) {
    uint32_t i, j;

    if(!(matrix_is_init(M_dest) && matrix_is_init(M_src)))
        return 1;

    if(!(M_dest->columns == M_src->columns && M_dest->rows == M_src->rows))
        return 1;

    for(i = 0; i < M_src->rows; i++)
        for(j = 0; j < M_src->columns; j++)
            (M_dest->values)[i][j] = (M_src->values)[i][j];
    
    return 0;
}

/*
    Computes the matrix inverse using Gauss-Jordan elimination
*/
uint32_t matrix_inverse(Matrix *M_dest, Matrix *M_src) {
    if(!(matrix_is_init(M_dest) && matrix_is_init(M_src)))
        return 1;

    if(!matrix_is_square(M_src))
        return 1;

    if(!matrix_have_same_dimentions(M_dest, M_src))
        return 1;

    uint32_t i, j, row_pivot;
    double pivot;
    Matrix A;

    if(!matrix_is_invertible(M_src))
        return 1;

    matrix_init(&A, M_src->columns, M_src->rows);

    matrix_clone(&A, M_src);

    matrix_identity(M_dest);

    // sort rows
    for(i = 0; i < A.columns; i++) {
        row_pivot = i;
        for(j = i; j < A.rows; j++) {
            if(fabs((A.values)[j][i]) >= fabs((A.values)[row_pivot][i]))
                row_pivot = j;
        }
        matrix_swap_rows(&A, i, row_pivot);
        matrix_swap_rows(M_dest, i, row_pivot);

        pivot = 1 / (A.values)[i][i];
        matrix_scale_row(&A, i, pivot);
        matrix_scale_row(M_dest, i, pivot);

        for(j = 0; j < A.rows; j++) {
            if(j == i)
                continue;

            pivot = -(A.values)[j][i];
            matrix_elementary_row_op1(&A, j, i, pivot);
            matrix_elementary_row_op1(M_dest, j, i, pivot);
        }
    }

    matrix_destroy(&A);
    return 0;
}

/*
    Performs an element-wise operation between two matrices
*/
uint32_t matrix_entrywise_operation(Matrix *M_dest, Matrix *M_x, Matrix *M_y, double (operacion)(double x, double y)) {
    if(!(matrix_is_init(M_x) && matrix_is_init(M_y) && matrix_is_init(M_dest)))
        return 1;

    if(!(matrix_have_same_dimentions(M_x, M_y) && matrix_have_same_dimentions(M_dest, M_x)))
        return 1;
    
    uint32_t i, j;
    Matrix A, B;

    matrix_init(&A, M_x->rows, M_x->columns);
    matrix_init(&B, M_y->rows, M_y->columns);

    matrix_clone(&A, M_x);
    matrix_clone(&B, M_y);

    for(i = 0; i < M_dest->rows; i++) 
        for(j = 0; j < M_dest->columns; j++)
            (M_dest->values)[i][j] = operacion((A.values)[i][j], (B.values)[i][j]);

    matrix_destroy(&A);
    matrix_destroy(&B);
    return 0;
}

/*
    Changes the sign of the entries of the src matrix
*/
uint32_t matrix_negate(Matrix *M_dest, Matrix *M_src) {
    if(!(matrix_is_init(M_dest) && matrix_is_init(M_src)))
        return 1;

    if(!matrix_have_same_dimentions(M_dest, M_src))
        return 1;

    uint32_t i, j;
    Matrix A;
    matrix_init(&A, M_src->rows, M_src->columns);
    matrix_clone(&A, M_src);
    for(i = 0; i < M_src->rows; i++)
        for(j = 0; j < M_src->columns; j++)
            (M_dest->values)[i][j] = - (A.values)[i][j];

    matrix_destroy(&A);
    return 0;
}

/*
    Performs multiplication of two given matrices
*/
uint32_t matrix_multiplication(Matrix *M_dest, Matrix *M_x, Matrix *M_y) {
    if(!(matrix_is_init(M_x) && matrix_is_init(M_y) && matrix_is_init(M_dest)))
        return 1;

    if(M_x->columns != M_y->rows)
        return 1;

    if(!(M_dest->rows == M_x->rows && M_dest->columns == M_y->columns))
        return 1;
    
    uint32_t i, j, k;
    Matrix A, B;

    matrix_init(&A, M_x->rows, M_x->columns);
    matrix_init(&B, M_y->rows, M_y->columns);

    matrix_clone(&A, M_x);
    matrix_clone(&B, M_y);

    for(i = 0; i < A.rows; i++)
        for(j = 0; j < B.columns; j++) {
            (M_dest->values)[i][j] = 0;
            for(k = 0; k < A.columns; k++)
                (M_dest->values)[i][j] += (A.values)[i][k] * (B.values)[k][j];
        }
            

    matrix_destroy(&A);
    matrix_destroy(&B);
    return 0;
}

/*
    Computes the Euclidean norm of a matrix
*/
uint32_t matrix_euclidean_norm(Matrix *M, double *norm) {
    if(!matrix_is_init(M))
        return 1;

    uint32_t i, j;
    *norm = 0;

    for(i = 0; i < M->rows; i++)
        for(j = 0; j < M->columns; j++)
            *norm += pow((M->values)[i][j], 2);
    
    *norm = sqrt(*norm);

    return 0;
}
/*
    Computes the Sum norm of a matrix
*/
uint32_t matrix_sum_norm(Matrix *M, double *norm) {
    if(!matrix_is_init(M))
        return 1;

    uint32_t i, j;
    *norm = 0;

    for(i = 0; i < M->rows; i++)
        for(j = 0; j < M->columns; j++)
            *norm += fabsl((M->values)[i][j]);

    return 0;
}

/*
    Computes the Max norm of a matrix
*/
uint32_t matrix_max_norm(Matrix *M, double *norm) {
    if(!matrix_is_init(M))
        return 1;

    if(M->columns != 1)
        return 1;

    uint32_t i, j;
    double aux;
    *norm = 0;

    for(i = 0; i < M->rows; i++) 
        for(j = 0; j < M->columns; j++) {
            aux = fabsl((M->values)[i][0]);
            if(aux > *norm)
                *norm = aux;
        }

    return 0;
}

double add(double x, double y) {
    return x + y;
}

double subtract(double x, double y) {
    return x - y; 
}