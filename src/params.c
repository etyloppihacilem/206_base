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
#include <stdio.h>

#define MAT_HEIGHT 2
#define MAT_WIDTH 4

uint8_t nb_robots = 0;
uint8_t nb_postes = 0;

static const uint32_t write_tab[MAT_HEIGHT + 1] = {
    17,      // 0.17
    18,      // 0.18
    3 << 17, // value needed to set all to 0
};

static const uint32_t read_mask  = (1 << 19 | 1 << 20 | 1 << 21);

// static const uint32_t read_tab[MAT_WIDTH] = {
//     3,
//     4,
//     5,
//     6,
// };

// /// pour lire un bit, inutile en pratique.
// uint8_t read_value(uint8_t x, uint8_t y) {
//     if (x >= MAT_WIDTH || y >= MAT_HEIGHT)
//         return 0;
//     if (!(LPC_GPIO0->FIOPIN & (1 << write_tab[y])))
//         LPC_GPIO0->FIOPIN = (LPC_GPIO0->FIOPIN & ~(write_tab[MAT_HEIGHT] & (1 << write_tab[y]))) | (1 <<
//         write_tab[y]);
//     return (LPC_GPIO0->FIOPIN & (1 << read_tab[x])) >> read_tab[x];
// }

uint8_t read_value(uint8_t y) {
    LPC_GPIO0->FIOPIN  = (LPC_GPIO0->FIOPIN & ~write_tab[MAT_HEIGHT]) | 1 << write_tab[y];
    uint32_t ret       = (((LPC_GPIO0->FIOPIN & (1 << 23)) >> 1) | (LPC_GPIO0->FIOPIN & read_mask)) >> 19;
    LPC_GPIO0->FIOPIN &= ~write_tab[MAT_HEIGHT]; // to prevent pull down consumption
    return ret;
}

void init_params() {
    // le pinsel est GPIO par défaut
    // le GPIO est en entrée par défaut
    printf("a\r\n");
    LPC_PINCON->PINSEL1 &= ~((3 << 6) | (3 << 8) | (3 << 10) | (3 << 14));
    LPC_GPIO0->FIODIR    |= (0x3 << 17); // 0.17 et 0.18 en sortie
    LPC_GPIO0->FIODIR    &= ~(1 << 19 | 1 << 20 | 1 << 21 | 1 << 23); // en entrées
    LPC_PINCON->PINMODE1 |= (3 << 6) | (3 << 8) | (3 << 10) | (3 << 14);
    LPC_GPIO0->FIOPIN &= ~write_tab[MAT_HEIGHT]; // to prevent pull down consumption
    printf("FIODIR : %08lx\r\n", LPC_GPIO0->FIODIR);
    printf("FIOPIN : %08lx\r\n", LPC_GPIO0->FIOPIN);
    printf("PINSEL : %08lx\r\n", LPC_PINCON->PINSEL1);
    printf("PINMODE : %08lx\r\n", LPC_PINCON->PINMODE1);
    printf("Init done\r\n");
}

void read_params() {
    nb_robots          = read_value(0);
    nb_postes          = read_value(1);
    // LPC_GPIO0->FIOPIN &= ~write_tab[MAT_HEIGHT]; // to prevent pull down consumption
    printf("CFGR%02dP%02d\r\n", nb_robots, nb_postes);
}
