#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "Fichier.ini"
#include "Ecran.h"
#include "Libs.h"


int main(int argc, char const *argv[]) {
    if (argc != 8) {
        TraceErreur("(Login) Mauvais nombre d'arguments");
        exit(1);
    }

    int idQ, hdLogin, hdLocation, hdReservation, hdLivre, nb_livres_reserves, livres_reserves[MAX_RESERVED_BOOKS], i, reserve_mais_non_loue;
    char line[MAX_USER_NAME_SIZE], username[MAX_USER_NAME_SIZE];
    long client_pid;
    MESSAGE M;
    MESSAGELIVRE MsgLivre;
    RESERVATION Reservation;
    LIVRE Livre;
    LOCATION Location;

    Trace("\t(Login) Debut de Login");
    idQ = atoi(argv[1]);
    hdLogin = atoi(argv[2]);
    client_pid = atol(argv[3]);
    hdLocation = atoi(argv[4]);
    hdReservation = atoi(argv[5]);
    hdLivre = atoi(argv[6]);
    strcpy(username, argv[7]);
    M.Pid = getpid();
    MsgLivre.Pid = getpid();

    Trace("\t(Login) Redirection du stdin vers le filedes %d", hdLogin);
    if(close(0)) {
        TraceErreur("(Login) IMPORTANT: Erreur close stdin");
        exit(1);
    }

    lseek(hdLogin, 0, SEEK_SET);
    dup(hdLogin); // Redirection de stdin vers Membres.dat afin de pouvoir utiliser gets

    Trace("\t(Login) Recherche de '%s'...", username);
    MsgLivre.Requete = -1; // Client non trouvé
    while (gets(line)) {
        if (strcmp(strtolower(line), username) == 0) {
            Trace("\t(Login) '%s' trouve!", username);

            // Si un livre réservé par le client est dispo, il faut le lui dire:
            MsgLivre.Requete = 0; // Sera utile pour connaitre le nombre de livres réservés et désormais disponibles

            Trace("\t(Login) Recherche des livres reserves par le client");
            nb_livres_reserves = 0;
            lseek(hdReservation, 0, SEEK_SET);
            while(read(hdReservation, &Reservation, sizeof(RESERVATION)) && nb_livres_reserves < MAX_RESERVED_BOOKS) {
                if (!strcmp(Reservation.Membre, username) && Reservation.NumeroLivre != -1) {
                    Trace("\t(Login) [DEBUG] Livre reserve : %d", Reservation.NumeroLivre);
                    livres_reserves[nb_livres_reserves] = Reservation.NumeroLivre;
                    ++nb_livres_reserves;
                }
            }
            Trace("\t(Login) %s a reserve %d livres", username, nb_livres_reserves);

            if (nb_livres_reserves) {
                Trace("\t(Login) Recherche des livres reserves par le client dans les locations");
                // Tous les livres réservés sont potentiellement non loués. Vérifions le nombre de livres réellement dispo
                reserve_mais_non_loue = nb_livres_reserves;
                lseek(hdLocation, 0, SEEK_SET);
                while (read(hdLocation, &Location, sizeof(LOCATION)) && reserve_mais_non_loue) {
                    for (i = 0; i < nb_livres_reserves; ++i) {
                        if (Location.NumeroLivre == livres_reserves[i] && Location.NumeroLivre != -1) {
                            Trace("\t(Login) Le livre %d est encore reserve", Location.NumeroLivre);
                            livres_reserves[i] = -1;
                            --reserve_mais_non_loue;
                            break;
                        }
                    }
                }
                if (reserve_mais_non_loue) {
                    // Chargement des livres réservés désormais disponibles dans le MessageLivre à envoyer au client
                    for (i = 0; i < nb_livres_reserves && reserve_mais_non_loue; ++i) {
                        if (livres_reserves[i] != -1) {
                            lseek(hdLivre, (livres_reserves[i]-1) * sizeof(LIVRE), SEEK_SET);
                            read(hdLivre, &Livre, sizeof(LIVRE));
                            memcpy(&MsgLivre.L[MsgLivre.Requete], &Livre, sizeof(LIVRE));
                            // Contiendra le nbre de livres qui avaient été réservés et qui sont désormais dispo
                            ++(MsgLivre.Requete);
                            --reserve_mais_non_loue;
                            Trace("\t(Login) %s (%d) est disponible! (on en est a %d bouquins)",
                                Livre.Titre, Livre.Numero, MsgLivre.Requete);
                        }
                    }
                }
            }
            break;
        }
    }

    Trace("\t(Login) Envoi de la nouvelle au serveur", username);
    // Envoi de la nouvelle au serveur
    M.Requete = MsgLivre.Requete == -1 ? AUTHENTICATION_FAIL : AUTHENTICATION_SUCCESS;
    M.Type = SERVER_ID;
    sprintf(M.N, "%ld", client_pid);
    send(idQ, M);

    // Envoi la nouvelle au client
    Trace("\t(Login) Envoi de la nouvelle au client");
    M.Type = client_pid;
    M.Requete = MsgLivre.Requete;
    send(idQ, M);
    if(M.Requete > 0) {
        // Envoi des livre dispo au client
        Trace("\t(Login) Envoi des bouquins dispos au client");
        MsgLivre.Type = client_pid;
        if (msgsnd(idQ, &MsgLivre, sizeof(MESSAGELIVRE) - sizeof(long), 0) == -1) {
            TraceErreur("\t(AccesFichier) Err de msgsnd()");
            exit(1);
        }
    }

    Trace("\t(Login) Fin de login");
    exit(0);
}
