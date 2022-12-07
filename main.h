#include <stdio.h> //on inclue tout les modules necessaires
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <locale.h>
#include <time.h>
#include <math.h>


#include "display.h" //tout les autres fichiers sont inclues
#include "tree.h"
#include "character.h"
#include "keyevent.h"
#include "log.h"


typedef struct random_wheel{ //structure contenant la totalité des informations d'un thème
    tree* combinations;
    line* start;
    uint16_t money;
    bool animation;
}random_wheel;

KeyEvent Main_Menu(random_wheel* rw); //prototype des fonctions
KeyEvent Game(random_wheel* rw);
KeyEvent Animation(random_wheel* rw);
KeyEvent Crash(char* arg);

void GetAmountMoney(random_wheel* rw);
void random_wheel_generate_on(line* llist[3],rect rlist[3]);
void random_wheel_generate_off(line* llist[3],rect rlist[3]);

void CloseRandomWheel(random_wheel* rw);
KeyEvent FileWindow(char* filename);

