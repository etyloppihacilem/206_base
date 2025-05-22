/* #####################################################################################################################

               """          com_super.c
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 14:35
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

#####################################################################################################################
*/

#include "com_super.h"
#include "LPC17xx.h"
#include "com_poste.h"
#include "params.h"
#include "utils.h"
#include <stdint.h>

#define MSG_LENGTH 32
#define INBOX_SIZE 4

char             msg_super[MSG_LENGTH + 1] = { 0 };
t_msg_from_super inbox_super[INBOX_SIZE]   = { 0 };

uint8_t c_super = 0;
uint8_t w_super = 0;
uint8_t r_super = 0;

static void parsing_super() {
    if (msg_super[0] != 'R')
        return;                                                        // delete message
    inbox_super[w_super].robot = parse_nb(msg_super[1], msg_super[2]); // conversion en chiffre
    if (inbox_super[w_super].robot > nb_robots)
        return;                                                        // delete message
    inbox_super[w_super].cible = parse_nb(msg_super[4], msg_super[5]); // conversion en chiffre
    switch (msg_super[3]) {
        case vitesse:
            inbox_super[w_super].type = vitesse;
            break; // plus rien à parser, au revoir
        case ordre_livraison:
            if (!is_livraison(msg_super[6]))
                return;
            inbox_super[w_super].type        = ordre_livraison;
            inbox_super[w_super].livraison   = msg_super[6];
            inbox_super[w_super].destination = parse_nb(msg_super[8], msg_super[9]);
            if (inbox_super[w_super].destination > nb_postes || inbox_super[w_super].cible > nb_postes)
                return;
            break;
        default:
            return; // delete message
    }
    // on valide le message
    if (w_super == INBOX_SIZE - 1)
        w_super = 0;
    else
        w_super++;
}

void UART0_IRQHandler(void) {
    if (LPC_UART0->IIR & 0x04) // RDA: Receive Data Available
    {
        if (c_super == MSG_LENGTH)
            c_super = 0;
        char c = LPC_UART0->RBR & 0xFF;
        if (c == '\0')
            return;
        msg_super[c_super++] = c;
        msg_super[c_super]   = '\0'; // to end string
    }
    if (c_super >= 2 && msg_super[c_super - 1] == '\n' && msg_super[c_super - 2] == '\r') {
        parsing_super();
        c_super            = 0;
        msg_super[c_super] = '\0'; // to end string
    }
}

static int uart0_putchar(int c) { // peut être bufferiser ça si les prints prennent trop de temps.
    while (!(LPC_UART0->LSR & 0x20))
        ; // attente THRE
    LPC_UART0->THR = c;
    return c;
}

void init_com_super(uint32_t baudrate) {
    LPC_SC->PCONP |= (1 << 3); // on active l'horloge et le power de UART 0

    LPC_PINCON->PINSEL0 &= ~((3 << 4) | (3 << 6));
    LPC_PINCON->PINSEL0 |= (1 << 4) | (1 << 6); // P0.2 = TXD0, P0.3 = RXD0

    uint32_t Fdiv;
    uint32_t pclk;

    // Le PCLK de UART0 est par défaut à CCLK / 4
    pclk = SystemCoreClock / 4;

    // Désactiver interruptions pendant init
    LPC_UART0->IER = 0;

    // 8 bits, 1 stop bit, pas de parité
    LPC_UART0->LCR = 0x83; // DLAB = 1 pour accéder à DLL/DLM

    Fdiv           = pclk / (16 * baudrate);
    LPC_UART0->DLM = Fdiv / 256;
    LPC_UART0->DLL = Fdiv % 256;

    LPC_UART0->LCR = 0x03; // DLAB = 0
    LPC_UART0->FCR = 0x07; // Activer FIFO, reset RX/TX

    // Attente que tout soit prêt
    while (!(LPC_UART0->LSR & 0x20))
        ;
    NVIC_EnableIRQ(UART0_IRQn);
    NVIC_SetPriority(UART0_IRQn, 5);
    LPC_UART0->IER = 1; // RBR interrupt enable
}

/*
 * returns message to treat, or NULL
 * */
t_msg_from_super *get_super_msg() {
    if (r_super == w_super)
        return 0;
    return inbox_super + r_super; // on retourne un pointeur sur le message
}

/*
 * call this when done with processing message.
 * */
void super_msg_done() {
    if (r_super == INBOX_SIZE - 1)
        r_super = 0;
    else
        r_super++;
}

void send_poste_info(t_msg_from_poste *msg) {
    uart0_putchar('P');
    uart0_putchar(msg->poste / 10 + '0');
    uart0_putchar(msg->poste % 10 + '0');
    switch (msg->type) {
        case robot:
            uart0_putchar('R');
            uart0_putchar(msg->robo_livr / 10 + '0');
            uart0_putchar(msg->robo_livr % 10 + '0');
            uart0_putchar(msg->vit_dest / 10 + '0');
            uart0_putchar(msg->vit_dest % 10 + '0');
            uart0_putchar(msg->statut);
            break;
        case info_livraison:
            uart0_putchar('P');
            uart0_putchar(msg->robo_livr);
            uart0_putchar('P');
            uart0_putchar(msg->vit_dest / 10 + '0');
            uart0_putchar(msg->vit_dest % 10 + '0');
        default:;
    }
    uart0_putchar('\r');
    uart0_putchar('\n');
}

void send_params() {
    uart0_putchar('C');
    uart0_putchar('R');
    uart0_putchar(nb_robots / 10 + '0');
    uart0_putchar(nb_robots % 10 + '0');
    uart0_putchar('P');
    uart0_putchar(nb_postes / 10 + '0');
    uart0_putchar(nb_postes % 10 + '0');
    uart0_putchar('\r');
    uart0_putchar('\n');
}

void debug_write(const char *str) {
    while (*str != '\0')
        uart0_putchar(*(str++));
}
