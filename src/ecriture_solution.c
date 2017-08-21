/***********************************************************************
Ce fichier contient les elements utiles à l'écriture d'une solution de
sudoku dans un fichier texte.
***********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Les en-têtes des fonctions
#include "sudoku.h"

                        /*DEBUT ECRITURE DANS LE FICHIER*/
int ecrireSolution(int tableau[TAILLE][TAILLE],char const methode[255], char const nomFichier[255])
    {
    FILE* fichier = fopen(nomFichier, "w");
    fprintf(fichier,"Sudoku Resolu:%s\n",methode);
    int r,c;
    for (r = 0; r < TAILLE; r++) {
			if (r % 3 == 0) {
				fprintf(fichier,"+---+---+---+\n");
			}
			for (c = 0; c < TAILLE; c++) {
				if (c % 3 == 0) {
					fprintf(fichier,"|");
				}
				fprintf(fichier,"%d",tableau[r][c]);
			}
			fprintf(fichier,"|");
			fprintf(fichier,"\n");
		}
		fprintf(fichier,"+---+---+---+\n");

    fclose(fichier);

    return 0;
    }
                        /*FIN ECRITURE DANS LE FICHIER*/
