#include <stdio.h>
#include <stdlib.h>
#include "log.h"

#ifndef TREE_H
    #define TREE_H //module inclue
    typedef struct tree{ //déclaration de la structure
        struct tree *next,*enter;
        int value;
    }tree;
#endif

tree* tree_add_value(tree* t,char* key,int value); //prototypes
tree* tree_get_value(tree* t,char* key);
void free_tree(tree* t);
tree* LoadCombinations(char* filename);
void generate_gain_grid(char* file_dst,char* file_src);
