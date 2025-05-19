/* #####################################################################################################################

               """          com_poste.c
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 14:35
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

#####################################################################################################################
*/

#include "LPC17xx.h"
#include <stdint.h>

#define MSG_LENGTH 32

char    msg_poste[MSG_LENGTH + 1] = { 0 };
uint8_t current_poste             = 0;

void UART1_IRQHandler(void) {
    if (LPC_UART1->IIR & 0x04) // RDA: Receive Data Available
    {
        if (current_poste == MSG_LENGTH)
            current_poste = 0; // on supprime le dernier message et recommence
        char c                     = LPC_UART1->RBR & 0xFF;
        msg_poste[current_poste++] = c;
        msg_poste[current_poste]   = '\0'; // to end string
    }
    if (current_poste >= 2 && msg_poste[current_poste - 1] == '\n'
        && msg_poste[current_poste - 2] == '\r') { // traiter le message
        current_poste            = 0;
        msg_poste[current_poste] = '\0'; // to end string
    }
}

void init_com_poste(uint32_t baudrate) {
    LPC_SC->PCONP |= (1 << 4); // on active l'horloge et le power de UART 1

    LPC_PINCON->PINSEL0 &= ~(3 << 30);
    LPC_PINCON->PINSEL0 |= 1 << 30; // P0.15 = TXD1
    LPC_PINCON->PINSEL1 &= ~3;
    LPC_PINCON->PINSEL1 |= 1; // P0.16 = RXD1

    uint32_t Fdiv;
    uint32_t pclk;

    // Le PCLK de UART1 est par défaut à CCLK / 4
    pclk = SystemCoreClock / 4;

    // Désactiver interruptions pendant init
    LPC_UART1->IER = 0;

    // 8 bits, 1 stop bit, pas de parité
    LPC_UART1->LCR = 0x83; // DLAB = 1 pour accéder à DLL/DLM

    Fdiv           = pclk / (16 * baudrate);
    LPC_UART1->DLM = Fdiv / 256;
    LPC_UART1->DLL = Fdiv % 256;

    LPC_UART1->LCR = 0x03; // 8 bits transmit
    LPC_UART1->FCR = 0x07; // Activer FIFO, reset RX/TX

    // Attente que tout soit prêt
    while (!(LPC_UART1->LSR & 0x20))
        ;
    NVIC_EnableIRQ(UART1_IRQn);
    LPC_UART1->IER = 1; // RBR interrupt enable
}

static void parser_poste(char *msg_poste) {
    // parser ici le message.
}

static int uart1_putchar(int c) { // peut être bufferiser ça si les prints prennent trop de temps.
    while (!(LPC_UART1->LSR & 0x20))
        ; // attente THRE
    LPC_UART1->THR = c;
    return c;
}
