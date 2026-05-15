#include "menu.c"

int main(int argc, char **argv) {
    MDP mdp;
    size_t i;
    
    if(argc < 2) {
        puts(not_enought_arguments_provided);
        return 0;
    }

    main_menu(&mdp, argc, argv);

    return 0;
}