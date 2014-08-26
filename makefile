# -----------------------------------
#             Variables
# -----------------------------------
OUT = build
CONFIG = config
LIBS = libs
HEADERS = headers
CLIENT = $(LIBS)/client
SERVER = $(LIBS)/server
COMMON = $(LIBS)/common
ADMIN  = $(LIBS)/admin
CLIENT_H = $(HEADERS)/client
SERVER_H = $(HEADERS)/server
COMMON_H = $(HEADERS)/common
ADMIN_H  = $(HEADERS)/admin

# SUN command line
COMP = g++ -I /usr/local/qt/include -I $(CONFIG) -D SUN
# Ben's command line
# COMP = g++ -I /usr/local/qt/include -I /usr/local/qt/include/Qt -I $(CONFIG) -D SUN

COMP_COMMON = $(COMP) -I $(COMMON_H)
COMP_CLIENT = $(COMP_COMMON) -I $(CLIENT_H)
COMP_SERVER = $(COMP_COMMON) -I $(SERVER_H)
COMP_ADMIN  = $(COMP_COMMON) -I $(ADMIN_H)

.SILENT:

# -----------------------------------
#             Programmes
# -----------------------------------
all: $(OUT)/Client $(OUT)/Server $(OUT)/Admin

$(OUT)/Server: $(SERVER)/Serveur.cpp $(SERVER_H)/Serveur.h $(COMMON)/Ecran.o $(COMMON)/Libs.o $(OUT)/AccesFichier $(OUT)/Login
	echo Compilation du $@
	$(COMP_SERVER) -o $@ $< $(COMMON)/Ecran.o $(COMMON)/Libs.o

$(OUT)/Client: $(CLIENT)/Main.cpp $(CLIENT)/FenetreSaisie.o $(CLIENT)/moc_FenetreSaisie.o $(COMMON)/Ecran.o
	echo Compilation du $@
	$(COMP_CLIENT) -o $@ -I /usr/local/qt/mkspec/default $^ -lqt

$(OUT)/LitLivre: $(COMMON)/LitLivre.cpp $(COMMON_H)/Livre.h
	echo Compilation de $@
	$(COMP_COMMON) -o $@ $<

$(OUT)/AccesFichier: $(SERVER)/AccesFichier.cpp $(COMMON)/Ecran.o $(COMMON)/Libs.o
	echo Compilation de AccesFichier
	$(COMP_SERVER) -o $@ $^

$(OUT)/Login: $(SERVER)/Login.cpp $(COMMON)/Ecran.o $(COMMON)/Libs.o
	echo Compilation de $@
	$(COMP_SERVER) -o $@ $^

$(OUT)/Admin: $(ADMIN)/Admin.cpp $(ADMIN_H)/Admin.h $(COMMON)/Ecran.o $(COMMON)/Libs.o
	echo Compilation de $@
	$(COMP_ADMIN) -o $@ $^

# -----------------------------------
#             Objets client
# -----------------------------------
$(CLIENT)/FenetreSaisie.o: $(CLIENT)/FenetreSaisie.cpp $(CLIENT_H)/FenetreSaisie.h $(CONFIG)/Fichier.ini
	echo Compilation de FenetreSaisie.o
	$(COMP_CLIENT) -o $@ -c $(CLIENT)/FenetreSaisie.cpp

$(CLIENT)/moc_FenetreSaisie.o: $(CLIENT)/moc_FenetreSaisie.cpp $(CLIENT_H)/FenetreSaisie.h
	echo Compilation de moc_FenetreSaisie.o
	$(COMP_CLIENT) -o $@ -c $<

# -----------------------------------
#             Objets serveur
# -----------------------------------
Livre.o:
	echo Pas encore pret...

# -----------------------------------
#             Objets communs
# -----------------------------------
$(COMMON)/Libs.o: $(COMMON)/Libs.cpp $(COMMON_H)/Libs.h
	echo Compilation de $@
	$(COMP_COMMON) -o $@ -c $<

$(COMMON)/Ecran.o: $(COMMON)/Ecran.cpp
	echo Compilation de $@
	$(COMP_COMMON) -o $@ -c $<

# -----------------------------------
#             Divers
# -----------------------------------
clean:
	rm -rf $(OUT)/*
	rm -rf $(COMMON)/*.o
	rm -rf $(SERVER)/*.o
	rm -rf $(CLIENT)/*.o
	rm -rf $(ADMIN)/*.o
