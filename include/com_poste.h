/* #####################################################################################################################

               """          com_poste.h
        -\-    _|__
         |\___/  . \        Created on 19 May. 2025 at 23:03
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#pragma once

#include <stdint.h>

typedef enum {
    vide      = 0,
    robot     = 'R',
    livraison = 'P',
} t_msg_poste_type;

typedef struct {
        t_msg_poste_type type;
        uint8_t          poste;
        char             msg[8];
        uint8_t          robo_livr; // numéro de robot ou lettre de livraison
        uint8_t          vit_dest;  // vitesse ou destination
        char             statut;    // statut du robot
} t_msg_from_poste;

extern t_msg_from_poste inbox_poste[];
extern uint8_t          w_poste;
extern uint8_t          r_poste;

void init_com_poste(uint32_t baudrate);
