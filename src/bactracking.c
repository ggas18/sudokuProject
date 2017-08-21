#include <stdlib.h>

/**Les en-têtes des fonctions**/

#include "sudoku.h"

int sur_ligne(int grille[TAILLE][TAILLE],int ligne, int valeur)
    {
    int colonne;
	for (colonne = 0; colonne < TAILLE ; colonne++) {
		if (grille[ligne][colonne] ==valeur) {
			return 1;
		}
	}
	return 0;
    }


int sur_colonne(int grille[TAILLE][TAILLE],int colonne, int valeur)
    {
     int ligne;
	 for (ligne= 0; ligne < TAILLE; ligne++) {
		if (grille[ligne][colonne] == valeur) {
			return 1;
		}
	}
	return 0;
    }


int sur_region(int grille[TAILLE][TAILLE],int ligne0_reg,int colonne0_reg, int valeur)
    {
    int ligne;
	for (ligne = 0; ligne < TAILLE/3; ligne++) {
        int colonne;
		for (colonne= 0; colonne< TAILLE/3; colonne++) {
			if (grille[ligne + ligne0_reg][colonne + colonne0_reg] == valeur) {
				return 1;
			}
		}
	}
	return 0;
    }


int est_libre(int grille[TAILLE][TAILLE],int ligne, int colonne, int valeur)
    { int Okligne=sur_ligne(grille,ligne,valeur);
      int Okcolonne=sur_colonne(grille,colonne,valeur);
      int Okregion=sur_region(grille,ligne-(ligne%3),colonne-(colonne%3),valeur);
     return (!Okligne && !Okcolonne && !Okregion);
    }

int cherche_vide(int grille[TAILLE][TAILLE],int *ligne, int *colonne)
    {
    for (*ligne = 0; *ligne < TAILLE; (*ligne)++) {
		for (*colonne = 0; *colonne < TAILLE; (*colonne)++) {
			if (grille[*ligne][*colonne] == 0) {
				return 1;
			}
		}
	}
	return 0;
    }

int remplirGrilleBT(int grille[TAILLE][TAILLE])
    {
    int ligne = 0;
	int colonne = 0;

	if (!cherche_vide(grille, &ligne, &colonne)){
		return 1;
	}
    int valeur;
	for (valeur = 1; valeur <= TAILLE; valeur++ ) {

		if (est_libre(grille, ligne, colonne, valeur)) {
			grille[ligne][colonne] = valeur;

			if (remplirGrilleBT(grille)) {
				return 1;
			}

			grille[ligne][colonne] = 0;
		}
	}

	return 0;

    }

                                          /**FIN BACTRACKING**/
