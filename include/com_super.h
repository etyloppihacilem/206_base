/* #####################################################################################################################

               """          com_super.h
        -\-    _|__
         |\___/  . \        Created on 16 May. 2025 at 14:35
         \     /(((/        by hmelica
          \___/)))/         hmelica@student.42.fr

##################################################################################################################### */

#ifndef INCLUDE_INCLUDE_COM_SUPER_H_
#define INCLUDE_INCLUDE_COM_SUPER_H_

#include <stdio.h>
#include <stdint.h>

void init_com_super(uint32_t baudrate);

int fputc(int c, FILE *f);
int _write(int file, char *ptr, int len);

#endif  // INCLUDE_INCLUDE_COM_SUPER_H_
