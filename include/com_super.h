/* #####################################################################################################################

               """          com_super.h
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 14:35
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#pragma once

#include "com_poste.h"
#include <stdint.h>

typedef enum {
    s_vide          = 0,
    ordre_livraison = 'P',
    vitesse         = 'V',
} t_msg_super_type;

typedef struct {
        t_msg_super_type type;
        uint8_t          robot;
        uint8_t          cible;       // soit la vitesse, soit le poste d'enlevement
        char             livraison;   // lettre de livraison
        uint8_t          destination; // le poste de destination de la livraison
} t_msg_from_super;

void              init_com_super(uint32_t baudrate);
t_msg_from_super *get_super_msg();
void              super_msg_done();
void              send_poste_info(t_msg_from_poste *msg);
void              send_params();
