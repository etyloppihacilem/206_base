/* #####################################################################################################################

               """          porteuse.c
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 09:17
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

#####################################################################################################################
*/

#include "porteuse.h"
#include "LPC17xx.h"
#include <stdint.h>

uint16_t send_index                      = 0;
uint16_t write_index                     = 10;
uint8_t  send_buffer[SEND_BUFFER_LENGTH] = { 1, 0, 1, 1, 1, 0, 0, 0, 1 };
uint16_t counter                         = entete;
uint8_t  message                         = MESSAGE_LENGTH;

void PWM1_IRQHandler() {
    LPC_PWM1->IR = 1 << 1;
    if (counter-- != 0)
        return;
    if (LPC_PWM1->PCR & (1 << 9)) { // pause are always the same duration
        counter        = pause;
        LPC_PWM1->PCR &= ~(1 << 9);                         // turn output off
    } else if (send_index == write_index || message == 0) { // En tête et rien aussi
        counter        = entete;
        LPC_PWM1->PCR |= 1 << 9;
        message        = MESSAGE_LENGTH;
    } else {
        if (send_buffer[send_index++]) // on doit envoyer un 1
            counter = b1;
        else // on doit envoyer un 0
            counter = b0;
        if (send_index >= SEND_BUFFER_LENGTH)
            send_index = 0;
        message--;
        LPC_PWM1->PCR |= 1 << 9;
    }
}

void init_porteuse() {
    LPC_PINCON->PINSEL4 |= 1; // setting GPIO 2.0 to PWM1.1

    LPC_PWM1->MR0 = 500;
    LPC_PWM1->MR1 = 250;

    LPC_PWM1->MCR |= (1 << 1) | (1 << 3); // interrupt and reset
    LPC_PWM1->PCR |= (1 << 9);            // output enabled

    // activation du PWM
    LPC_PWM1->TCR = 1 | (1 << 3); // 0b1001

    NVIC_EnableIRQ(PWM1_IRQn);
}

void send_message(uint8_t type, uint8_t robot, uint8_t argument) {
    // strucutre is ROBO TYP ARGUMEN
    uint16_t message    = ((robot & 0xF) << 10) | ((type & 0x7) << 7) | (argument & 0x3F);
    uint16_t temp_write = write_index;
    for (uint8_t i = 0; i < MESSAGE_LENGTH; i++) {
        send_buffer[temp_write++] = (message << i) >> i;
        if (temp_write >= SEND_BUFFER_LENGTH)
            temp_write = 0;
    }
    write_index = temp_write;
}
