#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "gamelib.h"

//Le 7 funzioni seguenti mi permettono di ordinare ogni array contenente i tipi pericolo ,oggetto e animale, in modo da poter inizializzare la scacchiera
static void scambia_interi(int *a,int*b);
static void scambia_enum1(enum Tipo_pericolo *a, enum Tipo_pericolo *b);
static void scambia_enum3(enum Tipo_animale *a, enum Tipo_animale *b);
static void scambia_enum2(enum Tipo_oggetto *a, enum Tipo_oggetto *b);
//la funzione ordinamento mi ordina gli array con i valori di probabilità in ordine crescente
static void ordinamento1(struct Pericolo array[], int elemento);
static void ordinamento2(struct Oggetto array[], int elemento);
static void ordinamento3(struct Animale array[], int elemento);
//funzione che mi dealloca i dati per una nuova partita
static void reset();
//funzione che fa iniziare a caso uno dei due giocatori
static void giocatore();
//funzione che mi stampa le statistiche del giocatore alla fine di ogni turno
static void stato_giocatore(struct Giocatore giocatore, char a[],int x,int y);
//funzione che mi permette di muovere il giocatore nella scacchiera
static int muovi(int s, int posizione);
//funzione per il combattimento con un alieno
static int combattimento(struct Giocatore giocatore, char i[], char f[], int* cane,int* vittoria);
//funzione richiamata quando incontro un animale sulla cella
static int prendi_animale(struct Giocatore giocatore, int a, char g[],int*cane);
//funzione richiamata nella scelta dell'opzione 'usa oggetto'
static int usa_oggetto(struct Giocatore giocatore, char a[], int *poz,int*med);
//funzone richiamata durante lo scontrofinale che stampa i piani della torre che ogni giocatore ha a disposizione
static void stampa_torre(struct Piano*p, char i[]);
//funzione che mi crea i piani della torre a seconda del materiale a disposizione
static struct Piano* torre(struct Giocatore giocatore, struct Piano*p);
//funzione che utilizzo per segnare le vittorie di ogni giocatoreper poi stamparle in termina_gioco
static int vittorie(char a[]);

unsigned int sleep(unsigned int __seconds);
int usleep(useconds_t usec);

int randomicita(){
    return rand()%101;
}

const char * tipo_peric[]={"nessun pericolo", "trappola", "alieno"}; //variabili che utilizzo per stampare il nome dei pericoli, degli oggetti, degli animali e dello stato del giocatore
const char * tipo_ogg[]={"nessuno","medikit","pozione","materiale","colpi_lanciarazzi"};
const char * tipo_an[]={"nessun animale", "cane","pantera"};
const char * stato_g[]={"vita","vulnerabile","scudo_vita","scudo"};

const char * tipo_p[]={"NP", "TR", "AL"};//variabili che utilizzo per stampare gli acronimi sulla scacchiera
const char * tipo_o[]={"NO","ME","PO","MA","CL"};
const char * tipo_a[]={"NA","CA","PA"};

// array per gestire il passaggio a stringa
char iniziale[10]={0};//giocatore iniziale
char finale[10]={0};//giocatore finale
char N[10]={0};//giocatore Ninja
char C[10]={0};//giocatore Ciccio


int n,b;
int cane_i,cane_f; // variabili per vedere se il giocatore possiede un animale o meno.
int count;
int fine=0; //per controllare se ho inserito i valori di probabilità
static int vittorie_n, vittorie_c;//variabili che utilizzo per memorizzare le vittorie di ogni giocatore

static unsigned int turno=0;

struct Cella *scacchiera=NULL;
struct Piano *piano_i=NULL,*piano_f=NULL; //puntatori per la creazione dei piani nell'attacco finale
struct Pericolo p[3];
struct Oggetto og[5];
struct Animale an[3];
struct Cella*pericolo;
static struct Giocatore Ninja={vita};
static struct Giocatore Ciccio={vita};
struct Giocatore giocatore_i;
struct Giocatore giocatore_f;



static void scambia_interi(int *a,int*b){
    int t;
    t=*a;
    *a=*b;
    *b=t;
}
static void scambia_enum1(enum Tipo_pericolo *a, enum Tipo_pericolo *b){
    enum Tipo_pericolo t;
    t=*a;
    *a=*b;
    *b=t;
}
static void scambia_enum2(enum Tipo_oggetto *a, enum Tipo_oggetto *b){
    enum Tipo_oggetto t;
    t=*a;
    *a=*b;
    *b=t;
}

static void scambia_enum3(enum Tipo_animale *a, enum Tipo_animale *b){
    enum Tipo_animale t;
    t=*a;
    *a=*b;
    *b=t;
}

//la funzione ordinamento mi ordina gli array con i valori di probabilità in ordine crescente
static void ordinamento1(struct Pericolo array[], int elemento){

    for(int i=0; i<elemento-1;i++){
        for(int j=elemento-1;j>i;j--){
            if(&array[j].probabilita< &array[j-1].probabilita){
                scambia_interi(&array[i].probabilita,&array[j-1].probabilita);
                scambia_enum1(&array[i].pericolo,&array[j-1].pericolo);

            }
        }
    }
}
static void ordinamento2(struct Oggetto array[], int elemento){

    for(int i=0; i<elemento-1;i++){
        for(int j=elemento-1;j>i;j--){
            if(&array[j].probabilita< &array[j-1].probabilita){
                scambia_interi(&array[i].probabilita,&array[j-1].probabilita);
                scambia_enum2(&array[i].oggetto,&array[j-1].oggetto);

            }
        }
    }
}

static void ordinamento3(struct Animale array[], int elemento){

    for(int i=0; i<elemento-1;i++){
        for(int j=elemento-1;j>i;j--){
            if(&array[j].probabilita< &array[j-1].probabilita){
                scambia_interi(&array[i].probabilita,&array[j-1].probabilita);
                scambia_enum3(&array[i].animale,&array[j-1].animale);

            }
        }
    }
}

//azzero i dati per una nuova partita
static void reset(){
    fine=0;
    free(scacchiera);
    free(piano_i);
    free(piano_f);
    Ninja.stato=0;
    Ciccio.stato=0;
    Ninja.anim=0;
    Ciccio.anim=0;
    for(int j=0; j<n;j++){
        Ninja.zaino[j]=0;
        Ciccio.zaino[j]=0;
    }
    turno=0;
}

//funzione che fa iniziare a caso uno dei due giocatori
static void giocatore(){
    time_t t;
    srand((unsigned) time(&t));
    int r=rand()%101;
    //la funzione strcpy mi memorizza dentro la stringa N il carattere "Ninja"
    strcpy(N,"Ninja");
    strcpy(C,"Ciccio");
    if(r%2==0){
        giocatore_i=Ninja;
        giocatore_f=Ciccio;
        strcpy(iniziale,"Ninja");
        strcpy(finale,"Ciccio");
    }else{
        giocatore_i=Ciccio;
        giocatore_f=Ninja;
        strcpy(iniziale,"Ciccio ");
        strcpy(finale,"Ninja");
    }
}
//quello che mi stampa alla fine di ogni turno
static void stato_giocatore(struct Giocatore giocatore, char a[],int x,int y){

    printf("TURNO %d\n",turno);
    printf("E' il turno di %s\n",a);
    printf("STATISTICHE GIOCATORE:\n");
    printf("Stato:%s\n",stato_g[giocatore.stato]);
    printf("Zaino:\n" );
    for(int i=0; i<4;i++){
        printf("%s\n",tipo_ogg[giocatore.zaino[i]] );
    }
    printf("Alieni uccisi:%d\n",giocatore.alieni_uccisi );
    printf("Animale:%s\n",tipo_an[giocatore.anim]);
    printf("Valore al coraggio:%d\n",giocatore.coraggio);

}


static int muovi(int s, int posizione){
    if(s==1 || s==4){
        posizione=posizione-1;
    }if(s==2 || s==3){
        posizione=posizione+1;
    }
    return posizione;
}



static int combattimento(struct Giocatore giocatore, char i[], char f[], int* cane,int* vittoria){
    time_t t;
    srand((unsigned) time(&t));
    int s= rand()%101;

    if(s<35){ //attacca per prima l'alieno
        int r=rand()%101;
        if(r<35){ //ATTACCO ALLA VITA
            if(giocatore.stato==1){
                printf("Il tuo stato è VULNERABILE. L'ALIENO è riuscito ad attaccarti!  \n" );
                if(giocatore.anim==1){
                    printf("Il tuo fedele CANE si lancia fra te e l'alieno, sacrificando la sua vita per salvarti.\nNon lasci che la sua vita sia stata sacrificata per niente e attacchi il nemico.\n");
                    --*cane;
                } else{
                    printf("Non riesci a sopravvivere al colpo. Sei MORTO.\n" );
                    printf("Il giocatore %s vince.\n",f );
                    sleep(2);
                    ++*vittoria;
                    return 0;
                }
            }if(giocatore.stato==0){
                printf("L'ALIENO è riuscito ad attaccarti!\n Non hai niente per proteggerti, perdi una vita. Ora sei in stato VULNERABILE.\n" );
                giocatore.stato=1;
            }if(giocatore.stato==2){
                printf("L'ALIENO è riuscito ad attaccarti!\nPerdi una vita. Ora ti rimane solo lo SCUDO.\n" );
                giocatore.stato=3;
            }if(giocatore.stato==3){
                printf("L'ALIENO è riuscito ad attaccarti!\n Ti proteggi con lo scudo. Ora sei in stato VULNERABILE.\n" );
                giocatore.stato=1;
            }
        }else{
            if(giocatore.stato==1){//attacco allo scudo
                printf(" Il tuo stato è VULNERABILE. L'ALIENO è riuscito ad attaccarti! \n" );
                if(giocatore.anim==1){
                    printf("Il tuo fedele CANE si lancia fra te e l'alieno, sacrificando la sua vita per salvarti.\nNon lasci che la sua vita sia stata sacrificata per niente e attacchi il nemico.\n");
                    --*cane;
                } else{
                    printf("Non riesci a sopravvivere al colpo. Sei MORTO.\n" );
                    printf("Il giocatore %s vince.\n",f );
                    sleep(2);
                    ++*vittoria;
                    return 0;
                }
            }if(giocatore.stato==0){
                printf("L'ALIENO è riuscito ad attaccarti!\n Non hai niente per proteggerti, perdi una vita. Ora sei in stato VULNERABILE.\n" );
                giocatore.stato=1;
            }if(giocatore.stato==2){
                printf("L'ALIENO è riuscito ad attaccarti!\n Ma pari il colpo con lo SCUDO. Ora sei in stato VITA.\n" );
                giocatore.stato=0;
            }if(giocatore.stato==3){
                printf("L'ALIENO è riuscito ad attaccarti!\n Ti proteggi con lo scudo. Ora sei in stato VULNERABILE.\n");
                giocatore.stato=1;
            }
        }

        printf("Riesci a sconfiggere l'ALIENO!\n");
        sleep(1);

    }else{//attacca per primo il giocatore
        printf("Riesci ad attaccare per primo. Hai sconfitto l'ALIENO!\n" );
        sleep(1);
    }



    return giocatore.stato;
}

static int prendi_animale(struct Giocatore giocatore, int a, char g[],int*cane){
    int b,c;
    sleep(1);
    printf("Improvvisamente senti un rumore dietro l'angolo.. Ti volti e trovi %s che ti osserva.\n",tipo_an[scacchiera[a].animale]);
    printf("Cosa scegli di fare, decidi di portarlo con te?\n1)PRENDI\n2)LASCIA\n");
    scanf("%d",&b);
    switch(b){
        case 1:
            if(giocatore.anim!=0){
                printf("Hai già un animale con te. Vuoi abbandonarlo per prendere %s?\n1)ABBANDONA\n2)NON PRENDERE %s\n",tipo_an[scacchiera[a].animale],tipo_an[scacchiera[a].animale]);
                scanf("%d",&c);
                switch (c){
                    case 1:
                        printf("%s prende %s.\n",g,tipo_an[scacchiera[a].animale]);
                        giocatore.anim=scacchiera[a].animale;
                        if(giocatore.anim==1){
                            ++*cane;
                        }
                        break;
                    case 2:
                        break;
                    default:
                        printf("COMANDO SBAGLIATO, RIPROVA.\n");
                }
            }else{
                giocatore.anim=scacchiera[a].animale;
                if(giocatore.anim==1){
                    ++*cane;
                }
            }
            break;
        case 2:
            printf("%s decide di lasciare lì %s.\n",g,tipo_an[scacchiera[a].animale]);
            break;
        default:
            printf("COMANDI SBAGLIATI, RIPROVARE\n");
    }
    sleep(1);
    return giocatore.anim;

}

static int vittorie(char a[]){
    //la funzione strncmp, in questo caso,mi confronta 6 caratteri di due stringhe
    int stringa= strncmp(a,N,6);
    int stringa2= strncmp (a,C,6);

    if(stringa==0){
        vittorie_n++;
        reset();
        //inizio();
    }if(stringa2==0){
        vittorie_c++;
        reset();
        //inizio();
    }
}

static int usa_oggetto(struct Giocatore giocatore, char a[],int* poz, int *med){
    count=0;    //variabile che uso per controllare se lo zaino è vuoto.
    int medikit=0;  //variabile che uso per controllare se nello zaino è presente medikit
    int pozione=0;  //variabile che uso per controllare se nello zaino è presente pozione.
    int p;

    for(int i=0; i<4;i++) {
        if (giocatore.zaino[i] == 0) {
            count++;
        }
    }
    for(int i=0; i<4;i++) {
        if (giocatore.zaino[i] != 1) {
            medikit++;
        }
    }
    for(int i=0; i<4;i++) {
        if(giocatore.zaino[i]!=2){
            pozione++;
        }
    }

    if(count==4){
        printf("Lo zaino è VUOTO. Non puoi usare nessun oggetto.\n" );
        sleep(2);
        return giocatore.stato;
    }else {
        printf("\n");
        printf("Quale OGGETTO vuoi usare?\n");
        printf("(Per usare MEDIKIT premi 1, per usare POZIONE premi 2.)\n");
        scanf("%d" , &p);
        switch (p) {
            case 1:
                if (medikit != 4) {
                    if (giocatore.stato == 0 || giocatore.stato == 2) {
                        printf("Hai già una VITA. IMPOSSIBILE UTILIZZARE OGGETTO.\n");
                        sleep(2);
                    }
                    if (giocatore.stato == 1) {
                        printf("%s usa MEDIKIT..\nVITA ristabilita.\n" , a);
                        sleep(2);
                        for (int i = 0; i < 4; i++) {
                            if (giocatore.zaino[i] == 1) {
                                --*med;
                                break;
                            }
                        }
                        giocatore.stato = 0;
                    }
                    if (giocatore.stato == 3) {
                        printf("%s usa MEDIKIT..\nVITA ristabilita.\n" , a);
                        for (int i = 0; i < 4; i++) {
                            if (giocatore.zaino[i] == 1) {
                                --*med;
                                break;
                            }
                        }
                        giocatore.stato = 2;
                    }
                } else {
                    printf("IMPOSSIBILE USARE OGGETTO. MEDIKIT non presente nello zaino.\n");
                }
                return giocatore.stato;
                break;

            case 2:
                if (pozione != 4) {

                    if (giocatore.stato == 3 || giocatore.stato == 2) {
                        printf("Hai già uno SCUDO. IMPOSSIBILE UTILIZZARE OGGETTO.\n");
                        sleep(2);

                    }
                    if (giocatore.stato == 1) {
                        printf("%s usa POZIONE..\nSCUDO ristabilito.\n" , a);
                        sleep(2);
                        for (int i = 0; i < 4; i++) {
                            if (giocatore.zaino[i] == 2) {
                                --*poz;
                                break;
                            }
                        }
                        giocatore.stato = 3;
                    }
                    if (giocatore.stato == 0) {
                        printf("%s usa POZIONE..\nSCUDO ristabilito.\n" , a);
                        sleep(2);
                        for (int i = 0; i < 4; i++) {
                            if (giocatore.zaino[i] == 2) {
                                --*poz;
                                break;
                            }
                        }
                        giocatore.stato = 2;
                    }
                } else{
                    printf("IMPOSSIBILE USARE OGGETTO. POZIONE non presente nello zaino.\n");
                }

                break;
            default:
                printf("COMANDI SBAGLIATI, RIPROVARE.\n");
                printf("\n");
        }
    }

    return giocatore.stato;
}



//per creare la lunghezza della scacchiera e inserire le probabilità degli oggetti, pericoli, animali in ogni cella
void crea_scacchiera(){
    time_t t;
    srand((unsigned) time(&t));

    int c;

    if(fine==0) {
        system("clear");
        printf("Inserisci la lunghezza della mappa:\n");
        scanf("%d" , &n);
        //creo in memoria la dimensione che deve avere la scacchiera
        scacchiera = (struct Cella *) malloc(n * n * sizeof(struct Cella));


        do {
            printf("E' ora di scegliere la DIFFICOLÀ del gioco. (premi il numero corrispondente)\n\n");
            printf("1)FACILE\n2)MEDIO\n3)DIFFICLE\n4)CREALO TU\n");
            scanf("%d" , &c);
            switch (c) {
                case 1:
                    p[0].probabilita = 75;
                    p[0].pericolo = nessun_pericolo;
                    p[1].probabilita = 5;
                    p[1].pericolo = trappola;
                    p[2].probabilita = 20;
                    p[2].pericolo = alieno;

                    og[0].probabilita = 0;
                    og[0].oggetto = nessuno;
                    og[1].probabilita = 25;
                    og[1].oggetto = medikit;
                    og[2].probabilita = 25;
                    og[2].oggetto = pozione;
                    og[3].probabilita = 25;
                    og[3].oggetto = materiale;
                    og[4].probabilita = 25;
                    og[4].oggetto = colpi_lanciarazzi;

                    an[0].probabilita = 20;
                    an[0].animale = nessun_animale;
                    an[1].probabilita = 45;
                    an[1].animale = cane;
                    an[2].probabilita = 35;
                    an[2].animale = pantera;
                    break;
                case 2:
                    p[0].probabilita = 50;
                    p[0].pericolo = nessun_pericolo;
                    p[1].probabilita = 20;
                    p[1].pericolo = trappola;
                    p[2].probabilita = 30;
                    p[2].pericolo = alieno;

                    og[0].probabilita = 20;
                    og[0].oggetto = nessuno;
                    og[1].probabilita = 20;
                    og[1].oggetto = medikit;
                    og[2].probabilita = 20;
                    og[2].oggetto = pozione;
                    og[3].probabilita = 20;
                    og[3].oggetto = materiale;
                    og[4].probabilita = 20;
                    og[4].oggetto = colpi_lanciarazzi;

                    an[0].probabilita = 50;
                    an[0].animale = nessun_animale;
                    an[1].probabilita = 25;
                    an[1].animale = cane;
                    an[2].probabilita = 25;
                    an[2].animale = pantera;
                    break;
                case 3:
                    p[0].probabilita = 20;
                    p[0].pericolo = nessun_pericolo;
                    p[1].probabilita = 25;
                    p[1].pericolo = trappola;
                    p[2].probabilita = 55;
                    p[2].pericolo = alieno;

                    og[0].probabilita = 20;
                    og[0].oggetto = nessuno;
                    og[1].probabilita = 10;
                    og[1].oggetto = medikit;
                    og[2].probabilita = 10;
                    og[2].oggetto = pozione;
                    og[3].probabilita = 30;
                    og[3].oggetto = materiale;
                    og[4].probabilita = 30;
                    og[4].oggetto = colpi_lanciarazzi;

                    an[0].probabilita = 70;
                    an[0].animale = nessun_animale;
                    an[1].probabilita = 15;
                    an[1].animale = cane;
                    an[2].probabilita = 15;
                    an[2].animale = pantera;

                    break;
                case 4:
                    //percentuale del pericolo che il giocatore incontrà
                    printf("DIFFICOLTA' DI GIOCO: (Probabilità tra 0 e 100)\n\n");
                    sleep(1);
                    printf("Probabilità di trovare PERICOLI:\nNESSUN PERICOLO:");
                    scanf("%d" , &p[0].probabilita);
                    p[0].pericolo = nessun_pericolo;
                    printf("TRAPPOLE:");
                    scanf("%d" , &p[1].probabilita);
                    p[1].pericolo = trappola;
                    printf("ALIENI:");
                    scanf("%d" , &p[2].probabilita);
                    p[2].pericolo = alieno;

                    printf("\n");
                    sleep(1);

                    printf("Probabilità di trovare OGGETTI:\nNESSUN OGGETTO:");
                    scanf("%d" , &og[0].probabilita);
                    og[0].oggetto = nessuno;
                    printf("MEDIKIT:");
                    scanf("%d" , &og[1].probabilita);
                    og[1].oggetto = medikit;
                    printf("POZIONE:");
                    scanf("%d" , &og[2].probabilita);
                    og[2].oggetto = pozione;
                    printf("MATERIALE:");
                    scanf("%d" , &og[3].probabilita);
                    og[3].oggetto = materiale;
                    printf("COLPI LANCIARAZZI:");
                    scanf("%d" , &og[4].probabilita);
                    og[4].oggetto = colpi_lanciarazzi;

                    printf("\n");
                    sleep(1);

                    printf("Probabilità di trovare ANIMALI:\nNESSUN ANIMALE:");
                    scanf("%d" , &an[0].probabilita);
                    an[0].animale = nessun_animale;
                    printf("CANE:");
                    scanf("%d" , &an[1].probabilita);
                    an[1].animale = cane;
                    printf("PANTERA:");
                    scanf("%d" , &an[2].probabilita);
                    an[2].animale = pantera;

            }

            sleep(1);
//se la somma delle probabilità è uguale a 100 mi continua, altrimenti fa reinserire i valori
            if (((p[0].probabilita) + (p[1].probabilita) + (p[2].probabilita) == 100) && ((og[0].probabilita) + (og[1].probabilita) + (og[2].probabilita) + (og[3].probabilita) + (og[4].probabilita) == 100) && ((an[0].probabilita) + (an[1].probabilita) + (an[2].probabilita) == 100)) {
                ordinamento1(p , 3);
                ordinamento2(og , 5);
                ordinamento3(an , 3);
                ++fine; //mi dice che la scacchiera è stata creata
                break;
            } else {
                printf("Valori errati\n");
                sleep(1);
            }
        } while (fine == 0);
        //posiziono casualmente con numeri random i giocatori all'interno della scacchiera
        int x_n=(rand()%n);
        int y_n=(rand()%n);
        int x_c=(rand()%n);
        int y_c=(rand()%n);
        Ninja.posizionex=x_n;
        Ninja.posizioney=y_n;
        Ciccio.posizionex=x_c;
        Ciccio.posizioney=y_c;
        //faccio iniziale i giocatori con lo stato vita e inizializzo gli oggetti dello zaino a 'nessun oggetto'
        Ninja.stato=0;
        Ciccio.stato=0;
        for(int j=0; j<n;j++){
            Ninja.zaino[j]=0;
            Ciccio.zaino[j]=0;
        }
    }

    for (int i = 0; i <n; i++) {
        for(int j=0; j<n;j++){
            int r=rand()%101;
            int c=rand()%101;
            int s=rand()%101;

            // indicizzo il campo pericolo di ogni cella
            if(r>=0 && r<p[0].probabilita){
                scacchiera[i*n+j].pericolo=p[0].pericolo;

            }if(r>=p[0].probabilita && r<((p[0].probabilita)+(p[1].probabilita))){
                scacchiera[i*n+j].pericolo=p[1].pericolo;

            }if(r>=(p[0].probabilita+p[1].probabilita) && r<=((p[0].probabilita)+(p[1].probabilita)+(p[2].probabilita))){
                scacchiera[i*n+j].pericolo=p[2].pericolo;

            }

            // indicizzo il campo oggetto di ogni cella
            if(c>=0 && c<og[0].probabilita){
                scacchiera[i*n+j].oggetto=og[0].oggetto;

            }if(c>=og[0].probabilita && c<((og[0].probabilita)+(og[1].probabilita))){
                scacchiera[i*n+j].oggetto=og[1].oggetto;

            }if(c>=((og[0].probabilita)+(og[1].probabilita)) && c<((og[0].probabilita)+(og[1].probabilita)+(og[2].probabilita))){
                scacchiera[i*n+j].oggetto=og[2].oggetto;

            }if(c>=(og[0].probabilita)+(og[1].probabilita)+(og[2].probabilita) && c<((og[0].probabilita)+(og[1].probabilita)+(og[2].probabilita)+(og[3].probabilita))){
                scacchiera[i*n+j].oggetto=og[3].oggetto;

            }if(c>=(og[0].probabilita)+(og[1].probabilita)+(og[2].probabilita)+(og[3].probabilita) && c<((og[0].probabilita)+(og[1].probabilita)+(og[2].probabilita)+(og[3].probabilita)+(og[4].probabilita))){
                scacchiera[i*n+j].oggetto=og[4].oggetto;
            }

            // indicizzo il campo animale di ogni cella
            if(s>0 && s<(an[0].probabilita)){
                scacchiera[i*n+j].animale=an[0].animale;
            }if(s>=an[0].probabilita && s<((an[0].probabilita)+(an[1].probabilita))){
                scacchiera[i*n+j].animale=an[1].animale;

            }if(s>=((an[0].probabilita)+(an[1].probabilita)) && c<((an[0].probabilita)+(an[1].probabilita)+(an[2].probabilita))){
                scacchiera[i*n+j].animale=an[2].animale;
            }
        }
    }
}



//stampa la scacchiera
void stampa_scacchiera(){
    int a;

    if(fine>0){
        system("clear");

        for (int i = 0; i < n; i++) {

            for (int j = 0; j < n; j++) {
                printf("+------------+");

            }
            printf("\n");

            for(int j=0; j < n; j++){
                printf("|             ");
            }
            printf("|");
            printf("\n");

            for(int j=0; j < n; j++){
                printf("|%s - %s - %s ",tipo_p[scacchiera[i * n + j].pericolo],tipo_o[scacchiera[i * n + j].oggetto],tipo_a[scacchiera[i * n + j].animale]);
            }
            printf("|");
            printf("\n");
            for(int j=0; j < n; j++){
                printf("|             ");
            }
            printf("|");
            printf("\n");
        }
        for (int j = 0; j < n; j++) {

            printf("+------------+");
        }
        printf("\n\n");
        printf("LEGENDA:\n°ZS = zona sicura\n°TR = trappola\n°AL = alieno\n°NO = nessun oggetto\n°ME = medikit\n°PO = pozione\n°MA = materiale\n°CL = colpi lanciarazzo\n°NA = Nessun animale\n°CA = cane\n°PA = pantera\n");


    }else{ //se fine==0
        printf("Non esiste ancora una MAPPA di gioco. Torna al menu e creala.\n");
        printf("Premi un tasto per continuare.\n");
        scanf("%d",&a);
        crea_mappa();
    }
    printf("Premi un tasto per continuare.\n");
    scanf("%d",&a);
}


static struct Piano* torre(struct Giocatore giocatore, struct Piano*p){
    int num=0;
    p=NULL;
    for(int i=0; i<4;i++){//creo i piani per la torre dello scontro finale in base al valore di num
        if (giocatore.zaino[i]==3) {
            num++; //conto quanti MATERIALE possiede il giocatore
        }
    }

    if(num==0){
        p=NULL;
    }else{
        p=(struct Piano*)malloc(sizeof(struct Piano));
        p->l=1;
        p->NextPiano=NULL;
        struct Piano*q;
        q=(struct Piano*)malloc(sizeof(struct Piano));
        q=p;
        for(int i=2; i<=num; i++) {
            q->NextPiano=(struct Piano*)malloc(sizeof(struct Piano));
            q=q->NextPiano;
            q->l=i;
        }
        q->NextPiano=NULL;
    }
    return p;
}
static void stampa_torre(struct Piano*p, char i[]){
    sleep(1);
    printf("TORRE GIOCATORE %s.\n",i);

    if(p==NULL){
        printf("Non hai MATERIALE per creare una torre.\n");

    }else {


        do{


            printf(" ______\n");
            printf("|      |\n");
            printf("|  %d   |\n", p->l);
            printf("|______|\n");

            p = p->NextPiano;
        }while(p!=NULL);
    }
    printf("\n\n");

    return;
}


void gioca(){
    time_t t;
    srand((unsigned)time(&t));

    int morte=0;

    if(fine==0){
        printf("Non hai creato la mappa, la tua avventura non può ancora cominciare.\n");
        sleep(2);
        crea_mappa();
    }

    giocatore(); //casualmente mi fa iniziare un gicatore, da ora in poi userò giocatore_i per il giocatore iniziale e giocatore_f per quello finale
    int s,e,f,h,c,d,g;
    int count1=0;

    do {
        //RESTRINGIMENTO MAPPA DEL GIOCO
        if (turno % 3 == 0) {
            free(scacchiera);
            n = n / 2;
            scacchiera = (struct Cella *) malloc(n * n * sizeof(struct Cella));
            crea_scacchiera(); //RIINIZIALIZZO LA SCACCHIERA CON LE STESSE PROBABILITÀ
            //RIPOSIZIONO CASUALMENTE I GIOCATORI
            giocatore_i.posizionex = rand() % n;
            giocatore_i.posizioney = rand() % n;
            giocatore_f.posizionex = rand() % n;
            giocatore_f.posizioney = rand() % n;
        }
        if (giocatore_i.posizionex * n + giocatore_i.posizioney == giocatore_f.posizionex * n +
                                                                   giocatore_f.posizioney) { //se i due giocatori si incontrano sulla stessa cella ha inizio lo SCONTRO FINALE

            system("clear");
            printf("ATTENZIONE! Hai incontrato %s e non sembra aver intenzione di lasciarti scappare..\n " , iniziale);
            sleep(1);
            printf("..HA INIZIO LO SCONTRO FINALE.\n\n\n");
            sleep(2);
//CREAZIONE DEI PIANI DELLA TORRE
            piano_i = torre(giocatore_i , piano_i);
            stampa_torre(piano_i , iniziale);
            piano_f = torre(giocatore_f , piano_f);
            stampa_torre(piano_f , finale);

            sleep(2);
            printf("Lancia un dado per tentare la fortuna. Se esce pari inizierà %s, al contrario %s. (premi un pulsante per lanciare il dado)\n" ,
                   iniziale , finale);
            scanf("%d" , &c);
            h = (rand() % 6) + 1;
            printf("E' uscito il numero %d...\n" , h);
            printf("\n");
            sleep(1);

            do {
//INSERISCO NELLE VARIABILI SOTTO IL NUMERO DEI COLPI LANCIARAZZO E DEL MATERIALE POSSEDUTI DA OGNI GIOCATORE
                int colpi_i = 0 , colpi_f = 0 , materiale_i = 0 , materiale_f = 0;
                for (int i = 0; i < 4; i++) {
                    if (giocatore_i.zaino[i] == 4) {
                        colpi_i++;
                    }
                    if (giocatore_f.zaino[i] == 4) {
                        colpi_f++;
                    }
                    if (giocatore_i.zaino[i] == 3) {
                        materiale_i++;
                    }
                    if (giocatore_f.zaino[i] == 3) {
                        materiale_f++;
                    }
                }

                printf("I due giocatori si trovano uno difronte all'altro.\n");
                sleep(2);

                if (materiale_i == 0 && materiale_f == 0) { //NESSUNO DEI DUE GIOCATORI HA DEL MATERIALE
                    printf("Nessuno dei due giocatori possiede MATERIALE per proteggersi nello scontro...\nLa partità finisce in PARITÀ.\n ");
                    sleep(3);
                    reset();
                    morte = 1;
                    //inizio();
                    break;
                }



                //GIOCATORE INIZIALE
                if (h % 2 == 0) {
                    printf("Il prossimo ad attaccare è %s.\n" , iniziale);
                    printf("\n");
                    sleep(2);
                    if (colpi_i == 0) { // IL GIOCATORE INIZIALE NON HA UN COLPO LANCIARAZZO
                        printf("Il giocatore %s non possiede nessun COLPO LANCIARAZZO per danneggiare l'avversario. Cerca di pensare ad un altro modo per colpire %s...\n" ,
                               iniziale , finale);
                        sleep(1);

                        if (colpi_f != 0) { //IL SECONDO GIOCATORE POSSIEDE UN COLPO LANCIARAZZO
                            printf("\n");
                            printf("Ma il giocatore %s, vedendo in difficoltà l'avversario, coglie l'opportunità e attacca per primo.\n" ,
                                   finale);
                            printf("%s premi un pulsante per sparare un COLPO LANCIARAZZO." , finale);
                            scanf("%d" , &g);
                            printf("%s colpisce %s.\n" , finale , iniziale);
                            printf("\n");
                            if (materiale_i == 0) { //SE IL GIOCATORE INIZIALE NON HA MATERIALE
                                printf("Il giocatore %s non possiede alcun MATERIALE per difendersi.\nIl giocatore %s VINCE LA PARTITA!\n " ,
                                       iniziale , finale);
                                sleep(3);
                                vittorie(finale);
                                morte = 1;
                                break;
                            }

                            // SE INVECE IL GIOCATORE INIZIALE POSSIEDE DEL MATERIALE, IL SECONDO GIOCATORE LO COLPISCE E VIENE ELIMINATO UN COLPO LANCIARAZZO DALLO ZAINO
                            for (int i = 0; i < 4; i++) {
                                if (giocatore_f.zaino[i] == 4) {
                                    giocatore_f.zaino[i] = 0;
                                    break;
                                }
                            }
                            //TOGLIE UN MATERIALE AL GIOCATORE INIZIALE, QUINDI DIMINUISCE DI UN PIANO LA TORRE
                            for (int i = 0; i < 4; i++) {
                                if (giocatore_i.zaino[i] == 3) {
                                    giocatore_i.zaino[i] = 0;
                                    break;
                                }
                            }

                        } else { //if(colpi_f==0) SE ANCHE IL SECONDO GIOCATORE NON HA COLPI LANCIARAZZO
                            printf("\n");
                            printf("Sorprendentemente nemmeno l'avversario, %s, possiede un COLPO LANCIARAZZO per attaccarlo.\n" ,
                                   finale);
                            sleep(1);
                            //VALUTO IL CORAGGIO DEI DUE GIOCATORI
                            if (giocatore_i.coraggio >= giocatore_f.coraggio) {
                                printf("Ma il coraggioso %s non perde la speranza e decide di attaccarlo a mani nude.\n" ,
                                       iniziale);
                                printf("%s digita un pulsante per attaccare." , iniziale);
                                scanf("%d" , &g);
                                printf("\n");
                                printf("%s colpisce %s.\n" , iniziale , finale);
                                sleep(1);

                                if (materiale_f == 0) {
                                    printf("Il giocatore %s non possiede alcun MATERIALE per difendersi.\nIl giocatore %s VINCE LA PARTITA!\n " ,
                                           finale , iniziale);
                                    sleep(3);
                                    vittorie(iniziale);
                                    morte = 1;
                                    break;
                                }

                                //TOGLIE UN MATERIALE, QUINDI DIMINUISCE DI UN PIANO LA TORRE
                                for (int i = 0; i < 4; i++) {
                                    if (giocatore_f.zaino[i] == 3) {
                                        giocatore_f.zaino[i] = 0;
                                        break;
                                    }
                                }
                                //CASO IN CUI IL SECONDO GIOCATORE ABBIA CORAGGIO MAGGIORE
                            } else {
                                printf("Ma il coraggioso %s ,vedendo in difficoltà l'avversario, coglie l'opportunità e decide di attaccarlo a mani nude.\n" ,
                                       finale);
                                printf("%s digita un pulsante per attaccare." , finale);
                                scanf("%d" , &g);
                                printf("\n");
                                printf("%s colpisce %s.\n" , finale , iniziale);
                                sleep(1);
                                if (materiale_i == 0) {
                                    printf("Il giocatore %s non possiede alcun MATERIALE per difendersi.\nIl giocatore %s VINCE LA PARTITA!\n " ,
                                           iniziale , finale);
                                    sleep(3);
                                    vittorie(finale);
                                }

                                //TOGLIE UN MATERIALE, QUINDI DIMINUISCE DI UN PIANO LA TORRE
                                for (int i = 0; i < 4; i++) {
                                    if (giocatore_i.zaino[i] == 3) {
                                        giocatore_i.zaino[i] = 0;
                                        break;
                                    }
                                }
                            }
                        }


                        //CASO IN CUI IL GIOCATORE POSSIEDE UN COLPO LANCIARAZZO
                    } else {
                        printf("%s premi un pulsante per sparare un COLPO LANCIARAZZO.\n" , iniziale);
                        scanf("%d" , &g);
                        printf("Il giocatore %s indietreggia per la paura, ma %s riesce ad attaccarlo.\n " , finale ,
                               iniziale);
                        printf("\n");
                        sleep(1);
                        if (materiale_f == 0) {
                            printf("Il giocatore %s non possiede alcun MATERIALE per difendersi.\nIl giocatore %s VINCE LA PARTITA!\n " ,
                                   finale , iniziale);
                            sleep(3);
                            vittorie(iniziale);

                        }
                        //TOGLIE UN COLPO LANCIARAZZO DALLO ZAINO
                        for (int i = 0; i < 4; i++) {
                            if (giocatore_i.zaino[i] == 4) {
                                giocatore_i.zaino[i] = 0;
                                break;
                            }
                        }
                        //TOGLIE UN MATERAILE, QUINDI DIMINUISCE DI UN PIANO LA TORRE
                        for (int i = 0; i < 4; i++) {
                            if (giocatore_f.zaino[i] == 3) {
                                giocatore_f.zaino[i] = 0;
                                break;
                            }
                        }

                    }


                } else {
                    printf("Il prossimo ad attaccare è %s.\n" , finale);
                    printf("\n");
                    sleep(2);

                    //GIOCATORE FINALE
                    if (colpi_f == 0) {
                        printf("Il giocatore %s non possiede nessun COLPO LANCIARAZZO per danneggiare l'avversario. Cerca di pensare ad un altro modo per colpire %s...\n" ,
                               finale , iniziale);
                        sleep(1);

                        if (colpi_i != 0) {
                            printf("\n");
                            printf("Ma il giocatore %s, vedendo in difficoltà l'avversario, coglie l'opportunità e attacca per primo.\n" ,
                                   iniziale);
                            printf("%s premi un pulsante per sparare un COLPO LANCIARAZZO.\n" , iniziale);
                            scanf("%d" , &g);
                            printf("%s colpisce %s.\n" , iniziale , finale);
                            printf("\n");
                            sleep(1);
                            if (materiale_f == 0) {
                                printf("Il giocatore %s non possiede alcun MATERIALE per difendersi.\nIl giocatore %s VINCE LA PARTITA!\n " ,
                                       finale , iniziale);
                                sleep(3);
                                vittorie(iniziale);

                            }
                            //TOGLIE UN COLPO LANCIARAZZO DALLO ZAINO
                            for (int i = 0; i < 4; i++) {
                                if (giocatore_i.zaino[i] == 4) {
                                    giocatore_i.zaino[i] = 0;
                                    break;
                                }
                            }
                            //TOGLIE UN MATERIALE, QUINDI DIMINUISCE DI UN PIANO LA TORRE
                            for (int i = 0; i < 4; i++) {
                                if (giocatore_f.zaino[i] == 3) {
                                    giocatore_f.zaino[i] = 0;
                                    break;
                                }
                            }

                            //CASO IN CUI L'AVVERSARIO NON POSSIEDA UN COLPO LANCIARAZZO
                        } else {
                            printf("Sorprendentemente nemmeno l'avversario, %s, possiede un COLPO LANCIARAZZO per attaccarlo.\n" ,
                                   iniziale);
                            sleep(1);
                            if (giocatore_f.coraggio >= giocatore_i.coraggio) {
                                printf("Ma il coraggioso %s non perde la speranza e decide di attaccarlo a mani nude.\n" ,
                                       finale);
                                printf("%s digita un pulsante per attaccare." , finale);
                                scanf("%d" , &g);
                                printf("\n");
                                printf("%s colpisce %s.\n" , finale , iniziale);
                                sleep(1);
                                if (materiale_i == 0) {
                                    printf("Il giocatore %s non possiede alcun MATERIALE per difendersi.\nIl giocatore %s VINCE LA PARTITA!\n " ,
                                           iniziale , finale);
                                    sleep(3);
                                    vittorie(finale);
                                }
                                //TOGLIE UN MATERIALE, QUINDI DIMINUISCE DI UN PIANO LA TORRE
                                for (int i = 0; i < 4; i++) {
                                    if (giocatore_i.zaino[i] == 3) {
                                        giocatore_i.zaino[i] = 0;
                                        break;
                                    }
                                }
                                //CASO IN CUI IL SECONDO GIOCATORE HA CORAGGIO MAGGIORE
                            } else {
                                printf("Ma il coraggioso %s ,vedendo in difficoltà l'avversario, coglie l'opportunità e decide di attaccarlo a mani nude.\n" ,
                                       iniziale);
                                printf("%s digita un pulsante per attaccare." , iniziale);
                                scanf("%d" , &g);
                                printf("\n");
                                printf("%s colpisce %s.\n" , iniziale , finale);
                                sleep(1);
                                if (materiale_f == 0) {
                                    printf("Il giocatore %s non possiede alcun MATERIALE per difendersi.\nIl giocatore %s VINCE LA PARTITA!\n " ,
                                           finale , iniziale);
                                    sleep(3);
                                    vittorie(iniziale);
                                    morte = 1;
                                    break;

                                }

                                //TOGLIE UN MATERIALE, QUINDI DIMINUISCE DI UN PIANO LA TORRE
                                for (int i = 0; i < 4; i++) {
                                    if (giocatore_f.zaino[i] == 3) {
                                        giocatore_f.zaino[i] = 0;
                                        break;
                                    }
                                }
                            }
                        }

                        //CASO IN CUI IL GIOCATORE POSSIEDE UN COLPO LANCIARAZZO
                    } else {
                        printf("%s premi un pulsante per sparare un COLPO LANCIARAZZO.\n" , iniziale);
                        scanf("%d" , &g);
                        printf("Il giocatore %s indietreggia per la paura, ma %s riesce ad attaccarlo.\n " , iniziale ,
                               finale);
                        printf("\n");
                        sleep(1);
                        if (materiale_i == 0) {
                            printf("Il giocatore %s non possiede alcun MATERIALE per difendersi.\nIl giocatore %s VINCE LA PARTITA!\n " ,
                                   iniziale , finale);
                            sleep(3);
                            vittorie(finale);
                            morte = 1;
                            break;
                        }
                        //TOGLIE UN COLPO LANCIARAZZO DALLO ZAINO
                        for (int i = 0; i < 4; i++) {
                            if (giocatore_f.zaino[i] == 4) {
                                giocatore_f.zaino[i] = 0;
                                break;
                            }
                        }
                        //TOGLIE UN MATERAILE, QUINDI DIMINUISCE DI UN PIANO LA TORRE
                        for (int i = 0; i < 4; i++) {
                            if (giocatore_i.zaino[i] == 3) {
                                giocatore_i.zaino[i] = 0;
                                break;
                            }
                        }

                    }


                }
                h++;
                sleep(2);
                system("clear");
//RICREO LA TORRE SECONDO I NUOVI DATI
                piano_i = torre(giocatore_i , piano_i);
                stampa_torre(piano_i , iniziale);
                piano_f = torre(giocatore_f , piano_f);
                stampa_torre(piano_f , finale);


            } while (morte != 1);

            break;
        }
        printf("cilco2\n\n");
        if (morte != 1) {
            int controllo = 0; //variabile che utilizzo per controllare se il giocatore si è mosso o ha utilizzato l'oggetto.
            do {

                // TURNO DEL PRIMO GIOCATORE
                system("clear");
                int med = 1;
                int poz = 1;
                int x = giocatore_i.posizionex;
                int y = giocatore_i.posizioney;
                stato_giocatore(giocatore_i , iniziale , x , y);
                printf("%s, qual è la tua prossima mossa?\n" , iniziale);
                printf("1)MUOVITI\n2)USA OGGETTO\n");
                scanf("%d" , &e);
                switch (e) {
                    case 1:
                        count = 0;  //variabile che uso per controllare se il giocatore si è spostato.
                        do {

                            printf("E' ora di riprendere il viaggio. Dove vuoi muoverti?\n");
                            printf("COMANDI: 1)Ovest  2)Sud  3)Est  4)Nord\n");
                            scanf("%d" , &s);

                            switch (s) {
                                case 1:
                                    if ((y - 1) < 0 || (y - 1) > n - 1) {
                                        printf("ERRORE: sei uscito fuori dal campo.\n");
                                    } else {
                                        giocatore_i.posizioney = muovi(s , y);
                                        count++;
                                    }
                                    break;
                                case 2:
                                    if ((x + 1) < 0 || (x + 1) > (n - 1)) {
                                        printf("ERRORE: sei uscito fuori dal campo.\n");
                                    } else {
                                        giocatore_i.posizionex = muovi(s , x);
                                        count++;
                                    }
                                    break;
                                case 3:
                                    if ((y + 1) < 0 || (y + 1) > (n - 1)) {
                                        printf("ERRORE: sei uscito fuori dal campo.\n");
                                    } else {
                                        giocatore_i.posizioney = muovi(s , y);
                                        count++;
                                    }
                                    break;
                                case 4:
                                    if ((x - 1) < 0 || (x - 1) > (n - 1)) {
                                        printf("ERRORE: sei uscito fuori dal campo.\n");
                                    } else {
                                        giocatore_i.posizionex = muovi(s , x);
                                        count++;
                                    }
                                    break;
                                default:
                                    printf("COMANDI SBAGLIATI, RIPROVARE.\n");
                                    sleep(1);
                            }
                        } while (count == 0);

                        int a;
                        count = 0;  //variabile che uso per controllare se il giocatore decide di COMBATTERE o SCAPPARE, se si trova in una zona SICURA o se si salva dalla TRAPPOLA.

                        if (scacchiera[giocatore_i.posizionex * n + giocatore_i.posizioney].pericolo == 1) {
                            printf("Sei caduto in una zona con una TRAPPOLA...\n");
                            sleep(1);
                            if (giocatore_i.anim == 2) {
                                printf("La tua fedele PANTERA si lancia verso di te e ti salva dalla trappola, perdendo la sua vita.\n");
                                sleep(2);
                                count = 1;
                                giocatore_i.anim = 0;
                            } else {
                                printf("Il giocatore %s ha perso. Vince %s\nFINE.\n" , iniziale , finale);
                                sleep(2);
                                vittorie(finale);
                                morte = 1;
                                break;
                            }

                        }
                        if (scacchiera[giocatore_i.posizionex * n + giocatore_i.posizioney].pericolo == 2) {


                            do {
                                printf("Hai incontrato un ALIENO. Vuoi combattere?\n");
                                printf("1)COMBATTI\n2)SCAPPA\n");
                                scanf("%d" , &a);

                                switch (a) {

                                    case 1:
                                        printf("%s ha deciso di COMBATTERE.\n" , iniziale);
                                        int c = cane_i;
                                        int v = giocatore_f.vittorie;
                                        giocatore_i.stato = combattimento(giocatore_i , iniziale , finale , &cane_i ,
                                                                          &v);

                                        if (v > giocatore_f.vittorie) {
                                            vittorie(finale);
                                            morte = 1;
                                            break;
                                        }
                                        if (c > cane_i) {
                                            giocatore_i.anim = 0;
                                        }
                                        giocatore_i.alieni_uccisi++;
                                        giocatore_i.coraggio++;
                                        count = 1;
                                        break;
                                    case 2:
                                        printf("%s ha deciso di SCAPPARE.\n..nessun oggetto preso.\n" , iniziale);
                                        giocatore_i.coraggio--;
                                        count = 2;
                                        break;

                                    default:
                                        printf("COMANDI SBAGLIATI.\n");
                                        sleep(1);
                                }

                            } while (count == 0 && morte != 1);
                        }
                        if (scacchiera[giocatore_i.posizionex * n + giocatore_i.posizioney].pericolo == 0) {
                            printf("Sei in una zona SICURA. Nessun pericolo imminente.\n");
                            count = 1;
                        }


                        //SE NELLA ZONA IN CUI MI TROVO È PRESENTE UN OGGETTO E IL GIOCATORE NON È SCAPPATO.
                        if (scacchiera[giocatore_i.posizionex * n + giocatore_i.posizioney].oggetto != 0 &&
                            count == 1) {
                            int c;
                            count1 = 0;

                            do {
                                sleep(1);
                                printf("Hai trovato un OGGETTO. Vuoi prenderlo?  \n");
                                printf("1)PRENDI OGGETTO\n2)LASCIA OGGETTO\n");

                                int count4 = 0;//variabile che utilizzo per vedere se nello zaino ci sono più di tre oggetti dello stesso tipo.
                                int count5 = 0; //variabile che uso per controllare che lo zaino non sia pieno.

                                scanf("%d" , &c);
                                switch (c) {
                                    case 1:
                                        for (int j = 0; j < 4; j++) {
                                            if (scacchiera[giocatore_i.posizionex * n +
                                                           giocatore_i.posizioney].oggetto ==
                                                giocatore_i.zaino[j - 1]) {
                                                count4++;
                                                if (count4 == 3) {
                                                    printf("IMPOSSIBILE PRENDERE L'OGGETTO. Hai già tre oggetti dello stesso tipo nello ZAINO. \n");
                                                    break;
                                                }
                                            }
                                            if (giocatore_i.zaino[j] != 0) {
                                                count5++;
                                            }
                                        }

                                        if (count5 == 4) {
                                            printf("IMPOSSIBILE PRENDERE L'OGGETTO. Lo zaino è PIENO. \n");
                                        } else {
                                            for (int i = 0; i < 4; i++) {
                                                if (giocatore_i.zaino[i] == 0) {
                                                    giocatore_i.zaino[i] = scacchiera[giocatore_i.posizionex * n +
                                                                                      giocatore_i.posizioney].oggetto;
                                                    printf("Hai preso %s\n" , tipo_ogg[giocatore_i.zaino[i]]);
                                                    break;
                                                }
                                            }
                                        }
                                        count1++;
                                        break;

                                    case 2:
                                        printf("Nessun oggetto preso.\n");
                                        count1++;
                                        break;

                                    default:
                                        printf("COMANDO SBAGLIATO.\n");
                                        sleep(1);
                                }
                            } while (count1 == 0);

                        } else {
                            printf("NESSUN OGGETTO presente in questa zona.\n");
                        }

                        //SE NELLA ZONA IN CUI MI TROVO È PRESENTE UN ANIMALE E IL GIOCATORE NON È SCAPPATO.
                        if (scacchiera[giocatore_i.posizionex * n + giocatore_i.posizioney].animale != 0 &&
                            count == 1) {
                            giocatore_i.anim = prendi_animale(giocatore_i ,
                                                              giocatore_i.posizionex * n + giocatore_i.posizioney ,
                                                              iniziale , &cane_i);

                        }
                        controllo++;
                        break;

                    case 2://usa oggetto
                        printf("Gli effetti degli oggetti sono:\nMEDIKIT,restituisce la vita.\nPOZIONE,ristabilisce lo scudo.\n");
                        printf("(MATERIALE e COLPI LANCIARAZZI saranno utili in seguito.)\n");
                        sleep(2);
                        printf("Il tuo zaino contiene questi oggetti:\n");
                        for (int i = 0; i < 4; i++) {
                            printf("%s\n" , tipo_ogg[giocatore_i.zaino[i]]);
                        }
                        int medik = med;
                        int pozi = poz;
                        giocatore_i.stato = usa_oggetto(giocatore_i , iniziale , &pozi , &medik);
                        if (medik < med) {
                            for (int i = 0; i < 4; i++) {
                                if (giocatore_i.zaino[i] == 1) {
                                    giocatore_i.zaino[i] = 0;
                                    break;
                                }
                            }
                        }
                        if (pozi < poz) {
                            for (int i = 0; i < 4; i++) {
                                if (giocatore_i.zaino[i] == 2) {
                                    giocatore_i.zaino[i] = 0;
                                    break;
                                }
                            }
                        }
                        printf("Lo stato attuale di %s è: %s\n" , iniziale , stato_g[giocatore_i.stato]);
                        controllo++;
                        break;
                    default:
                        printf("COMANDI SBAGLIATI, RIPROVARE.\n");
                        sleep(1);


                }
                sleep(1);

            } while (controllo == 0 && morte != 1);

        }

            int controllo = 0; //variabile che utilizzo per controllare se il giocatore si è mosso o ha utilizzato l'oggetto.
            if (morte != 1) {
                do {

                    // TURNO DEL SECONDO GIOCATORE
                    system("clear");
                    int med = 1;
                    int poz = 1;
                    int x = giocatore_f.posizionex;
                    int y = giocatore_f.posizioney;
                    stato_giocatore(giocatore_f , finale , x , y);
                    printf("%s, qual è la tua prossima mossa?\n" , finale);
                    printf("1)MUOVITI\n2)USA OGGETTO\n");
                    scanf("%d" , &e);
                    switch (e) {
                        case 1:
                            count = 0;  //variabile che uso per controllare se il giocatore si è spostato.
                            do {

                                printf("E' ora di riprendere il viaggio. Dove vuoi muoverti?\n");
                                printf("COMANDI: 1)Ovest   2)Sud   3)Est   4)Nord\n");
                                scanf("%d" , &s);

                                switch (s) {
                                    case 1:
                                        if ((y - 1) < 0 || (y - 1) > n - 1) {
                                            printf("ERRORE: sei uscito fuori dal campo.\n");
                                        } else {
                                            giocatore_f.posizioney = muovi(s , y);
                                            count++;
                                        }
                                        break;
                                    case 2:
                                        if (x + 1 < 0 || (x + 1) > (n - 1)) {
                                            printf("ERRORE: sei uscito fuori dal campo.\n");
                                        } else {
                                            giocatore_f.posizionex = muovi(s , x);
                                            count++;
                                        }
                                        break;
                                    case 3:
                                        if ((y + 1) < 0 || (y + 1) > n - 1) {
                                            printf("ERRORE: sei uscito fuori dal campo.\n");
                                        } else {
                                            giocatore_f.posizioney = muovi(s , y);
                                            count++;
                                        }
                                        break;
                                    case 4:
                                        if ((x - 1) < 0 || (x - 1) > n - 1) {
                                            printf("ERRORE: sei uscito fuori dal campo.\n");
                                        } else {
                                            giocatore_f.posizionex = muovi(s , x);
                                            count++;
                                        }
                                        break;
                                    default:
                                        printf("COMANDI SBAGLIATI, RIPROVARE.\n");
                                }
                            } while (count == 0);


                            if (scacchiera[giocatore_f.posizionex * n + giocatore_f.posizioney].pericolo ==
                                1) {  //TRAPPOLA
                                printf("Sei caduto in una zona con una TRAPPOLA...\n");
                                sleep(1);
                                if (giocatore_f.anim == 2) {
                                    printf("La tua fedele PANTERA si lancia verso di te e ti salva dalla trappola, perdendo la sua vita.\n");
                                    sleep(2);
                                    count = 1;
                                    giocatore_f.anim = 0;
                                } else {
                                    printf("Il giocatore %s ha perso. Vince %s\nFINE.\n" , finale , iniziale);
                                    sleep(2);
                                    vittorie(iniziale);
                                    morte = 1;
                                    break;
                                }

                            }
                            if (scacchiera[giocatore_f.posizionex * n + giocatore_f.posizioney].pericolo ==
                                2) {  //ALIENO

                                count = 0;  //variabile che uso per controllare se il giocatore decide di COMBATTERE o SCAPPARE, o se si trova in una zona SICURA.
                                int a;

                                do {
                                    printf("Hai incontrato un ALIENO. Vuoi combattere?\n");
                                    printf("1)COMBATTI\n2)SCAPPA\n");
                                    scanf("%d" , &a);

                                    switch (a) {

                                        case 1:
                                            printf("%s ha deciso di COMBATTERE.\n" , finale);
                                            int v = giocatore_i.vittorie;
                                            int c = cane_f;
                                            giocatore_f.stato = combattimento(giocatore_f , finale , iniziale ,
                                                                              &cane_f ,
                                                                              &v);

                                            if (v > giocatore_i.vittorie) {
                                                vittorie(iniziale);
                                                morte = 1;
                                                break;
                                            }
                                            if (c > cane_f) {
                                                giocatore_f.anim = 0;
                                            }
                                            giocatore_f.alieni_uccisi++;
                                            giocatore_f.coraggio++;
                                            count = 1;
                                            break;
                                        case 2:
                                            printf("%s ha deciso di SCAPPARE.\n..nessun oggetto preso." , finale);
                                            giocatore_f.coraggio--;
                                            count = 2;
                                            break;

                                        default:
                                            printf("COMANDI SBAGLIATI.\n");
                                    }

                                } while (count == 0 && morte != 1);
                            }
                            if (scacchiera[giocatore_f.posizionex * n + giocatore_f.posizioney].pericolo ==
                                0) {  //NESSUN PERICOLO.
                                printf("Sei in una zona SICURA. Nessun pericolo imminente.\n");
                                count = 1;
                            }

                            //SE NELLA ZONA IN CUI MI TROVO È PRESENTE UN OGGETTO E IL GIOCATORE NON È SCAPPATO.
                            if (scacchiera[giocatore_f.posizionex * n + giocatore_f.posizioney].oggetto != 0 &&
                                count == 1) {
                                int c;
                                count1 = 0;

                                do {

                                    printf("Hai trovato un OGGETTO. Vuoi prenderlo?  \n");
                                    printf("1)PRENDI OGGETTO\n2)LASCIA OGGETTO\n");

                                    int count4 = 0; //variabile che utilizzo per vedere se nello zaino ci sono più di tre oggetti dello stesso tipo.
                                    int count5 = 0; //variabile che uso per controllare che lo zaino non sia pieno.
                                    scanf("%d" , &c);
                                    switch (c) {
                                        case 1:
                                            for (int j = 0; j < 4; j++) {
                                                if (scacchiera[giocatore_f.posizionex * n +
                                                               giocatore_f.posizioney].oggetto ==
                                                    giocatore_f.zaino[j - 1]) {
                                                    count4++;
                                                    if (count4 == 3) {
                                                        printf("IMPOSSIBILE PRENDERE L'OGGETTO. Hai già tre oggetti dello stesso tipo nello ZAINO. \n");
                                                        count1++;
                                                        break;
                                                    }
                                                }
                                                if (giocatore_f.zaino[j] != 0) {
                                                    count5++;
                                                }

                                            }
                                            if (count5 == 4) {
                                                printf("IMPOSSIBILE PRENDERE L'OGGETTO. Lo zaino è PIENO. \n");

                                            } else {
                                                for (int i = 0; i < 4; i++) {
                                                    if (giocatore_f.zaino[i] == 0) {
                                                        giocatore_f.zaino[i] = scacchiera[giocatore_f.posizionex * n +
                                                                                          giocatore_f.posizioney].oggetto;
                                                        printf("Hai preso %s\n" , tipo_ogg[giocatore_f.zaino[i]]);
                                                        break;
                                                    }
                                                }
                                            }
                                            count1++;
                                            break;

                                        case 2:
                                            printf("Nessun oggetto preso.\n");
                                            count1++;
                                            break;

                                        default:
                                            printf("COMANDO SBAGLIATO.\n");
                                    }
                                } while (count1 == 0);
                            } else {
                                printf("NESSUN OGGETTO presente in questa zona.\n");
                            }


                            //SE NELLA ZONA IN CUI MI TROVO È PRESENTE UN ANIMALE E IL GIOCATORE NON È SCAPPATO.
                            if (scacchiera[giocatore_f.posizionex * n + giocatore_f.posizioney].animale != 0 &&
                                count == 1) {

                                giocatore_f.anim = prendi_animale(giocatore_f ,
                                                                  giocatore_f.posizionex * n + giocatore_f.posizioney ,
                                                                  finale , &cane_f);

                            }

                            controllo++;
                            break;
                        case 2://usa oggetto
                            system("clear");
                            printf("Gli effetti degli oggetti sono:\n MEDIKIT,restituisce la vita.\nPOZIONE,ristabilisce lo scudo.\n");
                            printf("(MATERIALE e COLPI LANCIARAZZI saranno utili in seguito.)\n");
                            printf("Il tuo zaino contiene questi oggetti:\n");
                            for (int i = 0; i < 4; i++) {
                                printf("%s\n" , tipo_ogg[giocatore_f.zaino[i]]);
                            }
                            int medik = med;
                            int pozi = poz;
                            giocatore_f.stato = usa_oggetto(giocatore_f , finale , &pozi , &medik);
                            if (medik < med) {
                                for (int i = 0; i < 4; i++) {
                                    if (giocatore_f.zaino[i] == 1) {
                                        giocatore_f.zaino[i] = 0;
                                        break;
                                    }
                                }
                            }
                            if (pozi < poz) {
                                for (int i = 0; i < 4; i++) {
                                    if (giocatore_f.zaino[i] == 2) {
                                        giocatore_f.zaino[i] = 0;
                                        break;
                                    }
                                }
                            }
                            printf("Lo stato attuale di %s è: %s\n" , finale , stato_g[giocatore_f.stato]);
                            controllo++;
                            break;
                        default:
                            printf("COMANDI SBAGLIATI, RIPROVARE.\n");
                            sleep(1);


                    }
                    sleep(1);
                } while (controllo == 0 && morte != 1);

                turno++;
            }
        }while (morte != 1);
}


//FUNZIONE CHE SPIEGA IL GIOCO NEI MINIMI PARTICOLARI
void istruzioni(){
    system("clear");
    int a;
    printf("BENVENUTI IN FORSENNAIT! Ecco alcune istruzioni che vi saranno utili prima di iniziare a giocare.\n\n\n");

    printf("Il gioco consiste nell'avventura di due giocatori, Ninja e Ciccio, attraverso un percorso di pericoli e insiedie.\n Tale percorso sarà creato dagli stessi giocatori,cioè voi, all'inizio della partita con le eventuali probabilità di trovare in ogni zona un certo PERICOLO, un certo OGGETTO e un certo ANIMALE.\n ");
    printf("ATTENZIONE! La somma di ogni tipo di probabilità dorà essere uguale a 100.\n\n");
    printf("I possibili pericoli saranno: 1)NESSUNO 2)TRAPPOLA 3)ALIENO\n\n");

    printf("I possibili oggetti saranno: 1)NESSUNO 2)MEDIKIT 3)POZIONE 4)MATERIALE 5)COLPI LANCIARAZZO\n\n");

    printf("I possibili animale saranno: 1)nESSUNO 2)CANE 3)PANTERA\n\n");

    printf("Premi un pulsante per continuare la spiegazione sui PERICOLI e sugli ANIMALI.\n");
    scanf("%d",&a);
    system("clear");


    printf("REGOLE PERICOLI e ANIMALI.\n");
    sleep(1);
    printf("Se capiterete in una zona con una TRAPPOLA potranno succedere due cose:\n1. Morirete automaticamente e la vittoria passerà al vostro avversario.\n2. Se avrete con voi PANTERA, questa potrà salvarvi, ma successivamente la perderete.\n\n");

    printf("Se capiterete in una zona con un ALIENO allora vi sarà chiesto di COMBATTERE o di SCAPPARE.\n");
    printf("-Se deciderete di SCAPPARE la vostra vita non sarà in pericolo, ma non potrete prendere l'OGGETTO presente in quella zona e vi sarà tolto un punto al CORAGGIO.(valore che sarà richiesto nello scontro finale)\n");
    printf("-Se deciderete di COMBATTERE potrete prendere l'OGGETTO, sarà aumentato di 1 il vostro CORAGGIO e avrà inizio lo SCONTRO CON L'ALIENO.\n");
    printf("Se riuscirete ad attaccare per primi la scamperete facilmente e avrete la VITTORIA automatica contro l'alieno.\n");
    printf("Altrimenti, quando l'alieno vi colpirà, se sarete in stato VULNERABILE, potranno accadere due cose:\n1. La vostra MORTE sarà inevitabile e perderete.\n2.Se avrete con voi il CANE, questo potrà salvarvi, ma successivamente lo perderete.\n\n ");
    printf("L'alieno avrà la possibilità di colpirvi o allo SCUDO o alla VITA.\n\n\n");


    printf("CASO VITA:\n ");
    printf("Se sarete in stato VITA, perderete solo una vita e passerete in stato VULNERABILE.\n");
    printf("Se sarete in stato SCUDO, perderete lo scudo e passerete in stato VULNERABILE.\n");
    printf("Se sarete in stato SCUDO_VITA, perderete una vita e passerete in stato SCUDO.\n\n\n");


    printf("CASO SCUDO:\n");
    printf("Se sarete in stato VITA, perderete solo una vita e passerete in stato VULNERABILE.\n");
    printf("Se sarete in stato SCUDO, perderete lo scudo e passerete in stato VULNERABILE.\n");
    printf("Se sarete in stato SCUDO_VITA, perderete lo scudo e passerete in stato VITA.\n\n\n");


    printf("Ovviamente se capiterete in una zona di NESSUN PERICOLO non incontrerete nessun ostacolo.\n\n");

    printf("Tutto chiaro? Premi un pulsante per continuare la spiegazione sugli OGGETTI.\n");
    scanf("%d",&a);
    system("clear");
    printf("REGOLE SUGLI OGGETTI.\n");
    sleep(1);
    printf("Premessa: gli oggetti MEDIKIT e POZIONE saranno spiegati anche durante il gioco al momento opportuno.\n\n");
    sleep(1);

    printf("Gli oggetti MATERIALE e COLPI LANICARAZZO saranno utili nello SCONTRO FINALE: ovvero quando i due giocatori capiteranno nella stessa cella.\n");
    printf("In questa fase del gioco la vittoria e la sconfitta del giocatore dipenderanno dalla quantità di MATERIALE e di COLPI LANCIARAZZO che il giocatore è riuscito a prendere.\n\n");

    printf("-Il MATERIALE che avrete a disposizione servirà per la costruzione di uno SCUDO con cui vi dovrete difendere dai colpi dell'avversario.\n Ogni volta che verrete colpiti perderete 1 MATERIALE. Se l'avversario vi attacca quando non avete più nessun MATERIALE la vostra sconfitta sarà immediata. \n\n");


    printf("-I COLPI LANCIARAZZO serviranno a colpire l'avversario. Se al vostro turno non avrete questo tipo di oggetto nel vostro zaino, potranno accadere due cose:\n");
    printf("1)Se l'avversario avrà un COLPO LANCIARAZZO, allora sarà lui ad attaccarvi per primo.\n\n");

    printf("2)Se nemmeno l'avversario avrà nello zaino un COLPO LANCIARAZZO, entrerà in gioco il valore del CORAGGIO dei due giocatori.\n");
    printf("Se l'avversario avrà un CORAGGIO maggiore del vostro, allora riuscirà ad attaccarvi con un attacco CORPO A CORPO.\n");
    printf("Altrimenti sarete voi i primi a colpire l'avversario con un attacco CORPO A CORPO.\n\n\n");


    printf("Gli effetti degli oggetti MEDIKIT e POZIONE sono rispettivamente di restituire la vita e di ristabilire lo scudo.\n\n\n");


    printf("Nota finale: se nessuno dei due giocatori possiede del MATERIALE per difendersi, lo scontro finisce in PARITÀ.\n\n\n");

    printf("Ovviamente se capiterete in una zona con presente NESSUN OGGETTO non prenderete niente.\n\n");

    printf("Non c'è altro da dire sugli oggetti. Premi un pulsante per continuare.\n");
    scanf("%d",&a);
    system("clear");
    sleep(1);
    printf("ORA CHE SEI BEN ISTRUITO IL GIOCO PUO INIZIARE, BUON DIVERTIMENTO!\n");
    sleep(2);
    system("clear");


}


//AL TERMINE DEL GIOCO STAMPO LE STATISTICHE DI VITTORIA DEI GIOCATORI
void termina_gioco(){
    int a;
    printf("STATISTICHE VITTORIE.\n\n");
    sleep(1);
    printf("Vittorie accumulate da Ninja: %d\n",vittorie_n);
    printf("Vittorie accumulate da Ciccio: %d\n", vittorie_c);
    sleep(2);
    printf("Premi un pulsante per chiudere il gioco.");
    scanf("%d",&a);
    exit(1);
}


//MI TORNA AL MENU PRINCIPALE
void termina_creazione(){

}

void crea_mappa(){
    int b;
    do{
        system("clear");

        printf("༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ \n");
        printf("༎ ༎                                                       ༎ ༎\n");
        printf("༎ ༎     1) Crea scacchiera 2) Stampa schacchiera          ༎ ༎\n");
        printf("༎ ༎              3) Termina creazione                     ༎ ༎\n");
        printf("༎ ༎                Digita il comando:                     ༎ ༎\n");
        printf("༎ ༎                                                       ༎ ༎\n");
        printf("༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ \n");
        scanf("%d",&b );
        switch (b) {
            case 1:
                crea_scacchiera();
                break;
            case 2:
                stampa_scacchiera();
                break;
            case 3:
                termina_creazione();
                break;
            default:
                printf("\a");
                printf("COMADO SBAGLIATO, RIPROVARE.\n" );
                sleep(1);
        }
    }while(b!=3);}
