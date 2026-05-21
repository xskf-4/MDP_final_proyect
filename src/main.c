/**
 * @file main.c
 * @brief Main entry point for the mdp solver application
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#include "menu.c"

#define not_enought_arguments_provided "No se ingresaron archivos fuente"

extern uint32_t main_menu(MDP *, uint32_t , char **);

int main(int argc, char **argv) {
    MDP mdp;
    
    if(argc < 2) {
        puts(not_enought_arguments_provided);
        return 0;
    }

    main_menu(&mdp, argc, argv);

    return 0;
}