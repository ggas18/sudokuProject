/********************************************************
Ce fichier contient les élements nécessaire à la resolution
d'un tableau de sudoku par brute force.
*********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Les en-têtes des fonctions
#include "sudoku.h"



                          /*BRUTE FORCE*/
int avoirPlacesVides(int grille[TAILLE][TAILLE],int PlacesVides[TAILLE*TAILLE][2])
    {
       int i=0;
       int nbPlaceVides=0;
       int r,c;
       for (r=0; r < TAILLE; r++) {
			for ( c = 0; c < TAILLE; c++) {
				if (grille[r][c]==0) {
					PlacesVides[i][0] = r;
					PlacesVides[i][1] = c;
					nbPlaceVides++;
					i++;
				}
			}
		}
		return nbPlaceVides;


    }

int pasDoublons(int Chiffres[TAILLE])
        {
        int j;
        int k;
        for (j=0; j < TAILLE; j++) {
			for (k = j + 1; k < TAILLE; k++) {
				if (Chiffres[j] == Chiffres[k] && Chiffres[j] != 0)
					return 1;
			}
		}
		return 0;
        }


int verifierLigne(int grille[TAILLE][TAILLE],int ligne)
        {
        int Chiffres[TAILLE];
        int c;
		for (c=0; c < TAILLE; c++) {
			Chiffres[c] = grille[ligne][c];
		}
		if (pasDoublons(Chiffres)==0) {
			return 0;
		}
		return 1;
        }

int verifierColonne(int grille[TAILLE][TAILLE],int colonne)
    {

        int Chiffres[TAILLE];
        int l = 0;
		for (; l < TAILLE; l++) {
			Chiffres[l] = grille[l][colonne];
		}
		if (pasDoublons(Chiffres)==0) {
			return 0;
		}
		return 1;
    }

int verifierRegion(int grille[TAILLE][TAILLE],int ligne,int colonne)
    {
        int Chiffres[TAILLE];
		ligne= (ligne/ 3) * 3;
		colonne= (colonne / 3) * 3;
		int i = 0;
		int r = 0;
		for ( ;r < 3; r++) {
		    int c = 0;
			for (; c < 3; c++) {
				Chiffres[i] = grille[ligne + r][colonne + c];
				i++;
			}
		}
		if (pasDoublons(Chiffres)==0)
		{
			return 0;
		}
		return 1;
    }

int estConforme(int grille[TAILLE][TAILLE],int ligne, int colonne)
    {
        int okLigne = verifierLigne(grille, ligne);
        int okColonne = verifierColonne(grille, colonne);
        int okRegion = verifierRegion(grille, ligne,colonne);
		if ((okLigne==0) && (okColonne==0) && (okRegion==0)) {
			return 0;
		}
		return 1;
    }

int resoudreSudokuBF(int grille[TAILLE][TAILLE],int placesVides[TAILLE*TAILLE][2],int nbPlacesVides)
    {
        int i = 0;
		int valPlace= 0;
		while (i <nbPlacesVides) {
			valPlace= grille[placesVides[i][0]][placesVides[i][1]];

			if (valPlace != 9) {

				valPlace++;
                grille[placesVides[i][0]][placesVides[i][1]] = valPlace;
				if (estConforme(grille, placesVides[i][0],placesVides[i][1])==0) {
					i++;
				}

			} else {

				valPlace = 0;
				grille[placesVides[i][0]][placesVides[i][1]] = valPlace;
				i--;
				if (i < 0) {
					return 1;
				}

			}
		}
		return 0;
    }
                          /*FIN BRUTE FORCE*/
