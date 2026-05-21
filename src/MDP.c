/**
 * @file MDP.c
 * @brief Implementation of interfaces and definition of structures of MDPs
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#ifndef UTILS_IMPLEMENTATION
    #include "utils.c"
#endif

typedef struct {
    uint32_t *decision_by_state_;
    double expected_average_cost;
    Matrix M;
    Matrix stationary_vector;
    uint8_t is_valid;
} Policy;

typedef enum {
    COSTS_SYSTEM = 0,
    REWARDS_SYSTEM,
    NONE_SYSTEM
} system_type;

typedef struct {
    uint32_t n_states;
    uint32_t n_decisions;
    uint8_t is_valid;
    Matrix *transition_matrix_;
    Matrix costs_matrix;
    List *valid_decision_states_lists; // valid decision k states lists (uint32_t)
    List *valid_state_decisions_lists; // valid state i decisions lists (uint32_t)
    Policy optimal_policy;
    system_type type;
} MDP;

uint32_t Policy_init(Policy *P, uint32_t n_states) {
    if(P == NULL)
        return 1;
    
    P->decision_by_state_ = (uint32_t *)malloc(sizeof(uint32_t) * n_states);
    if((P->decision_by_state_) == NULL)
        return -1;

    (P->expected_average_cost) = 0.0;
    // Init stationary_vector
    matrix_init_row_vector(&(P->stationary_vector), n_states);

    // Init P matrix
    (P->M).columns = n_states;
    (P->M).rows = n_states;
    (P->M).values = (double **)malloc(sizeof(double *) * n_states);

    if((P->M).values == NULL)
        return 1;
    
    (P->is_valid) = 0;

    return 0;
}

void Policy_clone(Policy *dest, Policy *src) {
    size_t i;
    uint32_t states = (dest->M).rows;
    for(i = 0; i < states; i++) {
        dest->decision_by_state_[i] = src->decision_by_state_[i];
        (dest->M).values[i] = (src->M).values[i];
    }

    dest->expected_average_cost = src->expected_average_cost;
    dest->is_valid = src->is_valid;

    matrix_clone(&(dest->stationary_vector), &(src->stationary_vector));
}

uint32_t Policy_are_equal(Policy p1, Policy p2) {
    uint32_t n_states = (p1.M).columns;
    size_t i;
    
    if(n_states != (p1.M).columns)
        return 0;

    for(i = 0; i < n_states; i++) {
        if(p1.decision_by_state_[i] != p2.decision_by_state_[i])
            return 0;
    }

    return 1;
}

uint32_t Policy_set_from_array(Policy *P, uint32_t *src, MDP mdp) {
    if(P == NULL)
        return -1;

    size_t i, decision;
    uint32_t states = mdp.n_states;
    // Clone src into
    if(src != P->decision_by_state_) {
        for(i = 0; i < states; i++)
            P->decision_by_state_[i] = src[i];
    }

    P->is_valid = 1;
    for(i = 0; i < states; i++) {
        decision = (P->decision_by_state_)[i] - 1;
        (P->M).values[i] = (mdp.transition_matrix_[decision]).values[i];
        if((mdp.transition_matrix_[decision]).values[i][0] == -1)
            P->is_valid = 0;
    }

    return 0;
}

uint32_t Policy_set_empty(Policy *P) {
    size_t i;
    
    for(i = 0; i < (P->M).rows; i++)
        (P->decision_by_state_)[i] = -1;

    P->expected_average_cost = INFINITY;
    
    matrix_zero(&(P->stationary_vector));

    P->is_valid = 0;
    return 0;
}

void Policy_set_matrix_A(Matrix *A, Matrix *src) {
    size_t i;

    matrix_transpose(A, src);
    for(i = 0; i < matrix_columns(A); i++)
        (A->values)[i][i] -= 1.0;

    for(i = 0; i < matrix_columns(A); i++)
        (A->values)[A->rows - 1][i] = 1.0;
}

void Policy_set_matrix_b(Matrix *b) {
    (b->values)[b->rows - 1][0] = 1.0;
}

uint32_t Policy_get_stationary_vector(Policy *P, Matrix_system *S) {
    if(P == NULL)
        return 1;
    
    Matrix_system W;
    double sum = 0.0;
    uint32_t states = (P->M).rows;
    matrix_init_square_matrices(states, 2, &W.A, &W.inv_A);
    matrix_init_column_vectors(states, 2, &W.x, &W.b);

    // Set b vector
    Policy_set_matrix_b(&W.b);
    // Set A matrix
    Policy_set_matrix_A(&W.A, &(P->M));

    // Solve system A * x = b
    if(Matrix_system_solve(&W) != 0)
        goto NOT_VALID_POLICY;

    // transpose vector x
    if(matrix_transpose(&(P->stationary_vector), &W.x)!= 0)
        goto NOT_VALID_POLICY;

    if(matrix_sum_norm(&(P->stationary_vector), &sum) != 0)
        goto NOT_VALID_POLICY;
    
    if(!are_equal(1.0, sum))
        goto NOT_VALID_POLICY;

    P->is_valid = 1;

    if(S == NULL)
        return 0;

    matrix_clone(&(S->A), &(W.A));
    matrix_clone(&(S->inv_A), &(W.inv_A));
    matrix_clone(&(S->b), &(W.b));
    matrix_clone(&(S->x), &(W.x));

    return 0;

    NOT_VALID_POLICY:
        (P->is_valid) = 0;
        return 1;
}

uint32_t Policy_get_expected_average_cost(Policy *P, Matrix *Costs) {
    if(P == NULL || Costs == NULL)
        return 1;

    size_t i;
    uint32_t decision, states = (P->M).rows;

    (P->expected_average_cost) = 0.0;
    for(i = 0; i < states; i++) {
        decision = (P->decision_by_state_)[i] - 1;
        (P->expected_average_cost) += (Costs->values)[i][decision] * (P->stationary_vector).values[0][i];
    }

    return 0;
}

void MDP_set_labels(char *labels[2], MDP mdp) {
    switch (mdp.type) {
        case COSTS_SYSTEM:
            labels[0] = "Costo";
            labels[1] = "o";
            break;
        case REWARDS_SYSTEM:
            labels[0] = "Ganancia";
            labels[1] = "a";
            break;
        default:
            labels[0] = "";
            labels[1] = "";
    }
}

void Policy_print(Policy P, char *s[2]) {
    uint32_t states = P.M.rows;
    
    terminal_color_content();
    print_uint32_array(P.decision_by_state_, states, 2);
    /*
    printf("\nMatriz de Transicion: \n");
    matrix_print(P.M, NUMBER_DECIMALS + 3, NUMBER_DECIMALS);
    */
    
    if(P.is_valid) {
        terminal_color_green();
    }
    else {
        terminal_color_red();
    }
    printf("\nLa politica %ses valida", (P.is_valid)?"":"no ");
    
    terminal_color_content();
    if(!(P.is_valid)) {
        printf("\nVector de probabilidad estacionaria no valido\n");
        return;
    }

    printf("\nVector de probabilidad estacionaria:\n");
    matrix_print(P.stationary_vector, NUMBER_DECIMALS + 3, NUMBER_DECIMALS);
    
    printf("\n%s Promedio Esperad%s: %*.*lf\n", s[0], s[1], NUMBER_LENGTH, NUMBER_DECIMALS, P.expected_average_cost);
}

void Policy_destroy(Policy *P) {
    // Destroy matrix
    matrix_destroy(&(P->stationary_vector));
    free((P->M).values);
    free(P->decision_by_state_);
}

uint32_t MDP_init(MDP *mdp, uint32_t n_states, uint32_t n_decisions) {
    size_t i;
    // Set n_states and n_decisions
    mdp->n_states = n_states;
    mdp->n_decisions = n_decisions;
    mdp->is_valid = 0;

    mdp->type = NONE_SYSTEM;

    // Init transition matrices
    mdp->transition_matrix_ = (Matrix *)malloc(sizeof(Matrix ) * n_decisions);

    if((mdp->transition_matrix_) == NULL)
        return -1;

    for(i = 0; i < n_decisions; i++) {
        if(matrix_init_square_matrix(&(mdp->transition_matrix_)[i], n_states) != 0)
            return -1;
    }
    
    // Init valid decision states lists
    mdp->valid_decision_states_lists = (List *)malloc(sizeof(List) * mdp->n_decisions);
    for(i = 0; i < mdp->n_decisions; i++)
        list_init(mdp->valid_decision_states_lists+i, free);
    
    
    // Init valid state decisions lists
    mdp->valid_state_decisions_lists = (List *)malloc(sizeof(List) * mdp->n_states);
    for(i = 0; i < mdp->n_states; i++)
        list_init(mdp->valid_state_decisions_lists+i, free);

    // Init costs matrix
    if(matrix_init(&(mdp->costs_matrix), n_states, n_decisions) != 0)
        return -1;

    // Init Policy
    if(Policy_init(&(mdp->optimal_policy), n_states) != 0)
        return -1;

    return 0;
}

void MDP_set_NULL(MDP *mdp) {
    if(mdp == NULL)
        return;

    mdp->n_decisions = -1;
    mdp->n_states = -1;
}

void MDP_destroy(MDP *mdp) {
    size_t i;
    uint32_t n_decisions = mdp->n_decisions, n_states = mdp->n_states;

    // Destroy matrices
    for(i = 0; i < n_decisions; i++)
        matrix_destroy(&(mdp->transition_matrix_)[i]);

    // Destroy Lists
    for(i = 0; i < n_decisions; i++)
        list_destroy(&(mdp->valid_decision_states_lists)[i]);

    for(i = 0; i < n_states; i++)
        list_destroy(&(mdp->valid_state_decisions_lists)[i]);

    free(mdp->valid_decision_states_lists);
    free(mdp->valid_state_decisions_lists);

    // Destroy Policy
    Policy_destroy(&(mdp->optimal_policy));
    free(mdp->transition_matrix_);
}

void MDP_set_valid_decision_states_list(MDP *mdp, size_t decision) {
    size_t i;
    uint32_t *valid_state;
    Matrix *transition_matrix = &(mdp->transition_matrix_[decision]);
    List *l = mdp->valid_decision_states_lists + decision;

    for(i = 0; i < transition_matrix->rows; i++) {
        if((transition_matrix->values)[i][0] == -1)
            continue;

        valid_state = (uint32_t *)malloc(sizeof(uint32_t));
        *valid_state = i;

        list_append(l, valid_state);
    }
}

void MDP_set_valid_state_decisions_list(MDP *mdp, size_t state) {
    size_t i;
    uint32_t *valid_decision;
    Matrix *transition_matrix;
    List *l = mdp->valid_state_decisions_lists + state;

    for(i = 0; i < mdp->n_decisions; i++) {
        transition_matrix = mdp->transition_matrix_ + i;

        if((transition_matrix->values)[state][0] == -1)
            continue;
        
        valid_decision = (uint32_t *)malloc(sizeof(uint32_t));
        *valid_decision = i + 1;

        list_append(l, valid_decision);
    }
}

void MDP_set_lists(MDP *mdp) {
    size_t i;
    for(i = 0; i < mdp->n_decisions; i++)
        MDP_set_valid_decision_states_list(mdp, i);

    for(i = 0; i < mdp->n_states; i++)
        MDP_set_valid_state_decisions_list(mdp, i);
}

void MDP_print(MDP mdp) {
    size_t i;
    char *s;
    terminal_color_subtitle();
    printf("Numero de Decisiones: %d\n", mdp.n_decisions);
    printf("Numero de Estados: %d\n", mdp.n_states);
    for(i = 0; i < mdp.n_decisions; i++) {
        terminal_color_subtitle();
        printf("\nProbabilidades de transicion para la decision %zd: \n", i + 1);
        terminal_color_content();
        matrix_print(mdp.transition_matrix_[i], NUMBER_DECIMALS + 3, NUMBER_DECIMALS);
    }

    terminal_color_subtitle();
    switch (mdp.type)
    {
    case COSTS_SYSTEM:
        s = "Costos";
        break;
    case REWARDS_SYSTEM:
        s = "Recompenzas";
        break;
    case NONE_SYSTEM:
        s = "";
        break;
    }
    printf("\nMatriz de %s: \n", s);
    terminal_color_content();
    matrix_print(mdp.costs_matrix, NUMBER_LENGTH, NUMBER_DECIMALS);

    // print valid decision states lists
    for(i = 0; i < mdp.n_decisions; i++) {
        printf("\nLa decision %3zd aplica en los estados: ", i+1);
        print_uint32_list(mdp.valid_decision_states_lists[i]);
    }
    printf("\n");

    // print valid state decisions lists
    for(i = 0; i < mdp.n_states; i++) {
        printf("\nEn el estado %3zd son validas las decisiones: ", i);
        print_uint32_list(mdp.valid_state_decisions_lists[i]);
    }
    printf("\n");
}

void MDP_check(MDP *mdp) {
    size_t i;
    Matrix *M;
    double posible_row_sum_values[2] = {1.0, -1*(mdp->transition_matrix_[0].columns)};
    char *s;

    switch (mdp->type) {
        case COSTS_SYSTEM:
            s = "Costos";
            break;
        case REWARDS_SYSTEM:
            s = "Recompenzas";
            break;
        case NONE_SYSTEM:
            mdp->is_valid = 0;
            return;
    }

    // for each transition matrix:
    for(i = 0; i < (*mdp).n_decisions; i++) {
        M = &((*mdp).transition_matrix_[i]);

        if(!matrix_is_init(M))
            goto TRANSITION_MATRIX_FALSE_RETURN;

        // Check if n_states is equat to matrix rows
        if((*mdp).n_states != matrix_rows(M))
            goto TRANSITION_MATRIX_FALSE_RETURN;

        // Check if matrix is square
        if(!matrix_is_square(M))
            goto TRANSITION_MATRIX_FALSE_RETURN;

        // Check if sum by row is equal to 1
        if(!matrix_check_values(*M, posible_row_sum_values, 2))
            goto TRANSITION_MATRIX_FALSE_RETURN;
    }

    M = &((*mdp).costs_matrix);
    // Check if n_states is equal to costs matrix rows
    if((*mdp).n_states != matrix_rows(M))
        goto COSTS_MATRIX_FALSE_RETURN;

    // Check if n_decisions is equal to costs matrix columns
    if((*mdp).n_decisions != matrix_columns(M))
        goto COSTS_MATRIX_FALSE_RETURN;

    mdp->is_valid = 1;
    return;
    TRANSITION_MATRIX_FALSE_RETURN:
        printf("\nMatriz de transicion para la decision %zd no valida\n", i + 1);
        mdp->is_valid = 0;
        return;
    COSTS_MATRIX_FALSE_RETURN:
        printf("\nMatriz de %s no valida\n", s);
        mdp->is_valid = 0;
        return;
}