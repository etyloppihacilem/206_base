/* #####################################################################################################################

               """          livraisons.h
        -\-    _|__
         |\___/  . \        Created on 20 May. 2025 at 15:11
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#pragma once

#include <stdint.h>

typedef struct {
        uint8_t origine;
        uint8_t destination;
        char    livraison;
} waitlist_livraisons;
