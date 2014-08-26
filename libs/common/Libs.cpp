#include "Libs.h"

/**
 * Transforme une chaine de caractères en minuscule
 * @param  string La chaine à transformer
 * @return char*
 */
char *strtolower(char string[]) {
    int i;
    for(i = 0; i < strlen(string); i++) {
        string[i] = tolower(string[i]);
    }
    return string;
}

/**
 * Envoie le message Msg sur le file de message correspondant à la variable globale idQ
 * @param int     idQ Id de la file de message
 * @param MESSAGE Msg Message à envoyer
 */
void send(int idQ, MESSAGE Msg) {
    if (msgsnd(idQ, &Msg, sizeof(Msg) - sizeof(long), 0) == -1) {
        TraceErreur("(Serveur) Erreur de msgsnd()");
        exit(1);
    }
}

void sem_wait(int idSem) {
    struct sembuf semBuffLoc;
    semBuffLoc.sem_num = 0;
    semBuffLoc.sem_flg = 0;
    semBuffLoc.sem_op = -1;
    semop(idSem, &semBuffLoc, 1);
}
void sem_signal(int idSem) {
    struct sembuf semBuffLoc;
    semBuffLoc.sem_num = 0;
    semBuffLoc.sem_flg = 0;
    semBuffLoc.sem_op = 1;
    semop(idSem, &semBuffLoc, 1);
}
