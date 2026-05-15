#ifndef MDP_IMPLEMENTATION
    #include "MDP.c"
#endif
#ifndef UTILS_IMPLEMENTATION
    #include "utils.c"
#endif

typedef struct {
    uint32_t **policy_;
    uint32_t n;
} feasible_policies;

uint32_t count_allowed_decision(MDP mdp, size_t state) {
    size_t i;
    uint32_t count = 0;
    Matrix *transition_matrix = NULL;

    for(i = 0; i < mdp.n_decisions; i++) {
        transition_matrix = &(mdp.transition_matrix_[i]);

        if((transition_matrix->values)[state][0] != -1)
            count++;
    }

    return count;
}

void feasible_policies_init(feasible_policies *f_p, MDP mdp) {
    size_t i;
    // set n_policies
    f_p->n = 1;
    for(i = 0; i < mdp.n_states; i++)
        f_p->n *= count_allowed_decision(mdp, i);

    // Init policy_ arrays array    
    f_p->policy_ = (uint32_t **)malloc(sizeof(uint32_t *) * (f_p->n));
    for(i = 0; i < f_p->n; i++)
        f_p->policy_[i] = (uint32_t *)malloc(sizeof(uint32_t) * (mdp.n_states));
}

void feasible_policies_destroy(feasible_policies *f_p, MDP mdp) {
    size_t i;
    uint32_t decisions = mdp.n_decisions, states = mdp.n_states;

    for(i = 0; i < f_p->n; i++)
        free(f_p->policy_[i]);
    
    free(f_p->policy_);
}

void feasible_policies_set_combination(feasible_policies *f_p, MDP mdp) {
    size_t i, j, k, l;

    List *valid_state_decision_list;
    ListNode *node;
    uint32_t *decision, n_blocks = 1;
    // set combinations
    for(i = 0; i < mdp.n_states; i++) {
        l = 0;
        // set entry_combination
        valid_state_decision_list = ((mdp.valid_state_decisions_lists) + i);
        for(j = 0; j < n_blocks; j++) {
            // set list block
            node = list_head(valid_state_decision_list);
            do {
                decision = list_data(node);
                // set node block
                for(k = 0; k < (f_p->n) / (n_blocks * list_size(valid_state_decision_list)); k++) {
                    (f_p->policy_)[l][i] = *decision;
                    l++;
                }
                node = list_next(node);
            } while (node != NULL);
        }
        n_blocks *= list_size(valid_state_decision_list);
    }
}

void feasible_policies_print(feasible_policies f_p, MDP mdp) {
    size_t i;

    terminal_color_subtitle();
    printf("\nCantidad de politicas: %d\n", f_p.n);
    terminal_color_content();
    // print policies
    for(i = 0; i < f_p.n; i++) {
        printf("%3zu: ", i + 1);
        print_uint32_array((f_p.policy_)[i], mdp.n_states, 2);
        printf("\n");
    }
}

void policy_enumeration_get_optimal_policy(MDP *mdp, feasible_policies *f_p, char *s[2], uint8_t (*compare)(double , double )){
    size_t i = 0;
    Policy p;
    uint32_t states = mdp->n_states;
    Matrix_system S;

    // Init matrix system
    matrix_init_square_matrices(states, 2, &S.A, &S.inv_A);
    matrix_init_column_vectors(states, 2, &(S.x), &(S.b));

    Policy_init(&p, states);

    for(i = 0; i < f_p->n; i++) {
        Policy_set_from_array(&p, (f_p->policy_)[i], *mdp);

        Policy_get_stationary_vector(&p, &S);

        Policy_get_expected_average_cost(&p, &(mdp->costs_matrix));

        // print results
        terminal_color_subtitle();
        printf("\n#%zu: ", i + 1);
        terminal_color_content();
        Policy_print(p, s);

        // print matrix system
        /*
        printf("\nA:\n");
        matrix_print(S.A, NUMBER_DECIMALS + 3, NUMBER_DECIMALS);
        printf("\ninv_A:\n");
        matrix_print(S.inv_A, NUMBER_DECIMALS + 3, NUMBER_DECIMALS);
        printf("\nb:\n");
        matrix_print(S.b, NUMBER_DECIMALS + 3, NUMBER_DECIMALS);
        */
        
        // If compare function is true overrive optimal policy
        if(p.is_valid && (compare(p.expected_average_cost, (mdp->optimal_policy).expected_average_cost) || i == 0))
            Policy_clone(&(mdp->optimal_policy), &p);
    }

    Policy_destroy(&p);
}

uint32_t policy_enumeration_solve_MDP(MDP *mdp) {
    feasible_policies f_p;
    char *s[2];

    feasible_policies_init(&f_p, *mdp);

    // Set policies
    feasible_policies_set_combination(&f_p, *mdp);

    // Set policies array   
    feasible_policies_print(f_p, *mdp);

    // Get max - min policy
    switch (mdp->type) {
        case COSTS_SYSTEM:
            s[0] = "Costo";
            s[1] = "o";
            policy_enumeration_get_optimal_policy(mdp, &f_p, s, min_comparison);
            break;
        case REWARDS_SYSTEM:
            s[0] = "Ganancia";
            s[1] = "a";
            policy_enumeration_get_optimal_policy(mdp, &f_p, s, max_comparison);
            break;
        default:
            s[0] = "";
            s[1] = "";
    }

    // print results
    terminal_color_subtitle();
    printf("\nPolitica optima: ");
    terminal_color_content();
    Policy_print(mdp->optimal_policy, s);

    // destroy data
    feasible_policies_destroy(&f_p, *mdp);
    return 0;
}