#pragma ident "$Id: sint.c,v 1.3 2012/02/14 20:22:07 dechavez Exp $"
/*======================================================================
 *
 *  Manipulate sample interval.
 *  seed_sintsplit: split sample interval into sample rate factor and
 *                  multiplier
 *  seed_makesint:  given sample rate factor and multiplier, create
 *                  sample interval
 *
 *  Assumes nominal sample intervals/rates with no fractional parts.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <math.h>
#include "seed.h"

void seed_sintsplit(double sint, INT16 *fact, INT16 *mult)
{

    if (sint >= 1.0) {
        *fact = (INT16) -sint;
        *mult = 1;
    } else {
        *fact = (INT16) rint((1.0 / sint));
        *mult = 1;
    }
}

double seed_makesint(INT16 fact, INT16 mult)
{
    if (fact > 0 && mult > 0) {
        return 1.0 / (double) fact * (double) mult;
    } else if (fact > 0 && mult < 0) {
        return (double) -mult / (double) fact;
    } else if (fact < 0 && mult > 0) {
        return (double) -fact / (double) mult;
    } else {
        return (double) fact / (double) mult;
    }
}

#ifdef DEBUG_TEST

#include <stdio.h>
#include <math.h>

main(argc, argv)
int argc;
char *argv[];
{
double sint;
INT16 fact, mult;

    if (argc == 2) {
        sint = atof(argv[1]);
        seed_sintsplit(sint, &fact, &mult);
        printf("%lf splits to fact = %hd, mult = %hd\n", sint, fact, mult);
    } else if (argc == 3) {
        fact = atoi(argv[1]);
        mult = atoi(argv[2]);
        sint = seed_makesint(fact, mult);
        printf("fact = %hd, mult = %hd => sint = %lf\n", fact, mult, sint);
    } else {
        fprintf(stderr, "usage: %s sint -OR- %s fact mult\n",
            argv[0], argv[0]
        );
        exit(1);
    }

    exit(0);
}

#endif /* DEBUG_TEST */

/* Revision History
 *
 * $Log: sint.c,v $
 * Revision 1.3  2012/02/14 20:22:07  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.2  2007/01/11 17:48:03  dechavez
 * added platform.h and stdtypes.h stuff
 *
 * Revision 1.1.1.1  2000/02/08 20:20:39  dec
 * import existing IDA/NRTS sources
 *
 */
