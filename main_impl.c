#include "main.h"

void *run(void *data)
{
    /** 
     * ------------------------------------------------------------------------------
     * Récuperation de la données
     * ------------------------------------------------------------------------------
    */
    thread_data *t_data = (thread_data *)data;
    fprintf(stderr, "Begin, mat(%d)\n", t_data->index_pending);

    /** 
     * ------------------------------------------------------------------------------
     * Boucle de parcours de toute les matrices lue dans le fichier.
     * ------------------------------------------------------------------------------
    */
    // Vérification de la condition de demarrage
    I_ASSERT_P(pthread_mutex_lock(&d.mutex) != 0, "pthread_mutex_lock(&d.mutex)");
    while (d.pending[t_data->index_pending] == 0 || d.state != STATE_START)
        I_ASSERT_P(pthread_cond_wait(&d.cond, &d.mutex) != 0, "pthread_cond_wait(&d.cond, &d.mutex)");
    I_ASSERT_P(pthread_mutex_unlock(&d.mutex) != 0, "pthread_mutex_unlock(&d.mutex)");

    // Demarrage des calcules.
    fprintf(stderr, "-> mat(%d)\n", t_data->index_pending);
    int line = t_data->i;
    int column = t_data->j;
    long int result = 0;
    for (long int _i = 0; _i < d.m_1[t_data->index_matrix].column; _i++)
        result += d.m_1[t_data->index_matrix].mat[line][_i] * d.m_2[t_data->index_matrix].mat[_i][column];
    d.m_result[t_data->index_matrix].mat[line][column] = result;
    d.pending[t_data->index_pending] = 0;
    fprintf(stderr, "<- mat(%d): %ld\n", t_data->index_pending, result);

    if (get_pending(&d, t_data->index_matrix) == 0)
        d.state = STATE_PRINT;

    I_ASSERT_P(pthread_mutex_lock(&d.mutex) != 0, "pthread_mutex_lock(&d.mutex)");
    I_ASSERT_P(pthread_cond_broadcast(&d.cond) != 0, "pthread_cond_broadcast(&d.cond)");
    I_ASSERT_P(pthread_mutex_unlock(&d.mutex) != 0, "pthread_mutex_unlock(&d.mutex)");

    return NULL;
}

int get_pending(data *d, int index)
{
    int count = 0;
    /** 
     * ------------------------------------------------------------------------------
     * Récupération de la taille de la mztice en cours. 
     * ------------------------------------------------------------------------------
    */
    long int size = d->m_result[index].column * d->m_result[index].line;

    /** 
     * ------------------------------------------------------------------------------
     * Somme de valeur, cette valeur réprésente le nombre de thread restant.
     * ------------------------------------------------------------------------------
    */
    for (long int i = 0; i < size; i++)
        count += d->pending[i];
    return count;
}

void init_pending(data *d, int index)
{
    /** 
     * ------------------------------------------------------------------------------
     * Récupération de la taille de la prochaine matrice. 
     * ------------------------------------------------------------------------------
    */
    long int size = d->m_result[index].column * d->m_result[index].line;

    /** 
     * ------------------------------------------------------------------------------
     * Initialisation du tableau à 1.
     * ------------------------------------------------------------------------------
    */
    for (long int i = 0; i < size; i++)
        d->pending[i] = 1;
}

int build_matrices(data *d, const char *builder)
{
    /** 
     * ------------------------------------------------------------------------------
     * Ouverture du fichier.
     * ------------------------------------------------------------------------------
    */
    int fd = open(builder, O_RDONLY);
    if (fd == -1)
        return -1;

    /** 
     * ------------------------------------------------------------------------------
     * Récupération des données rélative au fichier ouvert.
     * ------------------------------------------------------------------------------
    */
    struct stat f_stat;
    if (fstat(fd, &f_stat) == -1)
        return -1;

    /** 
     * ------------------------------------------------------------------------------
     * Mapping des données du fichier lue.
     * ------------------------------------------------------------------------------
    */
    void *m_mapped = mmap(NULL, f_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (m_mapped == MAP_FAILED)
        return -1;

    char *next;

    /** 
     * ------------------------------------------------------------------------------
     * Récuperation du nombre d'itération à effectuer
     * ------------------------------------------------------------------------------
    */
    d->nb_iteration = strtol(m_mapped, &next, 10);

    /** 
     * ------------------------------------------------------------------------------
     * Construiction des tableaux de matrices.
     * ------------------------------------------------------------------------------
    */
    d->m_1 = (matrix *)malloc(sizeof(matrix) * d->nb_iteration);
    if (d->m_1 == NULL)
        return -2;

    d->m_2 = (matrix *)malloc(sizeof(matrix) * d->nb_iteration);
    if (d->m_2 == NULL)
    {
        free(d->m_1);
        return -2;
    }

    d->m_result = (matrix *)malloc(sizeof(matrix) * d->nb_iteration);
    if (d->m_result == NULL)
    {
        free(d->m_1);
        free(d->m_2);
        return -2;
    }

    /** 
     * ------------------------------------------------------------------------------
     * Remplissage des champs de la matrice.
     * ------------------------------------------------------------------------------
    */
    for (long int i = 0; i < d->nb_iteration; i++)
    {
        // Matrice 1
        d->m_1[i].line = strtol(next, &next, 10);   // Nombre de ligne.
        d->m_1[i].column = strtol(next, &next, 10); // Nombre de colonne

        // Matrice 2
        d->m_2[i].line = strtol(next, &next, 10);   // Nombre de ligne
        d->m_2[i].column = strtol(next, &next, 10); // Nombre de colonne

        // Vérification des données récupere
        if (d->m_1[i].column != d->m_2[i].line)
        {
            free(d->m_1);
            free(d->m_2);
            free(d->m_result);
            return -2;
        }

        // Matrice resultat
        d->m_result[i].line = d->m_1[i].line;     // Nombre de ligne
        d->m_result[i].column = d->m_2[i].column; // Nombre de colonne

        // Initialisation des données de la matrice

        // Matrice 1
        d->m_1[i].mat = (long int **)malloc(sizeof(long int *) * d->m_1[i].line);
        for (long int j = 0; j < d->m_1[i].line; j++)
        {
            d->m_1[i].mat[j] = (long int *)malloc(sizeof(long int) * d->m_1[i].column);
            for (long int k = 0; k < d->m_1[i].column; k++)
                d->m_1[i].mat[j][k] = strtol(next, &next, 10);
        }

        // Matrice 2
        d->m_2[i].mat = (long int **)malloc(sizeof(long int *) * d->m_2[i].line);
        for (long int j = 0; j < d->m_2[i].line; j++)
        {
            d->m_2[i].mat[j] = (long int *)malloc(sizeof(long int) * d->m_2[i].column);
            for (long int k = 0; k < d->m_2[i].column; k++)
                d->m_2[i].mat[j][k] = strtol(next, &next, 10);
        }

        // Matrice resultat
        d->m_result[i].mat = (long int **)malloc(sizeof(long int *) * d->m_result[i].line);
        for (long int j = 0; j < d->m_result[i].line; j++)
        {
            d->m_result[i].mat[j] = (long int *)malloc(sizeof(long int) * d->m_result[i].column);
        }
    }

    munmap(m_mapped, f_stat.st_size);
    close(fd);
    return 0;
}

void write_i(data *d)
{
    int fd = open("result.txt", O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 00755);
    I_ASSERT_P(fd == -1, "open(\"result.txt\", O_WRONLY | O_APPEND | O_CREAT, 00755)");

    for (long int _i = 0; _i < d->nb_iteration; _i++)
    {
        char file_text[256];
        sprintf(file_text, "Resultat %ld\n", _i);
        I_ASSERT_P(write(fd, file_text, strlen(file_text)) != (ssize_t)strlen(file_text), "write(fd, file_text, strlen(file_text))");
        for (long int _j = 0; _j < d->m_result[_i].line; _j++)
        {
            for (long int _k = 0; _k < d->m_result[_i].column; _k++)
            {
                sprintf(file_text, "%ld ", d->m_result[_i].mat[_j][_k]);
                I_ASSERT_P(write(fd, file_text, strlen(file_text)) != (ssize_t)strlen(file_text), "write(fd, file_text, strlen(file_text)) ");
            }
            sprintf(file_text, "\n");
            I_ASSERT_P(write(fd, file_text, strlen(file_text)) != (ssize_t)strlen(file_text), "write(fd, file_text, strlen(file_text)) ");
        }
        sprintf(file_text, "\n\n");
        I_ASSERT_P(write(fd, file_text, strlen(file_text)) != (ssize_t)strlen(file_text), "write(fd, file_text, strlen(file_text)) ");
    }
}

int destroy_matrices(data *d)
{
    for (long int i = 0; i < d->nb_iteration; i++)
    {
        // Matrice 1
        for (long int j = 0; j < d->m_1[i].line; j++)
        {
            free(d->m_1[i].mat[j]);
        }
        free(d->m_1[i].mat);

        // Matrice 2
        for (long int j = 0; j < d->m_2[i].line; j++)
        {
            free(d->m_2[i].mat[j]);
        }
        free(d->m_2[i].mat);

        // Matrice Resultat
        for (long int j = 0; j < d->m_result[i].line; j++)
        {
            free(d->m_result[i].mat[j]);
        }
        free(d->m_result[i].mat);
    }

    free(d->m_result);
    free(d->m_1);
    free(d->m_2);

    d->m_result = NULL;
    d->m_1 = NULL;
    d->m_2 = NULL;

    return 0;
}