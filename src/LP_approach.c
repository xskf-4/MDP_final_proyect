/**
 * @file LP_aproach.c
 * @brief Implementation of Linear Programming algorithm
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#ifndef MDP_IMPLEMENTATION
    #include "MDP.c"
#endif
#ifndef UTILS_IMPLEMENTATION
    #include "utils.c"
#endif

#include <simplex.h>

void LPP_matrix_form_print(LPP_matrix_form LPP, MDP mdp) {
    // print A | b
    size_t i, j, l;
    uint32_t decision;
    ListNode *node;
    char label[100];
    size_t n_constrains = (LPP.A).rows, n_variables = (LPP.A).columns;
    printf("\n\n");
    terminal_color_subtitle();
    switch(LPP.type) {
        case MIN:
            printf("MIN\n");
            break;
        case MAX:
            printf("MAX\n");
            break;
        default:
            printf("??\n");
    }
    terminal_color_content();
    printf("|");
    l = 0;
    for(i = 0; i < (mdp.n_states); i++) {
        node = list_head((mdp.valid_state_decisions_lists + i));
        while (node != NULL) {
            decision = *(uint32_t *)list_data(node);
            sprintf(label, "y_%zu,%u", i, decision);
            printf(" %*s", NUMBER_LENGTH, label);
            node = list_next(node);
            l++;
        }
    }
    printf(" |\n");
    // print c
    matrix_print(LPP.c, NUMBER_LENGTH, NUMBER_DECIMALS);
    for(i = 0; i < n_constrains; i++) {
        printf("| ");
        for(j = 0; j < n_variables; j++)
            printf("%*.*lf ", NUMBER_LENGTH, NUMBER_DECIMALS, (LPP.A).values[i][j]);

        switch(LPP.constrain_type_[i]) {
            case EQUAL_CONSTRAIN:
                printf(" == ");
                break;
            case GREATER_THAN_OR_EQUAL_CONSTRAIN:
                printf(" >= ");
                break;
            case LESS_THAN_OR_EQUAL_CONSTRAIN:
                printf(" <= ");
                break;
            default:
                printf(" ?? ");
        }

        printf("%*.*lf |\n", NUMBER_LENGTH, NUMBER_DECIMALS, (LPP.b).values[i][0]);
    }

    print_int32_array((int32_t *)LPP.variable_type_, n_variables, 2);
}

void LP_approach_print_results(MDP mdp, LPP_matrix_form LPP) {
    size_t i, j, l;
    ListNode *node;
    uint32_t decision;

    char *s[2];
    MDP_set_labels(s, mdp);

    char *res;
    switch(LPP.solution_type) {
        case FEASIBLE_SOLUTION:
            res = "Solucion factible encontrada";
            break;
        case NON_FEASIBLE_SOLUTION:
            res = "No tiene solucion";
            break;
        case NON_BOUNDED_SOLUTION:
            res = "Solucion no acotada";
        case MAX_ITERATION:
            res = "Maximo de iteraciones alcanzadas";
            break;
    }
    terminal_color_subtitle();
    printf("\n\n%s\n", res);
    terminal_color_content();
    printf("z = %+-*.*lf\n", NUMBER_LENGTH, NUMBER_DECIMALS, LPP.z);
    l = 0;
    for(i = 0; i < (mdp.n_states); i++) {
        node = list_head((mdp.valid_state_decisions_lists + i));
        while (node != NULL) {
            decision = *(uint32_t *)list_data(node);
            printf("y_%zu,%u = %+-*.*lf\n", i, decision, NUMBER_LENGTH, NUMBER_DECIMALS, (LPP.x).values[l][0]);
            node = list_next(node);
            l++;
        }
    }

    terminal_color_subtitle();
    printf("\n\nPolitica optima: ");
    Policy_print((mdp.optimal_policy), s);
}

uint32_t LP_approach_set_n_variables(size_t *n_variables, MDP *mdp) {
    size_t i;

    *n_variables = 0;
    for(i = 0; i < mdp->n_decisions; i++)
        *n_variables += list_size(mdp->valid_decision_states_lists + i);

    return 0;
}

uint32_t LP_approach_set_objective_function(LPP_matrix_form *LPP, MDP *mdp) {
    uint32_t i, j, l;
    ListNode *node;

    // MAX / MIN
    if(mdp->type == REWARDS_SYSTEM)
        LPP->type = MAX;
    if(mdp->type == COSTS_SYSTEM)
        LPP->type = MIN;

    /* Costs values */
    // state
    l = 0;
    for(i = 0; i < mdp->n_states; i++) {
        // decition
        node = list_head(mdp->valid_state_decisions_lists + i);
        while (node != NULL) {
            j = *(uint32_t *)list_data(node) - 1;

            (LPP->c).values[0][l] = (mdp->costs_matrix).values[i][j];

            node = list_next(node);
            l++;
        }
    }
    return 0;
}

uint32_t LP_approach_set_constrains(LPP_matrix_form *LPP, MDP *mdp) {
    // first constrain
    size_t i, j, l, k, w;
    uint32_t n_variables = (LPP->A).columns, n_constrains = (LPP->A).rows;
    ListNode *node;
    for(i = 0; i < n_variables; i++)
        (LPP->A).values[n_constrains - 1][i] = 1.0;

    (LPP->b).values[n_constrains - 1][0] = 1.0;

    w = 0;
    for(j = 0; j < n_constrains - 1; j++) {
        // first sum
        node = list_head(mdp->valid_state_decisions_lists + j);
        while(node != NULL) {
            i = *(uint32_t *)list_data(node) - 1;
            (LPP->A).values[j][w] += 1.0;
            node = list_next(node);
            w ++;
        }
        // second sum
        // states
        l = 0;
        for(i = 0; i < mdp->n_states; i++) {
            // decisions
            node = list_head(mdp->valid_state_decisions_lists + i);
            while(node != NULL) {
                k = *(uint32_t *)list_data(node) - 1;
                (LPP->A).values[j][l] -= (mdp->transition_matrix_)[k].values[i][j];
                node = list_next(node);
                l++;
            }
        }
    }
    // All constrain is equality
    for(i = 0; i < n_constrains; i++)
        (LPP->constrain_type_)[i] = EQUAL_CONSTRAIN;
    return 0;
}

uint32_t LP_approach_set_variable_types(LPP_matrix_form *LPP) {
    size_t i;
    for(i = 0; i < LPP->c.columns; i++)
        (LPP->variable_type_)[i] = NON_NEGATIVE_VARIABLE;
    return 0;
}

uint32_t LP_approach_set_LPP(LPP_matrix_form *LPP, MDP *mdp) {
    // set objective function
    LP_approach_set_objective_function(LPP, mdp);
    // set constrains
    LP_approach_set_constrains(LPP, mdp);
    // set variable types
    LP_approach_set_variable_types(LPP);
    return 0;
}

uint32_t Set_optimal_policy(MDP *mdp, Matrix x, double z) {
    size_t i, j;
    Matrix D;
    matrix_init(&D, (mdp->n_states), (mdp->n_decisions));
    // Set matrix D
    size_t l;
    double sum_y_j, norm;
    ListNode *node;
    uint32_t decision;
    l = 0;

    for(i = 0; i < D.rows; i++) {
        // sum y_j
        sum_y_j = 0.0;
        node = list_head(mdp->valid_state_decisions_lists + i);
        for(j = 0; j < list_size(mdp->valid_state_decisions_lists + i); j++) {
            decision = *(uint32_t *)list_data(node) - 1;

            (D.values)[i][decision] = (x.values)[l][0];
            sum_y_j += (x.values)[l][0];
            node = list_next(node);
            l++;
        }
        // Set stationary_vector values
        (mdp->optimal_policy).stationary_vector.values[0][i] = sum_y_j;

        if(!are_equal(sum_y_j, 0.0))
            for(j = 0; j < D.columns; j++)
                (D.values)[i][j] /= sum_y_j;
    }
    
    // Set average cost
    (mdp->optimal_policy).expected_average_cost = z;

    for(i = 0; i < D.rows; i++) {
        (mdp->optimal_policy.decision_by_state_)[i] = 0;
        for(j = 0; j < D.columns; j++) {
            if(are_equal((D.values)[i][j], 1.0))
                (mdp->optimal_policy.decision_by_state_)[i] = j + 1;
        }

        if((mdp->optimal_policy.decision_by_state_)[i] == 0) {
            node = list_head(mdp->valid_state_decisions_lists + i);
            (mdp->optimal_policy.decision_by_state_)[i] = *(uint32_t *)list_data(node);
        }
    }

    Policy_set_from_array(&(mdp->optimal_policy), (mdp->optimal_policy).decision_by_state_, *mdp);

    norm = 0.0;
    matrix_sum_norm(&(mdp->optimal_policy).stationary_vector, &norm);

    if(are_equal(norm, 1.0))
        (mdp->optimal_policy).is_valid = 1;

    matrix_destroy(&D);
    return 0;
}


uint32_t LP_approach_solve_MDP(MDP *mdp) {
    LPP_matrix_form LPP;
    size_t n_variables, n_constrains;
    // set n_variables
    LP_approach_set_n_variables(&n_variables, mdp);
    // set n_constrains
    n_constrains = mdp->n_states;
    // init LPP
    printf("\nNumero de Variables: %zu\nNumero de restricciones: %zu", n_variables, n_constrains);
    LPP_matrix_form_init(&LPP, n_variables, n_constrains);

    // Set LPP
    LP_approach_set_LPP(&LPP, mdp);
    
    LPP_matrix_form_print(LPP, *mdp);

    // Solve LPP
    Simplex_solve(&LPP);

    // Get solution from LPP
    Set_optimal_policy(mdp, LPP.x, LPP.z);

    // Print Results
    LP_approach_print_results(*mdp, LPP);

    LPP_matrix_form_destroy(&LPP);
    return 0;
}