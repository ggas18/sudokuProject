#ifndef SUDOKU_H_INCLUDED
#define SUDOKU_H_INCLUDED

#include <pthread.h>
#define TAILLE 9
#define NbEspace 3
#define lgMethode 100


/******************************************************
Cette structure permet de creer des entités grilles de
sudoku avec sa méthode de reolution. Ceci permettra
d'empiler des grilles avec les méthodes dans un tableau.
******************************************************/

typedef struct Jeu
    {
    /**Pour le nom du fichier dans lequel la grille initiale était**/
    char nom[255];
    int grille[TAILLE][TAILLE];
    char methode[255];

    } Jeu;

typedef struct jeuGlob

{ pthread_mutex_t mutex;
  /**La variable à proteger
    **/
  Jeu  jeu;
} jeuGlob;


typedef struct restGlob

{ pthread_mutex_t mutex;
  /**La variable à proteger
    **/
  int  reste;
} restGlob;

typedef struct condGlob

{ pthread_mutex_t mutex;
  /**La variable à proteger
    **/
  pthread_cond_t cond;
  int  condition;
} condGlob;


typedef struct Collis
{
    int sock;
    Jeu jeu;
} Collis;
                                          /**BRUTE FORCE**/
/**************************************************************************
Cette fonction prend la grille initiale et trouve les places qui sont à zero
et retourne leurs nombres tout en gardant leurs emplacements dans un autre
tableau à deux colonnes. La première pour la ligne et la deuxième pour la
colonnes de l'emplacement
*****************************************************************************/
int avoirPlacesVides(int grille[TAILLE][TAILLE],int PlacesVides[TAILLE*TAILLE][2]);

/*****************************************************************************
Cette fonction prend une liste de nombre(colonne,ligne) et essaie de voir si
un chiffre n'est pas repeté.
********************************************************************************/
int pasDoublons(int Chiffres[TAILLE]);

/**********************************************************************************
Cette fonction prend la grille et la ligne ensuite verifie si on n'a pas de doublons
***********************************************************************************/
int verifierLigne(int grille[TAILLE][TAILLE],int ligne);

/************************************************************************************
Cette fonction prend la grille et la colonne ensuite verifie si on n'a pas de doublons
*************************************************************************************/
int verifierColonne(int grille[TAILLE][TAILLE],int colonne);

/**************************************************************************************
Cette fonction prend la grille et la liste des élements dans une région données en fonction
de la ligne et de la colonne ensuite verifie si on n'a pas de doublons.
*******************************************************************************************/
int verifierRegion(int grille[TAILLE][TAILLE],int ligne,int colonne);

/**************************************************************************************
Cette fonction verifie si on est conforme aux règles du sudoku à cet emplacement:
1. la règle de la ligne
2. la règle de la colonne
3. la règle de la region
******************************************************************************************/
int estConforme(int grille[TAILLE][TAILLE],int ligne, int colonne);

/*****************************************************************************************
On resoud le sudoku avec cette fonction en lui passant la grille, les emplacements des zeros
et leurs nombres.
********************************************************************************************/
int resoudreSudokuBF(int grille[TAILLE][TAILLE],int placesVides[TAILLE*TAILLE][2],int nbPlacesVides);

                                          /**FIN FORCE BRUTE**/


                                          /**BACKTRACKING**/
/****************************************************************************
Cette fonction teste si la valeur a placé existe déjà sur la ligne.
*******************************************************************************/
int sur_ligne(int grille[TAILLE][TAILLE],int ligne, int valeur);


/****************************************************************************
Cette fonction teste si la valeur a placé existe déjà sur la colonne.
*******************************************************************************/
int sur_colonne(int grille[TAILLE][TAILLE],int colonne, int valeur);

/****************************************************************************
Cette fonction teste si la valeur a placé existe déjà sur la ligne.
*******************************************************************************/
int sur_region(int grille[TAILLE][TAILLE],int ligne0_reg,int colonne0_reg, int valeur);


/****************************************************************************
Cette fonction teste si la valeur a placé existe déjà sur la ligne.
*******************************************************************************/
int est_libre(int grille[TAILLE][TAILLE],int ligne, int colonne, int valeur);

/********************************************************************************
Cette fonction trouve les zeros dans une grille
*********************************************************************************/
int cherche_vide(int grille[TAILLE][TAILLE],int *ligne, int *colonne);
/*******************************************************************************
Fonction qui remplit la grille en resolvant le sudoku par backtracking
********************************************************************************/
int remplirGrilleBT(int grille[TAILLE][TAILLE]);

                                          /**FIN BACTRACKING**/


                                          /**LECTURE FICHIERS**/
/*******************************************************************************
Cette fonction permet de lire un caractère entier en un entier
********************************************************************************/

int car_ent(const char c);
/*******************************************************************************
Cette fonction se charge de remplir la ligne d'un tableau par une ligne de
la grille donnée. Cette ligne de grille sera obtenue dans le fichier passer
en paramètre.
********************************************************************************/
void remplirTableau(int tableau[TAILLE][TAILLE], int ligne, char chaine[]);

/*******************************************************************************
Cette fonction lit les lignes dans un fichier de grille de sudoku et utilise
la fonction remplirTableau pour remplir tout le tableau de grille.
*********************************************************************************/
int lectureFichier(int tableau[TAILLE][TAILLE], char const nomFichier[], char *methode);
                                          /**FIN LECTURE FICHIERS**/



                                          /**DEBUT ECRITURE**/
/**
Cette fonction prend un tableau et l'ecrit dans un fichier.
**/
int ecrireSolution(int tableau[TAILLE][TAILLE],char const methhode[255],char const nomFichier[255]);
                                          /**FIN ECRITURE**/


                                          /**DEBUT FONCTIONS DES THREADS DE RESOLUTION**/
/***************************************************************************************
Cette fonction permet de resoudre un tableau de sudoku avec la methode donnée.
****************************************************************************************/
void* resoudre(void *arg);
                                           /**FIN FONCTIONS DES THREADS DE RESOLUTION**/



                                            /**DEBUT FONCTIONS DES THREADS D'ECRITURE DE LA SOLUTION**/

/***************************************************************************
Cette fonction permet d'écrire le tableau d'un jeu de sudoku avec la methode
donnée dans un fichier.
***************************************************************************/
void* ecrire(void *arg);
                                           /**FIN FONCTIONS DES THREADS DE D'ECRITURE DE LA SOLUTION**/

/********************************************************************************************************
Routine pour les threads du serveur
*********************************************************************************************************/

void * envoyer(void*arg);

void * resoudreServer(void*);

#endif // SUDOKU_H_INCLUDED
