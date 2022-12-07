#include "main.h"

/*
Dans ce fichier sont présentes les fonctions permettant l'interaction entre l'utilisateur et le programme.
*/

/*
La fonction main paramètre les propriété du terminal necessaire à un bon fonctionnement du programme.
Elle gère le bon enchaînement des autres fonctions. Dès qu'une fonction retourne le QUIT_EVENT,
la fonction arrête le programme en rétablissant les paramètres initials.
*/

int main(){
    OPENLOG(); //ouverture du fichier log.
    LOGWRITE("démarrage du programme..."); //écriture dans le fichier log
    system("stty raw -echo"); //changement des paramètres de la fenêtre
    SET_TERMINAL_DIMENSION(SCREEN_WIDTH,SCREEN_HEIGHT); //on redimensionne le terminal
    HIDE_CURSOR();
    setlocale(LC_CTYPE,""); //permet l'aafichage de tout les caractères y compris les caractères "larges"
    srand(time(NULL)); //initialisation de la génération des nombres aléatoires
    LOGWRITE("initialisation terminée");

    DisplayBitMap("data/start.bmp",SCREEN); //affichage de l'image de départ

    SET_COLOR(WHITE,BLACK); 
    PRINTXY(SCREEN,50,0,"\e[1m---Machine à sous - David Giraudeau - Guillaume Chayé---\e[0m");
    getchar(); //attente d'une touche
    SET_COLOR(BLACK,WHITE);
    
    for (KeyEvent e={0,Main_Menu,NULL};e.action!=NULL;e=Action(e)); //boucle principale

    system("stty cooked echo"); //on rétablit les paramètres précédents de la fenêtre
    SHOW_CURSOR();
    RESET_COLOR();
    CLEAR_SCREEN();
    GOTOXY(1,1);
    LOGWRITE("Fin du programme");
    CLOSELOG(); //fermeture du fichier log
    return 0;
}

/*
Cette fonction affiche un écran bleu ainsi qu'un message d'erreur.
Après avoir appuyé sur une touche, on sort de la fonction en retournant le QUIT_EVENT (sortie du programme)
*/

KeyEvent Crash(char* arg){ //fenêtre en cas d'erreur.
    fill_rect(WINDOW,BLUE); //écran bleu
    SET_COLOR(WHITE,BLUE);
    PRINTXY(WINDOW,1,1,"%s",arg); //on affiche le message
    LOGWRITE("Erreur: %s",arg); //on inscrit dans le fichier log l'erreur
    PRINTXY(WINDOW,1,2,"Appuyez sur une touche pour quitter le programme.");
    getchar(); //attente d'une touche
    return QUIT_EVENT;
}

/*
Cette fonction permet l'affichage et le chargement des différents thèmes.
L'utilisateur choisit son thème puis appuie sur entrer pour valider ou q pour quitter le programme
*/
KeyEvent Main_Menu(random_wheel* rw){
    KeyEvent e={0,Crash,"Erreur: aucun paquet disponible."}; //valeur de départ. On considère que le paquet n'est pas chargé initialement
    struct dirent *de,*DirectoryList[10]; //déclarations des variables
    int nbdir=0,loadpackage,loadimage,i=0;

    LOGWRITE("Appel du menu principal");
    if (chdir("themes")) return (KeyEvent){0,Crash,"Erreur: impossible de trouver le répertoire des paquets."}; //si le dossier thème n'existe pas on quitte le programme
    LOGWRITE("Dossier 'theme' ouvert avec succès");

    DIR* themes=opendir("."); //ouvre le dossiers des thèmes
    LOGWRITE("Parcours des sous dossiers (MAX 10)");
    while ((de=readdir(themes))!=NULL){ //boucle qui liste les dossiers
        if (de->d_type==DT_DIR && de->d_name[0]!='.'){
            DirectoryList[nbdir]=de;
            LOGWRITE("%d/10:%s",nbdir+1,de->d_name);
            nbdir++;
            if (nbdir==10) break;
        }
    }

    if (nbdir!=0){ //si on trouve des dossiers
        if (rw==NULL){ //si la roue n'est pas définie, on l'alloue dynamiquement et on l'initialise
            LOGWRITE("Aucune roue préexistante. Creation d'une nouvelle");
            rw=malloc(sizeof(random_wheel));
            *rw=(random_wheel){NULL,NULL};
            GetAmountMoney(rw); //on demande à l'utilisateur l'argent initial qu'il veut mettre dans la machine
        }
        KeyEvent EventList[6];
        EventList[0]=QUIT_EVENT;
        EventList[1]=(KeyEvent){'s',Increment,&i};
        EventList[2]=(KeyEvent){'p',Decrement,&i};
        EventList[3]=(KeyEvent){'l',FileWindow,LOG_FILE_NAME};
        EventList[4]=(KeyEvent){'i',FileWindow,"data/help.txt"};
        EventList[5]=(KeyEvent){'\r',NULL,rw};

         //liste des évènements claviers

        LOGWRITE("Affichage du menu principal");
        DisplayBitMap("../data/right.bmp",RIGHT_SIDE); //affichage des images
        DisplayBitMap("../data/left.bmp",LEFT_SIDE);
        fill_rect(BOTTOM,WHITE);
        SET_COLOR(BROWN,WHITE);
        PRINTXY(LEFT_SIDE,1,20,"[Touche p] précédent");
        PRINTXY(RIGHT_SIDE,10,20,"[Touche s] suivant");
        PRINTXY(BOTTOM,30,1,"Bienvenue! Veuillez choisir votre thème puis appuyez sur [Touche entrer].");
        PRINTXY(BOTTOM,20,2,"[Touche q]: quitter le programme, [touche l]: consulter le fichier log, [Touche i]: aide");

        while (1){ //boucle infinie
            CloseRandomWheel(rw);
            loadimage=1; //variables permettant de suivre le bon chargement du paquet
            loadpackage=1;
            chdir(DirectoryList[i]->d_name); //on change de répertoire courant

            LOGWRITE("Chargement du paquet:%s",DirectoryList[i]->d_name);
            loadpackage=(rw->combinations=LoadCombinations("combinations.txt")) && loadpackage; //fonctions de tree.c
            generate_gain_grid("../../data/grid.txt","combinations.txt");
            loadpackage=(rw->start=LoadCharacterLoop("characters.txt")) && loadpackage; //fonctions de character.c
            loadimage=DisplayBitMap("image.bmp",FRONT_IMAGE);
            chdir(".."); //on revient dans le dossier thème

            SET_COLOR(RED,WHITE);
            PRINTXY(BOTTOM,30,5,"Thème sélectionné:%20s",DirectoryList[i]->d_name);

            if (loadpackage==false){ //échec du chargement du paquet
                system("play -q ../data/error.wav 2>/dev/null &");
                PRINTXY(BOTTOM,80,5,"[Echec du chargement]");
                LOGWRITE("Erreur: Echec du chargement du paquet %s",DirectoryList[i]->d_name);
                DisplayBitMap("../data/error.bmp",FRONT_IMAGE);
                e=WaitForKeyEvent(EventList,5); //touche entrer interdite (touche='\r' -> retour chariot)
            }else{
                PRINTXY(BOTTOM,80,5,"[Chargé avec succès ]");
                if (loadimage==false) DisplayBitMap("../data/default.bmp",FRONT_IMAGE); //échec du chargement de l'image (optionnel)
                LOGWRITE("Le paquet %s a bien été chargé",DirectoryList[i]->d_name);
                e=WaitForKeyEvent(EventList,6); //touche entrer autorisée (inscrite en fin de liste)
            }
            if (e.action==FileWindow){
                chdir("..");
                CLOSELOG(); //sauve l'état actuel du fichier log
                Action(e);
                APPENDLOG(); //réouvre en mode append le fichier log
                return (KeyEvent){0,Main_Menu,rw}; //refresh
            }
            if (e.action!=NULL) Action(e);
            else break;
            i=BETWEEN(0,i,nbdir-1);
        }
        if (e.key=='q'){
            CloseRandomWheel(rw);
            free(rw); //si l'utilisateur choisi de quitter le programme on libère la mémoire prise par la roue
        }
        else e.action=Animation;
    }
    closedir(themes); //libération des ressources prises lors du parcours du dossier
    chdir(".."); //on revient dans le dossier parent du programme
    return e;
}

/*
Cette fonction permet de demander à l'utilisateur de choisir son argent initial
*/
void GetAmountMoney(random_wheel* rw){
    LOGWRITE("Execution du menu 'Choisir son nombre de crédit initial'");
    int a=3,b=0,c=0,n=300;
    KeyEvent EventList[7],e; //liste des évènements claviers
    EventList[0]=(KeyEvent){'s',Increment,&a};
    EventList[1]=(KeyEvent){'z',Decrement,&a};
    EventList[2]=(KeyEvent){'d',Increment,&b};
    EventList[3]=(KeyEvent){'e',Decrement,&b};
    EventList[4]=(KeyEvent){'f',Increment,&c};
    EventList[5]=(KeyEvent){'r',Decrement,&c};
    EventList[6]=(KeyEvent){'\r',NULL,NULL}; //retour chariot. /!\ '\r'!='\n'
    fill_rect(WINDOW,WHITE); //remplie toute la fenêtre d'un fond blanc
    SET_COLOR(RED,WHITE);
    PRINTXY(CENTER,-20,-6,"Veuillez sélectionner votre argent de départ puis appuyez sur entrer.");
    SET_COLOR(GREY,WHITE);
    PRINTXY(CENTER,4,-2,"[Touche z] -100$");
    PRINTXY(CENTER,4,-3,"[Touche e] -10$");
    PRINTXY(CENTER,4,-4,"[Touche r] -1$");
    PRINTXY(CENTER,4,6,"[Touche s] +100$");
    PRINTXY(CENTER,4,7,"[Touche d] +10$");
    PRINTXY(CENTER,4,8,"[Touche f] +1$");

    while (1){ //boucle principal
        display_number(CENTER,n,WHITE); //affichage en grand des chiffres style 7 segements (display.c)
        e=WaitForKeyEvent(EventList,7);
        if (e.action!=NULL) Action(e);
        else break;
        a=BETWEEN(0,a,9);
        b=BETWEEN(0,b,9);
        c=BETWEEN(0,c,9);
        n=a*100+b*10+c;
    }
    rw->money=n;
    LOGWRITE("Le nombre de crédit a été initialisé à %u$",rw->money);
    system("play -q ../data/insert-coins.wav 2>/dev/null &");
}

/*
Cette fonction affiche le menu pour choisir si l'utilisateur veut mettre les animations
*/
KeyEvent Animation(random_wheel* rw){
    LOGWRITE("Menu animation");
    KeyEvent e,EventList[2]={(KeyEvent){'o',Game,rw},(KeyEvent){'n',Game,rw}};//évènements
    DisplayBitMap("data/animation.bmp",WINDOW); //image
    e=WaitForKeyEvent(EventList,2);
    if (e.key=='o') rw->animation=true;
    else rw->animation=false;
    return e; //retourne l'évènements
}

/*
Cette fonction permet d'afficher un fichier sur le terminal. On peut le parcourir dans l'ordre.
Cependant il est assez compliqué de revenir en arrière dans un fichier car il faudrait mettre en place un sustème pour stocker
les offsets des différentes pages. Pour simplifier le problème la touche p revient au début du fichier
*/

KeyEvent FileWindow(char* filename){
    KeyEvent EventList[3]={(KeyEvent){'p'},QUIT_EVENT,(KeyEvent){'s'}},e={0,NULL}; //listes des évènements
    fill_rect(WINDOW,WHITE);
    fill_rect((rect){WINDOW.xo,WINDOW.yo,WINDOW.width,2},ORANGE); //affichage
    PRINTXY(WINDOW,0,0,"fichier: '%s'",filename);
    PRINTXY(WINDOW,0,1,"[Touche q]=quitter, [Touche s]=suivant, [Touche p]=Retour au début");
    SET_COLOR(BLACK,WHITE);

    FILE* f=fopen(filename,"r"); //ouverture du fichier
    if (f==NULL){
        PRINTXY(WINDOW,0,2,"Erreur. Pressez une touche pour retourner à la page précédente");
        getchar();
        return QUIT_EVENT;
    }
    char* line=malloc(WINDOW.width*3); //allocation d'une chaine de la largeur d'une ligne (x3 en cas d'ascii étendu)

    while (e.key!='q'){ //boucle de gestionde la fenêtre
        for (int y=2;y<WINDOW.height-1;y++){
            if (fgets(line,WINDOW.width*3,f)) PRINTXY(WINDOW,1,y,"%s",line); //récupère une ligne d'un fichier (max 99 caractères)
            else break;
        }
        e=WaitForKeyEvent(EventList,3);
        if (e.key=='p') fseek(f,0,SEEK_SET);
        fill_rect((rect){WINDOW.xo,WINDOW.yo+2,WINDOW.width,WINDOW.height-2},WHITE); //efface l'afichage du fichier
        SET_COLOR(BLACK,WHITE);
    }
    free(line);
    fclose(f);
    return e;
}

/*
Fenêtre de jeu. Gère le déroulement complet du jeux de la machine à sous
*/
KeyEvent Game(random_wheel* rw){
    line* wheel3[3]={rw->start,rw->start,rw->start}; //déclaration des variables
    rect wheelpos[3]={LEFT_WHEEL,CENTER_WHEEL,RIGHT_WHEEL};
    tree* t;
    unsigned int bet=1;
    KeyEvent e,EventList[5];
    EventList[0]=(KeyEvent){'q',Main_Menu,rw};
    EventList[1]=(KeyEvent){'a',Increment,&bet};
    EventList[2]=(KeyEvent){'z',Decrement,&bet};
    EventList[3]=(KeyEvent){'\r'};
    EventList[4]=(KeyEvent){'i',FileWindow,"data/grid.txt"};

    LOGWRITE("Démarrage du jeu avec les animations %s",(rw->animation?"activées":"désactivées"));
    fill_rect(TOP,BROWN); //affichage
    fill_rect(BOTTOM,WHITE);
    display_number(MONEY_RECT,rw->money,GREY);
    display_number(GAIN_RECT,0,GREY);
    SET_COLOR(ORANGE,WHITE);
    for (int i=0;i<3;i++) display_line(wheel3[i],wheelpos[i]);//affichage des caractères inititials sur la roue
    SET_COLOR(RED,WHITE);
    PRINTXY(GAIN_RECT,15,-1,"Gain");
    PRINTXY(MONEY_RECT,15,-1,"Crédit(s)");
    PRINTXY(BET_RECT,2,-1,"Mise");
    PRINTXY(BET_RECT,12,0,"[Touche q] Retour au Menu");
    PRINTXY(BET_RECT,12,1,"[Touche a] Ajouter 1 à la mise");
    PRINTXY(BET_RECT,12,2,"[Touche z] Retirer 1 à la mise");
    PRINTXY(BET_RECT,12,3,"[Touche entrer] Lancer le tirage");
    PRINTXY(BET_RECT,12,4,"[Touche i] Voir les combinaisons");

    while (rw->money>0){ //tant que l'utilisateur a de l'argent
        display_number(BET_RECT,bet,GREY); //si l'utilisateur a de quoi miser
        
        e=WaitForKeyEvent(EventList,5); //attente d'un évènement
        if (e.action==Main_Menu) return e;
        if (e.key!='\r'){
            Action(e);
            if (e.action==FileWindow) return (KeyEvent){0,Game,rw}; //refresh
            bet=BETWEEN(1,bet,3);
            continue;
        }
        
        display_number(MONEY_RECT,(rw->money-=bet),GREY);
        display_number(GAIN_RECT,0,GREY);

        if (rw->money-bet<0){
            system("play -q ../data/error.wav 2>/dev/null &");
            continue;//on retourne à la boucle while
        }
        system("play -q data/random-draw.wav 2>/dev/null &");
        if (rw->animation) random_wheel_generate_on(wheel3,wheelpos); //fonction qui génère avec animations
        else random_wheel_generate_off(wheel3,wheelpos);//fonction qui génère sans animations
        t=tree_get_value(rw->combinations,(char[]){wheel3[0]->character,wheel3[1]->character,wheel3[2]->character,'\0'});
        if (t!=NULL){ //en cas de gain (si la combinaison est trouvé dans l'arbre)
            if (t->value==1000){ //jackpot
                system("play -q data/jackpot.wav 2>/dev/null &");
                color c1,c2;
                for (int i=0;i<=2500;i+=20){
                    usleep(40*1000);
                    c1=(color){100,i%256,100}; //animation qui fait varier les couleurs
                    c2=(color){255,255,i%256};
                    SET_COLOR(c1,c2);
                    for (int j=0;j<3;j++) display_line(wheel3[j],wheelpos[j]);
                    display_number(GAIN_RECT,2*i,GREY);
                }
                rw->money+=5000;
            }else{ //le gain est multiplié par la mise à l'exception du jackpot où on a 1000 2000 et 5000
                system("play -q data/gain.wav 2>/dev/null &");
                for (int i=1;i<=bet;i++){
                    usleep(300*1000);
                    display_number(GAIN_RECT,i*t->value,GREY);//affichage du gain
                }
                rw->money+=bet*t->value;
            }
            display_number(MONEY_RECT,rw->money,GREY); //actualisation de la monnaie de l'utilisateur
        }
    }
    CloseRandomWheel(rw); //libération des ressources
    free(rw);
    return (KeyEvent){0,Crash,"Vous n'avez plus d'argent!"}; //l'utilisateur n'a plus d'argent
}

void random_wheel_generate_on(line* llist[3],rect rlist[3]){ //génération avec animations
    double k[3]={0.5,0.45,0.4},vo[3],t=0; //fait appel à un modèle physique très simple. Force de frottements F=-alpha*v
    uint32_t stop[3],position[3]={0,0,0},nstart=3,distance[3]; //initialisation de variables
    bool start[3]={true,true,true};

    for (int i=0,n;i<3;i++){
        n=rand()%6;
        stop[i]=(20+10*i+n)*(CHARACTER_HEIGHT);
        vo[i]=(stop[i]+(4-i)*(CHARACTER_HEIGHT))*k[i];
    }

    SET_COLOR(ORANGE,WHITE);
    LOGWRITE("Lancement de la roue avec vitesse initiale:[%.3lf;%.3lf;%.3lf]",vo[0],vo[1],vo[2]);

    while (nstart>0){
        for (int i=0;i<3;i++){
            if (position[i]<stop[i]){
                display_line(llist[i],rlist[i]);
                distance[i]=(vo[i]/k[i])*(1-exp(-k[i]*t))-position[i]; //la position doit être extrèmemnt précise
                llist[i]=LineScroll(llist[i],distance[i]); //on se déplace dans les lignes (character.c)
                position[i]+=distance[i];
            }else if (start[i]){
                nstart--;
                distance[i]=stop[i]-position[i];
                LineScroll(llist[i],distance[i]);
                display_line(llist[i],rlist[i]);
                start[i]=false;
            }
        }
        t+=0.016;
        usleep(16*1000);
    }
    LOGWRITE("La roue est arrêté en [%u;%u;%u]",stop[0],stop[1],stop[2]); //informations pratiques écrites dans le fichier log
}

void random_wheel_generate_off(line* llist[3],rect rlist[3]){ //sans animations
    LOGWRITE("Lancement de la roue sans animations"); //on recouvre les chiffres d'un rectangle en blanc
    for (int i=0;i<3;i++){
        fill_rect(rlist[i],WHITE);
        usleep(400*1000); //400ms de pause
    }
    SET_COLOR(ORANGE,WHITE);
    for (int i=0;i<3;i++){
        llist[i]=LineScroll(llist[i],(rand()%6)*(CHARACTER_HEIGHT)); //affichage d'un chiffre random
        display_line(llist[i],rlist[i]);
        usleep(400*1000);
    }
}

/*
Cette fonction libère toute les ressources prises par un roue
*/

void CloseRandomWheel(random_wheel* rw){
    LOGWRITE("Tentative de fermeture du paquet actuel");
    if (rw->combinations!=NULL) free_tree(rw->combinations); //libération des combinaisons
    if (rw->start!=NULL) FreeCharacterLoop(rw->start); //libération des caractères
    rw->combinations=NULL; //on remet les ponters à NULL étant donnée que les ressources ont été libérées
    rw->start=NULL;
}
