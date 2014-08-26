/*
   In.Pr.E.S.
   Denys MERCENIER
   Ecran.cpp
*/

#include "Ecran.h"

void Trace(char *pszTrace, ... ) {
	char szBuffer[200];
	va_list arg;
	va_start(arg, pszTrace);
	vsprintf(szBuffer, pszTrace, arg);
	fprintf(stdout, "%s\n", szBuffer);
	va_end(arg);
	return;
}

void TraceErreur(char *pszTrace, ... ) {
	char szBuffer[80];
	va_list arg;
	va_start(arg, pszTrace);
	vsprintf(szBuffer, pszTrace, arg);
	perror(szBuffer);
	va_end(arg);
	return;
}
