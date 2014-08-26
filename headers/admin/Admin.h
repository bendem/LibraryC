#ifndef ADMIN_h
#define ADMIN_h

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "Ecran.h"
#include "Fichier.ini"
#include "Libs.h"

int menu();
void fermer_bibli(int);
void ouvrir_bibli(int);
void broadcast(pid_t, int);
void clean_stdin();
void Sortie(int, int);

#endif
