#include "tree.h"

tree* LoadCombinations(char* filename){ //chargement du fichier "combination.txt"
    FILE* f=fopen(filename,"r");
    if (f==NULL){ //si on ne parvient pas à ouvrir le fichier
        LOGWRITE("Erreur: Impossible de charger les combinaisons contenues dans %s",filename);
        return NULL;
    }
    tree* t=NULL;
    int value,i=0;
    char word[4],line[100]; //3 caractères + '\0'
    while (fgets(line,100,f)){
        if (sscanf(line,"%3s %u",word,&value)==2){
            t=tree_add_value(t,word,value>1000?1000:value); //on remplie l'arbre de valeurs
            i++;
        }
    }
    fclose(f);
    LOGWRITE("%d combinaison(s) ont bien été chargé(es) (%s)",i,filename);
    return t;
}

void generate_gain_grid(char* file_dst,char* file_src){ 
    FILE *f1=fopen(file_dst,"w"); //ouverture des fichiers
    if (!f1){
        LOGWRITE("Erreur: Impossible d'ouvrir le fichier %s",file_dst);
        return;
    }
    FILE *f2=fopen(file_src,"r");
    if (!f2){
        LOGWRITE("Erreur: Impossible d'ouvrir le fichier %s",file_src);
        fclose(f1);
        return;
    }
    char line[100],word[4];
    int value,i=0;
    fputs("    ╔═════════════╦═════════════════════════════════════════╗\n",f1); //en tête de la grille
    fputs("    ║             ║                  Gain                   ║\n",f1);
    fputs("    ║ Combinaison ╠═════════════╦═════════════╦═════════════╣\n",f1);
    fputs("    ║             ║ 1er crédit  ║ 2ème credit ║ 3ème credit ║\n",f1);
    fputs("    ╠═════════════╬═════════════╬═════════════╬═════════════╣\n",f1);
    
    while (fgets(line,100,f2)){
        if (sscanf(line,"%3s %u",word,&value)==2){
            fprintf(f1,"    ║     %s     ║    %4d     ║    %4d     ║    %4d     ║\n",word,value,2*value,value>=1000?5000:3*value);
            fputs("    ╟─────────────╫─────────────╫─────────────╫─────────────╢\n",f1); //cases de la grille
            i++;
        }
    }
    LOGWRITE("Une grille de %d lignes a été généré dans le fichier %s",i,file_dst);
    fclose(f1);
    fclose(f2);
}

void free_tree(tree* t){ //libération de la mémoire prise par un arbre (très simple par récursivité)
    if (t->enter!=NULL) free_tree(t->enter);
    if (t->next!=NULL) free_tree(t->next);
    free(t);
}

/*
Les deux fonctions qui suivent sont surement les fonctions les plus complexes du programme.
Elles permettent de créer et de lire les valeurs dans un arbre. 
Il est très conseillé de lire la section du rapport sur ce sujet pour comprendre l'organisation des données.
L'avantage de cette structure est l'accès rapide à des données. Cette structure permet de représenter un dictionnaire en c.
Comme toutes les clées font la même longueur, la structure a été simplifié. On se sert de l'attribut value pour stocker les caractères et la valeurs.
On peut également utiliser n'importe quels caractères dans la clé.
La création d'une fonction de hachage est une alternative qui aurait pu être envisageable pour représenter un dictionnaire.
Nous n'avons pas choisi cette solution car elle est difficile à mettre en place (notemment pour éviter les doublons).
*/

tree* tree_get_value(tree* t,char* key){ //cette fonction permet de chercher une valeur dans un arbre. revoie NULL en cas d'échec
    if (t==NULL) return NULL; //si le bloc n'existe pas on abandonne la recherche
    if (*key=='\0') return t; // cas où on trouve le bloc
    for (tree* t1=t;t1!=NULL;t1=t1->next) if (t1->value==*key) return tree_get_value(t1->enter,key+1); //parcours de bloc à la recherche d'une lettre d'un certain rang
    return NULL; //si on ne trouve pas la lettre on retourne NULL
}

/*lire les données dans cette structure est relativement facile. Cependant créer la structure n'est pas si facile*/

tree* tree_add_value(tree* t,char* key,int value){ //fonction récursive
    if (t==NULL){ //si le bloc n'existe pas
        t=malloc(sizeof(tree)); //on alloue le bloc
        *t=(tree){NULL,NULL}; //o l'initialise
        if (*key=='\0') t->value=value; //si on est arrivé au bout de la clé on inscrit la valeur correspondante dans l'arbre
        else{
            t->value=*key; //sinon on inscrit la lettre dans l'arbre
            t->enter=tree_add_value(NULL,key+1,value); //on passe au bloc suivant qui n'existe évidemment pas
        }
        return t; //retourne le bloc
    }else{ //si le bloc existe
        if (*key=='\0'){ //si la valeur est déjà inscrite
            t->value=value; //on la remplace
            return t; //on le bloc parent de la globalité de la structure
        }else{
            for (tree* t1=t;;t1=t1->next){ //boucle for infinie (on déclare une variable locale à la boucle)
                if (t1->value==*key){ //recherche de la lettre dans l'arbre préexistant
                    t1->enter=tree_add_value(t1->enter,key+1,value);
                    return t;
                }
                if (t1->next==NULL){ //si la lettre n'existe pas, on l'ajoute
                    t1->next=tree_add_value(NULL,key,value);
                    return t;
                }
            }
        }
    }
}