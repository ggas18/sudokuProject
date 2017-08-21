/******************************************************************
ENSEM Nancy, INPL, Université de Lorraine
Projet 2A ISN

Professeur    Abdelkader    LAHMADI
              Jean-Philippe MANGEOT
              YeQuiong      SONG

Etudiant:     Arouna        GANOU

Version: V2 16/04/2017

*******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


// Les en-têtes des fonctions
#include "sudoku.h"


/**
Création de la mémoire tampon
**/

//BufferJeu buffer_jeu;

/**
INITIALISATION DES CONDITIONS ET DES MUTEXS
**/

//pthread_cond_init(&(buffer_jeu.cond));
BufferJeu buffer_jeu={.reste=0,
                      .mutex_reste=PTHREAD_MUTEX_INITIALIZER,
                      .cond=PTHREAD_COND_INITIALIZER,/**INITIALISATION DE LA CONDITION  **/
                      .mutex_cond=PTHREAD_MUTEX_INITIALIZER,/**INITIALISATION DU MUTEX DE  LA CONDITION**/
                      /**INITIALISATION du MUTEX DE LA VARIABLE PARTAGEE jeu_buffer**/
                      .mutex_jeu_buffer=PTHREAD_MUTEX_INITIALIZER,
                     };

int reste;// Le nombre de sudoku restant à résoudre.
Jeu jeu_ecrit;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; /**CRÉATION DE LA CONDITION ET SON INITIALISATION **/
pthread_mutex_t mutex_cond = PTHREAD_MUTEX_INITIALIZER; /**CREATION DU MUTEX DE CONDITION ET SON INITIALISATION**/
pthread_mutex_t mutex_jeu_ecrit = PTHREAD_MUTEX_INITIALIZER; /**CREATION DU MUTEX DE CONDITION ET SON INITIALISATION**/
pthread_mutex_t mutex_reste=PTHREAD_MUTEX_INITIALIZER;

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
        if(resoudreSudoku(tableau,PlacesVides,nbPlacesVides)==0){
            char nomFichierSol[255]="./SudokusResolusStanalone/SolutionBF.txt";
            char method[lgMethode]="BRUTE-FORCE";
            ecrireSolution(tableau,method,nomFichierSol);
            } else
                    {

                    printf("Aucune solution\n");
                    }

                }
  else if(algo==2){
       if(remplirGrille(tableau)==1){
            char nomFichierSol[255]="./SudokusResolusStanalone/SolutionBT.txt";
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
      buffer_jeu.reste=nbFichier;  // C'est le nombre de sudoku restant à résoudre, car on a une grille par fichier.
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
             err=pthread_create(&(threads_resolution[i]),NULL,resoudre,(void*)&(jeux[i]));
             if(err) printf("\nErreur creation de thread %d\n",i);

            }
      err=pthread_create(&thread_ecriture,NULL,ecrire,(void*)NULL);
      if(err) printf("\nErreur creation de thread ecriture\n");

      err=pthread_join(thread_ecriture,NULL);
      if(err) printf("\nErreur join de thread ecriture\n");
      }
  /********************************************
    Utilisation de la version professionnelle du solveur
    ********************************************/
  else if(strcmp(argv[1],"server") == 0)
    {
     printf("\nPas encore programmer");
    }
  /*********************************************
   Message d'erreur sur les options de l'utilisateur.
    ********************************************/
  else printf("\nOptions inconnues ou non supportées\n\n");

  return 0;
}


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
            if(resoudreSudoku(jeu->grille,PlacesVides,nbPlacesVides)==0){
            // On doit modifier la variable globale jeu_ecrit; donc on bloque l'accès.
            err=pthread_mutex_lock(&(buffer_jeu.mutex_jeu_buffer));
            if(err) printf("\n Erreur lock mutex_jeu_ecrit\n");
            buffer_jeu.jeu_buffer=*jeu;// Il a met sa solution dans le tampon
            err=pthread_mutex_lock(&(buffer_jeu.mutex_cond));
            if(err) printf("\n Erreur lock mutex_cond\n");
            err=pthread_cond_signal(&(buffer_jeu.cond));
            if(err) printf("\n Erreur signal\n");
            printf("\nSolution grille %s envoyée pour écriture\n",buffer_jeu.jeu_buffer.nom);
            err=pthread_mutex_unlock(&(buffer_jeu.mutex_cond));
            if(err) printf("\n Erreur unlock mutex_cond\n");
            pthread_exit(NULL);

            } else
                    {
                    err=pthread_mutex_lock(&(buffer_jeu.mutex_reste));
                    if(err) printf("\n Erreur lock mutex_reste\n");
                    buffer_jeu.reste=buffer_jeu.reste-1;
                    printf("Aucune solution\n");
                    err=pthread_mutex_unlock(&(buffer_jeu.mutex_reste));
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
             if(remplirGrille(jeu->grille)==1){
                // On doit modifier la variable globale jeu; donc on bloque l'accès.
                err=pthread_mutex_lock(&(buffer_jeu.mutex_jeu_buffer));
                if(err) printf("\n Erreur lock mutex_jeu_ecrit\n");
                buffer_jeu.jeu_buffer=*jeu;// Il a écrit sa solution dans le tampon
                err=pthread_mutex_lock(&(buffer_jeu.mutex_cond));
                if(err) printf("\n Erreur lock mutex_cond\n");
                err=pthread_cond_signal(&(buffer_jeu.cond));
                if(err) printf("\n Erreur signal BT\n");
                printf("\nAAAA");
                printf("\nSolution grille %s envoyée pour écriture\n",buffer_jeu.jeu_buffer.nom);
                err=pthread_mutex_unlock(&(buffer_jeu.mutex_cond));
                if(err) printf("\n Erreur unlock mutex_cond\n");
                pthread_exit(NULL);

             } else
                    {
                    err=pthread_mutex_lock(&buffer_jeu.mutex_reste);
                    if(err) printf("\n Erreur lock mutex_reste\n");
                    buffer_jeu.reste=buffer_jeu.reste-1;
                    printf("Aucune solution\n");
                    err=pthread_mutex_unlock(&buffer_jeu.mutex_reste);
                    if(err) printf("\n Erreur unlock mutex_reste\n");
                    pthread_exit(NULL);
                    }
      }
      else
      {
            err=pthread_mutex_lock(&buffer_jeu.mutex_reste);
            if(err) printf("\n Erreur lock mutex_reste\n");
            reste=reste-1;
            printf("Methode inconnue\n");
            err=pthread_mutex_unlock(&buffer_jeu.mutex_reste);
            if(err) printf("\n Erreur unlock mutex_reste\n");
            pthread_exit(NULL);
      }

    }


    void* ecrire(void *arg)
    { int err;

      while(buffer_jeu.reste>0){
      char nomFichier[255]="./SudodusResolus/";

      err=pthread_cond_wait(&(buffer_jeu.cond),&(buffer_jeu.mutex_cond));// attente d'un signal
      if(err) printf("\n Erreur wait\n");
      strcat(nomFichier,buffer_jeu.jeu_buffer.nom);
      ecrireSolution(buffer_jeu.jeu_buffer.grille,buffer_jeu.jeu_buffer.methode,nomFichier);

      err=pthread_mutex_lock(&(buffer_jeu.mutex_reste));
      if(err) printf("\n Erreur lock mutex_reste\n");
      buffer_jeu.reste=buffer_jeu.reste-1;// On a fini de résoudre 1 sudoku don  il faut en retirer 1.
      err=pthread_mutex_unlock(&(buffer_jeu.mutex_reste));
      if(err) printf("\n Erreur unlock mutex_reste\n");

      printf("Solution grille %s terminée l'écriture\n",buffer_jeu.jeu_buffer.nom);

      err=pthread_mutex_unlock(&buffer_jeu.mutex_jeu_buffer);
      if(err) printf("\n Erreur unlock mutex_jeu_ecrit\n");
      }
      pthread_exit(NULL);
    }

