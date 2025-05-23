/* #####################################################################################################################

               """          params.c
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 17:12
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "params.h"
#include "LPC17xx.h"
#include "com_super.h"
#include <stdint.h>

#define MAT_HEIGHT 2
#define MAT_WIDTH 4

uint8_t nb_robots = 0; // OPTI: peut être passer ça en static
uint8_t nb_postes = 0;

uint8_t read_value(uint8_t y) { // OPTI: peut être passer ça en static
    uint32_t ret = 1 << (17 + y);

    LPC_GPIO0->FIOPIN &= ~ret;      // on met la ligne où lire au niveau bas
    while (LPC_GPIO0->FIOPIN & ret) // il faut attendre que le GPIO soit niveau bas
        ;
    // lecture de la valeur
    ret = (((LPC_GPIO0->FIOPIN & (1 << 23)) >> 1) | (LPC_GPIO0->FIOPIN & (1 << 19 | 1 << 20 | 1 << 21))) >> 19;
    LPC_GPIO0->FIOPIN |= 3 << 17; // on remet la ligne niveau haut
    return (~ret) & 0xF;          // on renvoie la valeur NOT parce que niveau bas veut dire selectionné.
}

void init_params() {
    LPC_PINCON->PINSEL1 &= ~((3 << 6) | (3 << 8) | (3 << 10) | (3 << 14));
    LPC_GPIO0->FIODIR   |= (3 << 17); // 0.17 et 0.18 en sortie
    LPC_GPIO0->FIODIR   &= ~(1 << 19 | 1 << 20 | 1 << 21 | 1 << 23);
    LPC_GPIO0->FIOPIN   |= 3 << 17; // on remet la ligne niveau haut
}

void read_params() {
    nb_robots = read_value(0);
    nb_postes = read_value(1);
    send_params();
}
