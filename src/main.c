/* #####################################################################################################################

               """          main.c
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 09:16
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "LPC17xx.h" // Définitions registres périphériques
#include "com_super.h"
#include "params.h"
#include "porteuse.h"
#include <stdint.h>
#include <stdio.h>

void TIMER2_IRQHandler() {
    LPC_TIM2->IR |= 1;
    read_params();
}

int main(void) {
    init_com_super(9600);
    init_params();
    // init_porteuse();

    // LPC_SC->PCONP |= 1 << 22; // enable timer 2
    // NVIC_EnableIRQ(TIMER2_IRQn);
    // LPC_TIM2->TCR = 1 << 1;
    // LPC_TIM2->MR0 = 12500000;
    // LPC_TIM2->MCR |= 1 | (1 << 1);
    // LPC_TIM2->TCR = 1;
    while (1) {}
}
