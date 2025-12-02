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
void enregistrer_partie(t_Plateau plateau, char fichier[]);
void charger_partie(t_Plateau plateau, char fichier[]);
void afficher_entete(char fichier[],int nbDeplacement);
void afficher_plateau(t_Plateau plateau, int zoom);
void deplacer(t_Plateau plateau, char direction,int *nbDeplacement,t_tabDeplacement t, char touche, bool pousse_caisse, int i);
void recommencer(t_Plateau plateau, int nbDeplacement, char fichier[]);
void gotoxy(int x, int y);
bool gagne(t_Plateau plateau);
void deplacements(t_tabDeplacement t , char touche, int i, bool pousse_caisse);
void annuler_dernier_deplacement(t_tabDeplacement t, int *nbDeplacement, t_Plateau plateau, char touche, bool pousse_caisse, int i);
void enregistrer_deplacements(t_tabDeplacement tabDeplacement, int nb, char fic[]);

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
            if(touche == ABANDONNER ) {
                char enregistrement = ' ';
                char fichier[50] = " ";
                char enregistrementDep = ' ';
                char fichierDep[50] = " ";
                printf(" Voulez-vous enregistrer la partie ?\n O pour oui, N pour non\n");
                scanf("%c", &enregistrement);
                if ( enregistrement == OUI){
                    printf("Veuillez entrer le nom du fichier.sok pour enregistrer la partie : ");
                    scanf("%s", fichier);
                    enregistrer_partie(plateau, fichier);
                }
                printf("Voulez -vous enregistrer les déplacements ?\n O pour oui, N pour non\n");
                scanf(" %c", &enregistrementDep);
                if ( enregistrementDep == OUI){
                    printf("Veuillez entrer le nom du fichier.dep pour enregistrer les déplacements : ");
                    scanf("%s", fichierDep);
                    enregistrer_deplacements(t, nbDeplacement, fichierDep);
                }
                system("clear");
                break; 
            }
            else if (touche == RECOMMENCER){

                nbDeplacement = 0;
                recommencer(plateau, nbDeplacement,fichier);
            }
            deplacer(plateau, touche, &nbDeplacement, t, touche, pousse_caisse, i);
            
            afficher_entete(fichier, nbDeplacement);
            afficher_plateau(plateau, zoom);
            if(touche == ZOOMER && zoom < 3){

                zoom++;
                system("clear");
                deplacer(plateau, touche, &nbDeplacement, t, touche, pousse_caisse, i);
                afficher_entete(fichier, nbDeplacement);
                afficher_plateau(plateau, zoom);
            }
            else if(touche == DEZOOMER && zoom > 1){
                zoom--;
                system("clear");
                deplacer(plateau, touche, &nbDeplacement, t, touche, pousse_caisse, i);
                afficher_entete(fichier, nbDeplacement);
                afficher_plateau(plateau, zoom);
            }
            else if(touche == ANNULER){
                annuler_dernier_deplacement(t, &nbDeplacement, plateau, touche, pousse_caisse, i);
                system("clear");
                afficher_entete(fichier, nbDeplacement);
                afficher_plateau(plateau, zoom);
            }
            bool gagnee = gagne(plateau);
            if (gagnee){
                printf("\nFélicitations, vous avez gagné en %d déplacements !\n", nbDeplacement);
                char enregistrementDep = ' ';
                char fichierDep[50] = " ";                
                printf("Voulez -vous enregistrer les déplacements ?\n O pour oui, N pour non\n");
                scanf(" %c", &enregistrementDep);
                if ( enregistrementDep == OUI){
                    printf("Veuillez entrer le nom du fichier.dep pour enregistrer les déplacements : ");
                    scanf("%s", fichierDep);
                    enregistrer_deplacements(t, nbDeplacement, fichierDep);
                }
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
// Recommence la partie en rechargeant le plateau depuis le fichier actuel
void recommencer(t_Plateau plateau, int nbDeplacement, char fichier[]){

    char reponse = ' ';
    printf("Êtes vous sûr de recommencer ?\n O pour oui, N pour non");
    scanf("%c",&reponse);
    if (reponse == OUI){
        int zoom = 1;
        system("clear");
        afficher_entete(fichier,nbDeplacement);
        chargerPartie(plateau, fichier);
        afficher_plateau(plateau, zoom);    
    }
}
// Enregistre la partie dans un fichier lu au clavier
void enregistrer_partie(t_Plateau plateau, char fichier[]){

    FILE * f;
    char finDeLigne='\n';

    f = fopen(fichier, "w");
    for (int ligne=0 ; ligne<TAILLE ; ligne++){
        for (int colonne=0 ; colonne<TAILLE ; colonne++){
            fwrite(&plateau[ligne][colonne], sizeof(char), 1, f);
        }
        fwrite(&finDeLigne, sizeof(char), 1, f);
    }
    fclose(f);
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

// Procédure enregistrant les déplacements

void enregistrer_deplacements(t_tabDeplacement tabDeplacement, int nb, char fic[]){
    FILE * f;

    f = fopen(fic, "w");
    fwrite(tabDeplacement,sizeof(char), nb, f);
    fclose(f);
}
void deplacements(t_tabDeplacement t , char touche, int i, bool pousse_caisse){

    printf("%d et %c\n", pousse_caisse, touche);
    if(pousse_caisse == true && touche == BAS){
        t[i] = DEPLACE_CAISSE_BAS;
    }
    if(pousse_caisse == true && touche == HAUT){
        t[i] = DEPLACE_CAISSE_HAUT;
    }
    if(pousse_caisse == true && touche == GAUCHE){
        t[i] = DEPLACE_CAISSE_GAUCHE;
    }
    if(pousse_caisse == true && touche == DROITE){
        t[i] = DEPLACE_CAISSE_DROITE;
    }
    if(pousse_caisse == false && touche == BAS){
        t[i] = DEPLACE_BAS;
    }
    if(pousse_caisse == false && touche == HAUT){
        t[i] = DEPLACE_HAUT;
    }
    if(pousse_caisse == false && touche == GAUCHE){
        t[i] = DEPLACE_GAUCHE;
    }
    if(pousse_caisse == false && touche == DROITE){
        t[i] = DEPLACE_DROITE;
    }
}


// Fonction pour annuler le dernier déplacement, utilisant le tableau des déplacements des déplacement enregistrés

void annuler_dernier_deplacement(t_tabDeplacement t, int *nbDeplacement, t_Plateau plateau, char touche, bool pousse_caisse, int i){
    int deltalig = 0, deltaCol = 0;
    if(*nbDeplacement <= 0) return;
    (*nbDeplacement)--;
    char dernier = t[*nbDeplacement];
    bool annule_caisse = false;
    switch(dernier){
        case DEPLACE_BAS:
            deltalig = -1;
            break;
        case DEPLACE_HAUT:
            deltalig = 1;
            break;
        case DEPLACE_GAUCHE:
            deltaCol = 1;
            break;
        case DEPLACE_DROITE:
            deltaCol = -1;
            break;
        case DEPLACE_CAISSE_BAS:
            deltalig = -1; 
            annule_caisse = true;
            break;
        case DEPLACE_CAISSE_HAUT:
            deltalig = 1;
            annule_caisse = true;
            break;
        case DEPLACE_CAISSE_GAUCHE:
            deltaCol = 1;
            annule_caisse = true;
            break;
        case DEPLACE_CAISSE_DROITE:
            deltaCol = -1;
            annule_caisse = true;
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
        }if (sokobanlig != -1) break;
    }if (sokobanlig == -1) return;
    if (!annule_caisse) {
        int ligPrecedente = sokobanlig + deltalig;
        int colPrecedente = sokobanCol + deltaCol;
        if (ligPrecedente < 0 || ligPrecedente >= TAILLE || colPrecedente < 0 
            || colPrecedente >= TAILLE) return;
        plateau[sokobanlig][sokobanCol] = (plateau[sokobanlig][sokobanCol] 
            == SOKOBAN_SUR_CIBLE) ? CIBLE : CASE_VIDE;
        plateau[ligPrecedente][colPrecedente] = (plateau[ligPrecedente][colPrecedente] 
            == CIBLE) ? SOKOBAN_SUR_CIBLE : SOKOBAN;
        return;
    }
    int DeltaLigOriginal = -deltalig;
    int DeltaColO = -deltaCol;
    int ligneCaisse = sokobanlig + DeltaLigOriginal;
    int colonneCaisse = sokobanCol + DeltaColO;
    int lignePrecedenteSok = sokobanlig - DeltaLigOriginal;
    int colonnePrecedenteSok = sokobanCol - DeltaColO;
    if (ligneCaisse < 0 || ligneCaisse >= TAILLE || colonneCaisse < 0 
        || colonneCaisse >= TAILLE) return;
    if (lignePrecedenteSok < 0 || lignePrecedenteSok >= TAILLE 
        || colonnePrecedenteSok < 0 || colonnePrecedenteSok >= TAILLE) return;
    if (plateau[ligneCaisse][colonneCaisse] == CAISSE_SUR_CIBLE) {
        plateau[sokobanlig][sokobanCol] = CAISSE_SUR_CIBLE;
    } else {
        plateau[sokobanlig][sokobanCol] = CAISSE;
    }
    plateau[ligneCaisse][colonneCaisse] = (plateau[ligneCaisse][colonneCaisse] 
        == CAISSE_SUR_CIBLE) ? CIBLE : CASE_VIDE;
    plateau[lignePrecedenteSok][colonnePrecedenteSok] 
    = (plateau[lignePrecedenteSok][colonnePrecedenteSok] == CIBLE) ? SOKOBAN_SUR_CIBLE : SOKOBAN;
    return;
}
