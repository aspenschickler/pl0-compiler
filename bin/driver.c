/* Zach Schickler */

// Standard functionality libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

// Peripheral programs
#include "lex.h"
#include "par.h"
#include "vm.h"

int main (int argc, char **argv)
{
    int lex_v = lex(argv[1]);
    if (!lex_v)
    {
        int par_v = par("rsc/par_input.txt");
        if (!par_v)
        {
            int vm_v = vm("rsc/vm_input.txt");
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }

    return 0;
}