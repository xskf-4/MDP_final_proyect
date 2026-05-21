/**
 * @file utils.c
 * @brief Implementation of global interfaces
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "list.h"
#include "matrix_.h"

#include "utils.h"

uint32_t Matrix_system_init(Matrix_system *S, uint32_t n_variables) {
    if(S == NULL)
        return 1;

    if(matrix_init_square_matrices(n_variables, 2, &(S->A), &(S->inv_A)) != 0)
        return 1;
    
    if(matrix_init_column_vectors(n_variables, 2, &(S->x), &(S->b)) != 0)
        return 1;

    return 0;
}

uint32_t Matrix_system_destroy(Matrix_system *S) {
    if(S == NULL)
        return 1;

    if(matrix_destroy_matrices(4, &(S->A), &(S->inv_A), &(S->b), &(S->x)) != 0)
        return 1;

    return 0;
}

uint32_t Matrix_system_solve(Matrix_system *S) {
    if(matrix_inverse(&(S->inv_A), &(S->A)) != 0)
        return 1;
    
    if(matrix_multiplication(&(S->x), &(S->inv_A), &(S->b)) != 0)
        return 1;

    return 0;
}

void Matrix_system_print(Matrix_system S, uint8_t num_len, uint8_t n_decimals) {
    size_t i, j;

    for(i = 0; i < S.A.rows; i++) {
        printf("| ");
        // print A
        for(j = 0; j < S.A.columns; j++)
            printf("%+*.*lf ", num_len, n_decimals, S.A.values[i][j]);

        // print b
        printf("= %+*.*lf |\n", num_len, n_decimals, S.b.values[i][0]);
    }
}

uint32_t append_word_to_list(List *list, char *word, size_t len) {
    char *new_word = (char *)malloc(sizeof(char ) * (len + 1));
    size_t i;
    if(new_word == NULL)
        return -1;

    new_word[len] = '\0';
    for(i = 0; i < len; i++)
        new_word[i] = word[i];

    list_append(list, new_word);
    return 0;
}

uint32_t uint32_t_value_in_list(uint32_t x, List *l) {
    ListNode *node;
    node = list_head(l);
    while(node != NULL) {
        if(x == *(uint32_t *)list_data(node))
            return 1;

        node = list_next(node);
    }

    return 0;
}

uint32_t compare_strings_from_to(char *str_1, char *str_2, size_t str_1_init) {
    size_t i = 0;
    do {
        if(str_1[str_1_init + i] != str_2[i])
            return 0;

        i++;
    } while(str_1[str_1_init + i] != '\0' && str_2[i] != '\0');

    if(str_1[str_1_init + i] != str_2[i])
        return 0;
    
    return 1;
}

void replace_first_char(char *src, char target, char c) {
    size_t i = 0;
    while (src[i] != target) {
        if(src[i] == '\0')
            return;
        
        i++;
    }
    
    src[i] = c;
}

void matrix_print(Matrix M, uint8_t num_len, uint8_t n_decimals) {
    size_t i, j;
    for(i = 0; i < M.rows; i++) {
        printf("| ");
        for(j = 0; j < M.columns - 1; j++)
            printf("%*.*lf ", num_len, n_decimals, (M.values)[i][j]);

        printf("%*.*lf |\n", num_len, n_decimals, (M.values)[i][j]);
    }
}

uint32_t read_double_from_str(char *str, double *number) {
    if(str == NULL)
        return 0;
    // Check '/' in str
    size_t i = 0;
    uint8_t counter = 0;
    while(str[i] != '\0') {
        if(!is_valid_char_in_number(str[i]))
            return 0;

        if(str[i] == '/')
            counter++;

        if(counter > 1)
            return 0;

        i++;
    }

    // if there's no '/'
    if(counter == 0) {
        if(sscanf(str, "%lf", number) != 1)
            return 0;
        
        return 1;
    }

    double a, b;
    if(sscanf(str, "%lf/%lf", &a, &b) != 2)
        return 0;

    if(b == 0 || isnan(a) || isnan(b))
        return 0;

    *number = a / b;

    return 1;
}

uint32_t read_discount_factor_from_input(double *discount_factor, char *src, char **response) {
    if(read_double_from_str(src, discount_factor) != 1) {
        *response = " (Valor no legible)";
        return 0;
    }

    if(!(0.0 <= *discount_factor && *discount_factor <= 1.0)) {
        *response = " (Numero no valido)";
        return 0;
    }
    
    return 1;
}

uint32_t read_tol_from_input(double *tol, char *src, char **response) {
    if(read_double_from_str(src, tol) != 1) {
        *response = " (Valor no legible)";
        return 0;
    }

    if(!(0.0 < *tol)) {
        *response = " (Numero no valido)";
        return 0;
    }

    return 1;
}

uint32_t read_max_iterations_from_input(int32_t *max_iterations, char *src, char **response) {
    if(sscanf(src, "%d", max_iterations) != 1) {
        *response = " (Valor no legible)";
        return 0;
    }

    if(!(0.0 <= *max_iterations)) {
        *response = " (Numero no valido)";
        return 0;
    }
    
    return 1;
}

uint32_t matrix_check_values(Matrix M, double *targets, size_t length) {
    double sum;
    size_t i, j;

    for(i = 0; i < M.rows; i++) {
        sum = 0.0;
        for(j = 0; j < M.columns; j++) {
            if(!is_valid_in_matrix(M.values[i][j]))
                return 0;
            sum += M.values[i][j];
        }
        for(j = 0; j < length; j++)
            if(are_equal(sum, targets[j]))
                break;
        
        if(j == length)
            return 0;
    }

    return 1;
}

void print_title(char *str) {
    printf("\n%s\n\n", str);
}

/*
*/
void print_word_list(List l) {
    ListNode *node = list_head(&l);

    while(node != NULL) {
        printf("\'%s\'\n", (char *)list_data(node));
        node = list_next(node);
    }
}

void print_uint32_list(List l) {
    ListNode *node = list_head(&l);
    uint32_t *data;
    printf("[");
    while (node != NULL) {
        data = list_data(node);
        printf("%3u%s", *data, list_is_tail(node) ? "": ", ");

        node = list_next(node);
    }
    printf("]");
}

void print_uint32_array(uint32_t *src, size_t len, uint8_t number_size) {
    size_t i;
    printf("[");
    for(i = 0; i < len; i++) {
        printf("%*u%s", number_size, src[i], (i==len-1)?"":", ");
    }
    printf("]");
}

void print_int32_array(int32_t *src, size_t len, uint8_t number_size) {
    size_t i;
    printf("[");
    for(i = 0; i < len; i++) {
        printf("%*d%s", number_size, src[i], (i==len-1)?"":", ");
    }
    printf("]");
}

uint8_t max_comparison(double x, double y) {
    return (x > y);
}

uint8_t min_comparison(double x, double y) {
    return (x < y);
}