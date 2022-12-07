#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdint.h>
#include "log.h"

//On dessine les caractères dans une grille 10x10 puis on les aggrandi en 40x20

#define FILE_WIDTH 10 //propriétés
#define FILE_HEIGHT 10
#define X_MULTIPLICATION 4 //grossissement par 4 en x
#define Y_MULTIPLICATION 2 //grossissement par 2 en y
#define CHARACTER_MARGIN 1
#define CHARACTER_WIDTH FILE_WIDTH*X_MULTIPLICATION+4*CHARACTER_MARGIN
#define CHARACTER_HEIGHT FILE_HEIGHT*Y_MULTIPLICATION+2*CHARACTER_MARGIN

#ifndef CHARACTER_H
    #define CHARACTER_H
    
    typedef struct line{ //structure qui stocke une ligne
        wchar_t char_list[CHARACTER_WIDTH+1]; //pour le \0
        char character; //caractère associé à la représentation
        struct line *next;
    }line;
#endif
    
line* LoadCharacterLoop(char* characters_list_file); //prototypes
void FreeCharacterLoop(line* l);
line* LoadCharacter(char* filename,char character);
line* CreateLines(char c,uint16_t n);
line* CreateLine(char c);
line* LineScroll(line* l,uint16_t n);

