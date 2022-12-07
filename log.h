#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>

#define LOG_H //constante pour indiqué que le fichier log.h a déjà été inclue
#define LOG_FILE_NAME "data/machine.log" //nom du fichier log
#define LOGWRITE(format,...) LogWrite(logfile,format,##__VA_ARGS__) //macro pour rendre plus simple l'utilisation des fichiers log
#define OPENLOG() OpenLog(&logfile,"w")
#define APPENDLOG() OpenLog(&logfile,"a")
#define CLOSELOG() CloseLog(logfile)

extern FILE* logfile; //lorsque qu'on inclue log.h on a accès à la variable logfile

int LogWrite(FILE* f,char* format,...); //déclarations des fonctions contenue dans log.c
void OpenLog(FILE** f,char* mode);
void CloseLog(FILE* f);
