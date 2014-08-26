/****************************************************************************
** Form implementation generated from reading ui file 'FenetreSaisie.ui'
**
** Created: Sun Nov 1 13:58:00 2009
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.4   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "FenetreSaisie.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <errno.h>
#include <signal.h>

#include <fcntl.h>
#include <unistd.h>

extern int idQ;
extern int becauseNotConnected;
LIVRE Livres[MAX_BOOK];
LIVRE rentedBooks[MAX_RENTED_BOOKS];
int nbLivres, pageMax = 0, page, selectedBook = -1, nbRentedBooks = 0, tooManyBooks;
/*
 *  Constructs a FenetreSaisie as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
FenetreSaisie::FenetreSaisie( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
        setName( "FenetreSaisie" );

    textTitre = new QLabel( this, "textTitre" );
    textTitre->setGeometry( QRect( 30, 5, 750, 30 ) );
    QFont textTitre_font(  textTitre->font() );
    textTitre_font.setFamily( "Helvetica [Adobe]" );
    textTitre_font.setPointSize( 18 );
    textTitre_font.setBold( TRUE );
    textTitre->setFont( textTitre_font );
    // textTitre->setAlignment( int( QLabel::AlignCenter ) );

    textNom = new QLabel( this, "textNom" );
    textNom->setGeometry( QRect( 30, 50, 59, 20 ) );

    lineSaisieNom = new QLineEdit( this, "lineSaisieNom" );
    lineSaisieNom->setGeometry( QRect( 100, 50, 220, 20 ) );

    ButtonLogin = new QPushButton( this, "ButtonLogin" );
    ButtonLogin->setGeometry( QRect( 440, 50, 85, 20 ) );
    ButtonLogin->setAutoDefault( FALSE );

    ButtonReservation = new QPushButton( this, "ButtonReservation" );
    ButtonReservation->setGeometry( QRect( 730, 420, 110, 20 ) );
    ButtonReservation->setAutoDefault( FALSE );

    ButtonTerminer = new QPushButton( this, "ButtonTerminer" );
    ButtonTerminer->setGeometry( QRect( 550, 50, 85, 20 ) );
    ButtonTerminer->setAutoDefault( FALSE );

    lineNom[0] = new QLineEdit( this, "lineNom" );
    lineNom[0]->setGeometry( QRect( 30, 110, 220, 20 ) );
    lineNom[0]->setReadOnly( TRUE );
    lineNom[1] = new QLineEdit( this, "lineNom" );
    lineNom[1]->setGeometry( QRect( 30, 135, 220, 20 ) );
    lineNom[1]->setReadOnly( TRUE );
    lineNom[2] = new QLineEdit( this, "lineNom" );
    lineNom[2]->setGeometry( QRect( 30, 160, 220, 20 ) );
    lineNom[2]->setReadOnly( TRUE );

    linePrenom[0] = new QLineEdit( this, "linePrenom" );
    linePrenom[0]->setGeometry( QRect( 260, 110, 160, 20 ) );
    linePrenom[0]->setReadOnly( TRUE );
    linePrenom[1] = new QLineEdit( this, "linePrenom" );
    linePrenom[1]->setGeometry( QRect( 260, 135, 160, 20 ) );
    linePrenom[1]->setReadOnly( TRUE );
    linePrenom[2] = new QLineEdit( this, "linePrenom" );
    linePrenom[2]->setGeometry( QRect( 260, 160, 160, 20 ) );
    linePrenom[2]->setReadOnly( TRUE );

    lineTitre[0] = new QLineEdit( this, "lineTitre" );
    lineTitre[0]->setGeometry( QRect( 430, 110, 290, 20 ) );
    lineTitre[0]->setReadOnly( TRUE );
    lineTitre[1] = new QLineEdit( this, "lineTitre" );
    lineTitre[1]->setGeometry( QRect( 430, 135, 290, 20 ) );
    lineTitre[1]->setReadOnly( TRUE );
    lineTitre[2] = new QLineEdit( this, "lineTitre" );
    lineTitre[2]->setGeometry( QRect( 430, 160, 290, 20 ) );
    lineTitre[2]->setReadOnly( TRUE );

    ButtonRendu[0] = new QPushButton( this, "ButtonRendu" );
    ButtonRendu[0]->setGeometry( QRect( 730, 110, 110, 20 ) );
    ButtonRendu[0]->setAutoDefault( FALSE );
    ButtonRendu[1] = new QPushButton( this, "ButtonRendu" );
    ButtonRendu[1]->setGeometry( QRect( 730, 135, 110, 20 ) );
    ButtonRendu[1]->setAutoDefault( FALSE );
    ButtonRendu[2] = new QPushButton( this, "ButtonRendu" );
    ButtonRendu[2]->setGeometry( QRect( 730, 160, 110, 20 ) );
    ButtonRendu[2]->setAutoDefault( FALSE );

    ButtonPrecedent = new QPushButton( this, "ButtonPrecedent" );
    ButtonPrecedent->setGeometry( QRect( 60, 365, 85, 20 ) );
    ButtonPrecedent->setAutoDefault( FALSE );

    ButtonEffacer = new QPushButton( this, "ButtonEffacer" );
    ButtonEffacer->setGeometry( QRect( 330, 365, 85, 20 ) );
    ButtonEffacer->setAutoDefault( FALSE );

    ButtonSuivant = new QPushButton( this, "ButtonSuivant" );
    ButtonSuivant->setGeometry( QRect( 600, 365, 85, 20 ) );
    ButtonSuivant->setAutoDefault( FALSE );

    lineRecherche = new QLineEdit( this, "lineRecherche" );
    lineRecherche->setGeometry( QRect( 430, 190, 290, 20 ) );

    lineNomOk[0] = new QLineEdit( this, "lineNomOk" );
    lineNomOk[0]->setGeometry( QRect( 30, 230, 220, 20 ) );
    lineNomOk[0]->setReadOnly( TRUE );
    lineNomOk[1] = new QLineEdit( this, "lineNomOk" );
    lineNomOk[1]->setGeometry( QRect( 30, 255, 220, 20 ) );
    lineNomOk[1]->setReadOnly( TRUE );
    lineNomOk[2] = new QLineEdit( this, "lineNomOk" );
    lineNomOk[2]->setGeometry( QRect( 30, 280, 220, 20 ) );
    lineNomOk[2]->setReadOnly( TRUE );
    lineNomOk[3] = new QLineEdit( this, "lineNomOk" );
    lineNomOk[3]->setGeometry( QRect( 30, 305, 220, 20 ) );
    lineNomOk[3]->setReadOnly( TRUE );
    lineNomOk[4] = new QLineEdit( this, "lineNomOk" );
    lineNomOk[4]->setGeometry( QRect( 30, 330, 220, 20 ) );
    lineNomOk[4]->setReadOnly( TRUE );

    linePrenomOk[0] = new QLineEdit( this, "linePrenomOk" );
    linePrenomOk[0]->setGeometry( QRect( 260, 230, 160, 20 ) );
    linePrenomOk[0]->setReadOnly( TRUE );
    linePrenomOk[1] = new QLineEdit( this, "linePrenomOk" );
    linePrenomOk[1]->setGeometry( QRect( 260, 255, 160, 20 ) );
    linePrenomOk[1]->setReadOnly( TRUE );
    linePrenomOk[2] = new QLineEdit( this, "linePrenomOk" );
    linePrenomOk[2]->setGeometry( QRect( 260, 280, 160, 20 ) );
    linePrenomOk[2]->setReadOnly( TRUE );
    linePrenomOk[3] = new QLineEdit( this, "linePrenomOk" );
    linePrenomOk[3]->setGeometry( QRect( 260, 305, 160, 20 ) );
    linePrenomOk[3]->setReadOnly( TRUE );
    linePrenomOk[4] = new QLineEdit( this, "linePrenomOk" );
    linePrenomOk[4]->setGeometry( QRect( 260, 330, 160, 20 ) );
    linePrenomOk[4]->setReadOnly( TRUE );

    lineTitreOk[0] = new QLineEdit( this, "lineTitreOk" );
    lineTitreOk[0]->setGeometry( QRect( 430, 230, 270, 20 ) );
    lineTitreOk[0]->setReadOnly( TRUE );
    lineTitreOk[1] = new QLineEdit( this, "lineTitreOk" );
    lineTitreOk[1]->setGeometry( QRect( 430, 255, 270, 20 ) );
    lineTitreOk[1]->setReadOnly( TRUE );
    lineTitreOk[2] = new QLineEdit( this, "lineTitreOk" );
    lineTitreOk[2]->setGeometry( QRect( 430, 280, 270, 20 ) );
    lineTitreOk[2]->setReadOnly( TRUE );
    lineTitreOk[3] = new QLineEdit( this, "lineTitreOk" );
    lineTitreOk[3]->setGeometry( QRect( 430, 305, 270, 20 ) );
    lineTitreOk[3]->setReadOnly( TRUE );
    lineTitreOk[4] = new QLineEdit( this, "lineTitreOk" );
    lineTitreOk[4]->setGeometry( QRect( 430, 330, 270, 20 ) );
    lineTitreOk[4]->setReadOnly( TRUE );

    lineReserveOk[0] = new QLineEdit( this, "lineReserveOk" );
    lineReserveOk[0]->setGeometry( QRect( 700, 230, 20, 20 ) );
    lineReserveOk[0]->setReadOnly( TRUE );
    lineReserveOk[1] = new QLineEdit( this, "lineReserveOk" );
    lineReserveOk[1]->setGeometry( QRect( 700, 255, 20, 20 ) );
    lineReserveOk[1]->setReadOnly( TRUE );
    lineReserveOk[2] = new QLineEdit( this, "lineReserveOk" );
    lineReserveOk[2]->setGeometry( QRect( 700, 280, 20, 20 ) );
    lineReserveOk[2]->setReadOnly( TRUE );
    lineReserveOk[3] = new QLineEdit( this, "lineReserveOk" );
    lineReserveOk[3]->setGeometry( QRect( 700, 305, 20, 20 ) );
    lineReserveOk[3]->setReadOnly( TRUE );
    lineReserveOk[4] = new QLineEdit( this, "lineReserveOk" );
    lineReserveOk[4]->setGeometry( QRect( 700, 330, 20, 20 ) );
    lineReserveOk[4]->setReadOnly( TRUE );

    ButtonSelectionnerOk[0] = new QPushButton( this, "ButtonSelectionnerOk" );
    ButtonSelectionnerOk[0]->setGeometry( QRect( 730, 230, 110, 20 ) );
    ButtonSelectionnerOk[0]->setAutoDefault( FALSE );
    ButtonSelectionnerOk[1] = new QPushButton( this, "ButtonSelectionnerOk" );
    ButtonSelectionnerOk[1]->setGeometry( QRect( 730, 255, 110, 20 ) );
    ButtonSelectionnerOk[1]->setAutoDefault( FALSE );
    ButtonSelectionnerOk[2] = new QPushButton( this, "ButtonSelectionnerOk" );
    ButtonSelectionnerOk[2]->setGeometry( QRect( 730, 280, 110, 20 ) );
    ButtonSelectionnerOk[2]->setAutoDefault( FALSE );
    ButtonSelectionnerOk[3] = new QPushButton( this, "ButtonSelectionnerOk" );
    ButtonSelectionnerOk[3]->setGeometry( QRect( 730, 305, 110, 20 ) );
    ButtonSelectionnerOk[3]->setAutoDefault( FALSE );
    ButtonSelectionnerOk[4] = new QPushButton( this, "ButtonSelectionnerOk" );
    ButtonSelectionnerOk[4]->setGeometry( QRect( 730, 330, 110, 20 ) );
    ButtonSelectionnerOk[4]->setAutoDefault( FALSE );

    textSaisie = new QLabel( this, "textSaisie" );
    textSaisie->setGeometry( QRect( 30, 190, 390, 20 ) );

    textMessage = new QTextEdit( this, "textMessage" );
    textMessage->setGeometry( QRect( 30, 420, 685, 58 ) );
    textMessage->setReadOnly( TRUE );

    textInformation = new QLabel( this, "textInformation" );
    textInformation->setGeometry( QRect( 30, 390, 80, 20 ) );
    QFont textInformation_font(  textInformation->font() );
    textInformation_font.setFamily( "Helvetica [Adobe]" );
    textInformation_font.setBold( TRUE );
    textInformation->setFont( textInformation_font );

    textSelection = new QLabel( this, "textSelection" );
    textSelection->setGeometry( QRect( 30, 80, 180, 20 ) );
    languageChange();
    resize( QSize(850, 500).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( ButtonLogin, SIGNAL( clicked() ), this, SLOT( Login() ) );
    connect( ButtonReservation, SIGNAL( clicked() ), this, SLOT( Reservation() ) );
    connect( lineSaisieNom, SIGNAL( returnPressed() ), this, SLOT( Login() ) );
    connect( ButtonEffacer, SIGNAL( clicked() ), this, SLOT( Effacer() ) );
    connect( ButtonTerminer, SIGNAL( clicked() ), this, SLOT( Terminer() ) );
    connect( ButtonPrecedent, SIGNAL( clicked() ), this, SLOT( Precedent() ) );
    connect( ButtonSuivant, SIGNAL( clicked() ), this, SLOT( Suivant() ) );
    connect( lineRecherche, SIGNAL( returnPressed() ), this, SLOT( Recherche() ) );
    connect( ButtonRendu[0], SIGNAL( clicked() ), this, SLOT( Rendu0() ) );
    connect( ButtonRendu[1], SIGNAL( clicked() ), this, SLOT( Rendu1() ) );
    connect( ButtonRendu[2], SIGNAL( clicked() ), this, SLOT( Rendu2() ) );
    connect( ButtonSelectionnerOk[0], SIGNAL( clicked() ), this, SLOT( SelectionnerOk0() ) );
    connect( ButtonSelectionnerOk[1], SIGNAL( clicked() ), this, SLOT( SelectionnerOk1() ) );
    connect( ButtonSelectionnerOk[2], SIGNAL( clicked() ), this, SLOT( SelectionnerOk2() ) );
    connect( ButtonSelectionnerOk[3], SIGNAL( clicked() ), this, SLOT( SelectionnerOk3() ) );
    connect( ButtonSelectionnerOk[4], SIGNAL( clicked() ), this, SLOT( SelectionnerOk4() ) );
}

/*
 *  Destroys the object and frees any alrented resources
 */
FenetreSaisie::~FenetreSaisie() {
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void FenetreSaisie::languageChange() {
    setCaption( trUtf8( "Bibliothèque Inpres" ) );
    textTitre->setText( trUtf8( "Bibliothèque Arcangelo, Benjamin & Co" ) );
    textNom->setText( tr( "Nom :" ) );
    ButtonLogin->setText( tr( "Login" ) );
    ButtonReservation->setText( trUtf8( "Réservation" ) );
    ButtonPrecedent->setText( trUtf8( "Précédent" ) );
    ButtonEffacer->setText( tr( "Effacer" ) );
    ButtonSuivant->setText( tr( "Suivant" ) );
    ButtonRendu[0]->setText( tr( "Rendu" ) );
    ButtonRendu[1]->setText( tr( "Rendu" ) );
    ButtonRendu[2]->setText( tr( "Rendu" ) );
    ButtonSelectionnerOk[0]->setText( trUtf8( "Sélectionner" ) );
    ButtonSelectionnerOk[1]->setText( trUtf8( "Sélectionner" ) );
    ButtonSelectionnerOk[2]->setText( trUtf8( "Sélectionner" ) );
    ButtonSelectionnerOk[3]->setText( trUtf8( "Sélectionner" ) );
    ButtonSelectionnerOk[4]->setText( trUtf8( "Sélectionner" ) );
    ButtonTerminer->setText( tr( "Terminer" ) );
    textSaisie->setText( trUtf8( "Rechercher (Entrez le numéro d'un livre ou le nom d'un auteur)" ) );
    textInformation->setText( tr( "Information:" ) );
    textSelection->setText( trUtf8( "Vous êtes en possession de :" ) );
}

void FenetreSaisie::Rendu0() {
    Rendu(0);
}

void FenetreSaisie::Rendu1() {
    Rendu(1);
}

void FenetreSaisie::Rendu2() {
    Rendu(2);
}

void FenetreSaisie::SelectionnerOk0() {
    SelectionnerOk(0);
}

void FenetreSaisie::SelectionnerOk1() {
    SelectionnerOk(1);
}

void FenetreSaisie::SelectionnerOk2() {
    SelectionnerOk(2);
}

void FenetreSaisie::SelectionnerOk3() {
    SelectionnerOk(3);
}

void FenetreSaisie::SelectionnerOk4() {
    SelectionnerOk(4);
}

void FenetreSaisie::Login() {
    Trace( "(QtDossier2013) Dans Login()" );
    MESSAGE Msg;
    MESSAGELIVRE MsgLivre;

    Msg.Type = SERVER_ID;
    Msg.Requete = IDENTIFICATION_REQUEST;
    Msg.Pid = getpid();
    strcpy(Msg.N, lineSaisieNom->text());

    Trace("(QtDossier2013) Envoi de la demande au serveur");
    Trace("(QtDossier2013) Sizeof Msg %d", sizeof(Msg));
    if ((msgsnd(idQ, &Msg, sizeof(Msg) - sizeof(long), 0)) == -1) {
        TraceErreur("(QtDossier2013) Erreur de msgsnd()...");
        exit(1);
    }

    Trace("(QtDossier2013) Attente de la reponse du serveur");
    if ((msgrcv(idQ, &Msg, sizeof(Msg) - sizeof(long), getpid(), 0)) == -1) {
        TraceErreur("(QtDossier2013) Erreur de msgrcv()...");
        exit(1);
    }

    Trace("(QtDossier2013) Reponse du serveur : %d", Msg.Requete);

    switch(Msg.Requete) {
        case AUTHENTICATION_FAIL:
            Trace("(QtDossier2013) Erreur de connexion generique");
            textMessage->setText(trUtf8(Msg.N));
            return;
        case -1:
            Trace("(QtDossier2013) User inexistant");
            textMessage->setText(trUtf8("Nom d'utilisateurs incorrect"));
            return;
        case 0:
            Trace("(QtDossier2013) Connexion reussie");
            textMessage->setText(trUtf8("Connexion réussie"));
            RefreshRentedBooks();
            return;
        default:
            Trace("(QtDossier2013) Attente des livres");
            if ((msgrcv(idQ, &MsgLivre, sizeof(MsgLivre) - sizeof(long), getpid(), 0)) == -1) {
                TraceErreur("(QtDossier2013) Erreur de msgrcv()...");
                exit(1);
            }
            Trace("(QtDossier2013) Livre en attente...");
            BookAvailableNotification(MsgLivre.L, MsgLivre.Requete);
            RefreshRentedBooks();
            break;
    }


}

void FenetreSaisie::Terminer() {
    Trace( "(QtDossier2013) Dans Terminer()" );
    MESSAGE Msg;

    Msg.Type = SERVER_ID;
    Msg.Requete = FIN_CLIENT;
    Msg.Pid = getpid();
    Msg.N[0] = 0;
    if ((msgsnd(idQ, &Msg, sizeof(Msg) - sizeof(long), 0)) == -1) {
        TraceErreur("(QtDossier2013) Erreur de msgsnd()...");
        exit(1);
    }
    textMessage->setText("");
    lineSaisieNom->setText("");
    lineRecherche->setText("");
    nbRentedBooks = 0;
    nbLivres = 0;

    // Vidage des livres empruntés
    for(int i = 0; i < MAX_RENTED_BOOKS; ++i) {
        lineNom[i]   ->setText("");
        linePrenom[i]->setText("");
        lineTitre[i] ->setText("");
    }

    Effacer();
}


void FenetreSaisie::Rendu(int i) {
    Trace("(QtDossier2013) Livre rendu (%d)", i);
    MESSAGE Msg;

    if(i >= nbRentedBooks) {
        return;
    }

    Msg.Type = SERVER_ID;
    Msg.Requete = RETURN_BOOK;
    Msg.Pid = getpid();
    sprintf(Msg.N, "%d", rentedBooks[i].Numero);

    if ((msgsnd(idQ, &Msg, sizeof(Msg) - sizeof(long), 0)) == -1) {
        perror("(QtDossier2013) Erreur de msgsnd()...");
        exit(1);
    }

    if ((msgrcv(idQ, &Msg, sizeof(Msg) - sizeof(long), getpid(), 0)) == -1) {
        if(errno == EINTR && causedByNotConnected()) {
            return;
        }
        TraceErreur("(QtDossier2013) Erreur de msgrcv()...");
        exit(1);
    }

    textMessage->setText(trUtf8(Msg.N));
    // Recherche();
    RefreshRentedBooks();
}

void FenetreSaisie::Recherche() {
    EmptyPage();
    // Effacement du texte d'information affichant "Trop de résultats"
    if (tooManyBooks) {
        textMessage->setText("");
        tooManyBooks = 0;
    }

    Trace("(QtDossier2013) Dans Rechercher()");
    MESSAGE      M;
    MESSAGELIVRE M1;
    int i;

    if(strlen(lineRecherche->text()) == 0) {
        nbLivres = 0;
        pageMax = 0;
        return;
    }
    strcpy(M.N, lineRecherche->text());
    M.Type = SERVER_ID;
    M.Requete = RECHERCHE_REQUEST;
    M.Pid = getpid();

    if ((msgsnd(idQ, &M, sizeof(M) - sizeof(long), 0)) == -1) {
        perror("(QtDossier2013) Erreur de msgsnd()...");
        exit(1);
    }

    if ((msgrcv(idQ, &M1, sizeof(M1) - sizeof(long), getpid(), 0)) == -1) {
        if(errno == EINTR && causedByNotConnected()) {
            return;
        }
        TraceErreur("(QtDossier2013) Erreur de msgrcv()...");
        exit(1);
    }

    if (M1.Requete == -1) {
        Trace("(QtDossier2013) Trop de resultats");
        textMessage->setText(trUtf8("Trop de résultats: veuillez affiner la recherche"));
        tooManyBooks = 1;
        return;
    }

    nbLivres = M1.Requete; // nbLivres, page, pageMax et Livres[] sont des variables globales
    page = 0;
    pageMax = nbLivres/5 - 1;
    if (nbLivres%5)
        ++pageMax;
    memcpy(Livres, M1.L, sizeof(LIVRE) * nbLivres);

    Trace("(QtDossier2013) Reponse recue: nombre de livres trouves: %d", nbLivres);
    Trace("(QtDossier2013) Nombre de pages: %d", pageMax);

    DisplayPage(0);
    Trace("(QtDossier2013) Page actuelle: %d/%d", page, pageMax);
}

void FenetreSaisie::SelectionnerOk(int i) {
    Trace("(QtDossier2013) Dans SelectionnerOk : %d", i);
    int currentBook = i + (page * 5);
    MESSAGE Msg;

    if(currentBook >= nbLivres) {
        return;
    }
    Trace("(QtDossier2013) Location du livre %d", Livres[currentBook].Numero);

    selectedBook = currentBook;
    Msg.Type = SERVER_ID;
    Msg.Requete = RENT_REQUEST;
    Msg.Pid = getpid();
    sprintf(Msg.N, "%d", Livres[currentBook].Numero);

    if ((msgsnd(idQ, &Msg, sizeof(Msg) - sizeof(long), 0)) == -1) {
        perror("(QtDossier2013) Erreur de msgsnd()...");
        exit(1);
    }

    if ((msgrcv(idQ, &Msg, sizeof(Msg) - sizeof(long), getpid(), 0)) == -1) {
        if(errno == EINTR && causedByNotConnected()) {
            return;
        }
        TraceErreur("(QtDossier2013) Erreur de msgrcv()...");
        exit(1);
    }

    switch (Msg.Requete) {
        case RENT_SUCCESS:
            textMessage->setText(trUtf8("Le livre a été emprunté avec succès..."));
            // On réinitialise le livre sélectionné
            selectedBook = -1;
            // refresh des livres empruntés
            RefreshRentedBooks();
            lineReserveOk[i]->setText(" ");
            break;
        case ALREADY_RENTED:
            textMessage->setText(trUtf8("Le livre est déjà loué, cliquez sur 'Réservation' pour le réserver."));
            break;
        case TOO_MANY_RENTS:
            textMessage->setText(trUtf8("Vous avez déjà emprunté 3 livres..."));
            break;
        case ALREADY_RESERVED:
            textMessage->setText(trUtf8("Ce livre est réservé par quelqu'un d'autre..."));
            break;
    }
}

void FenetreSaisie::Precedent() {
    Trace( "(QtDossier2013) Dans Precedent()" );
    if (!pageMax || page < 1) {
        Trace("(QtDossier2013) Impossible d'aller a la page precedente");
        return;
    }
    DisplayPage(--page);
    Trace("(QtDossier2013) Page actuelle: %d/%d", page, pageMax);
}

void FenetreSaisie::Suivant() {
    Trace( "(QtDossier2013) Dans Suivant()" );
    if (!pageMax || page >= pageMax) {
        Trace("(QtDossier2013) Impossible d'aller a la page suivante");
        return;
    }
    EmptyPage();
    DisplayPage(++page);
    Trace("(QtDossier2013) Page actuelle: %d/%d", page, pageMax);
}

void FenetreSaisie::Effacer() {
    Trace("(QtDossier2013) Dans Effacer()");

    EmptyPage();
    nbLivres = 0;
    pageMax = 0;
    selectedBook = -1;
}

void FenetreSaisie::Reservation() {
    Trace( "(QtDossier2013) Dans Reservation()" );
    if(selectedBook == -1) {
        textMessage->setText(trUtf8("Sélectionnez un livre à réserver d'abord..."));
        return;
    }

    int i;
    for(i = 0; i < nbRentedBooks; ++i) {
        if(rentedBooks[i].Numero == Livres[selectedBook].Numero) {
            selectedBook = -1;
            textMessage->setText(trUtf8("Vous avez déjà loué ce livre..."));
            return;
        }
    }

    MESSAGE Msg;
    Msg.Pid = getpid();
    Msg.Type = SERVER_ID;
    Msg.Requete = RESERVATION_REQUEST;
    sprintf(Msg.N, "%d", Livres[selectedBook].Numero);

    if ((msgsnd(idQ, &Msg, sizeof(Msg) - sizeof(long), 0)) == -1) {
        perror("(QtDossier2013) Erreur de msgsnd()...");
        exit(1);
    }

    if ((msgrcv(idQ, &Msg, sizeof(Msg) - sizeof(long), getpid(), 0)) == -1) {
        if(errno == EINTR && causedByNotConnected()) {
            return;
        }
        TraceErreur("(QtDossier2013) Erreur de msgrcv()...");
        exit(1);
    }

    if(Msg.Requete == RESERVATION_SUCCESS) {
        selectedBook = -1;
    }
    textMessage->setText(trUtf8(Msg.N));
}

void FenetreSaisie::DisplayPage(int page) {
    int line, i;

    if (!nbLivres) {
        Trace("(QtDossier2013) Aucun livre a afficher");
        return;
    }

    for (i = page * 5, line = 0; i < nbLivres && line < 5; ++i, ++line) {
        lineNomOk[line]->setText(Livres[i].NomAuteur);
        linePrenomOk[line]->setText(Livres[i].PrenomAuteur);
        lineTitreOk[line]->setText(Livres[i].Titre);
        if (Livres[i].Libre) {
            lineReserveOk[line]->setText("X");
        } else {
            lineReserveOk[line]->setText(" ");
        }
    }
}

void FenetreSaisie::EmptyPage() {
    int i;

    for(i = 0; i < 5; ++i) {
        lineNomOk[i]->setText("");
        linePrenomOk[i]->setText("");
        lineTitreOk[i]->setText("");
        lineReserveOk[i]->setText("");
    }
}

void FenetreSaisie::RefreshRentedBooks() {
    Trace("(QtDossier2013) Rafraichissement des livres en cours de location");
    MESSAGE Msg;
    MESSAGELIVRE MsgLivre;
    Msg.Type = SERVER_ID;
    Msg.Pid = getpid();
    Msg.Requete = GET_RENTED_BOOKS;
    Msg.N[0] = 0;

    if ((msgsnd(idQ, &Msg, sizeof(Msg) - sizeof(long), 0)) == -1) {
        perror("(QtDossier2013) Erreur de msgsnd()...");
        exit(1);
    }

    if ((msgrcv(idQ, &MsgLivre, sizeof(MsgLivre) - sizeof(long), getpid(), 0)) == -1) {
        if(errno == EINTR && causedByNotConnected()) {
            return;
        }
        TraceErreur("(QtDossier2013) Erreur de msgrcv()...");
        exit(1);
    }

    nbRentedBooks = MsgLivre.Requete;
    memcpy(rentedBooks, MsgLivre.L, sizeof(LIVRE) * nbRentedBooks);

    Trace("(QtDossier2013) Affichage de %d livres", nbRentedBooks);
    int i;
    for(i = 0; i < nbRentedBooks; ++i) {
        lineNom[i]   ->setText(MsgLivre.L[i].NomAuteur);
        linePrenom[i]->setText(MsgLivre.L[i].PrenomAuteur);
        lineTitre[i] ->setText(MsgLivre.L[i].Titre);
    }
    // Nettoyage des lignes vides
    for(; i < MAX_RENTED_BOOKS; ++i) {
        lineNom[i]   ->setText("");
        linePrenom[i]->setText("");
        lineTitre[i] ->setText("");
    }
}

void FenetreSaisie::BookAvailableNotification(LIVRE Livres[], int nbLivres) {
    char buffer1[300], buffer2[300];

    sprintf(buffer1, "\"%s\" de %s", Livres[0].Titre, Livres[0].NomAuteur);
    for(int i = 1; i < nbLivres; ++i) {
        sprintf(buffer1, "%s, \"%s\" de %s", buffer1, Livres[i].Titre, Livres[i].NomAuteur);
    }

    if(nbLivres == 1) {
        sprintf(buffer2, "Le livre %s est disponible!", buffer1);
    } else {
        sprintf(buffer2, "Les livres %s sont disponibles!", buffer1);
    }
    textMessage->setText(buffer2);
}

int FenetreSaisie::causedByNotConnected() {
    int tmp = becauseNotConnected;
    Trace("(QtDossier2013) Utilisateur non connecte");
    // reset de becauseNotConnected
    becauseNotConnected = 0;
    return tmp;
}

void FenetreSaisie::setMessage(const char message[]) {
    textMessage->setText(message);
}
