#/usr/local/qt/bin/uic FenetreSaisie.ui -o FenetreSaisie.h
#/usr/local/qt/bin/uic FenetreSaisie.ui -i FenetreSaisie.h -o FenetreSaisie.cpp
echo creation de FenetreSaisie.o
g++ -c FenetreSaisie.cpp -I /usr/local/qt/include
#/usr/local/qt/bin/moc FenetreSaisie.h -o moc_FenetreSaisie.cpp
echo creation de moc_FenetreSaisie.o
g++ -c moc_FenetreSaisie.cpp -I /usr/local/qt/include
echo creation de Ecran.O
g++ -c Ecran.cpp -D SUN
echo creation de QtDossier2013
g++ -I /usr/local/qt/include -I /usr/local/qt/mkspec/default -o QtDossier2013 Main.cpp FenetreSaisie.o moc_FenetreSaisie.o Ecran.o -lqt
echo Creation de Serveur
g++ -o Serveur Serveur.cpp  Ecran.o
echo creation de AccesFichier
g++ -o AccesFichier AccesFichier.cpp Ecran.o 
