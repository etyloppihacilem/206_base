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
    p_vide         = 0,
    robot          = 'R',
    info_livraison = 'P',
    EOT            = 'N',
} t_msg_poste_type;

typedef struct {
        t_msg_poste_type type;
        uint8_t          poste;
        uint8_t          robo_livr; // numéro de robot ou lettre de livraison
        uint8_t          vit_dest;  // vitesse ou destination
        char             statut;    // statut du robot
} t_msg_from_poste;

void              init_com_poste(uint32_t baudrate);
t_msg_from_poste *get_poste_msg();
void              poste_msg_done();
