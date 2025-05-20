/* #####################################################################################################################

               """          com_poste.c
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 14:35
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

#####################################################################################################################
*/

#include "com_poste.h"
#include "LPC17xx.h"
#include "params.h"
#include "utils.h"
#include <stdint.h>

#define MSG_LENGTH 32
#define INBOX_SIZE 4

t_msg_from_poste inbox_poste[INBOX_SIZE]   = { 0 };
char             msg_poste[MSG_LENGTH + 1] = { 0 };

uint8_t c_poste    = 0;
uint8_t w_poste    = 0;
uint8_t r_poste    = 0;
uint8_t last_asked = 0; // last poste asked

static void parsing_poste() {
    if (last_asked == 0)
        return;
    inbox_poste[w_poste].poste = last_asked;
    if (msg_poste[0] == robot) {
        inbox_poste[w_poste].robo_livr = parse_nb(msg_poste[1], msg_poste[2]);
        if (inbox_poste[w_poste].robo_livr > nb_robots)
            return;
        inbox_poste[w_poste].vit_dest = parse_nb(msg_poste[3], msg_poste[4]);
        if (!is_state(msg_poste[5]))
            return;
        inbox_poste[w_poste].statut = msg_poste[5];
        inbox_poste[w_poste].type   = robot;
    } else if (msg_poste[1] == info_livraison) {
        if (!is_state(msg_poste[1]))
            return;
        inbox_poste[w_poste].robo_livr = msg_poste[1];
        inbox_poste[w_poste].vit_dest  = parse_nb(msg_poste[2], msg_poste[3]);
        if (inbox_poste[w_poste].vit_dest > nb_postes)
            return;
        inbox_poste[w_poste].type = info_livraison;
    } else {
        return;
    }
    if (w_poste == INBOX_SIZE - 1)
        w_poste = 0;
    else
        w_poste++;
}

void UART1_IRQHandler(void) {
    if (LPC_UART1->IIR & 0x04) // RDA: Receive Data Available
    {
        if (c_poste == MSG_LENGTH)
            c_poste = 0; // on supprime le dernier message et recommence
        char c               = LPC_UART1->RBR & 0xFF;
        msg_poste[c_poste++] = c;
        msg_poste[c_poste]   = '\0'; // to end string
    }
    if (c_poste >= 2 && msg_poste[c_poste - 1] == '\n' && msg_poste[c_poste - 2] == '\r') { // traiter le message
        parsing_poste();
        c_poste            = 0;
        msg_poste[c_poste] = '\0'; // to end string
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

/*
 * returns message to treat, or NULL
 * */
t_msg_from_poste *get_poste_msg() {
    if (r_poste == w_poste)
        return 0;
    return inbox_poste + w_poste; // on retourne un pointeur sur le message
}

/*
 * call this when done with processing message.
 * */
void post_msg_done() {
    if (r_poste == INBOX_SIZE - 1)
        r_poste = 0;
    else
        r_poste++;
}

static int uart1_putchar(int c) { // peut être bufferiser ça si les prints prennent trop de temps.
    while (!(LPC_UART1->LSR & 0x20))
        ; // attente THRE
    LPC_UART1->THR = c;
    return c;
}
