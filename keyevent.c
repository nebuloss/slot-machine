#include "keyevent.h"

KeyEvent Action(KeyEvent e){ //cette fonction est juste un raccourci
    return e.action(e.arg);
}

KeyEvent Increment(int* n){ //cette fonction permet d'incrémenter l'argument (utile pour les menu car permet de changer la sélection facilement)
    (*n)++;
    return (KeyEvent){0,NULL,NULL};
}

KeyEvent Decrement(int* n){ //décrémente l'argument
    (*n)--;
    return (KeyEvent){0,NULL,NULL};
}

char Lower(char c){//permet de passer un caractère majuscule en minuscule
    if (c>='A' && c<='Z') c+='a'-'A';
    return c;
}

KeyEvent WaitForKeyEvent(KeyEvent* EventList,uint8_t n){ //fonction principale dans la détection des touches (bloquante)
    if (EventList==NULL) return QUIT_EVENT; //QUIT_EVENT est défini dans keyevent.h
    for (char c;;){
        c=Lower(getchar());
        for (int i=0;i<n;i++) if (c==EventList[i].key) return EventList[i]; //si on trouve la touche on quitte la fonction
    }
}