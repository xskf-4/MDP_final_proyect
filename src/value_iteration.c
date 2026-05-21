/**
 * @file value_iteration.c
 * @brief Implementation of value iteration algorithm
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/

#include "utils.h"
#include "MDP.h"

typedef struct {
    double discount_factor;
    double tol;
    uint32_t max_iterations;
    uint32_t iteration;
    uint8_t (*comparison)(double, double);
    Matrix V_n;
    Matrix V_last;
    Policy P;
} value_iteration;

uint32_t value_iteration_init(value_iteration *X, MDP *mdp) {
    matrix_init_column_vectors(mdp->n_states, 2, &(X->V_n), &(X->V_last));
    Policy_init(&(X->P), (mdp->n_states));

    switch(mdp->type) {
        case COSTS_SYSTEM:
            X->comparison = &min_comparison;
            break;
        case REWARDS_SYSTEM:
            X->comparison = &max_comparison;
            break;
        default:
            X->comparison = NULL;
            break;
    }

    X->iteration = 1;
    return 0;
}

uint32_t value_iteration_destroy(value_iteration *X) {
    matrix_destroy_matrices(2, &(X->V_n), &(X->V_last));
    Policy_destroy(&(X->P));
    return 0;
}

uint32_t value_iteration_set_init_V(value_iteration *X, MDP *mdp) {
    size_t i;
    uint32_t decision, optimal_decision;
    ListNode *node;
    double optimal_c;

    for(i = 0; i < mdp->n_states; i++) {
        node = list_head(mdp->valid_state_decisions_lists + i);
        while(node != NULL) {
            decision = *(uint32_t *)list_data(node) - 1;

            if(node == list_head(mdp->valid_state_decisions_lists + i)) {
                optimal_c = (mdp->costs_matrix).values[i][decision];
                optimal_decision = decision;
                node = list_next(node);
                continue;
            }

            if((X->comparison)(((mdp->costs_matrix).values[i][decision]), optimal_c)) {
                optimal_c = (mdp->costs_matrix).values[i][decision];
                optimal_decision = decision;
            }

            node = list_next(node);
        }

        (X->V_n).values[i][0] = optimal_c;
        (X->P).decision_by_state_[i] = optimal_decision + 1; 
    }

    return 0;
}

uint32_t value_iteration_set_init_values(value_iteration *X, MDP *mdp) {
    char input[MAX_INPUT_SIZE], *response = "";

    memset(input, '\0', MAX_INPUT_SIZE);
    /* Read init values */
    // Read discount factor
    printf("\n\n");
    response = "";
    do {
        printf("Ingrese el factor de descuento%s: ", response);
        fgets(input, MAX_INPUT_SIZE, stdin);
        fflush(stdin);

        replace_first_char(input, '\n', '\0');
    } while(!read_discount_factor_from_input(&(X->discount_factor), input, &response));
    memset(input, '\0', MAX_INPUT_SIZE);

    // read max iterations
    response = "";
    do {
        printf("Ingrese maximo de iteraciones%s: ", response);
        fgets(input, MAX_INPUT_SIZE, stdin);
        fflush(stdin);

        replace_first_char(input, '\n', '\0');
    } while(!read_max_iterations_from_input((int32_t *)&(X->max_iterations), input, &response));
    memset(input, '\0', MAX_INPUT_SIZE);

    // Read tol
    response = "";
    do {
        printf("Ingrese la tolerancia%s: ", response);
        fgets(input, MAX_INPUT_SIZE, stdin);
        fflush(stdin);

        replace_first_char(input, '\n', '\0');
    } while(!read_tol_from_input(&(X->tol), input, &response));

    terminal_color_subtitle();
    printf("\nFactor de descuento ingresado: ");
    terminal_color_content();
    printf("%*.*lf", NUMBER_DECIMALS + 3, NUMBER_DECIMALS, X->discount_factor);
    terminal_color_subtitle();
    printf("\nMaximo de iteraciones: ");
    terminal_color_content();
    printf("%u", (X->max_iterations));
    terminal_color_subtitle();
    printf("\nTolerancia: ");
    terminal_color_content();
    printf("%*.*lf\n", NUMBER_LENGTH, NUMBER_DECIMALS, (X->tol));

    /* Set optimal V(1) */
    value_iteration_set_init_V(X, mdp);
    (X->P).is_valid = 1;

    return 0;
}

uint32_t value_iteration_set_new_V(value_iteration *X, MDP *mdp) {
    size_t i, j;
    List *decision_list;
    ListNode *node;
    uint32_t optimal_decision, decision;
    double optimal_C, C;

    for(i = 0; i < mdp->n_states; i++) {
        decision_list = mdp->valid_state_decisions_lists + i;
        node = list_head(decision_list);
        while (node != NULL) {
            // set decision
            decision = *(uint32_t *)list_data(node) - 1;
            // set C value
            C = 0.0;
            for(j = 0; j < mdp->n_states; j++)
                C += (mdp->transition_matrix_ + decision)->values[i][j] * (X->V_last).values[j][0];

            C *= (X->discount_factor);
            C += (mdp->costs_matrix).values[i][decision];

            // set optimal C && optimal decision
            if(node == list_head(decision_list)) {
                optimal_C = C;
                optimal_decision = decision;
                node = list_next(node);
                continue;
            }

            if((X->comparison)(C, optimal_C)) {
                optimal_C = C;
                optimal_decision = decision;
            }

            node = list_next(node);
        }

        (X->P).decision_by_state_[i] = optimal_decision + 1;
        (X->V_n).values[i][0] = optimal_C;
    }

    return 0;
}

uint32_t value_iteration_optimality_test(value_iteration *X) {
    size_t i;

    if(X->iteration >= X->max_iterations)
        return 1;

    for(i = 0; i < (X->V_n).rows; i++) {
        if(fabs((X->V_n).values[i][0] - (X->V_last).values[i][0]) >= X->tol)
            return 0;
    }

    return 1;
}

void value_iteration_print(value_iteration X) {
    size_t i;
    char label[MAX_WORD_SIZE];
    terminal_color_subtitle();
    printf("\nIteracion: %u", X.iteration);
    terminal_color_content();
    printf("\n| ");
    for(i = 0; i < (X.V_n).rows; i++) {
        sprintf(label, "V_%zu", i);
        printf("%*s ", NUMBER_LENGTH, label);
    }
    printf("|\n| ");
    for(i = 0; i < X.V_n.rows; i++)
        printf("%*.*lf ", NUMBER_LENGTH, NUMBER_DECIMALS, (X.V_n).values[i][0]);
    printf("|\n");
    terminal_color_subtitle();
    printf("\nPolitica: ");
    terminal_color_content();
    print_uint32_array((X.P).decision_by_state_, (X.V_n).rows, 3);
    printf("\n");
}

uint32_t value_iteration_solve_MDP(MDP *mdp) {
    value_iteration X;
    value_iteration_init(&X, mdp);

    // step 1 (Set init values)
    value_iteration_set_init_values(&X, mdp);
    do {
        value_iteration_print(X);
        matrix_clone(&(X.V_last), &(X.V_n));
        X.iteration ++;
        
        // step 2
        value_iteration_set_new_V(&X, mdp);
    } while(!value_iteration_optimality_test(&X));
    
    value_iteration_print(X);

    Policy_set_from_array(&(mdp->optimal_policy), (X.P).decision_by_state_, *mdp);

    // Print results
    Policy_get_stationary_vector(&(mdp->optimal_policy), NULL);
    Policy_get_expected_average_cost(&(mdp->optimal_policy), &(mdp->costs_matrix));
    policy_improvement_discount_factor_print_results(mdp);

    value_iteration_destroy(&X);
    return 0;
}
