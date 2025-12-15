// Projet Sokoban - main.c
// Auteur : ABOUT Clovis 1B2
// Date de finalisation : 29/11/2025

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdbool.h>

// ======================
// CONSTANTES
// ======================
const char MUR = '#';
const char CAISSE = '$';
const char CIBLE = '.';
const char SOKOBAN = '@';
const char CAISSE_SUR_CIBLE = '*';
const char SOKOBAN_SUR_CIBLE = '+';
const char CASE_VIDE = ' ';

#define GAUCHE 'q'
#define DROITE 'd'
#define BAS 's'
#define HAUT 'z'
#define TAILLE 12
#define MAX 1500
#define HEADER_LINES 19

// ======================
// TYPES
// ======================
typedef char t_Plateau[TAILLE][TAILLE];
typedef char t_tabDeplacement[MAX];

// ======================
// PROTOTYPES
// ======================
void charger_partie(t_Plateau plateau, char fichier[]);
void afficher_entete(int nbDeplacement);
void afficher_plateau(t_Plateau plateau);
void deplacer(t_Plateau plateau, char direction, int *nbDeplacement);
void gotoxy(int x, int y);
bool gagne(t_Plateau plateau);
void chargerDeplacements(t_tabDeplacement t, char fichier[], int *nb);

// ======================
// MAIN (STRUCTURE ORIGINALE)
// ======================
int main()
{
    t_Plateau plateau = {};
    t_tabDeplacement t = {};
    char fichierDep[50] = "";
    char fichier[50] = "";
    int nbDeplacement = 0;
    int nb = 0;
    char touche;

    printf("Veuillez choisir votre niveau au format 'niveauX.sok' : ");
    scanf("%s", fichier);
    printf("Veuillez choisir vos deplacements au format 'niveauX.dep' : ");
    scanf("%s", fichierDep);

    chargerDeplacements(t, fichierDep, &nb);
    charger_partie(plateau, fichier);

    for (int i = 0; i < nb; i++)
    {
        touche = t[i];
        deplacer(plateau, touche, &nbDeplacement);
        afficher_entete(nbDeplacement);
        afficher_plateau(plateau);
        usleep(50000);

        if (gagne(plateau))
        {
            printf("\nLa suite de déplacements %s est bien une solution pour la partie %s.\nElle contient %d déplacements!\n", fichierDep, fichier, nb);
            return EXIT_SUCCESS;
        }
    }

    printf("La suite de déplacements %s N'EST PAS une solution pour la partie %s\n", fichierDep, fichier);
    return EXIT_SUCCESS;
}

// ======================
// FONCTIONS
// ======================

void afficher_plateau(t_Plateau plateau)
{
    gotoxy(0, HEADER_LINES);
    for (int ligne = 0; ligne < TAILLE; ligne++)
    {
        for (int colonne = 0; colonne < TAILLE; colonne++)
            printf("%c", plateau[ligne][colonne]);
        printf("\n");
    }
}

void chargerDeplacements(t_tabDeplacement t, char fichier[], int *nb)
{
    FILE *f = fopen(fichier, "r");
    if (f == NULL)
    {
        printf("FICHIER NON TROUVE\n");
        return;
    }

    int c;
    *nb = 0;
    while ((c = fgetc(f)) != EOF && *nb < MAX)
    {
        if (c == '\n' || c == '\r' || c == ' ' || c == '\t')
            continue;
        t[*nb] = (char)c;
        (*nb)++;
    }
    fclose(f);
}

void afficher_entete(int nbDeplacement)
{
    printf("\nVous avez fait %d déplacements\n", nbDeplacement);
}

void charger_partie(t_Plateau plateau, char fichier[])
{
    FILE *f = fopen(fichier, "r");
    if (f == NULL)
    {
        printf("ERREUR SUR FICHIER\n");
        exit(EXIT_FAILURE);
    }
    char finDeLigne;
    for (int ligne = 0; ligne < TAILLE; ligne++)
    {
        for (int colonne = 0; colonne < TAILLE; colonne++)
            fread(&plateau[ligne][colonne], sizeof(char), 1, f);
        fread(&finDeLigne, sizeof(char), 1, f);
    }
    fclose(f);
}

void gotoxy(int x, int y)
{
    printf("\033[%d;%dH", y + 1, x + 1);
}

void deplacer(t_Plateau plateau, char direction, int *nbDeplacement)
{
    int deltaLig = 0, deltaCol = 0;

    switch (direction)
    {
        case HAUT:    deltaLig = -1; break;
        case BAS:     deltaLig =  1; break;
        case GAUCHE:  deltaCol = -1; break;
        case DROITE:  deltaCol =  1; break;
        default: return;
    }

    int sL = -1, sC = -1;
    for (int i = 0; i < TAILLE; i++)      
        for (int j = 0; j < TAILLE; j++){
            if (plateau[i][j] == SOKOBAN || plateau[i][j] == SOKOBAN_SUR_CIBLE)
            {
                sL = i; sC = j;
            }
        }
    if (sL == -1) return;

    int tL = sL + deltaLig;
    int tC = sC + deltaCol;
    if (tL < 0 || tL >= TAILLE || tC < 0 || tC >= TAILLE)
        return;

    char src = plateau[sL][sC];
    char tgt = plateau[tL][tC];

    if (tgt == CASE_VIDE || tgt == CIBLE)
    {
        plateau[sL][sC] = (src == SOKOBAN_SUR_CIBLE) ? CIBLE : CASE_VIDE;
        plateau[tL][tC] = (tgt == CIBLE) ? SOKOBAN_SUR_CIBLE : SOKOBAN;
        (*nbDeplacement)++;
        return;
    }

    if (tgt == CAISSE || tgt == CAISSE_SUR_CIBLE)
    {
        int bL = tL + deltaLig;
        int bC = tC + deltaCol;
        if (bL < 0 || bL >= TAILLE || bC < 0 || bC >= TAILLE)
            return;

        char behind = plateau[bL][bC];
        if (behind == CASE_VIDE || behind == CIBLE)
        {
            plateau[bL][bC] = (behind == CIBLE) ? CAISSE_SUR_CIBLE : CAISSE;
            plateau[tL][tC] = (tgt == CAISSE_SUR_CIBLE) ? SOKOBAN_SUR_CIBLE : SOKOBAN;
            plateau[sL][sC] = (src == SOKOBAN_SUR_CIBLE) ? CIBLE : CASE_VIDE;
            (*nbDeplacement)++;
        }
    }
}

bool gagne(t_Plateau plateau)
{
    for (int ligne = 0; ligne < TAILLE; ligne++)
        for (int colonne = 0; colonne < TAILLE; colonne++)
            if (plateau[ligne][colonne] == CAISSE)
                return false;
    return true;
}
