#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "log.h"

#include "character.h"

#define SCREEN_WIDTH 150 //constantes
#define SCREEN_HEIGHT 38

#define PUTPIXEL(r,g,b) printf("\e[48;2;%hhu;%hhu;%hhum  ",r,g,b) //permet d'afficher un pixel (pour l'affichage des bitmaps)
#define RESET_COLOR() printf("\e[0m")
#define SET_COLOR(fg,bg) printf("\e[38;2;%hhu;%hhu;%hhu;48;2;%hhu;%hhu;%hhum",fg.r,fg.g,fg.b,bg.r,bg.g,bg.b) //règle la couleur du cuseur
#define PRINTXY(r,x,y,format,...) printf("\e[%u;%uH"format,r.yo+y,r.xo+x,##__VA_ARGS__)

#define SET_TERMINAL_DIMENSION(width,height) printf("\e[8;%d;%dt",height,width) //dimensionne de force le terminal
#define GOTOXY(x,y) printf("\e[%d;%dH",y,x) //déplace le curseur
#define HIDE_CURSOR() printf("\e[?25l"); //cache le curseur
#define SHOW_CURSOR() printf("\e[?25h"); //affiche le curseur
#define CLEAR_SCREEN() printf("\e[2J"); //efface l'écran (scoll dans le terminal)

#define MIN2(a,b) (a<b?a:b)
#define MAX2(a,b) (a>b?a:b)
#define BETWEEN(a,n,b) MAX2(MIN2(n,b),a)

#ifndef DISPLAY_H //fichier inclue
    #define DISPLAY_H

    typedef struct color{ //structure couleur RGB256
        uint8_t r,g,b;
    }color;

    typedef struct rect{ //definition d'une structure permettant de stocker un rectangle (point d'origine, largeur, hauteur)
        uint8_t xo,yo,width,height;
    }rect;

    typedef struct BitMapHeader{ //en tête d'un fichier bitmap (taille:12 octets)
        uint32_t FileSize; //pour plus d'information sur le format bitmap, se référer à la bibliographie dans le rapport
        uint16_t Reserved1;
        uint16_t Reserved2;
        uint32_t Offset;
    }BitMapHeader;

    typedef struct BitMapInfoHeader{//partie de l'en tête qui contient les infomations de l'image (taille:40 octets)
        uint32_t Size;
        uint32_t Width;
        uint32_t Height;
        uint16_t Planes;
        uint16_t BitsPerPixel;
        uint32_t Compression;
        uint32_t ImageSize;
        uint32_t HorizontalResolution;
        uint32_t VerticalResolution;
        uint32_t ColorUsed;
        uint32_t ImportantColors;
    }BitMapInfoHeader;

#endif

#define BLUE (color){0x00,0x00,0xFF} //définition de quelques couleurs utiles
#define BLACK (color){0x00,0x00,0x00}
#define WHITE (color){0xFF,0xFF,0xFF}
#define RED (color){0xFF,0x00,0x00}
#define GREEN (color){0x00,0xFF,0x00}
#define BROWN (color){0x58,0x29,0x00}
#define LIGHT_BLUE (color){0x77,0xB5,0xFE}
#define LIGHT_RED (color){0xC0,0x40,0x40}
#define ORANGE (color){0xFC,0x93,0x03}
#define LIGHT_VIOLET (color){0xB0,0x9D,0xB9}
#define GREY (color){0x78,0x78,0x78}

#define SCREEN (rect){1,1,150,38} //surfaces utilisées dans le programmes
#define WINDOW (rect){3,2,146,36}
#define FRONT_IMAGE (rect){33,2,86,26}
#define LEFT_SIDE (rect){3,2,30,26}
#define RIGHT_SIDE (rect){119,2,30,26}
#define BOTTOM (rect){3,28,146,10}
#define TOP (rect){3,2,146,26}
#define CENTER (rect){60,15,24,5}
#define CENTER_WHEEL (rect){54,4,44,22}
#define LEFT_WHEEL (rect){7,4,44,22}
#define RIGHT_WHEEL (rect){101,4,44,22}
#define GAIN_RECT (rect){19,30,32,5}
#define MONEY_RECT (rect){55,30,40,5}
#define BET_RECT (rect){99,30,8,5}

bool DisplayBitMap(char* filename,rect r); //prototypes
void display_line(line* l,rect r);
void fill_rect(rect r,color c);
void display_number(rect r,uint16_t n,color background);
void display_09(int x,int y,uint8_t n,color background);
