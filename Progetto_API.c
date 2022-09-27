#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define bool _Bool
#define false 0
#define true 1

typedef struct occ
{
    int8_t elem;
    bool esatto_num;
} occ; // abbiamo un carattere e un booleano che ci servirà per il vincolo

typedef struct vincolo
{                         // Mi segno i caratteri nella giusta posizione e nelle occorrenze il numero minimo di caratteri
    occ occorrenze[64];   // Segnerò l'istogramma delle frequenze con un booleano per dirmi se il numero è esatto
    char *pos_corr;       // array che allocherò di dimensione k che mi dice le posizioni in cui vi è sicuramente un dato carattere
    bool **car_non_pres;  // matrice booleana che mi dice in quali posizioni non è presente un carattere
    char *ref;            // parola di riferimento
    u_int8_t ist_rif[64]; // istogramma della nostra parola di riferimento
    int len;              // lunghezza per non dover fare strlen() ogni volta -> sarebbe k
    //////////
    char *car_eff_pres; // caratteri effettivamente presenti per velocizzare la verifica se una data parola rispetta i vincoli
} vincolo;

typedef struct Node
{
    char *w;                          // parola allocata (word)
    struct Node *left, *right, *next; // nodo sx, dx, prossima parola filtrata
} BSTNode;

typedef struct BST
{
    BSTNode *root;
} BST;

char hcs(char x)
{
    if (x == '-')
        return 0;
    if (x >= '0' && x <= '9')
        return x - 47;
    if (x == '_')
        return 63;
    if (x >= 'A' && x <= 'Z')
        return x - 54;
    else
        return x - 60;
} // hash character set -> serve per poter fare l'istogramma dei caratteri di soli 64 elementi invece che allocare tutti e 256 elementi della tabella ASCII

void counting(char *string, u_int8_t *istogramma, int len)
{
    for (u_int8_t i = 0; i < len; i++)
        istogramma[(unsigned char)hcs(string[i])]++;
} // costruisce l'istogramma data una stringa

void confronto(char *da_confr, vincolo *vinc)
{
    int len = vinc->len, j = 0;
    char res[len + 1]; // allochiamo una stringa di dimensione uguale a k+1
    memset(res, 0, sizeof(char) * (len + 1));
    u_int8_t ist_da_confr[64] = {0};
    u_int8_t c_i[64] = {0};                // #occorrenze di carattere in posizione corretta prima di i
    u_int8_t num[64] = {0};                // #occorrenze di carattere in posizione SCORRETTA prima di i
    u_int8_t temp[64] = {0};               // #occorrenze per '|'
    counting(da_confr, ist_da_confr, len); // calcolo l'istogramma della mia parola da confrontare

    // Faccio una prima passata in cui non tengo conto del vincolo n_i-c_i
    /*
    Ricapitolando
    n_i = #occorrenze di p[i] in r
    c_i = #occorrenze di carattere in posizione corretta
    num = numero di caratteri in posizione scorretta prima di i
    */

    for (u_int8_t i = 0; i < len; i++)
    {
        j = hcs(da_confr[i]);
        if (vinc->ref[i] == da_confr[i])
        {
            res[i] = '+';
            c_i[j]++;                        // aggiorno il numero di caratteri in posizione corretta
            vinc->pos_corr[i] = da_confr[i]; // aggiorno le posizioni corrette
            temp[j]++;
        }
        else if (vinc->ist_rif[j] != 0 && ist_da_confr[j] != 0)
        {
            res[i] = '|';
            ist_da_confr[j]--; // decremento
            temp[j]++;         // per poter inserire il numero minimo di caratteri presenti nel caso non trovasse poi / col numero esatto
            vinc->car_non_pres[j][i] = true;
        }
        else
        {
            res[i] = '/';
            if (vinc->occorrenze[j].esatto_num != true)
            {                                  // magari avevo trovato in un altro caso il numero esatto
                vinc->occorrenze[j].elem = -1; //-1 indica che non ce ne sono completamente
                for (u_int8_t w = 0; w < len; w++)
                    vinc->car_non_pres[j][w] = true; // dirò che in ogni indice della lettera j non sarà presente
            }
        }
    }
    // dopo questa passata devo verificare il vincolo n_i - c_i

    for (u_int8_t i = 0; i < len; i++)
    {
        j = hcs(da_confr[i]); // ovvero mi restutuisce un indice da 0-63
        if (res[i] == '|')
        { // inizia il vero confronto
            if (vinc->ist_rif[j] - c_i[j] <= num[j])
            { // ovvero se num >= n_i - c_i
                res[i] = '/';
                if (vinc->occorrenze[j].esatto_num != true)
                    vinc->occorrenze[j].elem = num[j] + c_i[j];
                vinc->occorrenze[j].esatto_num = true;
            }
            if (res[i] != '+')
                num[j]++; // aggiorno il numero di caratteri in posizione scorretta
        }
    }

    for (u_int8_t i = 0; i < 64; i++)
    {
        if (vinc->occorrenze[i].esatto_num != true && temp[i] > vinc->occorrenze[i].elem && vinc->occorrenze[i].elem != -1)
            vinc->occorrenze[i].elem = temp[i]; // aggiorno solo se trovo un MINIMO più grande chiaramente
    }

    printf("%s\n", res); // Stampo la stringa derivata dal confronto tra riferimento e una generica parola del dizionario
}

bool rispetta_vincolo(vincolo *vinc, char *p) // Funzione che verifica se una data parola rispetta i vincoli stabiliti durante una data partita
{
    u_int8_t ist_da_confr[64] = {0};
    int k = vinc->len;
    if (strcmp(vinc->ref, p) == 0) // caso in cui coincida con la parola di riferimento
        return true;

    for (u_int8_t i = 0; i < k; i++)
    {
        u_int8_t j = hcs(p[i]);
        if (vinc->pos_corr[i] != 0 && vinc->pos_corr[i] != p[i]) // caso in cui non combaciano i caratteri
            return false;
        if (vinc->car_non_pres[j][i] == true) // caso in cui sappiamo che in quell'indice non è presente quel carattere
            return false;
    }
    counting(p, ist_da_confr, k);

    for (u_int8_t i = 0; i < k; i++)
    {
        u_int8_t j = hcs(vinc->car_eff_pres[i]);
        if (vinc->car_eff_pres[i] == 0)
            return true;
        if (vinc->occorrenze[j].elem > ist_da_confr[j]) // non rispetta vincolo minimo
            return false;
        if (ist_da_confr[j] > vinc->occorrenze[j].elem && vinc->occorrenze[j].esatto_num == true)
            return false; // caso in cui la nostra parola ha più caratteri ma sappiamo il numero esatto
    }

    return true;
}

bool is_in_RB(BSTNode *node, char *string) //Ricerca binaria per verificare se una parola appartiene al dizionario
{
    if (node == NULL)
        return false;
    if (strcmp(node->w, string) == 0)
        return true;
    if (strcmp(node->w, string) > 0)
        return is_in_RB(node->left, string);
    else
        return is_in_RB(node->right, string);
}

BSTNode *insert(BST *T, char *temp, int k)
{
    BSTNode *pre = NULL, *z = malloc(sizeof(BSTNode)), *cur;
    z->w = (char *)malloc(sizeof(char) * (k));
    strcpy(z->w, temp);
    z->right = NULL;
    z->left = NULL;
    z->next = NULL;
    cur = T->root;
    while (cur != NULL)
    {
        pre = cur;
        if (strcmp(temp, cur->w) == 0)
        {
            free(z->w);
            free(z);
            return cur;
        }
        if (strcmp(temp, cur->w) < 0)
        {
            cur = cur->left;
        }
        else
            cur = cur->right;
    }
    if (pre == NULL)
        T->root = z;
    else if (strcmp(z->w, pre->w) < 0)
        pre->left = z;
    else
        pre->right = z;

    return z;
}

void inorder(BSTNode *T) //Funzione che stampa tutto l'albero con una visita in-order nel caso venga richiesto "+stampa_filtrate" prima di avere dei vincoli, così da non dovere costruire una lista inutilmente
{
    if (T == NULL)
        return;
    inorder(T->left);
    printf("%s\n", T->w);
    inorder(T->right);
}

void distruggi_RB(BSTNode *node) //Funzione per fare la free di tutti i nodi dell'albero 
{
    BSTNode *L, *R;
    if (node == NULL)
        return;
    L = node->left;
    R = node->right;
    free(node->w);
    free(node);
    distruggi_RB(L);
    distruggi_RB(R);
}

void inserisci_inizio(BST *T, int k) //Nel caso venga richiesto "+inserisci_inizio" al di fuori di una partita si limita ad aggiungere nel dizionario
{
    char buffer[20];
    if (scanf(" %s", buffer));
    while (strcmp(buffer, "+inserisci_fine") != 0)
    {
        insert(T, buffer, k + 1);
        if (scanf(" %s", buffer));
    }
}

//////////////////////////////////

/////////////////////////
void stampa_lista(BSTNode *Testa) // Per quando viene chiamato "+stampa_filtrate"
{
    while (Testa != NULL)
    {
        printf("%s\n", Testa->w);
        Testa = Testa->next;
    }
}

void Ins_Ordine(BSTNode **Testa, BSTNode *new) //Funzione che serve ad inserire una stringa in ordine lessicografico in una lista 
{
    if (*Testa == NULL || strcmp((*Testa)->w, new->w) >= 0)
    {
        new->next = *Testa;
        *Testa = new;
        return;
    }

    BSTNode *curr = *Testa;
    while (curr->next != NULL && strcmp(curr->next->w, new->w) < 0)
    {
        curr = curr->next;
    }
    new->next = curr->next;
    curr->next = new;
}

int conta_nodi(BSTNode *Testa) // Conta il numero di elementi di una data lista
{
    int count = 0;
    while (Testa != NULL)
    {
        count += 1;
        Testa = Testa->next;
    }
    return count;
}

/////////////////////////////

void primo_inserimento(BSTNode **filtr, BSTNode *node, vincolo *vinc) //è il primo inserimento nella "lista" dei filtrati; fa quindi una visita in_order decrescente dell'albero e inserisce le parole nella lista nel caso rispettino il vincolo
{
    if (node == NULL)
        return;
    primo_inserimento(filtr, node->right, vinc);
    if (rispetta_vincolo(vinc, node->w) == true)
    {
        node->next = *filtr;
        *filtr = node;
    }
    primo_inserimento(filtr, node->left, vinc);
}

void filtra_lista(BSTNode **Testa, vincolo *vinc) // Ogni volta che aggiorniamo i vincoli dobbiamo verificare se le stringhe presenti in lista siano ancora ammissibili
{
    BSTNode *curr = *Testa, *prev = NULL;
    bool risp_vinc, flag = false;
    while (curr != NULL)
    {
        risp_vinc = rispetta_vincolo(vinc, curr->w);
        if (curr == *Testa && risp_vinc == false)
        { // caso in cui debba eliminare la testa
            *Testa = curr->next;
            curr = *Testa;
            flag = true;
        }
        else if (risp_vinc == false)
        {
            prev->next = curr->next;
            curr = prev;
        }
        prev = curr;
        if (flag != true)
            curr = curr->next;
        else
            flag = false;
    }
}

BSTNode *merge(BSTNode *L1, BSTNode *L2) // Funzione che fa un merge di due liste mantenendo l'ordine lessicografico
{
    if (!L1)
        return L2;
    if (!L2)
        return L1;

    BSTNode *Testa, *curr;
    if (strcmp(L1->w, L2->w) < 0)
    { // All'inizio stabiliamo chi è la testa della nuova lista
        Testa = L1;
        L1 = L1->next;
    }
    else
    {
        Testa = L2;
        L2 = L2->next;
    }

    curr = Testa;
    while (L1 && L2)
    {
        if (strcmp(L1->w, L2->w) < 0)
        {
            curr->next = L1;
            L1 = L1->next;
        }
        else
        {
            curr->next = L2;
            L2 = L2->next;
        }
        curr = curr->next;
    }
    //dopo aver scorso entrambe le liste vediamo se ci sono elementi rimasti
    if(!L1)
        curr -> next = L2;
    else
        curr -> next = L1;
            
    return Testa;
}

void inserisci_inizio_mod(BSTNode **filtr, BST *T, vincolo *vinc) 
{ // per quando viene chiamato "+inserisci_inizio" a partita iniziata
    char buffer[20];
    BSTNode *tmp;
    int k = vinc->len;
    if (scanf(" %s", buffer))
    {
    }
    //////////////////////////
    BSTNode *nuovi_filtrati = NULL; // lista dei nuovi filtrati -> serve per mantenere leggero l'inserimento in ordine su una lista che nel caso pessimo mi costa quadraticamente
    //////////////////////////
    while (strcmp(buffer, "+inserisci_fine") != 0)
    {
        tmp = insert(T, buffer, k + 1);
        if (rispetta_vincolo(vinc, tmp->w) == true) // Inserisco solo se strettamente necessario
            Ins_Ordine(&nuovi_filtrati, tmp);
        if (scanf(" %s", buffer));
    }
    (*filtr) = merge(*filtr, nuovi_filtrati); // dopo aver terminato l'inserimento faccio il merge delle due liste ordinate modificando la testa della nuova lista dei filtrati
}

char hcs_reverse(int x) // funzione che è l'inversa di hcs()
{
    if (x == 0)
        return '-';
    if (x >= 1 && x <= 10)
        return x + 47;
    if (x >= 11 && x <= 36)
        return x + 54;
    if (x >= 37 && x <= 62)
        return x + 60;
    else
        return '_';
}

void riduci_occorrenze(vincolo *vinc)
{
    int i, j = 0;
    char tmp;
    if (vinc->car_eff_pres[vinc->len - 1] != 0) // nel caso si conoscano già tutti i caratteri presenti; è chiaramente una semplificazione perchè non è detto che siano tutti e k diversi. Si potrebbe quindi ottimizzare ma non si è rivelato necessario in quanto i bottleneck col tempo si sono rivelati altri
        return;
    for (i = 0; i < 64; i++)
    {
        if (vinc->occorrenze[i].elem > 0)
        {
            tmp = hcs_reverse(i);
            for (j = 0; j < vinc->len; j++)
            {
                if (vinc->car_eff_pres[j] == tmp)
                    break;
                else if (vinc->car_eff_pres[j] == 0)
                {
                    vinc->car_eff_pres[j] = tmp;
                    break;
                }
            }
        }
    }
}

void resetta_filtro(BSTNode *T) // Nel caso finisse la partita è necessario resettare la lista delle parole ammissibili
{
    BSTNode *tmp;
    while (T != NULL)
    {
        tmp = T->next;
        T->next = NULL;
        T = tmp;
    }
}

////////////////////////////
void nuova_partita(BST *T, vincolo *vinc)
{
    char buffer[20];
    int num_iter = 0, k = vinc->len; // numero iterazioni
    if (scanf(" %s", buffer)){}; // leggo la parola di riferimento
    /////////////////////
    // INIZIALIZZAZIONE LISTA FILTRATI
    BSTNode *filtrati = NULL; // testa dei miei filtrati
    /////////////////////
    // RESET VINCOLO
    memset(vinc->occorrenze, 0, sizeof(vinc->occorrenze));
    memset(vinc->ist_rif, 0, sizeof(vinc->ist_rif)); // azzera istogramma parola riferimento
    memset(vinc->pos_corr, 0, sizeof(char) * (k + 1));
    memset(vinc->car_eff_pres, 0, sizeof(char) * k);
    for (int n = 0; n < 64; ++n)
        memset(vinc->car_non_pres[n], 0, sizeof(bool) * k);
    /////////////////////

    strcpy(vinc->ref, buffer);                     // copiamo la stringa
    counting(vinc->ref, vinc->ist_rif, vinc->len); // inizializzo l'istogramma
    /////////////////////
    if (scanf(" %d", &num_iter));
    while (num_iter >= 0) // continua ad iterare finchè l'utente ha dei tentativi disponibili
    {
        if (num_iter == 0)
        { // nel caso non avessimo più tentativi disponibili
            printf("ko\n");
            resetta_filtro(filtrati);
            return;
        }
        if (scanf(" %s", buffer));
        if (strcmp(buffer, vinc->ref) == 0)
        {
            resetta_filtro(filtrati);
            printf("ok\n");
            return;
        }
        else if (strcmp(buffer, "+inserisci_inizio") == 0)
        {
            if (filtrati == NULL)
                inserisci_inizio(T, k);
            else
            {
                inserisci_inizio_mod(&filtrati, T, vinc);
            }
        }
        else if (strcmp(buffer, "+stampa_filtrate") == 0)
        {
            if (filtrati == NULL)
                inorder(T->root); // se non ci sono vincoli stampa tutto il dizionario
            else
                stampa_lista(filtrati);
        }
        else if (strcmp(buffer, "+nuova_partita") == 0) // nel caso chiamasse "+nuova_partita" durante una partita si limita a riiniziarla da capo
        {
            resetta_filtro(filtrati);
            nuova_partita(T, vinc);
            return;
        }
        else if (is_in_RB(T->root, buffer) == true) // se è presente nel dizionario continua
        {
            confronto(buffer, vinc);            // aggiorniamo i vincoli
            if (vinc->car_eff_pres[k - 1] == 0) // per velocizzare rispetta_vincolo
                riduci_occorrenze(vinc);
            if (filtrati == NULL)
                primo_inserimento(&filtrati, T->root, vinc); // creazione lista filtrati
            else
                filtra_lista(&filtrati, vinc); // filtraggio nuovo secondo i vincoli aggiornati
            printf("%d\n", conta_nodi(filtrati)); // stampiamo il numero di parole ammissibili e diminuiamo il numero di tentativi disponibili
            num_iter -= 1;
        }
        else
        {
            printf("not_exists\n"); // se la parola digitata dall'utente non appartiene al dizionario di riferimento
        }
    }
}

int main()
{
    //////////////////////
    int k = 0;
    char buffer[20];
    //////////////////////
    // Allocazione dizionario T (Tree)
    BST *T = (BST *)malloc(sizeof(BST));
    T->root = NULL;
    //////////////////////
    if (scanf(" %d", &k));
    /////////////////////
    // ALLOCAZIONE VINCOLO
    vincolo *vinc = (vincolo *)calloc(sizeof(vincolo), 1);
    vinc->ref = (char *)malloc(sizeof(char) * (k + 1));
    vinc->pos_corr = (char *)calloc(sizeof(char), k + 1);
    vinc->car_non_pres = (bool **)calloc(sizeof(bool *), 64);
    for (u_int8_t i = 0; i < 64; i++)
        vinc->car_non_pres[i] = (bool *)calloc(sizeof(bool), k); // alloco la matrice
    vinc->len = k;
    vinc->car_eff_pres = (char *)calloc(sizeof(char), k);
    ////////////////////
    ///|/, 1
    while (scanf(" %s", buffer) == true)
    {
        if (strcmp(buffer, "+nuova_partita\0") == 0)
        {
            nuova_partita(T, vinc);
        }
        else if (strcmp(buffer, "+inserisci_inizio") == 0)
        {
            inserisci_inizio(T, k);
        }
        else
            insert(T, buffer, k + 1);
    }
    ////////////////////
    // Fase pulizia memoria
    distruggi_RB(T->root);
    free(T);
    ///////////////
    for (u_int8_t i = 0; i < 64; i++)
        free(vinc->car_non_pres[i]);
    free(vinc->car_non_pres);
    free(vinc->car_eff_pres);
    free(vinc->pos_corr);
    free(vinc->ref);
    free(vinc);
    //////////////
    ///////////////
    return 0;
}
