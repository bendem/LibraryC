/*
   IN.PR.E.S.
   Denys MERCENIER
   Ecran.h
*/

#ifndef        ECRAN_H
#define        ECRAN_H

#define        NORM      0
#define        GRAS      1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef SUN
#include <sys/varargs.h>
#endif

#ifdef COMPAC
#include <varargs.h>
#include <stdarg.h>
#endif

/* fonctions simplifiees de gestion d'ecran */
void Trace(char *,...);
void TraceErreur(char *,...);

#endif
