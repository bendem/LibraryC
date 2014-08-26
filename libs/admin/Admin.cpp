#include "Admin.h"

int main() {
    char pass[MAX_PASSWORD_SIZE];
    int idSemOuverture, idShm, hdServerPid, notConnected, choix;
    pid_t server_id;

    do {
        strcpy(pass, getpass("Mot de passe administrateur: "));
        if(notConnected = strcmp(pass, ADMIN_PASSWORD)) {
            Trace("Mauvais mot de passe.");
        }
    } while(notConnected);

    Trace("\tRecuperation du semaphore 'Ouverture'");
    if ((idSemOuverture = semget(SEM_OPEN_KEY, 1, 0)) == -1) {
        TraceErreur("\tErreur de semget()");
        Sortie(1, idShm);
    }

    // On sait que le PID est à jour dans le fichier, car l'admin ne se lance pas si le server n'est pas lancé.
    // Et si le server est lancé, le pid dans le fichier est à jour.
    Trace("\tRecuperation du pid du serveur");
    if ((hdServerPid = open("data/Server.pid", O_RDONLY)) == -1) {
        TraceErreur("\t(Serveur) Erreur d'ouverture du fichier data/Server.pid");
        Sortie(1, idShm);
    }
    read(hdServerPid, &server_id, sizeof(pid_t));
    close(hdServerPid);

    Trace("\tCreation d'une memoire partagee");
    if ((idShm = shmget(SHARED_MEM_KEY, sizeof(char) * SHARED_MEM_SIZE, IPC_CREAT | IPC_EXCL | 0600)) == 0) {
        TraceErreur("Erreur de shmget(%d)", errno);
        Sortie(1, idShm);
    }

    while(1) {
        choix = menu();
        switch(choix) {
            case 0:
                Sortie(0, idShm);
            case 1:
                fermer_bibli(idSemOuverture);
                break;
            case 2:
                ouvrir_bibli(idSemOuverture);
                break;
            case 3:
                broadcast(server_id, idShm);
                break;
        }
    }

    return 0;
}

int menu() {
    int choix;
    system("clear");
    Trace("-------------------------------");
    Trace("             MENU");
    Trace("-------------------------------");
    Trace("1. Fermer la bibliotheque");
    Trace("2. Ouvrir la bibliotheque");
    Trace("3. Broadcast");
    Trace("0. Quitter");
    scanf("%d", &choix);
    return choix;
}

void fermer_bibli(int idSemOuverture) {
    if(semctl(idSemOuverture, 0, GETVAL) == 0) {
        Trace("Bibliotheque deja fermee...");
        return;
    }

    sem_wait(idSemOuverture);
}

void ouvrir_bibli(int idSemOuverture) {
    if(semctl(idSemOuverture, 0, GETVAL) == 1) {
        Trace("Bibliotheque deja ouverte...");
        return;
    }

    sem_signal(idSemOuverture);
}

void broadcast(pid_t server_id, int idShm) {
    char *message;
    char buff[SHARED_MEM_SIZE];

    message = (char*) shmat(idShm, NULL, 0);

    Trace("\nMessage a diffuser: ");
    clean_stdin();
    fgets(buff, SHARED_MEM_SIZE, stdin);
    strcpy(message, buff);
    shmdt(message);

    kill(server_id, SIGUSR2);
}

void clean_stdin() {
    int c;

    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

void Sortie(int status, int idShm) {
    shmctl(idShm, IPC_RMID, 0);
    exit(status);
}
