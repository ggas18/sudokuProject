/******************************************************************
ENSEM Nancy, INPL, Université de Lorraine
Projet 2A ISN

Professeur    Abdelkader    LAHMADI
              Jean-Philippe MANGEOT
              YeQuiong      SONG

Etudiant:     Arouna        GANOU

Version: V4 13/05/2017

*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h> /* getprotobyname */
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>

// Les en-têtes des fonctions
#include "sudoku.h"

/**
Création de la mémoire tampon
**/

jeuGlob buffer_jeu={.mutex=PTHREAD_MUTEX_INITIALIZER};

condGlob buffer_stat={.mutex=PTHREAD_MUTEX_INITIALIZER,
                      .cond=PTHREAD_COND_INITIALIZER,
                      .condition=0};

restGlob Reste={.mutex=PTHREAD_MUTEX_INITIALIZER};

/**
INITIALISATION DES CONDITIONS ET DES MUTEXS
**/



int main(int argc, char* argv[])
{   if(argc==1) printf("\nIl faut passer des paramètres. Voici les options offertes:"
                       "\n-standalone:   lancer le programme en mode basique"
                       "\n-multhithread: lancer le programme en mode multi-thread"
                       "\n-server:       lancer le programme en mode serveur");
    /********************************************
    Utilisation de la version basique du solveur
    ********************************************/
    else if(strcmp(argv[1],"standalone") == 0)
    {
    char nomFichier[255];
    printf("\nSaisir le nom du fichier\n");
    scanf("%s",nomFichier);
    int tableau[TAILLE][TAILLE];
    char methode[lgMethode];
    lectureFichier(tableau,nomFichier,methode);
    int algo;
    printf("Choisir une methode, en saisisant 1 ou 2: \n 1.BRUTE-FORCE \n 2.BACKTRACKING\n");
    scanf("%d",&algo);
    if( algo==1){
        int nbPlacesVides=0;
        int PlacesVides[TAILLE*TAILLE][2];
        nbPlacesVides=avoirPlacesVides(tableau,PlacesVides);
        if(resoudreSudokuBF(tableau,PlacesVides,nbPlacesVides)==0){
            char nomFichierSol[255]="./SudokusResolusStanalone/SolutionBF.txt";
            char method[lgMethode]="BRUTE-FORCE";
            ecrireSolution(tableau,method,nomFichierSol);
            } else
                    {

                    printf("Aucune solution\n");
                    }

                }
  else if(algo==2){
       if(remplirGrilleBT(tableau)==1){
            char nomFichierSol[255]="./SudokusResolusStandalone/SolutionBT.txt";
            char method[lgMethode]="BACKTRACKING";
            ecrireSolution(tableau,method,nomFichierSol);
       }
       else
            {
                printf("Pas de solution\n");
            }

            }
  else
        {
         printf("Methode inconnue\n");
         }
   }
  /********************************************
    Utilisation de la version avec threads du solveur
    ********************************************/
  else if(strcmp(argv[1],"multithread") == 0)
    { // On demande le nombre de fichier
      int err;
      int nbFichier;
      printf("\nSaisir le nombre de fichier:\n");
      scanf("%d",&nbFichier);
      Reste.reste=nbFichier;  // C'est le nombre de sudoku restant à résoudre, car on a une grille par fichier.
      Jeu jeux[nbFichier];
      Jeu jeu;
      int i;
      char nomFichier[255];
      for(i=0;i<nbFichier;i++)
            {
            printf("\nSaisir le nom du fichier:\n");
            err=scanf("%s",nomFichier);
            strcpy(jeu.nom,nomFichier);
            lectureFichier(jeu.grille,nomFichier,jeu.methode);
            jeux[i]=jeu;

            }
      /************************************************************************
      Creation des threads de resolutions.
      *************************************************************************/
      int nbThread=nbFichier;
      pthread_t thread_ecriture;
      pthread_t threads_resolution[nbThread];
      for(i=0;i<nbThread;i++)
            {
             err=pthread_create(&(threads_resolution[i]),NULL,&resoudre,(void*)&(jeux[i]));
             if(err) printf("\nErreur creation de thread %d\n",i);

            }
      err=pthread_create(&thread_ecriture,NULL,&ecrire,(void*)NULL);
      if(err) printf("\nErreur creation de thread ecriture\n");

      err=pthread_join(thread_ecriture,NULL);
      if(err) printf("\nErreur join de thread ecriture\n");
      }
  /********************************************
    Utilisation de la version professionnelle du solveur
    ********************************************/
  else if(strcmp(argv[1],"server") == 0)
    {
     int V_shutdown;
    int ent_re;
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000]="Recu";
    Jeu *jeu_s=malloc(sizeof(Jeu));
    char nomFichier_jeu_s[2000]="test.txt";

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 2018 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");
   int nb_jeu=0;
   read(client_sock,&nb_jeu,sizeof(nb_jeu));
 while(nb_jeu>0){
    //Receive a message from client
    char methode[255];
    int tableau[TAILLE][TAILLE];
    int ligne=0, colonne=0;
    pthread_t thread_resolution, thread_envoie;
    Jeu *jeu_recu=malloc(sizeof(Jeu));

    while( (ligne<TAILLE)&&(read_size = read(client_sock ,&ent_re , sizeof(int))) > 0 )
    {

        (jeu_recu->grille)[ligne][colonne]=ent_re;

        if(colonne==8){
            ligne++;
            colonne=0;
        } else colonne++;

    }
    read_size =read(client_sock,methode,sizeof(methode));

    strcpy(jeu_recu->methode,methode);

    pthread_create(&thread_resolution,NULL,resoudreServer,(void*)jeu_recu);
    pthread_join(thread_resolution,NULL);

    ecrireSolution(jeu_recu->grille,jeu_recu->methode,nomFichier_jeu_s);

    Collis *collis=malloc(sizeof(Collis));
    collis->jeu=*jeu_recu;
    collis->sock=client_sock;

    // On envoie la solution au client
    pthread_create(&thread_envoie,NULL,envoyer,(void*)collis);
    pthread_join(thread_envoie,NULL);

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    free(jeu_recu);
    nb_jeu--;
    }


    V_shutdown=shutdown(client_sock,SHUT_RDWR );
	if (V_shutdown==-1)
	{
		perror("Problem of end  of session\n");
		exit(-4);
	}

close(client_sock);

    return 0;

    }
  /*********************************************
   Message d'erreur sur les options de l'utilisateur.
    ********************************************/
  else printf("\nOptions inconnues ou non supportées\n\n");

  return 0;
}


/*************************************************************************************
Routines des threads pour la version multithread
*************************************************************************************/
void* resoudre(void *arg)
    { Jeu *jeu=(Jeu*)arg;

      char BF[]="BRUTE-FORCE";
      char BT[]="BACKTRACKING";
      int err;
      if(strcmp(jeu->methode,BF)==0)
      {
            int nbPlacesVides=0;
            int PlacesVides[TAILLE*TAILLE][2];
            nbPlacesVides=avoirPlacesVides(jeu->grille,PlacesVides);
            /** On a fini de résoudre, il doit écriture la solution dans une variable à laquelle
            le thread d'écriture a accès. Ensuite, il faut envoyer le signal au thread
            d'écriture.
            **/
            if(resoudreSudokuBF(jeu->grille,PlacesVides,nbPlacesVides)==0){
            // On doit modifier la variable globale jeu_ecrit; donc on bloque l'accès.
            err=pthread_mutex_lock(&buffer_jeu.mutex);
            if(err) printf("\n Erreur lock mutex_jeu_ecrit\n");
            buffer_jeu.jeu=*jeu;// Il a met sa solution dans le tampon

            err=pthread_mutex_lock(&buffer_stat.mutex);
            if(err) printf("\n Erreur lock mutex_cond\n");
            err=pthread_cond_signal(&buffer_stat.cond);
            if(err) printf("\n Erreur signal\n");
            printf("\nSolution grille %s envoyée pour écriture\n",buffer_jeu.jeu.nom);
            buffer_stat.condition=1;
            err=pthread_mutex_unlock(&buffer_stat.mutex);

            if(err) printf("\n Erreur unlock mutex_cond\n");
            pthread_exit(NULL);

            } else
                    {
                    err=pthread_mutex_lock(&buffer_jeu.mutex);
                    if(err) printf("\n Erreur lock jeu buffer\n");
                    buffer_jeu.jeu=*jeu;

                    err=pthread_mutex_lock(&buffer_stat.mutex);
                    if(err) printf("\n Erreur lock mutex_cond\n");
                    err=pthread_cond_signal(&buffer_stat.cond);
                    buffer_stat.condition=1;
                    if(err) printf("\n Erreur signal BT\n");
                    err=pthread_mutex_unlock(&buffer_stat.mutex);

                    err=pthread_mutex_lock(&Reste.mutex);
                    if(err) printf("\n Erreur lock mutex_reste\n");
                    Reste.reste=Reste.reste-1;
                    printf("Aucune solution pour %s\n",buffer_jeu.jeu.nom);
                    err=pthread_mutex_unlock(&Reste.mutex);
                    if(err) printf("\n Erreur unlock mutex_reste\n");
                    pthread_exit(NULL);
                    }
      }
      if(strcmp(jeu->methode,BT)==0)
      {
            /** On a fini de résoudre, il doit écriture la solution dans une variablelaquelle
            le thread d'écriture  a accès. Ensuite, il faut envoyer le signal au thread
            d'écriture.
            **/
             if(remplirGrilleBT(jeu->grille)==1){
                // On doit modifier la variable globale jeu; donc on bloque l'accès.
                err=pthread_mutex_lock(&buffer_jeu.mutex);
                if(err) printf("\n Erreur lock jeu buffer\n");
                buffer_jeu.jeu=*jeu;// Il a écrit sa solution dans le tampon

                err=pthread_mutex_lock(&buffer_stat.mutex);
                if(err) printf("\n Erreur lock mutex_cond\n");
                err=pthread_cond_signal(&buffer_stat.cond);
                buffer_stat.condition=1;
                if(err) printf("\n Erreur signal BT\n");
                printf("\nSolution grille %s envoyée pour écriture\n",buffer_jeu.jeu.nom);
                err=pthread_mutex_unlock(&buffer_stat.mutex);

                if(err) printf("\n Erreur unlock buffer state\n");
                pthread_exit(NULL);


             } else{

                    err=pthread_mutex_lock(&buffer_jeu.mutex);
                    if(err) printf("\n Erreur lock jeu buffer\n");
                    buffer_jeu.jeu=*jeu;

                    err=pthread_mutex_lock(&buffer_stat.mutex);
                    if(err) printf("\n Erreur lock mutex_cond\n");
                    err=pthread_cond_signal(&buffer_stat.cond);
                    buffer_stat.condition=1;
                    if(err) printf("\n Erreur signal BT\n");
                    err=pthread_mutex_unlock(&buffer_stat.mutex);

                    err=pthread_mutex_lock(&Reste.mutex);
                    if(err) printf("\n Erreur lock mutex_reste\n");
                    Reste.reste=Reste.reste-1;
                    printf("Aucune solution pour %s\n",buffer_jeu.jeu.nom);
                    err=pthread_mutex_unlock(&Reste.mutex);
                    if(err) printf("\n Erreur unlock mutex_reste\n");
                    pthread_exit(NULL);
                    }
      }
      else
      {
            err=pthread_mutex_lock(&Reste.mutex);
            if(err) printf("\n Erreur lock mutex_reste\n");
            Reste.reste=Reste.reste-1;
            printf("Methode inconnue\n");
            err=pthread_mutex_unlock(&Reste.mutex);
            if(err) printf("\n Erreur unlock mutex_reste\n");
            pthread_exit(NULL);
      }

    }


    void* ecrire(void *arg)
    { int err;

      pthread_mutex_lock(&buffer_stat.mutex);

      while(Reste.reste>0){

      char nomFichier[255]="./SudodusResolus/";

      while(!buffer_stat.condition){

        err=pthread_cond_wait(&buffer_stat.cond,&buffer_stat.mutex);// attente d'un signal
        if(err) printf("\n Erreur wait\n");

      }




      strcat(nomFichier,buffer_jeu.jeu.nom);
      ecrireSolution(buffer_jeu.jeu.grille,buffer_jeu.jeu.methode,nomFichier);

      err=pthread_mutex_lock(&Reste.mutex);
      if(err) printf("\n Erreur lock mutex_reste\n");
      Reste.reste=Reste.reste-1;// On a fini de résoudre 1 sudoku donc il faut en retirer 1.
      err=pthread_mutex_unlock(&Reste.mutex);
      if(err) printf("\n Erreur unlock mutex_reste\n");

      printf("Solution grille %s terminée l'écriture\n",buffer_jeu.jeu.nom);

      err=pthread_mutex_unlock(&buffer_jeu.mutex);
      if(err) printf("\n Erreur unlock mutex_jeu_ecrit\n");
      buffer_stat.condition=0;
      }

      pthread_mutex_unlock(&buffer_stat.mutex);
      pthread_exit(NULL);
    }

/*****************************************************************************************
Routines pour la version server
*****************************************************************************************/



void * resoudreServer(void*arg){
        int *res=malloc(sizeof(int));
        *res=1;
        Jeu *jeu=(Jeu*)arg;
        if(strcmp(jeu->methode,"BRUTE-FORCE")==0)
            {   int resBf;
                int nbPlacesVides=0;
                int PlacesVides[TAILLE*TAILLE][2];
                nbPlacesVides=avoirPlacesVides(jeu->grille,PlacesVides);
                resBf=resoudreSudokuBF(jeu->grille,PlacesVides,nbPlacesVides);
                if(resBf==0) *res=0;

                }
        else if(strcmp(jeu->methode,"BACKTRACKING")==0){
              int resBt;
              resBt=remplirGrilleBT(jeu->grille);
              if(resBt==1) *res=0;

        }
        else {
            printf("\nMethode inconnue\n");
        }
        return res;
        pthread_exit(NULL);
    }

void * envoyer(void*arg){
    char client_reply[200];
    int ent_en;
    Collis collis= *(Collis*)arg;
    int ligne,colonne;
    for(ligne=0;ligne<TAILLE;ligne++){
        for(colonne=0;colonne<TAILLE;colonne++){
           ent_en=collis.jeu.grille[ligne][colonne];
           if( write(collis.sock ,&ent_en,sizeof(ent_en) ) < 0)
           {
                puts("Send failed");
                return ;
            }
            }
            }
}
