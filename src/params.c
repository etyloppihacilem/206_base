/* #####################################################################################################################

               """          params.c
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 17:12
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "params.h"
#include "LPC17xx.h"
#include <stdint.h>

#define MAT_HEIGHT 2
#define MAT_WIDTH 4

static const uint32_t write_tab[MAT_HEIGHT + 1] = {
    1,      // 0.17
    2,      // 0.18
    3 << 1, // value needed to set all to 0
};

static const uint32_t read_tab[MAT_WIDTH] = {
    3,
    4,
    5,
    6,
};

void init_params() {
    // le pinsel est GPIO par défaut
    LPC_GPIO0->FIODIR |= 0x3 << 1; // 0.17 et 0.18 en sortie
}

uint8_t read_value(uint8_t x, uint8_t y) {
    if (x >= MAT_WIDTH || y >= MAT_HEIGHT)
        return 0;
    if (!(LPC_GPIO0->FIOPIN & (1 << write_tab[y])))
        LPC_GPIO0->FIOPIN = (LPC_GPIO0->FIOPIN & ~(write_tab[MAT_HEIGHT] & (1 << write_tab[y]))) | (1 << write_tab[y]);
    return (LPC_GPIO0->FIOPIN & (1 << read_tab[x])) >> read_tab[x];
}
