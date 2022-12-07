#include "display.h"

/*
Toutes les fonctiosn en rapport avec l'aafichage sont présentes dans ce fichier
*/

static rect rect_tab[7]={ //rectangle de l'affichage 7 segment
    (rect){0,0,6,1},
    (rect){5,0,1,3},
    (rect){5,2,1,3},
    (rect){0,4,6,1},
    (rect){0,2,1,3},
    (rect){0,0,1,3},
    (rect){0,2,6,1},
};

static uint8_t l_tab[10]={ //tableau qui stocke l'état de chaque segments
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00000111,
    0b01111111,
    0b01101111
};

void display_number(rect r,uint16_t n,color background){ //affichage d'un nombre centré dans le rectangle
    fill_rect(r,background);
    int l=r.width/8-1,mx=(r.width%8+2)/2,my=(r.height-5)/2;
    for (;l>=0;l--){
        display_09(r.xo+8*l+mx,r.yo+my,n%10,background);
        n/=10;
    }
}

void display_09(int x,int y,uint8_t n,color background){ //permet d'afficher un chiffre en 7 segment
    n%=10; //n doit être entre 0 et 9
    n=l_tab[n];
    rect r;
    for (uint8_t i=0;i<7;i++){
        r=rect_tab[i];
        if ((n>>i)&1) fill_rect((rect){x+r.xo,y+r.yo,r.width,r.height},GREEN);
    }
}

/*
Le problème pricipal est dans l'affichage du block-charactère n°219 dans la table ascii. 
La seule solution que nous avons trouvé est de passer par les caractères larges.
Cependant dans un programme sur terminal: soit tout les caractères affichées sont larges, soit ils sont ascii.
Mais on ne peut pas utiliser les fonctions d'affichage de caractère larges (ex: wprintf) puis un printf dans le même programme.
*/
void display_line(line* l,rect r){ //affichage de lignes dans un rectangle (pour l'affichage des caractères)
    for (int i=0;i<r.height;i++){ 
        PRINTXY(r,0,i,"%ls\n",l->char_list); //convertie une chaine de caractères puis l'affiche
        l=l->next;
    }
}

void fill_rect(rect r,color c){ //permet de remplir une zone d'une certaine couleur
    SET_COLOR(WHITE,c);//on règle la couleur du curseur
    for (uint8_t x,y=0;y<r.height;y++){
        GOTOXY(r.xo,r.yo+y);
        for (x=0;x<r.width;x++) putchar(' ');
    }
    putchar('\n'); //le \n permet d'actualiser l'affichage sur le terminal
}

bool DisplayBitMap(char* filename,rect r){
    uint16_t magic,pitch;
    BitMapHeader header;
    BitMapInfoHeader info;

    FILE* file=fopen(filename,"rb"); //lecture binaire du fichier
    if (file==NULL){
        LOGWRITE("Erreur: impossible d'ouvrir l'image %s",filename);
        return false; //le fichier n'existe pas
    }

    fread(&magic,2,1,file);
    if (magic!=0x4d42){
        LOGWRITE("Erreur: le fichier %s n'est pas un bitmap",filename);
        return false; //les caractères BM doit être présent au début de chaque bitmap ('B'=0x42,'M'=0x4d)
    }

    fread(&header,sizeof(BitMapHeader),1,file); //lecture de l'en tête du fichier
    fread(&info,sizeof(BitMapInfoHeader),1,file);

    uint8_t BytePerPixel=info.BitsPerPixel/8,pixel[4]; //déclaration de quelques variables necessaire à la lecture
    pitch=info.ImageSize/info.Height;
    r.width=MIN2(r.width/2,info.Width);
    r.height=MIN2(r.height,info.Height);
    if (BytePerPixel<3){
        LOGWRITE("Erreur: Le programme ne supporte pas ce type de bitmap (%s)",filename);
        return false; //supporte que les pixels sur 32 bits et 24 bits
    }

    for (int i=0;i<r.height;i++){ //parcours des lignes et colonnes
        fseek(file,header.Offset+(info.Height-i-1)*pitch,SEEK_SET); //on se positionne aux début des données d'une ligne de pixels
        GOTOXY(r.xo,r.yo+i);
        for (int j=0;j<r.width;j++){
            fread(pixel,1,BytePerPixel,file);//lecture
            PUTPIXEL(pixel[2],pixel[1],pixel[0]); //affichage retounée
        }
    }

    fclose(file); //fermeture du fichier
    LOGWRITE("Affichage avec succès du bitmap %s",filename);
    return true;
}
