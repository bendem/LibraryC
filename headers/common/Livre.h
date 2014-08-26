#ifndef LIVRE_h
#define LIVRE_h

#include "Fichier.ini"

typedef struct
{
	int  Numero;
	char Titre[MAX_TITLE_SIZE];
	char NomAuteur[MAX_AUTHOR_NAME_SIZE];
	char PrenomAuteur[MAX_AUTHOR_NAME_SIZE];
	int  Libre;
} LIVRE;

void AfficheLivre   (const LIVRE Livre);
int  RechercheNumero(const void*, const void*);
int  RechercheAuteur(const void*, const void*);

#endif
