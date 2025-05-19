/* #####################################################################################################################

               """          params.h
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 17:12
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#pragma once

#include <stdint.h>

extern uint8_t nb_robots;
extern uint8_t nb_postes;

void init_params();
void read_params(); // doit être appelé après com_super
