#include "Livre.h"
#include <stdio.h>
#include <strings.h>

void AfficheLivre(const LIVRE Livre)
{
	printf("%4d  %-20s %-20s (%s)\n",Livre.Numero,Livre.NomAuteur,
		Livre.PrenomAuteur,Livre.Titre);
	fflush(stdout);
	return;
}

int RechercheAuteur(const LIVRE* Livre,const void* Nom)
{
	LIVRE* p1 = (LIVRE*)Livre;
	char* p2 = (char*)Nom;
	return strcmp(p1->NomAuteur,p2) ;
}

int RechercheNumero(const void* Livre,const void* Numero)
{
	LIVRE* p1 = (LIVRE*)Livre;
	int N = *(int*)Numero;

	return p1->Numero - N;
}
