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
#include "params.h"
#include "porteuse.h"
#include "utils.h"

#define MAX_ROBOTS 16

uint8_t vitesse_actuelle[MAX_ROBOTS] = { 0 };
uint8_t vitesse_voulue[MAX_ROBOTS]   = { 0 };
uint8_t enlevement[MAX_ROBOTS]       = { 0 };
uint8_t livraison[MAX_ROBOTS]        = { 0 };
uint8_t en_cours[MAX_ROBOTS]         = { 0 };

int main(void) {
    init_com_super(9600);
    init_params();
    init_porteuse();

    t_msg_from_super *super_todo;
    t_msg_from_poste *poste_todo;
    while (1) {
        super_todo = get_super_msg(); // message from super
        if (super_todo) {
            switch (super_todo->type) {
                case ordre_livraison:
                    for (uint8_t i = 1; i <= nb_robots; i++) {
                        if (enlevement[i] || livraison[i])
                            continue;
                        enlevement[i] = super_todo->cible;
                        livraison[i]  = super_todo->destination;
                        en_cours[i]   = super_todo->livraison;
                        send_to_rob(
                            cote_depot(super_todo->livraison) ? chargement_droite : chargement_gauche,
                            super_todo->robot, super_todo->cible
                        );
                        super_msg_done();
                    }
                    break;
                case vitesse:
                    if (super_todo->robot == 0)
                        break;
                    vitesse_voulue[super_todo->robot - 1] = super_todo->cible;        // on enregistre
                    send_to_rob(ordre_vitesse, super_todo->robot, super_todo->cible); // on ordonne
                    super_msg_done();
                default:;
            }
        }
        poste_todo = get_poste_msg(); // message from poste
        if (poste_todo)
            poste_msg_done();
    }
}
