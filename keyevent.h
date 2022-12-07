#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define KEYEVENT_H //le fichier a été inclue

typedef struct KeyEvent{ //structure qui attribue à une touche une action. L'argument permet de lancer la fonction action avec un argument
    char key;
    struct KeyEvent (*action)();
    void *arg;
}KeyEvent;

#define QUIT_EVENT (KeyEvent){'q',NULL} //en général on choisi la touche q pour quitter

KeyEvent WaitForKeyEvent(KeyEvent* EventList,uint8_t n); //prototypes de fonctions
KeyEvent Action(KeyEvent e);
KeyEvent Increment(int* n);
KeyEvent Decrement(int* n);
char Lower(char c);
