#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "utils.h"

int main(int argc, char *argv[]) {

    int result = 0;
    FILE *fOut = stdout, *fIn = stdin;

    /* skip over program name */
    ++argv, --argc;
    if ( argc > 0 ) {
        fIn = fopen( argv[0], "r" );
    }

    while (true) {

        result = getWord(fIn);

        if (result == -11) {
            printf(" %i ", result); printf("--> EOF");
            break;
        }
        if (result < 0) {
            printf(" %i ", result); printf("--> <0");
            /*continue;*/
            break;
        }

        fprintf(fOut, " => %i", result);
    }

    fprintf(fOut, "\n\n done \n ");
    exit(EXIT_SUCCESS);
}