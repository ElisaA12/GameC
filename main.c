#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "gamelib.h"


int main (){
    int a;
    system("clear");
        printf("                  BENVENUTI IN FORSENNAIT\n");
        sleep(2);
    do{
        system("clear");

        printf("༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎  ༎ ༎\n");
        printf("༎ ༎            1) Crea mappa 2) Gioca                      ༎ ༎\n" );
        printf("༎ ༎         3) Termina gioco  4)Istruzioni                 ༎ ༎\n" );
        printf("༎ ༎               Digita il comando:                       ༎ ༎\n");
        printf("༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎ ༎  ༎ ༎\n");
        scanf("%d",&a );
        switch(a){
            case 1:
                crea_mappa();
                break;
            case 2:
                gioca();
                break;
            case 3:
                termina_gioco();
                break;
            case 4:
                istruzioni();
                break;
            default:
                printf("\a");
                printf("COMANDO SBAGLIATO, RIPROVARE.\n");
                sleep(1);
        }

    }while(1);
}
