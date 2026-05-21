/**
 * @file policy_improvement.c
 * @brief Implementation of policy improvement algorithms (with / without discount factor)
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#ifndef MDP_IMPLEMENTATION
    #include "MDP.c"
#endif
#ifndef UTILS_IMPLEMENTATION
    #include "utils.c"
#endif

typedef struct {
    double discount_factor;
    uint32_t iteration;
    uint8_t (*comparison)(double, double);
    Matrix_system S;
    Policy P;
} policy_improvement;

uint32_t policy_improvement_init(policy_improvement *X, MDP *mdp) {
    Policy_init(&(X->P), mdp->n_states);

    switch(mdp->type) {
        case COSTS_SYSTEM:
            X->comparison = &min_comparison;
            break;
        case REWARDS_SYSTEM:
            X->comparison = &max_comparison;
            break;
        default:
            break;
    }

    X->iteration = 1;

    return 0;
}

uint32_t policy_improvement_destroy(policy_improvement *X) {
    Policy_destroy(&(X->P));
    Matrix_system_destroy(&(X->S));

    return 0;
}


uint32_t Policy_read_from_input(Policy *P, char *src, MDP *mdp, char **response) {
    List word_list;
    ListNode *node;
    size_t i, len;
    char c, word[MAX_WORD_SIZE], *data;
    list_init(&word_list, free);

    i = 0;
    len = 0;
    memset(word, '\0', MAX_WORD_SIZE);
    do {
        c = src[i];
        if(!isnumber(c) && len == 0) {
            i++;
            continue;
        }

        if(!isnumber(c) && len != 0) {
            append_word_to_list(&word_list, word, len);
            memset(word, '\0', MAX_WORD_SIZE);
            len = 0;
            i++;
            continue;
        }

        word[len] = c;
        len++;

        i++;
    } while(c != '\0');

    node = list_head(&word_list);
    if((P->M).columns > (uint32_t )list_size(&word_list)) {
        *response = " (Politica incompleta)";
        list_destroy(&word_list);
        return 0;
    }

    for(i = 0; i < (P->M).columns; i++) {
        data = list_data(node);

        sscanf(data, "%d", &(P->decision_by_state_)[i]);

        // Validate Policy
        if(!uint32_t_value_in_list((P->decision_by_state_)[i], (mdp->valid_state_decisions_lists) + i)) {
            list_destroy(&word_list);
            *response = " (Politica no valida)";
            return 0;
        }

        node = list_next(node);
    }

    list_destroy(&word_list);
    return 1;
}

/* Policy improvement */

uint32_t policy_improvement_set_first_policy(Policy *P, MDP *mdp) {
    char input[MAX_INPUT_SIZE], *response = "";
    memset(input, '\0', MAX_INPUT_SIZE);
    terminal_color_content();
    printf("\n\n");
    do {
        printf("Ingrese la politica inicial%s: ", response);
        fgets(input, MAX_INPUT_SIZE, stdin);
        fflush(stdin);
    } while(!Policy_read_from_input(P, input, mdp, &response));

    Policy_set_from_array(P, (P->decision_by_state_), *mdp);

    terminal_color_subtitle();
    printf("\nPolitica ingresada: ");
    terminal_color_content();
    print_uint32_array(P->decision_by_state_, mdp->n_states, 3);
    return 0;
}

void policy_improvement_print_iteration(policy_improvement X) {
    size_t i;
    char label[256];

    terminal_color_subtitle();
    printf("\n\nIteracion: %u", X.iteration);
    printf("\n\nSistema generado:");
    terminal_color_content();
    printf("\n| ");
    for(i = 0; i < (X.S).A.columns - 1; i++) {
        sprintf(label, "V_%zu", i);
        printf("%*s ",  NUMBER_LENGTH, label);
    }
    printf("%*s ", NUMBER_LENGTH, "G(R)");
    printf("|\n");
    Matrix_system_print(X.S, NUMBER_LENGTH, NUMBER_DECIMALS);

    terminal_color_subtitle();
    printf("\nResultados:\n");
    terminal_color_content();
    printf("| ");
    for(i = 0; i < (X.S).A.columns - 1; i++) {
        sprintf(label, "V_%zu", i);
        printf("%*s ", NUMBER_LENGTH, label);
    }
    printf("%*s ", NUMBER_LENGTH, "G(R)");
    printf("|\n| ");
    for(i = 0; i < (X.S).A.columns; i++) {
        printf("%*.*lf ", NUMBER_LENGTH, NUMBER_DECIMALS, (X.S).x.values[i][0]);
    }
    printf("|\n");

    printf("\nDecisiones optimas: ");
    print_uint32_array((X.P).decision_by_state_, (X.S).A.columns - 1, 3);
}

uint32_t policy_improvement_set_value(policy_improvement *X, MDP *mdp) {
    size_t i, j;
    /* Set matrix system */
    // set A
    for(i = 0; i < mdp->n_states; i++) {
        for(j = 0; j < mdp->n_states; j++)
            (X->S).A.values[i][j] = -(X->P).M.values[i][j];

        (X->S).A.values[i][i] ++;
        (X->S).A.values[i][j] = 1;
    }
    (X->S).A.values[i][mdp->n_states - 1] = 1.0;
    // set b
    for(i = 0; i < mdp->n_states; i++)
        (X->S).b.values[i][0] = (mdp->costs_matrix).values[i][(X->P).decision_by_state_[i] - 1];

    (X->S).b.values[i][0] = 0.0;
    /* Solve matrix system */
    if(Matrix_system_solve(&(X->S)) != 0)
        printf("\nError al resolver el sistema\n");

    // Print results
    return 0;
}

uint32_t policy_improvement_set_optimal_decision(uint32_t state, policy_improvement *X, MDP *mdp) {
    List *decision_list = (mdp->valid_state_decisions_lists + state);
    ListNode *node;
    uint32_t optimal_decision, decision;
    size_t i;
    double G_R, optimal_G_R;

    node = list_head(decision_list);
    
    while (node != NULL) {
        // Caltulate G(R)
        decision = *(uint32_t *)list_data(node) - 1;
        G_R = (mdp->costs_matrix).values[state][decision];

        for(i = 0; i < mdp->n_states; i++)
            G_R += (mdp->transition_matrix_ + decision)->values[state][i] * ((X->S).x.values[i][0]);
        
        G_R -= ((X->S).x.values[state][0]);

        if(node == list_head(decision_list)) {
            optimal_G_R = G_R;
            optimal_decision = decision;
        }

        if((X->comparison)(G_R, optimal_G_R)) {
            optimal_G_R = G_R;
            optimal_decision = decision;
        }

        node = list_next(node);
    }
    
    return optimal_decision + 1;
}

uint32_t policy_improvement_improve(policy_improvement *X, MDP *mdp) {
    size_t i;
    uint32_t *optimal_decisions;

    optimal_decisions = (uint32_t *)malloc(sizeof(uint32_t) * (mdp->n_states));

    // calculate optimal_decisions decision G(R)
    
    for(i = 0; i < mdp->n_states; i++)
        optimal_decisions[i] = policy_improvement_set_optimal_decision(i, X, mdp);

    Policy_set_from_array(&(X->P), optimal_decisions, *mdp);
    free(optimal_decisions);
    return 0;
}

void policy_improvement_print_results(MDP *mdp) {
    char *s[2];
    MDP_set_labels(s, *mdp);
    terminal_color_subtitle();
    printf("\n\nPolitica optima: ");
    terminal_color_content();
    Policy_print(mdp->optimal_policy, s);
}

uint32_t policy_improvement_solve_MDP(MDP *mdp) {
    policy_improvement X;
    policy_improvement_init(&X, mdp);
    Matrix_system_init(&X.S, mdp->n_states + 1);

    // step 0 (Set arbitrary policy)
    policy_improvement_set_first_policy(&(X.P), mdp);

    do {
        Policy_clone(&(mdp->optimal_policy), &(X.P));
        // step 1 (Set policy value)
        policy_improvement_set_value(&X, mdp);

        // step 2 (Policy improve)
        policy_improvement_improve(&X, mdp);
        policy_improvement_print_iteration(X);
        X.iteration++;
    } while(!Policy_are_equal(mdp->optimal_policy, X.P));

    // Print results
    Policy_get_stationary_vector(&(mdp->optimal_policy), NULL);
    (mdp->optimal_policy).expected_average_cost = (X.S).x.values[mdp->n_states][0];
    policy_improvement_print_results(mdp);
    /*
    */

    policy_improvement_destroy(&X);
    return 0;
}

/* Policy improvement (Discount Factor) */

uint32_t policy_improvement_discount_factor_set_init_values(policy_improvement *X, MDP *mdp) {
    char input[MAX_INPUT_SIZE], *response = "";
    memset(input, '\0', MAX_INPUT_SIZE);
    // Read first policy
    terminal_color_content();
    printf("\n\n");
    do {
        printf("Ingrese la politica inicial%s: ", response);
        fgets(input, MAX_INPUT_SIZE, stdin);
        fflush(stdin);

        replace_first_char(input, '\n', '\0');
    } while(!Policy_read_from_input(&(X->P), input, mdp, &response));

    // Read discount factor
    response = "";
    do {
        printf("Ingrese el factor de descuento%s: ", response);
        fgets(input, MAX_INPUT_SIZE, stdin);
        fflush(stdin);

        replace_first_char(input, '\n', '\0');
    } while(!read_discount_factor_from_input(&(X->discount_factor), input, &response));

    Policy_set_from_array(&(X->P), ((X->P).decision_by_state_), *mdp);

    char *s[2];
    MDP_set_labels(s, *mdp);
    terminal_color_subtitle();
    printf("\nPolitica ingresada: ");
    terminal_color_content();
    print_uint32_array((X->P).decision_by_state_, mdp->n_states, 3);
    terminal_color_subtitle();
    printf("\nFactor de descuento ingresado: ");
    terminal_color_content();
    printf("%*.*lf", NUMBER_DECIMALS + 2, NUMBER_DECIMALS, (X->discount_factor));
    return 0;
}

void policy_improvement_discount_print_iteration(policy_improvement X) {
    size_t i;
    char label[256];

    terminal_color_subtitle();
    printf("\n\nIteracion: %u", X.iteration);
    printf("\n\nSistema generado:");
    terminal_color_content();
    printf("\n| ");
    for(i = 0; i < (X.S).A.columns; i++) {
        sprintf(label, "V_%zu", i);
        printf("%*s ",  NUMBER_LENGTH, label);
    }
    printf("|\n");
    Matrix_system_print(X.S, NUMBER_LENGTH, NUMBER_DECIMALS);

    terminal_color_subtitle();
    printf("\nResultados:\n");
    terminal_color_content();
    printf("| ");
    for(i = 0; i < (X.S).A.columns; i++) {
        sprintf(label, "V_%zu", i);
        printf("%*s ", NUMBER_LENGTH, label);
    }
    printf("|\n| ");
    for(i = 0; i < (X.S).A.columns; i++) {
        printf("%*.*lf ", NUMBER_LENGTH, NUMBER_DECIMALS, (X.S).x.values[i][0]);
    }
    printf("|\n");

    printf("\nDecisiones optimas: ");
    print_uint32_array((X.P).decision_by_state_, (X.S).A.rows, 3);
}


uint32_t policy_improvement_discount_factor_set_value(policy_improvement *X, MDP *mdp) {
    size_t i, j;
    /* Set matrix system */
    // set A
    for(i = 0; i < mdp->n_states; i++) {
        // sum
        for(j = 0; j < mdp->n_states; j++) {
            (X->S).A.values[i][j] = - (X->discount_factor) * (X->P).M.values[i][j];
        }

        (X->S).A.values[i][i] += 1.0;
    }

    // set b
    for(i = 0; i < mdp->n_states; i++)
        (X->S).b.values[i][0] = (mdp->costs_matrix).values[i][(X->P).decision_by_state_[i] - 1];

    /* Solve matrix system */
    if(Matrix_system_solve(&(X->S)) != 0)
        printf("\nError al resolver el sistema\n");

    return 0;
}

uint32_t policy_improvement_discount_factor_set_optimal_decision(uint32_t state, MDP *mdp, policy_improvement *X) {
    List *decision_list = (mdp->valid_state_decisions_lists + state);
    ListNode *node;
    uint32_t optimal_decision, decision;
    size_t i;
    double G_R, optimal_G_R;

    node = list_head(decision_list);
    
    while (node != NULL) {
        // Caltulate G(R)
        decision = *(uint32_t *)list_data(node) - 1;
        G_R = (mdp->costs_matrix).values[state][decision];

        for(i = 0; i < mdp->n_states; i++)
            G_R += (X->discount_factor) * (mdp->transition_matrix_ + decision)->values[state][i] * ((X->S).x.values[i][0]);

        if(node == list_head(decision_list)) {
            optimal_G_R = G_R;
            optimal_decision = decision + 1;
        }

        if((X->comparison)(G_R, optimal_G_R)) {
            optimal_G_R = G_R;
            optimal_decision = decision + 1;
        }

        node = list_next(node);
    }
    
    return optimal_decision;
}

uint32_t policy_improvement_discount_improve(policy_improvement *X, MDP *mdp) {
    size_t i;
    uint32_t *optimal_decisions;

    optimal_decisions = (uint32_t *)malloc(sizeof(uint32_t) * (mdp->n_states));

    // calculate optimal_decisions
    for(i = 0; i < mdp->n_states; i++)
        optimal_decisions[i] = policy_improvement_discount_factor_set_optimal_decision(i, mdp, X);

    Policy_set_from_array(&(X->P), optimal_decisions, *mdp);
    free(optimal_decisions);
    return 0;
}

void policy_improvement_discount_factor_print_results(MDP *mdp) {
    char *s[2];
    MDP_set_labels(s, *mdp);
    terminal_color_subtitle();
    printf("\n\nPolitica optima: ");
    terminal_color_content();
    Policy_print(mdp->optimal_policy, s);
}

uint32_t policy_improvement_discount_factor_solve_MDP(MDP *mdp) {
    policy_improvement X;
    policy_improvement_init(&X, mdp);
    Matrix_system_init(&(X.S), mdp->n_states);

    // step 0 (Set init values)
    policy_improvement_discount_factor_set_init_values(&X, mdp);

    do {
        Policy_clone(&(mdp->optimal_policy), &(X.P));
        // step 1 (Set policy value)
        policy_improvement_discount_factor_set_value(&X, mdp);

        // step 2 (Policy improve)
        policy_improvement_discount_improve(&X, mdp);

        policy_improvement_discount_print_iteration(X);
        X.iteration++;
    } while(!Policy_are_equal((mdp->optimal_policy), X.P));
    
    // Print results
    Policy_get_stationary_vector(&(mdp->optimal_policy), NULL);
    Policy_get_expected_average_cost(&(mdp->optimal_policy), &(mdp->costs_matrix));
    policy_improvement_discount_factor_print_results(mdp);

    policy_improvement_destroy(&X);
    return 0;
}