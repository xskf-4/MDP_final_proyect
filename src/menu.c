/**
 * @file menu.c
 * @brief Implementation of the main menu and set application flow
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#include "include.c"

#define not_enought_arguments_provided "No se ingresaron archivos fuente"
#define proccess_error "Error durante el proceso"

void print_main_title() {
    printf("\033[2J"); // clean screen
    printf("\033[H");   // move cursor to the start
    terminal_color_main_title();
    print_title("Proyecto Final Procesos Estocasticos");
}

uint32_t proccess_input_file(MDP *mdp, char *src) {
    terminal_color_title();
    printf("\nResultado para '%s':\n", src);
    terminal_color_content();

    // Read MDP from file
    switch(read_mdp_from_file(mdp, src)) {
        case -2:
            MDP_destroy(mdp);
        case -1:
            printf("\n%s para %s\n", proccess_error, src);
            printf("No se pudo leer del archivo\n");
            return 1;
    }

    MDP_print(*mdp);
    MDP_check(mdp);

    terminal_bold_text();

    if(mdp->is_valid) {
        terminal_color_green();
    }
    else {
        terminal_color_red()
    }

    printf("\nEl modelo %ses valido\n", (!mdp->is_valid)?"no ": "");
    terminal_color_reset();

    // Try all methods
    if(mdp->is_valid) {
        // Policy enumeration
        terminal_color_subtitle();
        printf("\nMetodo de Enumeracion Exhaustiva de Politicas");
        terminal_color_content();
        policy_enumeration_solve_MDP(mdp);
        /*
        */

        // LP approach
        Policy_set_empty(&(mdp->optimal_policy)); // empty optimal policy
        terminal_color_subtitle();
        printf("\nSolucion por Programacion Lineal");
        terminal_color_content();
        LP_approach_solve_MDP(mdp);
        /*
        */

        // Policy Improvement
        Policy_set_empty(&(mdp->optimal_policy)); // empty optimal policy
        terminal_color_subtitle();
        printf("\nMetodo de Mejoramiento de Politicas");
        terminal_color_content();
        policy_improvement_solve_MDP(mdp);
        /*
        */

        // Policy Improvement (Discount factor)
        Policy_set_empty(&(mdp->optimal_policy)); // empty optimal policy
        terminal_color_subtitle();
        printf("\nMetodo de Mejoramiento de Politicas (Factor de descuento)");
        terminal_color_content();
        policy_improvement_discount_factor_solve_MDP(mdp);
        /*
        */
        
        // Value Iteration
        Policy_set_empty(&(mdp->optimal_policy)); // empty optimal policy
        terminal_color_subtitle();
        printf("\nMetodo de Aproximaciones Sucesivas");
        terminal_color_content();
        value_iteration_solve_MDP(mdp);
        /*
        */
    }

    // Destroy MDP
    if(mdp->n_decisions != 0)
        MDP_destroy(mdp);

    return 0;
}

void print_end() {
    terminal_color_main_title();
    printf("\n\nFin\n");
    terminal_color_reset();
}

void end_proccess() {
    char input[MAX_INPUT_SIZE];
    terminal_color_main_title();
    printf("\n\nFin del Proceso (Presione Enter)");
    terminal_color_reset();
    fgets(input, MAX_INPUT_SIZE, stdin);
}

void menu_print_options() {
    terminal_color_subtitle();
    printf("\nMenu");
    terminal_color_content();
    printf("\n1.-Metodo de Enumeracion Exhaustiva de Politicas");
    printf("\n2.-Solucion por Programacion Lineal");
    printf("\n3.-Metodo de Mejoramiento de Politicas");
    printf("\n4.-Metodo de Mejoramiento de Politicas (Factor de descuento)");
    printf("\n5.-Metodo de Aproximaciones Sucesivas");
    printf("\n6.-Siguiente archivo");
    printf("\n7.-Salir");
}

uint32_t read_option(uint32_t *option, uint32_t lower_bound, uint32_t upper_bound, char **response) {
    char input[MAX_INPUT_SIZE];

    fgets(input, MAX_INPUT_SIZE, stdin);

    if(sscanf(input, "%d", option) != 1) {
        *response = " (entrada no valida)";
        return 0;
    }

    if(*option < lower_bound || *option > upper_bound) {
        *response = " (entrada fuera de rango)";
        return 0;
    }

    return 1;
}

uint32_t menu_read_option() {
    uint32_t read_value;
    char *response = "";
    printf("\n");
    do {
        printf("Opcion%s: ", response);
    } while(!read_option(&read_value, 1, 7, &response));

    return read_value;
}

uint32_t file_menu(MDP *mdp, char *src) {
    uint32_t read_status = read_mdp_from_file(mdp, src);
    uint32_t option;
    do {
        print_main_title();
        terminal_color_title();
        printf("\nResultado para '%s':\n", src);
        terminal_color_content();

        switch(read_status) {
            case -2:
                MDP_destroy(mdp);
            case -1:
                printf("\n%s para %s\n", proccess_error, src);
                printf("No se pudo leer del archivo\n");
                end_proccess();
                return 0;
        }

        MDP_print(*mdp);
        MDP_check(mdp);

        terminal_bold_text();

        if(mdp->is_valid) {
            terminal_color_green();
        }
        else {
            terminal_color_red()
        }

        printf("\nEl modelo %ses valido\n", (!mdp->is_valid)?"no ": "");
        terminal_color_reset();

        if(!(mdp->is_valid)) {
            end_proccess();
            return 0;
        }


        menu_print_options();
        option = menu_read_option();

        switch(option) {
            case 1:
                Policy_set_empty(&(mdp->optimal_policy)); // empty optimal policy
                terminal_color_subtitle();
                printf("\nMetodo de Enumeracion Exhaustiva de Politicas");
                terminal_color_content();
                policy_enumeration_solve_MDP(mdp);
                break;
            case 2:
                Policy_set_empty(&(mdp->optimal_policy)); // empty optimal policy
                terminal_color_subtitle();
                printf("\nSolucion por Programacion Lineal");
                terminal_color_content();
                LP_approach_solve_MDP(mdp);
                break;
            case 3:
                Policy_set_empty(&(mdp->optimal_policy)); // empty optimal policy
                terminal_color_subtitle();
                printf("\nMetodo de Mejoramiento de Politicas");
                terminal_color_content();
                policy_improvement_solve_MDP(mdp);
                break;
            case 4:
                Policy_set_empty(&(mdp->optimal_policy)); // empty optimal policy
                terminal_color_subtitle();
                printf("\nMetodo de Mejoramiento de Politicas (Factor de descuento)");
                terminal_color_content();
                policy_improvement_discount_factor_solve_MDP(mdp);
                break;
            case 5:
                Policy_set_empty(&(mdp->optimal_policy)); // empty optimal policy
                terminal_color_subtitle();
                printf("\nMetodo de Aproximaciones Sucesivas");
                terminal_color_content();
                value_iteration_solve_MDP(mdp);
                break;
            case 7:
                goto EXIT_RETURN;
        }

        end_proccess();
    } while(option != 6);

    EXIT_RETURN:
        MDP_destroy(mdp);
        return 0;
}

uint32_t main_menu(MDP *mdp, uint32_t argc, char **argv) {
    size_t index_file = 1;
    uint32_t return_value;

    do {
        return_value = file_menu(mdp, argv[index_file]);
        if(return_value == -1)
            break;

        index_file ++;
    } while(index_file < argc);

    print_end();
    return 0;
}