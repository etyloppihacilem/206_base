/* ##################################646f75627420796f7572206f776e206578697374656e6365###################################

               """          com_poste.c
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 14:35
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "com_poste.h"
#include "LPC17xx.h"
#include "params.h"
#include "utils.h"
#include <stdint.h>

#define MSG_LENGTH 32
#define INBOX_SIZE 32

t_msg_from_poste inbox_poste[INBOX_SIZE]   = { 0 };
char             msg_poste[MSG_LENGTH + 1] = { 0 };

static uint8_t c_poste      = 0;
static uint8_t w_poste      = 0;
static uint8_t r_poste      = 0;
static uint8_t last_asked   = 0; // last poste asked
static uint8_t poste_to_ask = 1;
static uint8_t to_ask       = 0;

static void parsing_poste() {
    if (last_asked == 0)
        return;
    inbox_poste[w_poste].poste = last_asked;
    if (msg_poste[0] == robot) {
        inbox_poste[w_poste].robo_livr = parse_hex(msg_poste[1]);
        if (inbox_poste[w_poste].robo_livr > nb_robots)
            return;
        inbox_poste[w_poste].vit_dest = parse_hex(msg_poste[2]);
        if (!is_state(msg_poste[3]))
            return;
        inbox_poste[w_poste].statut = msg_poste[3];
        inbox_poste[w_poste].type   = robot;
    } else if (msg_poste[1] == info_livraison) {
        if (!is_state(msg_poste[0]))
            return;
        inbox_poste[w_poste].robo_livr = msg_poste[0];
        inbox_poste[w_poste].vit_dest  = parse_nb(msg_poste[2], msg_poste[3]);
        if (inbox_poste[w_poste].vit_dest > nb_postes)
            return;
        inbox_poste[w_poste].type = info_livraison;
    } else if (msg_poste[0] == EOT) { // End Of Transmission : NULL\r\n
        last_asked = 0;
        return;
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
        char c = LPC_UART1->RBR & 0xFF;
        if (c == '\0')
            return;
        msg_poste[c_poste++] = c;
        msg_poste[c_poste]   = '\0';                                                            // to end string
        if (c_poste >= 2 && msg_poste[c_poste - 1] == '\n' && msg_poste[c_poste - 2] == '\r') { // traiter le message
            parsing_poste();
            c_poste            = 0;
            msg_poste[c_poste] = '\0'; // to end string
        }
    }
}

static uint32_t get_uart1_pclk(void) {
    uint32_t             sel        = (LPC_SC->PCLKSEL0 >> 8) & 0x03;
    static const uint8_t div_lut[4] = { 4, 1, 2, 8 };
    return SystemCoreClock / div_lut[sel];
}

/*
 * Il faut une précision de l'horloge de UART1 pour pouvoir monter à 115200 bauds.
 * Pour 9600, pas besoin d'un calcul si compliqué.
 * */
void set_uart1_baud(uint32_t baud) {
    uint32_t pclk     = get_uart1_pclk();
    uint32_t best_dll = 0, best_mul = 1, best_divadd = 0;
    uint32_t best_err = 0xFFFFFFFF;

    for (uint32_t mul = 1; mul <= 15; ++mul) {
        for (uint32_t divadd = 0; divadd < mul; ++divadd) {
            uint32_t dll = (pclk + 8 * baud * mul) / (16 * baud * mul) - divadd; // arrondi
            if (dll == 0 || dll > 0xFFFF)
                continue;

            uint32_t real = pclk / (16 * (dll + ((float) divadd / mul)));
            uint32_t err  = (real > baud) ? real - baud : baud - real;

            if (err < best_err) {
                best_err    = err;
                best_dll    = dll;
                best_mul    = mul;
                best_divadd = divadd;
            }
        }
    }

    LPC_UART1->LCR |= 0x80; // DLAB = 1
    LPC_UART1->DLM  = best_dll >> 8;
    LPC_UART1->DLL  = best_dll & 0xFF;
    LPC_UART1->FDR  = (best_mul << 4) | best_divadd;
    LPC_UART1->LCR &= ~0x80; // DLAB = 0
}

void init_com_poste(uint32_t baudrate) {
    LPC_SC->PCONP |= (1 << 4); // on active l'horloge et le power de UART 1

    LPC_PINCON->PINSEL0 &= ~(3 << 30);
    LPC_PINCON->PINSEL0 |= 1 << 30; // P0.15 = TXD1
    LPC_PINCON->PINSEL1 &= ~3;
    LPC_PINCON->PINSEL1 |= 1; // P0.16 = RXD1

    // Désactiver interruptions pendant init
    LPC_UART1->IER = 0;

    // 8 bits, 1 stop bit, pas de parité
    set_uart1_baud(baudrate);

    LPC_UART1->LCR = 0x03; // 8 bits transmit
    LPC_UART1->FCR = 0x07; // Activer FIFO, reset RX/TX

    // Attente que tout soit prêt
    while (!(LPC_UART1->LSR & 0x20))
        ;
    NVIC_EnableIRQ(UART1_IRQn);
    NVIC_SetPriority(UART1_IRQn, 2); // priorité haute parce que ça doit aller vite
    LPC_UART1->IER = 1;              // RBR interrupt enable

    // Activation du polling
    // Pas d'utilisation de systick parce qu'on veut que l'int passe par le NVIC et pas au dessus
    LPC_TIM0->MCR |= 1 | 1 << 1;
    if (nb_postes)
        LPC_TIM0->MR0 = 8333333 / nb_postes; // ~300Hz
    else
        LPC_TIM0->MR0 = 8333333; // ~300Hz
    LPC_TIM0->TCR = 1;
    NVIC_EnableIRQ(TIMER0_IRQn);
}

void disable_poste_rx() {
    LPC_UART1->IER &= ~1;
}

void enable_poste_rx() {
    LPC_UART1->IER |= 1;
}

/*
 * returns message to treat, or NULL
 * */
t_msg_from_poste *get_poste_msg() {
    t_msg_from_poste *msg;
    disable_poste_rx();
    if (r_poste == w_poste)
        msg = 0;
    else
        msg = inbox_poste + r_poste; // on retourne un pointeur sur le message
    enable_poste_rx();
    return msg;
}

/*
 * call this when done with processing message.
 * */
void poste_msg_done() {
    disable_poste_rx();
    if (r_poste == INBOX_SIZE - 1)
        r_poste = 0;
    else
        r_poste++;
    enable_poste_rx();
}

static int uart1_putchar(int c) { // peut être bufferiser ça si les prints prennent trop de temps.
    while (!(LPC_UART1->LSR & 0x20))
        ; // attente THRE
    LPC_UART1->THR = c;
    return c;
}

void TIMER0_IRQHandler() {
    LPC_TIM0->IR = 1;
    if (!last_asked)
        to_ask = 1;
}

void poll_poste() {
    if (!to_ask
        || last_asked) // si un poste bloque, il n'y a plus aucune info des autres postes (feature, comme ça on sait quel
        return;        // poste à planté...)
    to_ask = 0;
    uart1_putchar('@');
    uart1_putchar('T');
    uart1_putchar(poste_to_ask / 10 + '0');
    uart1_putchar(poste_to_ask % 10 + '0');
    uart1_putchar('\r');
    uart1_putchar('\n');
    last_asked = poste_to_ask;
    if (++poste_to_ask > nb_postes)
        poste_to_ask = 1;
}
