
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "Fichier.ini"


int main() {
    LIVRE L;
    int hd;
    int hd1;
    int i = 1;

    hd = open("data/StockLivreSun.dat", O_RDONLY);

    while(read(hd, &L, sizeof(LIVRE))) {
        printf("%2d %-20s  %-20s  %-60s(%d)\n", L.Numero, L.NomAuteur,
            L.PrenomAuteur, L.Titre, L.Libre);
        L.Numero=i;
        i++;
        L.Libre=1;

    }
    close(hd);

    return 0;
}
