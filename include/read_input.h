/**
 * @file read_input.h
 * @brief Definition of interfaces for parsing source files
 * @author Hernandez Reyes Sebastian (xskf-4)
 * @date 2026
*/
#ifndef READ_INPUT_H
#define READ_INPUT_H

#include <stdint.h>

#include "MDP.h"

uint32_t read_mdp_from_file(MDP *mdp, const char *input_file);

#endif