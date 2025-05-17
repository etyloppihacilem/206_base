/* #####################################################################################################################

               """          params.h
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 17:12
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#ifndef INCLUDE_INCLUDE_PARAMS_H_
#define INCLUDE_INCLUDE_PARAMS_H_

#include <stdint.h>

extern uint8_t nb_robots;
extern uint8_t nb_postes;

void init_params();
void read_params(); // doit être appelé après com_super

#endif  // INCLUDE_INCLUDE_PARAMS_H_
