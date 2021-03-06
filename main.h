/** 
 * --------------------------------------------------------------------------------------
 * Dans le fichier vous trouverez essentiellement 4 catègorie de code avec leur
 * documentation:
 * 
 * -> Inclusion des différents headers nécessaires au bon fonctionnement du programme
 *    avec les raisons d'inclusion de celles çi.
 * 
 * -> Définition de deux marcos afin d'allèger le code côté implementation et avoir
 *    une bonne lisibilité.
 * 
 * -> Définition des strutures nécessaires au bon fonctionnement du programme
 *    et du pourquoi ces strutures existe dans le codé et leur utilité dans le programme
 *    avec une documentation.
 * 
 * -> Définition des prototypes des fonctions permettant de resoudre une problèmatique
 *    parmit les problèmatiques de l'éxercice.
 * 
 * Le code source est disponible sur github.
 * --------------------------------------------------------------------------------------
 * @author Ismaël MOHAMED BOUH
 * @github https://github.com/AielLeia/sys-2
 * --------------------------------------------------------------------------------------
*/
#ifndef __MAIN__H__
#define __MAIN__H__

/** 
 * --------------------------------------------------------------------------------------
 * Inclusion des bibliothèques standards.
 * --------------------------------------------------------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>

/** 
 * --------------------------------------------------------------------------------------
 * Inclusion de la bibliothèque permettant la gestion des chaines de caractères.
 * --------------------------------------------------------------------------------------
*/
#include <string.h>

/** 
 * --------------------------------------------------------------------------------------
 * Inclusion de la bibliothèque de gestion des erreurs.
 * --------------------------------------------------------------------------------------
*/
#include <errno.h>

/** 
 * --------------------------------------------------------------------------------------
 * Inclusion de la bibliothèque de gestion des threads.
 * --------------------------------------------------------------------------------------
*/
#include <pthread.h>

/** 
 * --------------------------------------------------------------------------------------
 * Inclusion de labibliothèque partagée des fonctions fstat et open.
 * --------------------------------------------------------------------------------------
*/
#include <sys/types.h>

/** 
 * --------------------------------------------------------------------------------------
 * Inclusion de la bibliothèque permettant la manipulation de la fonction fstat pour 
 * obtenir certaines propriétés d'un fichier.
 * --------------------------------------------------------------------------------------
*/
#include <sys/stat.h>

/** 
 * --------------------------------------------------------------------------------------
 * Inclusion des bibliothèques permettant la manipulation des fonction open, close.
 * --------------------------------------------------------------------------------------
*/
#include <fcntl.h>
#include <unistd.h>

/** 
 * --------------------------------------------------------------------------------------
 * Inclusion de la bibliothèque permattant la manipulation de la fonction mmap.
 * --------------------------------------------------------------------------------------
*/
#include <sys/mman.h>

/** 
 * --------------------------------------------------------------------------------------
 * Inclusion de la bibliothèque permettant la gestion du temps.
 * --------------------------------------------------------------------------------------
*/
#include <sys/time.h>

/** 
 * --------------------------------------------------------------------------------------
 * Vérifie les conditions associées à des erreurs qui serons spéficier par la 
 * constante errno, par exemple open revoie -1 en cas d'erreur le code d'erreur
 * est spéficier dans la constante errno et peut être afficher par la fonction
 * perror.
 * --------------------------------------------------------------------------------------
 * @param boolean Condition à vérifier.
 * @param char    Message en cas d'erreue.
 * --------------------------------------------------------------------------------------
*/
#define I_ASSERT_P(assertion, msg) \
    if (assertion)                 \
    {                              \
        perror(msg);               \
        exit(EXIT_FAILURE);        \
    }

/** 
 * --------------------------------------------------------------------------------------
 * Vérifie les conditions associées à des erreurs plus spécifique, par exemple
 * en cas de non respect du nombre d'argument passer à la fonction main.
 * --------------------------------------------------------------------------------------
 * @param boolean Condition à vérifier.
 * @param char    Message en cas d'erreur.
 * --------------------------------------------------------------------------------------
*/
#define I_ASSERT_I(assertion, msg) \
    if (assertion)                 \
    {                              \
        fprintf(stderr, msg);      \
        exit(EXIT_FAILURE);        \
    }

/** 
 * --------------------------------------------------------------------------------------
 * Enumère les differents étapes de calcule d'une matrice.
 * --------------------------------------------------------------------------------------
 * STATE_WAIT  -> Attende d'initialisation des données
 * STATE_START -> Les threads de calcule démarre.
 * STATE_PRINT -> Affichage des données calculer par les threads.
 * --------------------------------------------------------------------------------------
*/
typedef enum _state
{
    STATE_WAIT,
    STATE_START,
    STATE_PRINT
} state;

/** 
 * --------------------------------------------------------------------------------------
 * Répresente une matrice.
 * --------------------------------------------------------------------------------------
 * @property long int line      Nombre de ligne de la matrice.
 * @property long int column    Nombre de colonne de la matrice.
 * @property long int **mat     Contenue de la matrice.
 * --------------------------------------------------------------------------------------
*/
typedef struct _matrix
{
    long int line;
    long int column;
    long int **mat;
} matrix;

/** 
 * -------------------------------------------------------------------------------------------
 * Données partagées par les differents threads du programme.
 * -------------------------------------------------------------------------------------------
 * @property state              state        État du programme.
 * @property int                *pending     Thread en attende.
 * @property pthread_cond_t     cond         Synchronisation des threads.
 * @property pthread_mutex_t    mutex        Utilisation d'un thread à la fois.
 * @property long int           nb_iteration Nombre d'itération à effectuer.
 * @property matrix             *m_1         Tableau regroupant toute les matrices 1.
 * @property matrix             *m_2         Yableau regroupant toute les matrices 2.
 * @property matrix             *m_result    Tableau regroupant toute les matrices resultats.
 * ------------------------------------------------------------------------------------------
*/
typedef struct _data
{
    state state;
    int *pending;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    long int nb_iteration;
    matrix *m_1;
    matrix *m_2;
    matrix *m_result;
} data;

/** 
 * ------------------------------------------------------------------------------------------
 * Données envoyée au thread afin d'effectuer le traitement.
 * ------------------------------------------------------------------------------------------
 * @property int    index_matrix    L'index dans le tableaux de matrice.
 * @property int    index_pending   L'index dans le tableaux des attendes.
 * @property int    i               Données sur la ligne.
 * @property int    j               Données sur la colonne.
 * ------------------------------------------------------------------------------------------
*/
typedef struct _thread_data
{
    long int index_matrix;
    long int index_pending;
    long int i;
    long int j;
} thread_data;

/** 
 * --------------------------------------------------------------------------------------
 * Variable partagées par les differents threads.
 * --------------------------------------------------------------------------------------
*/
data d;

/** 
 * --------------------------------------------------------------------------------------
 * Construit les matrices à partir d'un fichier de construction respectant
 * un certain format.
 * Cette fonction charge en une fois le contenue du fichier et les stockes
 * dans la structure partagée par les différents threads.
 * --------------------------------------------------------------------------------------
 * @param data *d           Données à construire.
 * @param char *builder     Fichier a partir duquel les matrices sont construite.
 * --------------------------------------------------------------------------------------
 * @return int -1 En cas d'erreur de type errno, -2 en cas d'erreur classique, 0 sinon.
 * --------------------------------------------------------------------------------------
*/
int build_matrices(data *d, const char *builder);

/** 
 * --------------------------------------------------------------------------------------
 * Detruit les matrices.
 * --------------------------------------------------------------------------------------
 * @param data *d   Données à detruire.
 * --------------------------------------------------------------------------------------
 * @return int 0 en cas de succè, -1 sinon.
 * --------------------------------------------------------------------------------------
*/
int destroy_matrices(data *d);

/** 
 * --------------------------------------------------------------------------------------
 * Initialisation permettant de savoir si un thread est autorisé a effectuée un
 * un calcule sur les matrices.
 * Le paramètre data est la struture dans lequel s'y trouve le tableaux permettant de 
 * determiné si un thread est autorisée ou non a effectuée un calcule.
 * L'index répresente la position dans le tableau des matrice resultat afin de
 * derteminé la taille du tableau initiale.
 * --------------------------------------------------------------------------------------
 * @param data  *d      Données a initialiser.
 * @param int   index   Emplacement des données de la matrice resultat.
 * --------------------------------------------------------------------------------------
 * @return void
 * -------------------------------------------------------------------------------------- 
*/
void init_pending(data *d, int index);

/** 
 * --------------------------------------------------------------------------------------
 * Récupère le nombre de thread en attende.
 * -------------------------------------------------------------------------------------- 
 * @param data *d Données par laquel récupèrer les threads en attende.
 * -------------------------------------------------------------------------------------- 
 * @return int Nombre de thread en attende.
 * --------------------------------------------------------------------------------------
*/
int get_pending(data *d, int index);

/** 
 * --------------------------------------------------------------------------------------
 * Fonction exécuter par les threads.
 * --------------------------------------------------------------------------------------
 * @param void  *data Données envoyer au thread. (Içi la struture thread_data)
 * --------------------------------------------------------------------------------------
 * @return void* Renvoie aucun resultat. 
 * --------------------------------------------------------------------------------------
*/
void *run(void *data);

/** 
 * --------------------------------------------------------------------------------------
 * Écrit dans le fichier les calcules éffectués par les threads.
 * --------------------------------------------------------------------------------------
 * @param data *d Données à écrire dans le fichier.
 * --------------------------------------------------------------------------------------
*/
void write_i(data *d);

#endif