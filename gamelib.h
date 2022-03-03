#include <stdio.h>
#include<stdlib.h>
#include <string.h>

//FUNZIONE CHE MI RICHIAMA LE FUNZIONI PER CREARE E STAMPARE LA MAPPA
void crea_mappa();
//FUNZIONE CHE FA INIZIALE LA PARTITA
void gioca ();
//STAMPA LE STATISTICHE DI VITTORIA DEI GIOCATORI E CHIUDE IL PROGRAMMA
void termina_gioco();
//SPIEGA IL GIOCO NEI MINIMI PARTICOLARI
void istruzioni();
//PERMETTE DI STAMPARE E DI FARMI ACCEDERE PIÙ FACILMENTE DURANTE IL PROGRAMMA AL MENU PRINCIPALE
void inizio();

//ENUM PER GESTIRE PIÙ SEMPLICEMENTE I VALORI CHE POSSONO ASSUMERE LE VARIABILI DI QUESTE TIPOLOGIE
  enum Stato_giocatore{vita,vulnerabile,scudo_vita,scudo};
  enum Tipo_pericolo{nessun_pericolo, trappola, alieno};
  enum Tipo_oggetto{nessuno,medikit,pozione,materiale,colpi_lanciarazzi};
  enum Tipo_animale{nessun_animale,cane,pantera};
//DEFINIZIONE DELLA STRUTTURA DEL GIOGATORE
struct Giocatore {
  enum Stato_giocatore stato;
  int posizionex;
  int posizioney;
  int zaino[4];
  int alieni_uccisi;
  int coraggio;
  enum Tipo_animale anim;
  int vittorie;
};
//DEFINIZIONE DELLA STRUTTURA DEI PERICOLI
struct Pericolo{
  int probabilita;
  enum Tipo_pericolo pericolo;
};
//DEFINIZIONE DELLA STRUTTURA DEGLI OGGETTI
struct Oggetto{
  int probabilita;
  enum Tipo_oggetto oggetto;
};
//DEFINIZIONE DELLA STRUTTURA DEGLI ANUMALI
struct Animale{
    int probabilita;
    enum Tipo_animale animale;
};
//DEFINIZIONE DELLA STRUTTURA DELLE CELLE, OGNUNA CONTENENTE UN PERICOLO, UN OGGETTO E UN ANIMALE
struct Cella{
  enum Tipo_pericolo pericolo;
  enum Tipo_oggetto oggetto;
  enum Tipo_animale animale;
};
//DEFINIZIONE DELLA STRUTTURA DI UN PIANO DELLA TORRE.
struct Piano{
  unsigned int l;
  struct Piano * NextPiano;
};

