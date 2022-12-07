#include "log.h"

FILE* logfile; //variable globale qui correspond au flux du fichier log

int LogWrite(FILE* f,char* format,...){ //permet d'écrire dans le fichier log
    time_t t;
    struct tm *now;
    va_list v;

    if (f!=NULL){
        t=time(&t); //on récupère l'heure
        now=localtime(&t);
        fprintf(f,"%4d-%02d-%02d %02d:%02d:%02d> ",now->tm_year+1900,now->tm_mon+1,now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec);
        va_start(v,format); //utilisation des va_args pour gérer les arguments à nombres variables
        vfprintf(f,format,v);
        va_end(v); //libération des ressources des va_args
        putc('\n',f); //retour à la ligne
        return 0;
    }
    return -1;
}

void OpenLog(FILE** f,char* mode){ //ouverture du fichier log
    *f=fopen(LOG_FILE_NAME,mode);
}

void CloseLog(FILE* f){
    if (f!=NULL) fclose(f); //fermeture du fichier log
}
