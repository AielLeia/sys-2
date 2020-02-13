#include "main.h"

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
    void *m_mapped = mmap(NULL, f_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
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

    for (long int i = 0; i < d->nb_iteration; i++)
    {
        // Matrice 1
        for (long int j = 0; j < d->m_1[i].line; j++)
        {
            for (long int k = 0; k < d->m_1[i].column; k++)
                printf("%d ", d->m_1[i].mat[j][k]);
            printf("\n");
        }

        printf("\n\n");

        // Matrice 2
        for (long int j = 0; j < d->m_2[i].line; j++)
        {
            for (long int k = 0; k < d->m_2[i].column; k++)
                printf("%d ", d->m_2[i].mat[j][k]);
            printf("\n");
        }
    }

    munmap(m_mapped, f_stat.st_size);
    close(fd);
    return 0;
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

    return 0;
}

int main(int argc, char *argv[])
{
    I_ASSERT_I(argc < 2, "Usage: ./prog filename");

    const char *filename = argv[1];
    int build = build_matrices(&d, filename);

    I_ASSERT_P(build == -1, "build_matrices");
    I_ASSERT_I(build == -2, "Erreur: impossible d'effectuer les allocations\n");

    destroy_matrices(&d);

    return EXIT_SUCCESS;
}