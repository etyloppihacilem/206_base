/* #####################################################################################################################

               """          porteuse.h
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 09:16
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#pragma once

#include <stdint.h>

#define MESSAGE_LENGTH 14
#define SEND_BUFFER_LENGTH MESSAGE_LENGTH * 16 // define here size of buffer.

typedef enum e_duree_pwm {
    pause  = 25,  // 0.5 ms
    entete = 125, // 2.5ms
    b1     = 50,  // 1 ms
    b0     = 88,  // 1.75 approx (87.5)
} duree_pwm;

typedef enum e_message_type {
    vitesse             = 0b000,
    chargement_gauche   = 0b001,
    chargement_droite   = 0b010,
    dechargement_gauche = 0b011,
    dechargement_droite = 0b100,
} message_type;

void init_porteuse();
void send_message(uint8_t type, uint8_t robot, uint8_t argument);
