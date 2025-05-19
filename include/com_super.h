/* #####################################################################################################################

               """          com_super.h
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 14:35
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#pragma once

#include <stdint.h>
#include <stdio.h>

typedef enum {
    vide      = 0,
    livraison = 2,
    vitesse   = 1,
} t_msg_super_type;

typedef struct {
        t_msg_super_type type;
        uint8_t          robot;
        uint8_t          cible;     // soit la vitesse, soit le poste de destination
        char             livraison; // lettre de livraison
        uint8_t          origine;   // le poste d'origine de la livraison
} t_msg_from_super;

extern t_msg_from_super inbox_super[];
extern uint8_t          w_super;
extern uint8_t          r_super;

void init_com_super(uint32_t baudrate);

int fputc(int c, FILE *f); // needed for printf, do not use this
int _write(int file, char *ptr, int len);
