/**
 * @file main.c
 * @brief Main entry point for the mdp solver application
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/

#include <stdio.h>

#include "menu_.h"
#include "MDP.h"

#define not_enought_arguments_provided "No se ingresaron archivos fuente"

int main(int argc, char **argv) {
    MDP mdp;
    
    if(argc < 2) {
        puts(not_enought_arguments_provided);
        return 0;
    }

    main_menu(&mdp, argc, argv);

    return 0;
}