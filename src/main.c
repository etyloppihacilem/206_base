/* #####################################################################################################################

               """          main.c
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 09:16
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include "LPC17xx.h" // Définitions registres périphériques
#include "com_poste.h"
#include "com_super.h"
#include "livraisons.h"
#include "params.h"
#include "porteuse.h"
#include "utils.h"

#define AUTOMATIQUE // comment to disable automatic livraison attribution

#define MAX_ROBOTS 16
#define MAX_LIVRAISONS 16

uint8_t vitesse_actuelle[MAX_ROBOTS] = { 0 };
uint8_t vitesse_voulue[MAX_ROBOTS]   = { 0 };
uint8_t enlevement[MAX_ROBOTS]       = { 0 };
uint8_t livraison[MAX_ROBOTS]        = { 0 };
uint8_t en_cours[MAX_ROBOTS]         = { 0 }; // contient la lettre de la livraison en cours

/*
 * returns 0 if livraison if found.
 * */
uint8_t recherche_livraison(uint8_t origine, uint8_t destination, char lettre) {
    uint8_t i;
    for (i = 1; i <= nb_robots; i++) {
        if (en_cours[i])
            continue;
        enlevement[i] = origine;
        livraison[i]  = destination;
        en_cours[i]   = lettre;
        send_to_rob(cote_depot(lettre) ? chargement_droite : chargement_gauche, i, origine);
        i = 0;
    }
    return i;
}

#ifdef AUTOMATIQUE

waitlist_livraisons fifo_livraisons[16] = { 0 };
uint8_t             w_livr              = 0;
uint8_t             r_livr              = 0;

void save_livraison(uint8_t origine, uint8_t destination, char lettre) {
    fifo_livraisons[w_livr].origine     = origine;
    fifo_livraisons[w_livr].destination = destination;
    fifo_livraisons[w_livr].livraison   = lettre;
    if (w_livr == MAX_LIVRAISONS - 1)
        w_livr = 0;
    else
        w_livr++;
}

void process_livraison() {
    if (w_livr == r_livr)
        return;
    if (!recherche_livraison(
            fifo_livraisons[r_livr].origine, fifo_livraisons[r_livr].destination, fifo_livraisons[r_livr].livraison
        )) {
        if (r_livr == MAX_LIVRAISONS - 1)
            r_livr = 0;
        else
            r_livr++;
    }
}

#endif

void process_super(t_msg_from_super *todo) {
    if (!todo)
        return;
    switch (todo->type) {
        case ordre_livraison:
#ifdef AUTOMATIQUE
            if (recherche_livraison(todo->cible, todo->destination, todo->livraison))
                // on sauvegarde la livraison parce qu'aucun robot n'est dispo
                save_livraison(todo->cible, todo->destination, todo->livraison);
#else
            recherche_livraison(todo->cible, todo->destination, todo->livraison);
#endif
            break;
        case vitesse:
            if (todo->robot == 0)
                break;
            vitesse_voulue[todo->robot - 1] = todo->cible;        // on enregistre
            send_to_rob(ordre_vitesse, todo->robot, todo->cible); // on ordonne
        default:;
    }
    super_msg_done(); // on supprime même si pas de robot dispo (feature)
}

void process_poste(t_msg_from_poste *todo) {
    uint8_t rob;
    if (!todo)
        return;
    send_poste_info(todo);
    switch (todo->type) {
        case robot:
            rob                   = todo->robo_livr;
            vitesse_actuelle[rob] = todo->vit_dest;
            if (todo->statut == 'C' && enlevement[rob]) { // si colispris
                enlevement[rob] = 0;
                send_to_rob(
                    cote_reception(en_cours[rob]) ? dechargement_droite : dechargement_gauche, rob, livraison[rob]
                );
            }
            if (todo->statut == 'D' && (livraison[rob] || en_cours[rob])) { // si dispo après livraison
                livraison[rob] = 0;
                en_cours[rob]  = 0;
            }
            break;
#ifdef AUTOMATIQUE
        case info_livraison:
            if (recherche_livraison(todo->poste, todo->vit_dest, todo->robo_livr))
                save_livraison(todo->poste, todo->vit_dest, todo->robo_livr);
#endif
        default:;
    }
    poste_msg_done();
}

int main(void) {
    init_com_super(9600);
    init_params();
    read_params(); // on initilise le nombre de robots et de postes
    init_porteuse();

    while (1) {
        process_super(get_super_msg()); // message from super
        process_poste(get_poste_msg()); // message from poste
#ifdef AUTOMATIQUE
        process_livraison(); // processing waiting livraisons
#endif
    }
}
