// Progetto di Antonio Labranca
// Numero di matricola 1217252
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct{
	int x, y;
}posizione;

typedef struct{
	posizione pos;
	char ch;
	bool visitato;
}cella;

typedef struct nodo{
	posizione pos;
	struct nodo* next;
}nodo;

// variabile bandiera indica il raggiungimento della fine del labirinto
bool bandiera = false;

/* leggi_griglia function declaration
 *
 * apre in lettura il File filename
 * legge il numero di righe e di colonna e le resituisce per riferimento
 * crea una matrice labirinto
 * restituisce il puntatore alla prima Cella della matrice labirinto
 *
 */
cella** leggi_griglia(char* filename, int* righe, int* colonne);

/* isWall function declaration
 *
 * data una matrice di Celle e una posizione
 * verifica se nella matrice[x][y].ch c'è un muro
 * restituisce un Booleano
 *
 */
bool isWall (cella** griglia, posizione pos);

/* isCulDeSac function declaration
 *
 * data una matrice di Celle e una posizione
 * verifica attraverso la chiamata alla funzione isWall
 * se è un vicolo cieco e restituisce un Booleano
 *
 */
bool isCulDeSac (cella** griglia, posizione pos);

/* hasDifferentWays function declaration
 *
 * data una matrice di Celle e una posizione
 * verifica attraverso la chiamata alla funzione isWall
 * se ha almeno due strade possibili non già attraversate
 * restituisce un Booleano
 *
 */
bool hasDifferentWays(cella** griglia, posizione pos);

/* isEmpty function declaration
 *
 * dato un puntatore ad una lista
 * restituisce vero se è vuota
 *
 */
bool isEmpty(nodo* lista);

/* initialize_search function declaration
 *
 * data una griglia, una posizione, le righe e le colonne
 * inizializza le posizioni su, giù, destra, sinistra e le restituisce per riferimento
 * inizializza la posizione snodo a 0 e la restituisce per riferimento
 *
 */
void initialize_search(cella** griglia, int righe, int colonne, posizione pos, posizione* snodo, posizione* up, posizione* down, posizione* left, posizione* right);

/* push_lista function declaration
 *
 * dato un puntatore ad una lista e una posizione
 * inserisce un nuovo nodo con la posizione in testa alla lista
 * restituisce un puntatore alla testa della lista
 *
 */
nodo* push_lista(nodo* lista, posizione pos);

/* ricerca function declaration
 *
 * funzione con chiamata ricorsiva
 *
 * dati una griglia, una posizione, un arrivo, le righe e le colonne
 * invoca le funzioni initialize_search, push_lista, hasDifferentWays, isCulDeSac, isWall
 * trova la strada di uscita del labirinto
 * restituisce un puntatore ad una lista di nodi contenenti le posizioni in sequenza
 *
 */
nodo* ricerca(cella** griglia, posizione pos, posizione goal, int righe, int colonne);

/* stampa_griglia function declaration
 *
 * data una griglia, le righe, le colonne e una Posizione pos
 * stampa la griglia con un '*' in posizione pos
 *
 */
void stampa_griglia(cella** griglia, int righe, int colonne, posizione pos);

/* stampa function declaration
 *
 * funzione con chiamata ricorsiva
 *
 * data una lista di nodi, una griglia, le righe e le colonne
 * invoca la funzione stampa_griglia
 * stampa N volte la griglia con la soluzione al labirinto
 *
 */
void stampa(nodo* lista, cella** griglia, int righe, int colonne);

/* main function definition */
int main(int argc, char* argv[]){
    cella** matrice;
    nodo* percorso = NULL;
    posizione partenza, arrivo;
    int righe, colonne;
    
    // leggo da file la griglia e la restituisco come matrice[x][y]
    matrice = leggi_griglia(argv[1], &righe, &colonne);
    
    // inizializzo le posizioni di partenza e arrivo
    partenza.x = 1;
    partenza.y = 1;
    arrivo.x = colonne - 2;
    arrivo.y = righe - 1;
    
    /* eseguo una ricerca ricorsiva del percorso per uscire dal labirinto
       e lo restituisco tramite un puntatore ad una lista di nodi */
    percorso = ricerca(matrice, partenza, arrivo, righe, colonne);
    
    // controllo se la lista è stata inizializzata
    if(isEmpty(percorso)){
        fprintf(stderr, "\nERRORE! La lista è vuota\n");
        exit(EXIT_FAILURE);
    } /* end if */
    
    // eseguo la stampa del labirinto con la soluzione
    stampa(percorso, matrice, righe, colonne);
    
    printf("\n\n");
    free(percorso);
    free(matrice);
    return 0;
}

/* leggi_cella function definition */
cella** leggi_griglia(char* filename, int* righe, int* colonne){
    FILE* file;
    char carattere;
    cella** matrice;
    int i, j;
    
    // inizializzo a -1 righe e colonne per verificare la corretta lettura delle stesse dal file
    *righe = -1;
    *colonne = -1;
    
    // apro il file in lettura
    file = fopen(filename, "r");
    
    // eseguo il controllo sull'avvenuta apertura del file
    if (file == NULL){
        fprintf(stderr, "\nERRORE! Non è stato possibile aprire il file\n\n");
        exit(EXIT_FAILURE);
    } /* end if */
    
    // leggo il numero di righe e colonne dal file
    fscanf(file, "%d\n%d", righe, colonne);
    
    // eseguo il controllo sulle righe e colonne della griglia
    if (*righe == -1 || *colonne == -1){
        fprintf(stderr, "\nERRORE! Non è possibile riconoscere il numero di righe e/o colonne\n\n");
        exit(EXIT_FAILURE);
    } /* end if */
    else if (*righe < 3 || *colonne < 3){
        fprintf(stderr, "\nERRORE! Non è possibile avere un labirinto con %d righe e %d colonne", *righe, *colonne);
        exit(EXIT_FAILURE);
    } /* end else if */
    
    // alloco spazio in memoria per la matrice[x][y]
    matrice = malloc(sizeof(cella*) * (*colonne));
    for (i = 0; i < *colonne; i++) {
        matrice[i] =  malloc(sizeof(cella) * (*righe));
    } /* end for */
    
    // inizializzo la matrice con i dati contenuti nel file
    for (j = 0; j < (*righe); j++) {
        for(i = 0; i < (*colonne); i++){
            fscanf(file, "%c", &carattere);
            if (carattere == 'X' || carattere == ' ') {
                matrice[i][j].ch = carattere;
                matrice[i][j].pos.x = i;
                matrice[i][j].pos.y = j;
            } /* end if */
            else
                i--;
        } /* end for */
    } /* end for */
    
    // chiudo il file e restituisco la matrice
    fclose(file);
    return matrice;
} /*  end of leggi_griglia  */

/* isWall function definition */
bool isWall (cella** griglia, posizione pos){
    return (griglia[pos.x][pos.y].ch == 'X' || griglia[pos.x][pos.y].ch != ' ');
} /*  end of isWall  */

/* isCulDeSac function definition */
bool isCulDeSac (cella** griglia, posizione pos){
    posizione up = pos, down = pos, left = pos, right = pos;
    up.y--;
    down.y++;
    left.x--;
    right.x++;
    
    if (isWall(griglia, up) && isWall(griglia, down) && isWall(griglia, left))
        return true;
    else if(isWall(griglia, up) && isWall(griglia, down) && isWall(griglia, right))
        return true;
    else if(isWall(griglia, up) && isWall(griglia, right) && isWall(griglia, left))
        return true;
    else if(isWall(griglia, down) && isWall(griglia, right) && isWall(griglia, left))
        return true;
    else
        return false;
} /*  end of isCulDeSac  */

/* hasDifferentWays function definition */
bool hasDifferentWays(cella** griglia, posizione pos){
    posizione up = pos, down = pos, left = pos, right = pos;
    
    up.y--;
    down.y++;
    left.x--;
    right.x++;
    
    if(!isWall(griglia, down) && !isWall(griglia, right) && !griglia[down.x][down.y].visitato && !griglia[right.x][right.y].visitato)
        return true;
    else if (!isWall(griglia, up) && !isWall(griglia, down) && !griglia[up.x][up.y].visitato && !griglia[down.x][down.y].visitato)
        return true;
    else if (!isWall(griglia, up) && !isWall(griglia, right) && !griglia[up.x][up.y].visitato && !griglia[right.x][right.y].visitato)
        return true;
    else if (!isWall(griglia, left) && !isWall(griglia, right) && !griglia[left.x][left.y].visitato && !griglia[right.x][right.y].visitato)
        return true;
    else if(!isWall(griglia, up) && !isWall(griglia, left) && !griglia[up.x][up.y].visitato && !griglia[left.x][left.y].visitato)
        return true;
    else if(!isWall(griglia, down) && !isWall(griglia, left) && !griglia[down.x][down.y].visitato && !griglia[left.x][left.y].visitato)
        return true;
    else
        return false;
} /*  end of hasDifferentWays  */

/* isEmpty function definition */
bool isEmpty(nodo* lista){
    return lista->next == NULL;
} /*  end of isEmpty  */

/* initialize_search function definition */
void initialize_search(cella** griglia, int righe, int colonne, posizione pos, posizione* snodo, posizione* up, posizione* down, posizione* left, posizione* right){
    *up = pos;
    *down = pos;
    *left = pos;
    *right = pos;
    snodo->x = 0;
    snodo->y = 0;
    griglia[pos.x][pos.y].visitato = true;
    (up->y)--;
    (down->y)++;
    (left->x)--;
    (right->x)++;
} /*  end of inizialize_search  */

/* push_lista function definition */
nodo* push_lista(nodo* lista, posizione pos){
    nodo* tmp = (nodo*)malloc(sizeof(nodo));
    
    tmp->pos.x = pos.x;
    tmp->pos.y = pos.y;
    tmp->next = lista;
    
    return tmp;
} /*  end of push_lista  */

/* ricerca function definition */
nodo* ricerca(cella** griglia, posizione pos, posizione goal, int righe, int colonne){
    posizione up, down, left, right, snodo, pos_next;
    nodo* lista;
    
    initialize_search(griglia, righe, colonne, pos, &snodo, &up, &down, &left, &right);
        
    // verifica se si è raggiunto l'arrivo
    if(pos.x == goal.x && pos.y == goal.y){
        bandiera = true;
        return push_lista(NULL, pos);
    } /* end if */
        
    // verifica se ci sono più vie d'uscita
    if (hasDifferentWays(griglia, pos)) {
        snodo.x = pos.x;
        snodo.y = pos.y;
    } /* end if */
        
    // controllo principale delle posizioni up, dw, dx, sx
    if (isCulDeSac(griglia, pos) && !(pos.x == 1 && pos.y == 1))
        return NULL;
    else if (!isWall(griglia, down) && !griglia[down.x][down.y].visitato)
        pos_next = down;
    else if (!isWall(griglia, right) && !griglia[right.x][right.y].visitato)
        pos_next = right;
    else if (!isWall(griglia, left) && !griglia[left.x][left.y].visitato)
        pos_next = left;
    else if(!isWall(griglia, up) && !griglia[up.x][up.y].visitato)
        pos_next = up;
    else if (snodo.x == 0)
        return NULL;
    else{
        fprintf(stderr, "\nERRORE! comportamento inaspettato in posizione\n>>> x = %d\n>>> y = %d", pos.x, pos.y);
        exit(EXIT_FAILURE);
    } /* end else */
    
    // chiamata ricorsiva
    lista = ricerca(griglia, pos_next, goal, righe, colonne);
    
    // crea la lista di nodi se si è arrivati all'arrivo
    if(bandiera)
        return push_lista(lista, pos);
     
    // serie di chiamate ricorsive nel caso in cui ci possano essere più vie in una posizione
    // (le prova tutte e 4)
    if (pos.x == snodo.x && pos.y == snodo.y && !bandiera){
        lista = ricerca(griglia, pos, goal, righe, colonne);
        if (pos.x == snodo.x && pos.y == snodo.y && !bandiera) {
            lista = ricerca(griglia, pos, goal, righe, colonne);
            if (pos.x == snodo.x && pos.y == snodo.y && !bandiera) {
                lista = ricerca(griglia, pos, goal, righe, colonne);
            } /* end if */
        } /* end if */
    } /* end if */
    else if(lista == NULL && !bandiera)
        return NULL;
    
    return push_lista(lista, pos);
} /*  end of ricerca  */

/* stampa_griglia function definition */
void stampa_griglia(cella** griglia, int righe, int colonne, posizione pos){
    int i, j; // support variables for cycles
    
    griglia[pos.x][pos.y].ch = '*';
    printf("\n");
    
    for (j = 0; j < righe; j++) {
        for (i = 0; i < colonne; i++) {
            printf("%c", griglia[i][j].ch);
        } /* end for */
        printf("\n");
    } /* end for */
    
    griglia[pos.x][pos.y].ch = ' ';
} /*  end of stampa_griglia  */

/* stampa function definition */
void stampa(nodo* lista, cella** griglia, int righe, int colonne){
    // se non si è raggiunta la fine della lista esegue stampa la griglia con la posizione '*'
    if (lista != NULL){
        stampa_griglia(griglia, righe, colonne, lista->pos);
        printf("\n");
        stampa(lista->next, griglia, righe, colonne);
    } /* end if */
} /*  end of stampa  */
