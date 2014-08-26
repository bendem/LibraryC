#ifndef SERVEUR_H
#define SERVEUR_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>

#include "Ecran.h"
#include "Libs.h"
//#include "FichierIndex.h"

#include "Fichier.ini"


typedef struct {
    char userName[MAX_USER_NAME_SIZE];
    long pidAccesFichier;
    long pidClient;
    char waitAuthentication;
} UserConnection;

// Gestion UserConnection[]
UserConnection *getUserByPid(long);
UserConnection *getUserByUsername(char[MAX_USER_NAME_SIZE]);
int  isUsernameConnected(char[]);
int  isPidConnected     (long);
int  addToConnected     (char[], long);
void removeFromConnected(long);
int  getAccesFichierPid (long);

// Requêtes
void recherche_request     (MESSAGE);
void rent_request          (MESSAGE);
void reservation_request   (MESSAGE);
void identification_request(MESSAGE Msg);
void identification_result (MESSAGE Msg);
void get_rented_books      (MESSAGE Msg);
void return_book           (MESSAGE Msg);

// Divers
void Sortie          (int);
void quitHandler     (int);
void broadcastHandler(int signal);
void debug           ();

#endif
