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

int main(void) {
    init_com_super(9600);
    init_params();
    init_porteuse();

    while (1) {}
}
