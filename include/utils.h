/**
 * @file utils.h
 * @brief Definition of interfaces, macros and structures for general use
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <ctype.h>

#include "matrix_.h"

/*
    Struct for Matrix system
*/
typedef struct  {
    Matrix b, A, inv_A, x;
} Matrix_system;

/*
    Public Interfaces
*/

// Matrix system interfaces

uint32_t Matrix_system_init(Matrix_system *S, uint32_t n_variables);
uint32_t Matrix_system_destroy(Matrix_system *S);
uint32_t Matrix_system_solve(Matrix_system *S);
void Matrix_system_print(Matrix_system S, uint8_t num_len, uint8_t n_decimals);

// List interfaces

uint32_t append_word_to_list(List *list, char *word, size_t len);
uint32_t uint32_t_value_in_list(uint32_t x, List *l);

// String interfaces

uint32_t compare_strings_from_to(char *str_1, char *str_2, size_t str_1_init);
void replace_first_char(char *src, char target, char c);

// Matrix interfaces

void matrix_print(Matrix M, uint8_t num_len, uint8_t n_decimals);

// Read input interfaces

uint32_t read_double_from_str(char *str, double *number);
uint32_t read_discount_factor_from_input(double *discount_factor, char *src, char **response);
uint32_t read_tol_from_input(double *tol, char *src, char **response);
uint32_t read_max_iterations_from_input(int32_t *max_iterations, char *src, char **response);

// MDP interfaces

uint32_t matrix_check_values(Matrix M, double *targets, size_t length);

// 

void print_title(char *str);
void print_word_list(List l);
void print_uint32_list(List l);
void print_uint32_array(uint32_t *src, size_t len, uint8_t number_size);
void print_int32_array(int32_t *src, size_t len, uint8_t number_size);

// comparison interfaces

uint8_t max_comparison(double x, double y);
uint8_t min_comparison(double x, double y);

/*
    Macros
*/

// Global Macros
#define MAX_INPUT_SIZE 256
#define MAX_WORD_SIZE 256
#define EPSILON 1e-9
// Char Macros
#define is_valid_char_in_number(x) (isdigit((x)) || (x) == '.' || (x) == '-' || (x) == '+' || (x) == '/' || (x) == 'e' || (x) == 'a' || (x) == 'n')
#define is_valid_char_in_word(x) (isalpha((x)) || is_valid_char_in_number((x)) || (x) == '#')
// List Macros
#define list_append(l, x) list_ins_next((l), list_tail((l)), (void *)(x));
// int Macros
#define int_in(x, a, b) ((x) >= (a) && (x) <= (b))
// Number Macros
#define are_equal(x, y) (fabs((x) - (y)) < EPSILON)
// Matrix Macros
#define is_valid_in_matrix(x) ((0.0 - EPSILON <= (x) && (x) <= 1.0 + EPSILON) || are_equal((x), -1.0))
// Cost Char* Macros
#define DECITIONS_TAG "decisiones"
#define STATES_TAG "estados"
#define COSTS_TAG "costos"
#define REWARD_TAG "recompenzas"
// Number Size Macros
#define NUMBER_LENGTH 16
#define NUMBER_DECIMALS 8
// Terminal color macros
#define terminal_color_set(r, g, b) printf("\033[38;2;%s;%s;%sm", (r), (g), (b))
#define terminal_bold_text() printf("\033[1m")
#define terminal_color_reset() printf("\033[0m")
#define terminal_color_main_title()                  \
            terminal_bold_text();                    \
            terminal_color_set("255", "130", "222") // 255, 130, 222

#define terminal_color_title()                       \
            terminal_bold_text();                    \
            terminal_color_set("212", "179", "252") // 212, 179, 252

#define terminal_color_subtitle()                    \
            terminal_color_reset();                  \
            terminal_color_set("138", "208", "255") // 138, 208, 255
            
#define terminal_color_content()                    \
            terminal_color_reset();                 \
            terminal_color_set("211", "219", "242") // 211, 219, 242

#define terminal_color_red() terminal_color_set("217", "13", "38"); // 217, 13, 38
#define terminal_color_green() terminal_color_set("29", "161", "50"); // 29, 161, 50

#endif
    