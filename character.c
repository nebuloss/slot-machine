#include "character.h"

/*
Dans ce fichier sont présentes toutes le fonctions en rapport avec l'affichage
*/

line* LoadCharacterLoop(char* characters_list_file){ //charge un fichier contenant la liste de tout les fichiers caractères
    FILE *file=fopen(characters_list_file,"r");
    if (file==NULL){
        LOGWRITE("Erreur: Impossible de charger les caractères car le programme ne parvient pas à ouvrir %s",characters_list_file);
        return NULL;
    }
    char current_line[32],current_character; //nom des fichiers max 31 caractères
    int n=0;
    line *l=NULL,*l1=NULL;
    while (fscanf(file,"%31s %c",current_line,&current_character)==2){
        if (l1==NULL){ //permier caractère
            l=LoadCharacter(current_line,current_character); 
            l1=l;
            n++;
        }
        else if (l->next=LoadCharacter(current_line,current_character)) n++; //charges les charactères puis les positionnes les uns à la suite des autres
        if (l!=NULL) while (l->next!=NULL) l=l->next;
    }
    if (l!=NULL) l->next=l1; //on raccorde le dernier caractère au premier pour former une boucle (expliqué en détail dans le rapport)
    fclose(file);
    LOGWRITE("%d caractère(s) ont été chargé(s) avec succès",n);
    return l1;
}

void FreeCharacterLoop(line* l){ //libération de la mémoire prise par une boucle de lignes (chaines de lignes fermé). Attention: si la chaine n'est pas fermé le programme va planter
    for (line *l1=l,*l2=NULL;l1!=l2;free(l2)){
        l=l->next;
        l2=l;
    }
}

line* LoadCharacter(char* filename,char character){ //fonction qui déchiffre la représentation d'un caractère stocké dans un fichier
    FILE* file=fopen(filename,"r");
    int y=0;
    if (file==NULL){ //gestions des erreurs
        LOGWRITE("Erreur: Impossible de charger le caractère %c contenu dans %s",character,filename);
        return NULL;
    }

    line *l=CreateLines(character,CHARACTER_HEIGHT),*l1=l,*l2; //allocation de mémoire pour les lignes
    for (char i=0;i<CHARACTER_MARGIN;i++) l=l->next; 

    for (char c=fgetc(file),x=0;c!=EOF;c=fgetc(file)){ //on parcoure caractère par caractère le fichier
        if (c=='\n'){
            y++;
            x=0;
            l2=l;
            for (char j=1;j<Y_MULTIPLICATION;j++){ //multiplication des lignes
                l=l->next;
                wcscpy(l->char_list,l2->char_list);
            }
            l=l->next;
            if (y==FILE_HEIGHT) break; //si le fichier est trop long, on ne prend pas en compte les lignes supplémentaires
        }else if (x<FILE_WIDTH){
            if (c==character) for (char j=0;j<X_MULTIPLICATION;j++) l->char_list[2*CHARACTER_MARGIN+X_MULTIPLICATION*x+j]=L'█'; //multiplication des colonnes
            x++;
        }
    }
    fclose(file);
    LOGWRITE("le caractère %c contenu dans %s a bien été chargé (%d ligne(s) détectée(s))",character,filename,y);
    return l1;
}

line* CreateLines(char c,uint16_t n){ //fonction qui permet d'allouer de la mémoire (indirectement)
    line* l=CreateLine(c),*l1=l; 
    for (char i=1;i<n;i++){
        l->next=CreateLine(c);
        l=l->next;
    }
    return l1;
}

line* CreateLine(char c){ //cette fonction alloue la mémoire pour une ligne
    line* l=malloc(sizeof(line));
    l->character=c;
    wmemset(l->char_list,L' ',CHARACTER_WIDTH); //on remplie d'espaces de base la lignes
    l->char_list[CHARACTER_WIDTH]=L'\0'; //caractère de fin de wide string
    l->next=NULL;
    return l; //on retourne la ligne
}

line* LineScroll(line* l,uint16_t n){ //permet de se déplacer dans la liste chainé
    for (int i=0;i<n;i++) l=l->next;
    return l;
}
