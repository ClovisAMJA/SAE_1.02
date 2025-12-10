// Projet Sokoban - main.c
// Auteur : ABOUT Clovis 1B2
// Date de finalisation : 29/11/2025

//² Inclusion des bibliothèques

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdbool.h>

//Declaration des constantes

const char MUR = '#';
const char CAISSE = '$';
const char CIBLE = '.';
const char SOKOBAN = '@';
const char CAISSE_SUR_CIBLE = '*';
const char SOKOBAN_SUR_CIBLE = '+';
const char CASE_VIDE = ' ';
const char OUI = 'O';
const char NON = 'N';
const char ZOOMER = '+';
const char DEZOOMER = '-';

#define DEPLACE_CAISSE_BAS 'B'
#define DEPLACE_CAISSE_HAUT 'H'
#define DEPLACE_CAISSE_DROITE 'D'
#define DEPLACE_CAISSE_GAUCHE 'G'
#define DEPLACE_BAS 'b'
#define DEPLACE_HAUT 'h'
#define DEPLACE_DROITE 'd'
#define DEPLACE_GAUCHE 'g'
#define ANNULER 'u'
#define GAUCHE 'q'
#define DROITE 'd' 
#define BAS 's'
#define HAUT 'z'
#define RECOMMENCER 'r'
#define ABANDONNER 'x'
#define TAILLE 12
#define MAX 1500

// Nombre de lignes imprimées par afficher_entete

#define HEADER_LINES 19

//Definition des types

typedef char t_Plateau[TAILLE][TAILLE];
typedef char t_tabDeplacement[MAX];
//Prototype

int kbhit();
void charger_partie(t_Plateau plateau, char fichier[]);
void afficher_entete(char fichier[],int nbDeplacement);
void afficher_plateau(t_Plateau plateau, int zoom);
void deplacer(t_Plateau plateau, char direction,int *nbDeplacement,t_tabDeplacement t, char touche, bool pousse_caisse, int i);
void gotoxy(int x, int y);
bool gagne(t_Plateau plateau);

//MAIN
int main() {
    //Declaration et initialisation des variables

    t_Plateau plateau = {};
    t_tabDeplacement t = {};
    char fichier[50] = "";
    int nbDeplacement = 0;    
    int zoom = 1;
    bool pousse_caisse = false;
    int i = 0;
    //Saisie des données

    printf(" Veuillez choisir votre niveau au format 'niveauX.sok' : ");
    scanf("%s", fichier);

    afficher_entete(fichier, nbDeplacement);
    charger_partie(plateau, fichier);
    afficher_plateau(plateau, zoom);

    char touche;
    while(1) {
        if(kbhit()) {
            touche = getchar();
            deplacer(plateau, &nbDeplacement, t, touche, pousse_caisse, i);
            
            afficher_entete(fichier, nbDeplacement);
            afficher_plateau(plateau, zoom);
            bool gagnee = gagne(plateau);
            if (gagnee){
                printf("\nFélicitations, vous avez gagné en %d déplacements !\n", nbDeplacement);
                break; 
            }
        }
    }
    return EXIT_SUCCESS;
}

// Déclaration des fonctions
// Affiche le plateau de jeu à la position 
void afficher_plateau(t_Plateau plateau, int zoom){

    gotoxy(0, HEADER_LINES);

    for (int ligne = 0; ligne < TAILLE * zoom; ligne++){
        for (int colonne = 0; colonne < TAILLE * zoom; colonne++){
            printf("%c", plateau[ligne / zoom][colonne / zoom]);
        }
        printf("\n");
    }
}
// Affiche l'entête du jeu avec le nom du fichier et le nombre de déplacements
void afficher_entete(char fichier[],int nbDeplacement){
    system("clear");
    printf("Vous jouez actuellement au %s\n ----------------------\n", fichier);
    printf("\nRappel des touches : \n");
    printf("\n| Touche Q : gauche\n| Touche Z : haut\n");
    printf("| Touche S : bas\n| Touche D : droite\n");
    printf("| Touche X : abandonner\n| Touche R : recommencer\n");
    printf("| Touche + : zoomer\n| Touche - : dézoomer\n");
    printf("| Touche U : annuler le dernier déplacement\n");
    printf("\nVous avez fait %d déplacements\n", nbDeplacement);
}

int kbhit(){

	// la fonction retourne :
	// 1 si un caractere est present
	// 0 si pas de caractere présent
	int unCaractere=0;
	struct termios oldt, newt;
	int ch;
	int oldf;

	// mettre le terminal en mode non bloquant

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
	ch = getchar();

	// restaurer le mode du terminal

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
 
	if(ch != EOF){
		ungetc(ch, stdin);
		unCaractere=1;
	} 
	return unCaractere;
}
// Charge une partie depuis un fichier
void charger_partie(t_Plateau plateau, char fichier[]){

    FILE * f;
    char finDeLigne;

    f = fopen(fichier, "r");
    if (f==NULL){
        printf("ERREUR SUR FICHIER");
        exit(EXIT_FAILURE);
    } else {
        for (int ligne=0 ; ligne<TAILLE ; ligne++){
            for (int colonne=0 ; colonne<TAILLE ; colonne++){
                fread(&plateau[ligne][colonne], sizeof(char), 1, f);
            }
            fread(&finDeLigne, sizeof(char), 1, f);
        }
        fclose(f);
    }
}

// Structure pour stocker une position sur le plateau
typedef struct {
    int x;
    int y;
} t_Position;
// Déplace le curseur à la position (x, y) dans le terminal
void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y+1, x+1);
}
// Déplace le sokoban dans la direction spécifiée si le mouvement est valide
void deplacer(t_Plateau plateau, char direction,int *nbDeplacement,t_tabDeplacement t, char touche, bool pousse_caisse, int i){
    int deltalig = 0, deltaCol = 0;
    switch (direction) {
        case HAUT: 
            deltalig = -1;
            break;
        case BAS:
            deltalig = 1;
            break;
        case GAUCHE: 
            deltaCol = -1;
            break;
        case DROITE: 
            deltaCol = 1;
            break;
        default:
            return; 
    }
    int sokobanlig = -1, sokobanCol = -1;
    for (int lig = 0; lig < TAILLE; lig++){
        for (int col = 0; col < TAILLE; col++){
            if (plateau[lig][col] == SOKOBAN || plateau[lig][col] == SOKOBAN_SUR_CIBLE){    
                sokobanlig = lig; sokobanCol = col; break;
            }
        }
        if (sokobanlig != -1) break;
    }
    if (sokobanlig == -1) return; 
    int targetlig = sokobanlig + deltalig, targetCol = sokobanCol + deltaCol; 
    if (targetlig < 0 || targetlig >= TAILLE || targetCol < 0 || targetCol >= TAILLE) return;
    char sourceChar = plateau[sokobanlig][sokobanCol];
    char targetChar = plateau[targetlig][targetCol];
    if (targetChar == MUR) return;
    if (targetChar == CASE_VIDE || targetChar == CIBLE){
        plateau[sokobanlig][sokobanCol] = (sourceChar == SOKOBAN_SUR_CIBLE)
            ? CIBLE : CASE_VIDE;
        plateau[targetlig][targetCol] = (targetChar == CIBLE) 
        ? SOKOBAN_SUR_CIBLE : SOKOBAN;
        (*nbDeplacement)++;
        i = *nbDeplacement - 1;
        deplacements(t,  touche,  i,  pousse_caisse);
        return;
    }
    if (targetChar == CAISSE || targetChar == CAISSE_SUR_CIBLE){
        pousse_caisse = true;
        int behindBoxlig = targetlig + deltalig,
        behindBoxCol = targetCol + deltaCol; 
        if (behindBoxlig < 0 || behindBoxlig >= TAILLE 
            || behindBoxCol < 0 || behindBoxCol >= TAILLE) return;
        char beyondChar = plateau[behindBoxlig][behindBoxCol];
        if (beyondChar == CASE_VIDE || beyondChar == CIBLE){
            plateau[behindBoxlig][behindBoxCol] = 
            (beyondChar == CIBLE) ? CAISSE_SUR_CIBLE : CAISSE;
            plateau[targetlig][targetCol] = 
            (targetChar == CAISSE_SUR_CIBLE) ? SOKOBAN_SUR_CIBLE : SOKOBAN  ;
            plateau[sokobanlig][sokobanCol] = 
            (sourceChar == SOKOBAN_SUR_CIBLE) ? CIBLE : CASE_VIDE;
            (*nbDeplacement)++;
            i = *nbDeplacement - 1;
        }
        deplacements(t,  touche,  i,  pousse_caisse);
        return;
    }
    return;
}
// Vérifie si le joueur a gagné en s'assurant qu'il n'y a plus de caisses non placées sur les cibles
bool gagne(t_Plateau plateau){

    for (int ligne = 0; ligne < TAILLE; ligne++){

        for (int colonne = 0; colonne < TAILLE; colonne++){

            if (plateau[ligne][colonne] == CAISSE){

                return false;
            }
        }
    }
    return true;
}

