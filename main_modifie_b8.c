// Projet Sokoban - main.c
// Auteur : ABOUT Clovis 1B2
// Date de finalisation : 29/11/2025

// Inclusion des bibliothèques

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

#define DEPLACE_CAISSE_BAS 'B'
#define DEPLACE_CAISSE_HAUT 'H'
#define DEPLACE_CAISSE_DROITE 'D'
#define DEPLACE_CAISSE_GAUCHE 'G'
#define DEPLACE_BAS 'b'
#define DEPLACE_HAUT 'h'
#define DEPLACE_DROITE 'd'
#define DEPLACE_GAUCHE 'g'
#define GAUCHE 'q'
#define DROITE 'd' 
#define BAS 's'
#define HAUT 'z'
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
void chargerDeplacements(t_tabDeplacement t, char fichier[], int * nombreDeplacements);
void afficher_entete(char fichier[],int nbDeplacement);
void afficher_plateau(t_Plateau plateau, int zoom);
void deplacer(t_Plateau plateau, char direction, int *index);
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

    charger_partie(plateau, fichier);
    
    char fichierDep[50] = "";
    strcpy(fichierDep, fichier);
    strcpy(fichierDep + strlen(fichierDep) - 4, ".dep");
    
    char choixDep[50] = "";
    int c = getchar(); 
    if (c != '\n' && c != EOF) ungetc(c, stdin);
    do {
        printf("Entrez le fichier .dep à utiliser : ");
        if (fgets(choixDep, sizeof(choixDep), stdin) == NULL) {
            choixDep[0] = '\0';
        } else {
            size_t lenChoix = strlen(choixDep);
            if (lenChoix > 0 && choixDep[lenChoix-1] == '\n') choixDep[lenChoix-1] = '\0';
        }
    } while (strlen(choixDep) == 0);
    strcpy(fichierDep, choixDep);

    // Charger les déplacements
    chargerDeplacements(t, fichierDep, &nbDeplacement);
    
    afficher_entete(fichier, 0);
    afficher_plateau(plateau, zoom);
    
    
    for (int index = 0; index < nbDeplacement; index++) {
        deplacer(plateau, t[index], &index);
        usleep(200000); 
        afficher_entete(fichier, index + 1);
        afficher_plateau(plateau, zoom);
    }  
    // Vérifier la victoire uniquement après le dernier mouvement
    bool gagnee = gagne(plateau);
    if (gagnee){
        printf("\nFélicitations, vous avez gagné en %d déplacements !\n", nbDeplacement);
    } else {
        printf("\nDéfaite : toutes les caisses ne sont pas sur les cibles après %d déplacements.\n", nbDeplacement);
    }
    
    return EXIT_SUCCESS;
}

// Affiche le plateau de jeu.
void afficher_plateau(t_Plateau plateau, int zoom){

    gotoxy(0, HEADER_LINES);

    for (int ligne = 0; ligne < TAILLE * zoom; ligne++){
        for (int colonne = 0; colonne < TAILLE * zoom; colonne++){
            printf("%c", plateau[ligne / zoom][colonne / zoom]);
        }
        printf("\n");
    }
}
// Affiche l'entête du jeu avec le nom du fichier et le nombre de déplacements.
void afficher_entete(char fichier[],int nbDeplacement){
    system("clear");
    printf("Vous jouez actuellement au %s\n ----------------------\n", fichier);
    printf("\nRappel des touches : \n");
    printf("| Touche + : zoomer\n| Touche - : dézoomer\n");
    printf("\nVous avez fait %d déplacements\n", nbDeplacement);
}


// Indique si un caractère est disponible sur l'entrée standard (non bloquant).
int kbhit(){

    int unCaractere=0;
	struct termios oldt, newt;
    int caractereLu;
	int oldf;

	// mettre le terminal en mode non bloquant

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
    caractereLu = getchar();

	// restaurer le mode du terminal

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
 
    if(caractereLu != EOF){
        ungetc(caractereLu, stdin);
        unCaractere=1;
    } 
	return unCaractere;
}
// Charge une partie depuis un fichier .sok
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
// Charge la liste des déplacements depuis un fichier .dep
void chargerDeplacements(t_tabDeplacement t, char fichier[], int * nombreDeplacements){
    FILE * f;
    char dep;
    *nombreDeplacements = 0;

    f = fopen(fichier, "r");
    if (f==NULL){
        printf("FICHIER NON TROUVE\n");
        return;
    }
    
    while (fread(&dep, sizeof(char), 1, f) == 1){
        if (dep != '\n' && dep != '\r') {
            t[*nombreDeplacements] = dep;
            (*nombreDeplacements)++;
        }
    }
    
    fclose(f);
    printf("Chargement de %d mouvements depuis %s\n", *nombreDeplacements, fichier);
}

void optimisationFichierDeplacement(t_tabDeplacement t, int * nbDeplacements){

    
}

typedef struct {
    int x;
    int y;
} t_Position;
// Place le curseur à la position (x, y) dans le terminal
void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y+1, x+1);
}
// Applique un déplacement du sokoban selon la direction donnée.
void deplacer(t_Plateau plateau, char direction, int *index){
    int deltalig = 0, deltaCol = 0;
    char direction_convertie = direction;
    
    // Mapper les caractères de déplacement au code interne
    switch (tolower(direction)) {
        case 'h': // haut
            direction_convertie = HAUT;
            break;
        case 'b': // bas
            direction_convertie = BAS;
            break;
        case 'g': // gauche
            direction_convertie = GAUCHE;
            break;
        case 'd': // droite
            direction_convertie = DROITE;
            break;
        default:
            return; 
    }
    
    switch (direction_convertie) {
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
        return;
    }
    if (targetChar == CAISSE || targetChar == CAISSE_SUR_CIBLE){
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
        }
        return;
    }
    return;
}
// Vérifie si toutes les caisses sont sur les cibles (condition de victoire).
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
