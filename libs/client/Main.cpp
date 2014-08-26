#include <qapplication.h>
#include "FenetreSaisie.h"

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "Fichier.ini"
#include "Ecran.h"

int idQ;
// Sers à tester si l'interruption de réception d'un message est
// du au fait que l'utilisateur n'est pas connecté
int becauseNotConnected = 0;

FenetreSaisie *F1;

void quitHandler(int);
void notConnectedHandler(int sig);
void bookReturnedHandler(int sig);
void broadcastHandler(int sig);

int idShm;

int main(int argc, char *argv[])
{
    struct sigaction sigAction;

    Trace("(Main) Debut de l'application (%ld)", getpid());
    if ((idQ = msgget(IPC_KEY,0)) == -1) {
        TraceErreur("(Main) Erreur de recuperation de la file");
        exit(1);
    }

    sigAction.sa_handler = quitHandler;
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = 0;

    Trace("(Main) Armement de SIGINT (quitHandler)");
    if(sigaction(SIGINT, &sigAction, NULL) == -1) {
        TraceErreur("(Main) Erreur d'armement de SIGINT");
        exit(1);
    }

    Trace("(Main) Armement de SIGQUIT (quitHandler)");
    if(sigaction(SIGQUIT, &sigAction, NULL) == -1) {
        TraceErreur("(Main) Erreur d'armement de SIGQUIT");
        exit(1);
    }

    Trace("(Main) Armement de SIGUSR1 (notConnectedHandler)");
    sigAction.sa_handler = notConnectedHandler;
    if(sigaction(SIGUSR1, &sigAction, NULL) == -1) {
        TraceErreur("(Main) Erreur d'armement de SIGUSR1");
        exit(1);
    }

    Trace("(Main) Armement de SIGPWR (bookReturnedHandler)");
    sigAction.sa_handler = bookReturnedHandler;
    if(sigaction(SIGPWR, &sigAction, NULL) == -1) {
        TraceErreur("(Main) Erreur d'armement de SIGPWR");
        exit(1);
    }

    Trace("(Main) Armement de SIGUSR2 (broadcastHandler)");
    sigAction.sa_handler = broadcastHandler;
    if(sigaction(SIGUSR2, &sigAction, NULL) == -1) {
        TraceErreur("(Main) Erreur d'armement de SIGUSR2");
        exit(1);
    }

    Trace("(Main) Recuperation de la memoire partagee");
    if ((idShm = shmget(SHARED_MEM_KEY, SHARED_MEM_SIZE, 0)) == 0) {
        TraceErreur("Erreur de shmget()");
    }

    QApplication appl(argc,argv);

    F1 = new FenetreSaisie();
    F1->show();

    return appl.exec();
}

void quitHandler(int signal) {
    F1->Terminer();
    delete F1;
    exit(0);
}

void notConnectedHandler(int sig) {
    // Si c'était une méthode de FenetreSaisie, ce serait plus pratique...
    Trace("(Main) notConnectedHandler");
    becauseNotConnected = 1;
}

void bookReturnedHandler(int sig) {
    MESSAGELIVRE Msg;
    if ((msgrcv(idQ, &Msg, sizeof(MESSAGELIVRE) - sizeof(long), getpid(), 0)) == -1) {
        TraceErreur("(Main) Erreur de msgrcv()...");
        exit(1);
    }
    Trace("(Main) %d livres recus...", Msg.Requete);
    F1->BookAvailableNotification(Msg.L, Msg.Requete);
}

void broadcastHandler(int sig) {
    char *message;
    char buff[SHARED_MEM_SIZE];

    Trace("(Main) broadcast handler");
    message = (char*) shmat(idShm, NULL, SHM_RDONLY);
    Trace("(Main) shmat reussi");
    strcpy(buff, message);
    Trace("(Main) Message copie");
    F1->setMessage(F1->trUtf8(buff));
    shmdt(message);
}
