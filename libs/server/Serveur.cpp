#include "Serveur.h"

int idQ, idSemLocation, idSemReservation, idSemOuverture;
int hdLivre, hdLogin, hdLocation, hdReservation;
int nbUserConnected = 0;
long pidServer = getpid();
UserConnection userConnected[MAX_CLIENT_CONNECTIONS];

int main(int argc,char *argv[])
{
    MESSAGE Msg;
    int idFils, hdServerPid;
    UserConnection *pUserConnection;

    Trace("(Serveur) Ecriture du pid du serveur dans data/Server.pid");
    if ((hdServerPid = open("data/Server.pid", O_WRONLY | O_TRUNC | O_CREAT, S_IWRITE | S_IREAD)) == -1) {
        TraceErreur("\t(Serveur) Erreur de FIOuverture(Server.pid)");
        Sortie(1);
    }
    write(hdServerPid, &pidServer, sizeof(pidServer));
    close(hdServerPid);

    Trace("(Serveur) Creation des ressources...");
    Trace("\t(Serveur) Creation de la file de msg");
    if ((idQ = msgget(IPC_KEY, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        TraceErreur("\t(Serveur) Erreur de msgget()");
        Sortie(1);
    }

    Trace("\t(Serveur) Ouverture des fichiers");
    if ((hdLivre = open(BOOKS_FILE, O_RDWR)) == -1) {
        TraceErreur("\t(Serveur) Erreur de FIOuverture(LivrePc.dat)");
        Sortie(1);
    }
    if ((hdLogin = open(MEMBERS_FILE, O_RDWR)) == -1) {
        TraceErreur("\t(Serveur) Erreur de FIOuverture(Membres.dat)");
        Sortie(1);
    }
    if ((hdLocation = open(RENT_FILE, O_RDWR)) == -1) {
        TraceErreur("\t(Serveur) Erreur de FIOuverture(Locations.dat)");
        Sortie(1);
    }
    if ((hdReservation = open(RESERVATION_FILE, O_RDWR)) == -1) {
        TraceErreur("\t(Serveur) Erreur de FIOuverture(Reservations.dat)");
        Sortie(1);
    }

    Trace("\t(Serveur) Creation du semaphore 'Locations'");
    if ((idSemLocation = semget(SEM_LOC_KEY, 1, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        TraceErreur("\t(Serveur) Erreur de semget()");
        Sortie(1);
    }
    Trace("\t(Serveur) Creation du semaphore 'Reservation'");
    if ((idSemReservation = semget(SEM_RES_KEY, 1, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        TraceErreur("\t(Serveur) Erreur de semget()");
        Sortie(1);
    }
    Trace("\t(Serveur) Creation du semaphore 'Ouverture'");
    if ((idSemOuverture = semget(SEM_OPEN_KEY, 1, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        TraceErreur("\t(Serveur) Erreur de semget()");
        Sortie(1);
    }
    // Initialisation des sémaphores à 1
    union semun {
        int val;
    } arg;
    arg.val = 1;
    semctl(idSemLocation, 0, SETVAL, arg);
    semctl(idSemReservation, 0, SETVAL, arg);
    semctl(idSemOuverture, 0, SETVAL, arg);

    Trace("\t(Serveur) Armement des signaux");
    struct sigaction sigAction;
    sigAction.sa_handler = quitHandler;
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = 0;
    Trace("\t\t(Serveur) Armement de SIGINT");
    if(sigaction(SIGINT, &sigAction, NULL) == -1) {
        TraceErreur("\t\t(Serveur) Erreur d'armement des signaux");
        Sortie(1);
    }
    Trace("\t\t(Serveur) Armement de SIGQUIT");
    if(sigaction(SIGQUIT, &sigAction, NULL) == -1) {
        TraceErreur("\t\t(Serveur) Erreur d'armement des signaux");
        Sortie(1);
    }
    sigAction.sa_handler = broadcastHandler;
    sigemptyset(&sigAction.sa_mask);
    Trace("\t\t(Serveur) Armement de SIGUSR2");
    if(sigaction(SIGUSR2, &sigAction, NULL) == -1) {
        TraceErreur("\t\t(Serveur) Erreur d'armement des signaux");
        Sortie(1);
    }

    Trace("(Serveur) Serveur ready (%ld)...", pidServer);
    while (1) {
        Trace("(Serveur) Chuis en attente...");
        if (msgrcv(idQ, &Msg, sizeof(Msg) - sizeof(long), SERVER_ID, 0) == -1) {
            // Si on reçoit un signal, on recommence !
            if(errno == EINTR) {
                continue;
            }
            TraceErreur("(Serveur) Erreur de msgrcv");
            Sortie(1);
        }
        Trace("");
        Trace("(Serveur) Message recu : %s", Msg.N);
        Trace("(Serveur) idP : %ld", Msg.Pid);

        switch(Msg.Requete) {
            case RECHERCHE_REQUEST:
                recherche_request(Msg);
                break;
            case IDENTIFICATION_REQUEST:
                identification_request(Msg);
                break;
            case RENT_REQUEST:
                rent_request(Msg);
                break;
            case AUTHENTICATION_FAIL:
            case AUTHENTICATION_SUCCESS:
                identification_result(Msg);
                break;
            case FIN_CLIENT:
                Trace("(Serveur) Dans FIN_CLIENT");
                removeFromConnected(Msg.Pid);
                break;
            case GET_RENTED_BOOKS:
                get_rented_books(Msg);
                break;
            case RESERVATION_REQUEST:
                reservation_request(Msg);
                break;
            case RETURN_BOOK:
                return_book(Msg);
                break;
            case RETURNED_BOOK_NOTIF:
                Msg.Type = Msg.Pid;
                if(pUserConnection = getUserByUsername(Msg.N)) {
                    Msg.Pid = getUserByUsername(Msg.N)->pidClient;
                } else {
                    Msg.Pid = 0; // User not connected
                }
                send(idQ, Msg);
                break;
            default:
                Trace("(Serveur) (WARNING) Message recu mais non traite !");
        }
    }

    Sortie(0);
}

void Sortie(int Code) {
    Trace("(Serveur) Suppression des clients...");
    for(int i = 0; i < nbUserConnected; ++i) {
        removeFromConnected(userConnected[i].pidClient);
    }
    Trace("(Serveur) Suppression de la file de msg...");
    if(msgctl(idQ, IPC_RMID, NULL)) {
        TraceErreur("(Serveur) Erreur de supression de file de msg");
    }
    Trace("(Serveur) Suppression des semaphores");
    if(semctl(idSemLocation, 0, IPC_RMID)) {
        TraceErreur("(Serveur) Erreur de suppresion du semaphore de Location");
    }
    Trace("(Serveur) Suppression des semaphores");
    if(semctl(idSemReservation, 0, IPC_RMID)) {
        TraceErreur("(Serveur) Erreur de suppresion du semaphore de Reservation");
    }
    Trace("(Serveur) Suppression des semaphores");
    if(semctl(idSemOuverture, 0, IPC_RMID)) {
        TraceErreur("(Serveur) Erreur de suppresion du semaphore de Reservation");
    }
    Trace("(Serveur) Fermeture du fichier Livres...");
    if(close(hdLivre) == -1) {
        TraceErreur("(Serveur) Erreur de fermeture du fichier de Livres");
    }
    Trace("(Serveur) Fermeture du fichier Membres...");
    if(close(hdLogin) == -1) {
        TraceErreur("(Serveur) Erreur de fermeture du fichier Membres");
    }

    unlink("data/Server.pid");

    exit(Code);
}

/**
 * Vérifie si l'utilisateur est déjà connecté
 * @param  char[] username Le nom d'utilisateur à vérifier
 * @return bool
 */
int isUsernameConnected(char username[]) {
    int i;
    for (i = 0; i < nbUserConnected; ++i) {
        if(strcmp(userConnected[i].userName, strtolower(username)) == 0 && userConnected[i].waitAuthentication == 0) {
            return 1;
        }
    }

    return 0;
}

/**
 * Vérifie si l'utilisateur est déjà connecté
 * @param  long pid Le pid du client à vérifier
 * @return bool
 */
int isPidConnected(long pid) {
    int i;
    for (i = 0; i < nbUserConnected; ++i) {
        if(userConnected[i].pidClient == pid && userConnected[i].waitAuthentication == 0) {
            return 1;
        }
    }

    return 0;
}

/**
 * Ajout du nom d'utilisateur et du pid client + waitAuthentication = 1 par défaut
 * @param  char[] userName Nom de l'utilisateur
 * @return Indice de l'utilisateur ajouté
 */
int addToConnected(char userName[], long clientPid) {
    strcpy(userConnected[nbUserConnected].userName, userName);
    userConnected[nbUserConnected].waitAuthentication = 1;
    userConnected[nbUserConnected].pidClient = clientPid;
    userConnected[nbUserConnected].pidAccesFichier = 0;
    ++nbUserConnected;

    return nbUserConnected - 1;
}

void removeFromConnected(long clientPid) {
    UserConnection *user = getUserByPid(clientPid);
    if(!user) {
        Trace("(Serveur) Impossible de supprimer le client %ld", clientPid);
        return;
    }

    Trace("\t(Serveur) Suppression du client %ld", clientPid);
    if(user->pidAccesFichier != 0) {
        Trace("\t(Serveur) Fermeture du proc AccesFichier %ld", user->pidAccesFichier);
        MESSAGE Msg;
        Msg.Type = user->pidAccesFichier;
        Msg.Requete = KILL_ACCES_FICHIER;
        Msg.Pid = pidServer;
        send(idQ, Msg);
    }
    --nbUserConnected;

    // Mettre le dernier à la place de celui qu'on enlève sauf si on enlève le dernier
    if(user != &userConnected[nbUserConnected]) {
        *user = userConnected[nbUserConnected];
    }
}

UserConnection *getUserByPid(long clientPid) {
    for (int i = 0; i < nbUserConnected; ++i) {
        if(userConnected[i].pidClient == clientPid) {
            return &userConnected[i];
        }
    }

    return NULL;
}

UserConnection *getUserByUsername(char username[MAX_USER_NAME_SIZE]) {
    for (int i = 0; i < nbUserConnected; ++i) {
        if(strcmp(userConnected[i].userName, username) == 0) {
            return &userConnected[i];
        }
    }

    return NULL;
}

int getAccesFichierPid(long clientPid) {
    UserConnection *user = getUserByPid(clientPid);
    if(!user) {
        return 0;
    }
    int idAccesFichier = user->pidAccesFichier;

    if(user->pidAccesFichier) {
        return user->pidAccesFichier;
    }

    // Création de AccesFichier
    if ((idAccesFichier = fork()) == -1) {
        TraceErreur("(Serveur) Erreur de fork(RECHERCHE)");
        Sortie(1);
    }
    if (!idAccesFichier) {
        // processus fils
        char BuffIdQ[10], BuffhdLivre[5], BuffhdLocation[5], BuffhdReservation[5];

        sprintf(BuffIdQ, "%d", idQ);
        sprintf(BuffhdLivre, "%d", hdLivre);
        sprintf(BuffhdLocation, "%d", hdLocation);
        sprintf(BuffhdReservation, "%d", hdReservation);
        execlp("build/AccesFichier", "build/AccesFichier",
            BuffIdQ, BuffhdLivre, BuffhdLocation, BuffhdReservation, user->userName, NULL);
        TraceErreur("(Serveur) Erreur de execlp(AccesFichier)");
        Sortie(1);
    }
    user->pidAccesFichier = idAccesFichier;

    return idAccesFichier;
}

void recherche_request(MESSAGE Msg) {
    Trace("(Serveur) requete : RECHERCHE_REQUEST");
    int idAccesFichier;

    // Si non connecté
    if(!isPidConnected(Msg.Pid)) {
        Trace("(Serveur) (WARNING) Utilisateur non connecte");
        // Envoi d'un signal qui va interrompre le client avec errno == EINTR
        kill(Msg.Pid, SIGUSR1);
        return;
    }

    idAccesFichier = getAccesFichierPid(Msg.Pid);
    Msg.Type = idAccesFichier;
    Trace("(Serveur) Debut de la recherche");
    send(idQ, Msg);
}

void identification_request(MESSAGE Msg) {
    int idFils;
    Trace("(Serveur) IDENTIFICATION_REQUEST : '%s'", Msg.N);

    strcpy(Msg.N, strtolower(Msg.N));

    Msg.Type = Msg.Pid;
    Msg.Pid = pidServer;
    Msg.Requete = AUTHENTICATION_FAIL;

    if(semctl(idSemOuverture, 0, GETVAL) == 0) {
        Trace("(Serveur) (WARNING) Bibliotheque fermee");
        strcpy(Msg.N, "La bibliothèque est fermée, désolé...");
        send(idQ, Msg);
        return;
    }

    // Si trop d'utilisateurs connectés
    if(nbUserConnected >= MAX_CLIENT_CONNECTIONS) {
        strcpy(Msg.N, "Trop d'utilisateurs connectés");
        Trace("(Serveur) (WARNING) Nombre d'utilisateurs maximum atteint (%d)", MAX_CLIENT_CONNECTIONS);
        send(idQ, Msg);
        return;
    }

    // Si le client est déjà connecté
    if(isPidConnected(Msg.Type)) {
        strcpy(Msg.N, "Veuillez-vous déconnecter avant de vous connecter avec un autre nom d'utilisateur");
        Trace("(Serveur) (WARNING) Session deja ouverte depuis ce client");
        send(idQ, Msg);
        return;
    }

    // Si sur déjà connecté ou en attente de connexion
    if(isUsernameConnected(Msg.N)) {
        strcpy(Msg.N, "Vous êtes déjà connecté ailleurs...");
        Trace("(Serveur) (WARNING) Utilisateur deja connecte");
        send(idQ, Msg);
        return;
    }

    // Sinon, on ajoute aux gens connectés avec un champs en attente
    addToConnected(Msg.N, Msg.Type);

    // et on vérifie via le processus login
    if ((idFils = fork()) == -1) {
        TraceErreur("(Serveur) Erreur de fork(IDENTIFICATION_REQUEST)");
        Sortie(1);
    }
    if (!idFils) {
        // processus fils
        char BuffIdQ[10], BuffHdLogin[5], BuffClientPid[10], BuffhdLocation[5], BuffhdReservation[5], BuffhdLivre[5];

        sprintf(BuffIdQ, "%d", idQ);
        sprintf(BuffHdLogin, "%d", hdLogin);
        sprintf(BuffClientPid, "%ld", Msg.Type);
        sprintf(BuffhdLocation, "%d", hdLocation);
        sprintf(BuffhdReservation, "%d", hdReservation);
        sprintf(BuffhdLivre, "%d", hdLivre);

        execlp("build/Login", "build/Login",
            BuffIdQ, BuffHdLogin, BuffClientPid, BuffhdLocation, BuffhdReservation, BuffhdLivre, Msg.N, NULL);
        TraceErreur("(Serveur) Erreur de execlp(Login)");
        Sortie(1);
    }
}

void identification_result(MESSAGE Msg) {
    // Si fail on enlève le bonhomme du tableau
    // Si success, on passe le champs waiting à false
    int i;
    long client_pid = atol(Msg.N);

    Trace("(Serveur) requete : IDENTIFICATION_RESULT (%s) : %s",
        Msg.N, Msg.Requete == AUTHENTICATION_FAIL ? "fail" : "sucess");
    if(Msg.Requete == AUTHENTICATION_FAIL) {
        removeFromConnected(client_pid);
        return;
    }

    for (i = 0; i < nbUserConnected; ++i) {
        if(userConnected[i].pidClient == client_pid) {
            userConnected[i].waitAuthentication = 0;
            return;
        }
    }
    Trace("(Serveur) (WARNING) Impossible d'authentifier le client %ld", client_pid);
    // debug(userConnected, *nbUserConnected);
}

void rent_request(MESSAGE Msg) {
    int idAccesFichier = getAccesFichierPid(Msg.Pid);

    Msg.Type = idAccesFichier;
    send(idQ, Msg);
}

void get_rented_books(MESSAGE Msg) {
    Trace("(Serveur) GET_RENTED_BOOKS");
    if(!isPidConnected(Msg.Pid)) {
        Trace("(Serveur) (WARNING) Client non connecte");
        kill(Msg.Pid, SIGUSR1);
        return;
    }

    Msg.Type = getAccesFichierPid(Msg.Pid);
    send(idQ, Msg);
}

void reservation_request(MESSAGE Msg) {
    Trace("(Serveur) RESERVATION_REQUEST");
    int numeroLivre = atoi(Msg.N), firstPlace = -1;
    UserConnection *user = getUserByPid(Msg.Pid);
    RESERVATION reservation;

    if(!isPidConnected(Msg.Pid)) {
        Trace("(Serveur) (WARNING) Client non connecte");
        kill(Msg.Pid, SIGUSR1);
        return;
    }

    // Préparation du Msg de retour au client
    Msg.Type = Msg.Pid;
    Msg.Pid = pidServer;

    lseek(hdReservation, 0, SEEK_SET);
    sem_wait(idSemReservation);
    while(read(hdReservation, &reservation, sizeof(RESERVATION))) {
        // Livre déjà réservé
        if(reservation.NumeroLivre == numeroLivre) {
            sem_signal(idSemReservation);
            Trace("(Serveur) Livre %d deja reserve par %s", reservation.NumeroLivre, reservation.Membre);
            strcpy(Msg.N, "Ce livre est déjà réservé...");
            Msg.Requete = RESERVATION_FAIL;
            send(idQ, Msg);
            return;
        }

        // Récupération du premier offset dispo pour l'ajout de réservation
        if(firstPlace == -1 && reservation.NumeroLivre == -1) {
            firstPlace = tell(hdReservation);
        }
    }

    // Ajout au fichier de réservation
    if(firstPlace == -1) {
        firstPlace = tell(hdReservation);
    }
    Trace("(Serveur) Ajout de la reservation à l'offset %ld", firstPlace);
    reservation.NumeroLivre = numeroLivre;
    strcpy(reservation.Membre, user->userName);
    write(hdReservation, &reservation, sizeof(RESERVATION));
    Msg.Requete = RESERVATION_SUCCESS;
    strcpy(Msg.N, "Livre réservé avec succès!");
    send(idQ, Msg);

    sem_signal(idSemReservation);
}

void return_book(MESSAGE Msg) {
    Trace("(Serveur) RETURN_BOOK");
    if(!isPidConnected(Msg.Pid)) {
        Trace("(Serveur) (WARNING) Client non connecte");
        kill(Msg.Pid, SIGUSR1);
        return;
    }

    Msg.Type = getAccesFichierPid(Msg.Pid);
    send(idQ, Msg);
}

void quitHandler(int signal) {
    Sortie(0);
}

void broadcastHandler(int signal) {
    int i;
    Trace("(Serveur) Broadcasting message from Admin...");
    for(i = 0; i < nbUserConnected; ++i) {
        if(userConnected[i].waitAuthentication == 0) {
            kill(userConnected[i].pidClient, SIGUSR2);
        }
    }
}

/**
 * Affiche le tableau des utilisateurs connectés
 */
void debug() {
    for (int i = 0; i < nbUserConnected; ++i) {
        Trace("\t\t(Serveur) (DEBUG) username : %s", userConnected[i].userName);
        Trace("\t\t(Serveur) (DEBUG) pidAccesFichier : %ld", userConnected[i].pidAccesFichier);
        Trace("\t\t(Serveur) (DEBUG) pidClient : %ld", userConnected[i].pidClient);
        Trace("\t\t(Serveur) (DEBUG) waitAuthentication : %d", userConnected[i].waitAuthentication);
    }
}
