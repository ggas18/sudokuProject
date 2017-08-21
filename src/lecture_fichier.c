/*****************************************************
Ce fichier permet la lecture d'un fichier contenant une
grille de sudoku et la méthode de résolution.
******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Les en-têtes des fonctions
#include "sudoku.h"


                                         /*LECTURE De FICHIER*/
int car_ent(const char c)
{
    switch (c)
    {
    case '0':
        return 0;
    case '1':
        return 1;
    case '2':
        return 2;
    case '3':
        return 3;
    case '4':
        return 4;
    case '5':
        return 5;
    case '6':
        return 6;
    case '7':
        return 7;
    case '8':
        return 8;
    case '9':
        return 9;
    default:
        return 0;
    }
}

void remplirTableau(int tableau[TAILLE][TAILLE], int ligne, char chaine[])
{
    int i=0;
    int j=0;
    while(i<=2*TAILLE+NbEspace){
        if(i==6 || i==13)
        {
         i++;
        }
        else
        {

            if(chaine[i]=='_')
            {
                tableau[ligne][j]=0;
            }
            else
            {
                tableau[ligne][j]=car_ent(chaine[i]);
            };
            j=j+1;;
            i=i+2;
      }
 }

}


int lectureFichier(int tableau[TAILLE][TAILLE], char const nomFichier[],char *methode)
{
    FILE* fichier = fopen(nomFichier, "r");
    char chaine[256];
    int ligne=0;
    int l=1;
    while (l<15) {
        fgets(chaine, sizeof(chaine), fichier);
        if(l==1 || l==5 || l==9 || l==13 )
        {
            l++;
        }
        else if (l==14)
        {   char *m=(char*)malloc(lgMethode);
            int fin_ligne=strlen(chaine);
            int i;
            for(i=0;i<fin_ligne;i++){
              if(chaine[i]=='\n') chaine[i]='\0';
            }
            strcpy(m,chaine);
            m+=8;
            strcpy(methode,m);
            l++;
        }
        else
        {
            remplirTableau(tableau,ligne,chaine);
            ligne++;
            l++;
        }

    }

    fclose(fichier);

    return 0;
}

                                       /*FIN LECTURE DE FICHIERS*/
