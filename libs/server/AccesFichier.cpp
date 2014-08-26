#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Fichier.ini"
#include "Ecran.h"
#include "Libs.h"


int main(int argc, char *argv[])
{
    MESSAGE M; // Message reçu
    MESSAGELIVRE M1; // Message à envoyer au client
    int rc, hdLivres, hdLoc, hdRes, idQ, nbLivres, found;
    LIVRE Livre;
    LOCATION Location;
    RESERVATION Reservation;
    struct stat st;
    char membre [MAX_USER_NAME_SIZE];
    flock lockLoc, lockRes;


    Trace("\t(AccesFichier) Debut de AccesFichier");
    idQ = atoi(argv[1]);
    hdLivres = atoi(argv[2]);
    hdLoc = atoi(argv[3]);
    hdRes = atoi(argv[4]);
    strcpy(membre, strtolower(argv[5]));

    while (1) {
        Trace("\t(AccesFichier) En attente d'une action a effectuer");
        // Attente d'un message jusqu'à ce que papa décide de nous tuer.
        if (msgrcv(idQ, &M, sizeof(M) - sizeof(long), getpid(), 0) == -1) {
            TraceErreur("\t(AccesFichier) Err de msgrcv()");
            exit(1);
        }
        Trace("\t(AccesFichier) Message du serveur recu (requete %d)", M.Requete);

        switch(M.Requete) {
            case KILL_ACCES_FICHIER:
                Trace("\t(AccesFichier) Fermeture de AccesFichier");
                exit(0);

            case RECHERCHE_REQUEST:
                // Trace("Avant atoi");
                rc = atoi(M.N);
                // Trace("apres atoi");
                if (rc) {
                    // Si c'est un numéro de livre

                    Trace("\t(AccesFichier) Reception d'un entier (%d)", rc);
                    --rc;
                    stat("data/LivrePc.dat", &st);
                    Trace("\t(AccesFichier) [DEBUG] Taille du fichier: %ld", st.st_size);
                    Trace("\t(AccesFichier) [DEBUG] Offset du livre recherche: %ld", rc*sizeof(LIVRE));
                    if (rc*sizeof(LIVRE) >= st.st_size || rc < 0) {
                        nbLivres = 0;
                    } else {
                        lseek(hdLivres, rc*sizeof(LIVRE), SEEK_SET);
                        read(hdLivres, &Livre, sizeof(LIVRE));
                        memcpy(&M1.L[0], &Livre, sizeof(LIVRE));
                        nbLivres = 1;
                    }
                } else {
                    // Si c'est un auteur

                    Trace("\t(AccesFichier) Reception d'un auteur (%s)", M.N);
                    lseek(hdLivres, 0, SEEK_SET);
                    nbLivres = 0;
                    while(read(hdLivres, &Livre, sizeof(LIVRE))) {
                        if (!strncmp(strtolower(Livre.NomAuteur), strtolower(M.N), strlen(M.N))) {
                            if (nbLivres >= MAX_BOOK) {
                                Trace("\t(AccesFichier) Trop de livres trouves!");
                                break;
                            }
                            memcpy(&M1.L[nbLivres], &Livre, sizeof(LIVRE));
                            ++nbLivres;
                        }
                    }
                }

                Trace("\t(AccesFichier) Envoi du message au client");
                M1.Type = M.Pid; // Étant donné que le serveur nous envoie le pid du client dans M.Pid....
                if (nbLivres >= MAX_BOOK) {
                    M1.Requete = -1;
                } else {
                    M1.Requete = nbLivres; // C'est de cette façon que nous connaîtrons le nombre de livres trouvés dans le client
                }
                Trace("\t(AccesFichier) Nombre de livres a envoyer: %d", M1.Requete);

                if (msgsnd(idQ, &M1, sizeof(M1) - sizeof(long), 0) == -1) {
                    TraceErreur("\t(AccesFichier) Err de msgsnd()");
                    exit(1);
                }

                break;

            case GET_RENTED_BOOKS:
                Trace("\t(AccesFichier) Pose d'un verrou en lecture sur %s", RENT_FILE);
                lockLoc.l_type = F_RDLCK;
                lockLoc.l_whence = SEEK_SET;
                lockLoc.l_start = 0;
                lockLoc.l_len = 0;
                fcntl(hdLoc, F_SETLKW, &lockLoc);

                Trace("\t(AccesFichier) Recherche des livres que '%s' a loue ", membre);
                lseek(hdLoc, 0, SEEK_SET);
                found = 0;
                while(read(hdLoc, &Location, sizeof(LOCATION)) && found < MAX_RENTED_BOOKS) {
                    if (!strcmp(membre, Location.Membre) && Location.NumeroLivre != -1) {
                        Trace("\t(AccesFichier) Le livre %d est loue par %s", Location.NumeroLivre, membre);
                        lseek(hdLivres, (Location.NumeroLivre - 1) * sizeof(LIVRE), SEEK_SET);
                        read(hdLivres, &Livre, sizeof(LIVRE));
                        memcpy(&M1.L[found], &Livre, sizeof(LIVRE));
                        ++found;
                    }
                }
                Trace("\t(AccesFichier) Suppression du verrou sur %s", RENT_FILE);
                lockLoc.l_type = F_UNLCK;
                fcntl(hdLoc, F_SETLKW, &lockLoc);

                Trace("\t(AccesFichier) %d livres ont ete loues par %s", found, membre);
                if (found > MAX_RENTED_BOOKS) {
                    Trace("\t(AccesFichier) Trop de livres en location");
                    M1.Requete = -1;
                } else {
                    M1.Requete = found; // Nombre de livres loués
                }
                M1.Type = M.Pid; // Étant donné que le serveur nous envoie le pid du client dans M.Pid....

                if (msgsnd(idQ, &M1, sizeof(M1) - sizeof(long), 0) == -1) {
                    TraceErreur("\t(AccesFichier) Err de msgsnd()");
                    exit(1);
                }

                break;

            case RENT_REQUEST:
                rc = atoi(M.N); // N° de livre a réserver
                Trace("\t(AccesFichier) Livre a louer: %d", rc);

                Trace("\t(AccesFichier) Pose d'un verrou en ecriture sur %s", RENT_FILE);
                lockLoc.l_type = F_WRLCK;
                lockLoc.l_whence = SEEK_SET;
                lockLoc.l_start = 0;
                lockLoc.l_len = 0;
                fcntl(hdLoc, F_SETLKW, &lockLoc);

                found = 0;
                lseek(hdLoc, 0, SEEK_SET);
                while(read(hdLoc, &Location, sizeof(LOCATION))) {
                    // Vérification des livres déjà en location
                    if (!strcmp(membre, Location.Membre) && Location.NumeroLivre != -1) {
                        ++found;
                        if (found >= MAX_RENTED_BOOKS) {
                            Trace("\t(AccesFichier) Trop de livres en location");
                            found = -1;
                            break;
                        }
                    }
                    // Vérification de la disponibilité du livre dans les locations
                    if (Location.NumeroLivre == rc) {
                        Trace("\t(AccesFichier) Ce livre est deja loue");
                        found = -2;
                        break;
                    }
                }
                // Vérification de la disponnibilité du livre dans les réservations
                if (found >= 0) {
                    Trace("\t(AccesFichier) Recherche du livre %d dans les reservations", rc);

                    Trace("\t(AccesFichier) Pose d'un verrou en ecriture sur %s", RESERVATION_FILE);
                    lockRes.l_type = F_WRLCK;
                    lockRes.l_whence = SEEK_SET;
                    lockRes.l_start = 0;
                    lockRes.l_len = 0;
                    fcntl(hdRes, F_SETLKW, &lockRes);

                    lseek(hdRes, 0, SEEK_SET);
                    while (read(hdRes, &Reservation, sizeof(RESERVATION))) {
                        // Si le livre est déjà réservé
                        if (Reservation.NumeroLivre == rc) {
                            // Par moi même: je ne le réserve plus et je le loue
                            if (!strcmp(membre, Reservation.Membre)) {
                                Reservation.NumeroLivre = -1;
                                lseek(hdRes, -sizeof(RESERVATION), SEEK_CUR);
                                write(hdRes, &Reservation, sizeof(RESERVATION));
                            } else { // Par quelqu'un d'autre: on annule la location
                                Trace("\t(AccesFichier) Ce livre est deja reserve", rc);
                                found = -3;
                            }
                            break;
                        }
                    }
                    Trace("\t(AccesFichier) Suppression du verrou sur %s", RESERVATION_FILE);
                    lockRes.l_type = F_UNLCK;
                    fcntl(hdRes, F_SETLKW, &lockRes);
                }

                switch (found) {
                    case -1 :
                        Trace("\t(AccesFichier) Trop de livres en location");
                        M.Requete = TOO_MANY_RENTS;
                        break;

                    case -2 :
                        Trace("\t(AccesFichier) Livre deja en location");
                        M.Requete = ALREADY_RENTED;
                        break;

                    case -3:
                        Trace("\t(AccesFichier) Livre deja réserve");
                        M.Requete = ALREADY_RESERVED;

                    default:
                        Trace("\t(AccesFichier) Ce livre est dispo. Sauvegarde de la location...");
                        Location.NumeroLivre = rc;
                        strcpy(Location.Membre, membre);
                        // sleep(4); /**/
                        write(hdLoc, &Location, sizeof(LOCATION));

                        Trace("\t(AccesFichier) Modification de l'etat du livre");
                        lseek(hdLivres, (Location.NumeroLivre - 1) * sizeof(LIVRE), SEEK_SET);
                        read(hdLivres, &Livre, sizeof(LIVRE));
                        Livre.Libre = 0;
                        lseek(hdLivres, (Location.NumeroLivre - 1) * sizeof(LIVRE), SEEK_SET);
                        write(hdLivres, &Livre, sizeof(LIVRE));

                        M.Requete = RENT_SUCCESS;
                        break;
                }

                Trace("\t(AccesFichier) Suppression du verrou sur %s", RENT_FILE);
                lockLoc.l_type = F_UNLCK;
                fcntl(hdLoc, F_SETLKW, &lockLoc);

                Trace("\t(AccesFichier) Envoi du resultat au client");
                M.Type = M.Pid; // Étant donné que le serveur nous envoie le pid du client dans M.Pid....
                send(idQ, M);

                break;

            case RETURN_BOOK:
                rc = atoi(M.N);
                Trace("\t(AccesFichier) Livre rendu: %d", rc);

                Trace("\t(AccesFichier) Pose d'un verrou en ecriture sur %s", RENT_FILE);
                lockLoc.l_type = F_WRLCK;
                lockLoc.l_whence = SEEK_SET;
                lockLoc.l_start = 0;
                lockLoc.l_len = 0;
                fcntl(hdLoc, F_SETLKW, &lockLoc);

                lseek(hdLoc, 0, SEEK_SET);
                found = 0;
                while(read(hdLoc, &Location, sizeof(LOCATION))) {
                    if (Location.NumeroLivre == rc) {
                        Trace("\t(AccesFichier) Mise a jour du fichier de location");
                        Location.NumeroLivre = -1;
                        lseek(hdLoc, -sizeof(LOCATION), SEEK_CUR);
                        write(hdLoc, &Location, sizeof(LOCATION));
                        found = 1;

                        Trace("\t(AccesFichier) Mise a jour de la disponnibilite dans le fichier Livres");
                        lseek(hdLivres, (rc -1) * sizeof(LIVRE), SEEK_SET);
                        read(hdLivres, &Livre, sizeof(LIVRE));
                        Livre.Libre = 1;
                        lseek(hdLivres, (rc -1) * sizeof(LIVRE), SEEK_SET);
                        write(hdLivres, &Livre, sizeof(LIVRE));

                        break;
                    }
                }
                Trace("\t(AccesFichier) Suppression du verrou sur %s", RENT_FILE);
                lockLoc.l_type = F_UNLCK;
                fcntl(hdLoc, F_SETLKW, &lockLoc);

                if (!found) {
                    Trace("\t(AccesFichier) ERREUR: Le livre a rendre n'est pas en location");
                    strcpy(M.N, "Impossible de rendre le livre");
                } else {
                    Trace("\t(AccesFichier) Livre rendu avec succes!");
                    strcpy(M.N, "Livre rendu avec succes!");
                }
                M.Type = M.Pid; // Étant donné que le serveur nous envoie le pid du client dans M.Pid....
                send(idQ, M);

                if (!found)
                    break; // Sortie du switch et on ne se préoccupe pas des réservations

                Trace("\t(AccesFichier) Verification des reservations concernant le livre %d", rc);

                Trace("\t(AccesFichier) Pose d'un verrou en lecture sur %s", RESERVATION_FILE);
                lockLoc.l_type = F_RDLCK;
                lockLoc.l_whence = SEEK_SET;
                lockLoc.l_start = 0;
                lockLoc.l_len = 0;
                fcntl(hdRes, F_SETLKW, &lockLoc);

                lseek(hdRes, 0, SEEK_SET);
                while (read(hdRes, &Reservation, sizeof(RESERVATION))) {
                    if (Reservation.NumeroLivre == rc) {
                        /* Quelqu'un a reserve ce livre, il faut l'avertir du changement */
                        Trace("\t(AccesFichier) Envoi d'un message au serveur pour connaitre la cible du kill()");
                        M.Type = SERVER_ID;
                        M.Requete = RETURNED_BOOK_NOTIF;
                        M.Pid = getpid();
                        strcpy(M.N, Reservation.Membre);
                        send(idQ, M);

                        Trace("\t(AccesFichier) Attente de la reponse du serveur connaitre la cible du kill()");
                        if (msgrcv(idQ, &M, sizeof(M) - sizeof(long), getpid(), 0) == -1) {
                            TraceErreur("\t(AccesFichier) Err de msgrcv()");
                            exit(1);
                        }

                        if (M.Pid == 0) {
                            Trace("\t(AccesFichier) L'utilisateur n'est pas connecte");
                            break;
                        }

                        Trace("\t(AccesFichier) Envoi d'un signal au client dont le pid est %ld", M.Pid);
                        kill(M.Pid, SIGPWR);

                        Trace("\t(AccesFichier) Envoi d'un message au client car le livre est dispo");
                        M1.Type = M.Pid;
                        M1.Requete = 1; // Nombre de livres
                        M1.Pid = getpid();
                        memcpy(&M1.L[0], &Livre, sizeof(LIVRE));
                        Trace("\t(AccesFichier) [DEBUG] No %d %s de %s", M1.L[0].Numero, M1.L[0].Titre, M1.L[0].NomAuteur);
                        Trace("\t(AccesFichier) [DEBUG] %d livres a envoyer", M1.Requete);
                        if (msgsnd(idQ, &M1, sizeof(MESSAGELIVRE) - sizeof(long), 0) == -1) {
                            TraceErreur("\t(AccesFichier) Err de msgsnd()");
                            exit(1);
                        }
                    }
                } // end while(read)

                Trace("\t(AccesFichier) Suppression du verrou sur %s", RESERVATION_FILE);
                lockLoc.l_type = F_UNLCK;
                fcntl(hdRes, F_SETLKW, &lockLoc);

                break;

            default:
                Trace("\t(AccesFichier) ERREUR: Type de requete inconnu");
                break;

        } // end switch
    } // end while(1)
}
