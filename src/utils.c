/* #####################################################################################################################

               """          utils.c
        -\-    _|__
         |\___/  . \        Created on 19 May. 2025 at 23:09
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#include <stdint.h>

inline uint8_t is_number(char c) {
    return '0' <= c && c <= '9';
}

uint8_t is_livraison(char c) {
    return 'A' <= c && c <= 'D';
}

uint8_t is_state(char c) {
    return ('C' <= c && c <= 'E') || c == 'L';
}

// parse number ab
uint8_t parse_nb(char a, char b) {
    if (!is_number(a) || !is_number(b))
        return -1; // 255, ce qui est toujours plus grand que le nombre de robots ou de postes.
    return (a - '0') * 10 + (b - '0');
}
