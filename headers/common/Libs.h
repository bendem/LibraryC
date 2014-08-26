#ifndef LIBS_H
#define LIBS_H

#include <string.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include "Fichier.ini"
#include "Ecran.h"

char *strtolower(char[]);
void send(int, MESSAGE);
void sem_wait(int idSem);
void sem_signal(int idSem);

#endif
