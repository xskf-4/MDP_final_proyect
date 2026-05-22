/**
 * @file read_input.c
 * @brief Implementation of parsing input files proccess
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "MDP.h"

#include "read_input.h"

typedef struct {
    uint32_t n_states;
    uint32_t n_decitions;
    uint8_t *states_seen;
    size_t state;
} Reading_status;

static uint32_t get_word_list(List *list, FILE *f) {
    char word[MAX_WORD_SIZE], c[2] = {0, 0};
    size_t len = 0;

    memset(word, 0, MAX_WORD_SIZE);

    do {
        c[0] = c[1];
        c[1] = getc(f);

        c[1] = tolower(c[1]);
        if(c[1] == '\n')
            c[1] = ' ';

        // Build word
        if(is_valid_char_in_word(c[1])) {
            word[len] = c[1];
            len++;
        }

        // Append word
        if(!is_valid_char_in_word(c[1]) && is_valid_char_in_word(c[0]) && len > 0) {
            if(append_word_to_list(list, word, len) != 0)
                return -1;

            len = 0;
        }

        if(len == MAX_WORD_SIZE)
            return -1;

    } while(c[1] != EOF);

    return 0;
}

static void read_tag(Reading_status *reading_status, char **word, ListNode **node) {
    if(reading_status->n_decitions == 0 && compare_strings_from_to(*word, DECITIONS_TAG, 1)) {
        *node = list_next(*node);
        *word = list_data(*node);
        sscanf(*word, "%d", &(reading_status->n_decitions));
        return;
    }

    if(reading_status->n_states == 0 && compare_strings_from_to(*word, STATES_TAG, 1)) {
        *node = list_next(*node);
        *word = list_data(*node);
        sscanf(*word, "%d", &(reading_status->n_states));
        return;
    }

    return;
}

static uint32_t read_matrix_from_starting_node(Matrix *M, ListNode *node) {
    size_t i, j;
    char *word;

    for(i = 0; i < (M->rows); i++) {
        for(j = 0; j < (M->columns); j++) {
            if(node == NULL)
                return -1;

            word = list_data(node);

            if(word == NULL)
                return -1;

            if((M->values)[i][j] != 0) {
                printf("\nMatriz ya leida\n");
                return -1;
            }

            if(read_double_from_str(word, &(M->values)[i][j]) != 1) {
                printf("\nNo se pudo leer numero para \'%s\'\n", word);
                return -1;
            }

            node = list_next(node);
        }
    }

    return 0;
}

static uint32_t read_word_list(MDP *mdp, List *word_list) {
    if(list_size(word_list) < 4)
        return -1;
    
    ListNode *node = list_head(word_list);
    char *word;

    Reading_status reading_status = {
        .n_decitions = 0,
        .n_states = 0,
        .states_seen = NULL,
        .state = 0
    };

    // Read n_decitions and n_states
    while(node != NULL && (reading_status.n_states == 0 || reading_status.n_decitions == 0)) {
        word = list_data(node);
        if(word[0] == '#') {
            read_tag(&reading_status, &word, &node);
        }
        node = list_next(node);
    }

    // Init MDP
    if(reading_status.n_states < 1 || reading_status.n_decitions < 1)
        return -1;

    if(MDP_init(mdp, reading_status.n_states, reading_status.n_decitions) != 0)
        return -2;

    reading_status.states_seen = (uint8_t *)malloc(sizeof(uint8_t) * (reading_status.n_decitions + 1));
    if(reading_status.states_seen == NULL)
        return -2;

    // Read info
    node = list_head(word_list);
    while(node != NULL) {
        word = list_data(node);

        if(word[0] != '#' || word[1] == '\n') {
            node = list_next(node);
            continue;
        }

        if(compare_strings_from_to(word, COSTS_TAG, 1)) {
            node = list_next(node);

            if(reading_status.states_seen[reading_status.n_decitions] != 0) {
                printf("\nMatriz de costos o ganancias ya leida\n");
                goto RETURN_STATUS_2;
            }

            if(read_matrix_from_starting_node(&(mdp->costs_matrix), node) != 0) {
                printf("\nInformacion de la matriz de costos incompleta\n");
                goto RETURN_STATUS_2;
            }

            reading_status.states_seen[reading_status.n_decitions]++;
            mdp->type=COSTS_SYSTEM;
        }

        if(compare_strings_from_to(word, REWARD_TAG, 1)) {
            node = list_next(node);

            if(reading_status.states_seen[reading_status.n_decitions] != 0) {
                printf("\nnMatriz de costos o ganancias ya leida\n");
                goto RETURN_STATUS_2;
            }

            if(read_matrix_from_starting_node(&(mdp->costs_matrix), node) != 0) {
                printf("\nInformacion de la matriz de costos incompleta\n");
                goto RETURN_STATUS_2;
            }

            reading_status.states_seen[reading_status.n_decitions]++;
            mdp->type=REWARDS_SYSTEM;
        }

        if(isdigit(word[1])) {
            if(sscanf(word + 1, "%zd", &(reading_status.state)) != 1)
                goto RETURN_STATUS_2;

            reading_status.state --;

            if(reading_status.state >= reading_status.n_decitions) {
                printf("\nLa decision %zd esta fuera del alcance\n", reading_status.state + 1);
                goto RETURN_STATUS_2;
            }
            
            if(reading_status.states_seen[reading_status.state] != 0) {
                printf("\nMatriz de transcicion para la decision %zd ya leida\n", reading_status.state + 1);
                goto RETURN_STATUS_2;
            }

            node = list_next(node);
            if(read_matrix_from_starting_node(&(mdp->transition_matrix_[reading_status.state]), node) != 0) {
                printf("\nInformacion de la matriz de la matriz de transicion para la decision %zd incomleta\n", reading_status.state + 1);
                goto RETURN_STATUS_2;
            }
            
            reading_status.states_seen[reading_status.state]++;
        }

        node = list_next(node);
    }

    return 0;
    RETURN_STATUS_2:
        free(reading_status.states_seen);
        return -2;
}

static uint32_t read_mdp(MDP *mdp, FILE *f) {
    // Init word list
    List word_list;
    list_init(&word_list, free);

    // Set word list
    if(get_word_list(&word_list, f) != 0)
        return -1;

    // print_word_list(word_list);

    // Read word list into mdp
    if(read_word_list(mdp, &word_list) != 0)
        return -1;
        
    // Destroy word list
    list_destroy(&word_list);
    return 0;
}

uint32_t read_mdp_from_file(MDP *mdp, const char *input_file) {
    // Open file
    FILE *file = fopen(input_file, "r");

    if(file == NULL)
        return -1;

    // Read file into mdp
    switch(read_mdp(mdp, file)) {
        case -1:
            return -1;
            break;
        case -2:
            MDP_set_NULL(mdp);
            return -2;
    }

    // Determine lists
    MDP_set_lists(mdp);

    // Close file
    fclose(file);
    return 0;
}