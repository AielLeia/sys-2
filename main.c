#include "main.h"

int main(int argc, char *argv[])
{
    I_ASSERT_I(argc < 2, "Usage: ./prog filename");

    const char *filename = argv[1];
    int build = build_matrices(&d, filename);

    I_ASSERT_P(build == -1, "build_matrices");
    I_ASSERT_I(build == -2, "Erreur: impossible d'effectuer les allocations\n");

    d.state = STATE_WAIT;

    I_ASSERT_P(pthread_mutex_init(&d.mutex, NULL) != 0, "pthread_mutex_init(&d.mutex, NULL)");
    I_ASSERT_P(pthread_cond_init(&d.cond, NULL) != 0, "pthread_cond_init(&d.cond, NULL)");
    for (long int i = 0; i < d.nb_iteration; i++)
    {
        long int size = d.m_result[i].column * d.m_result[i].line;
        pthread_t *mat_th = (pthread_t *)malloc(sizeof(pthread_t) * size);
        thread_data **t_data = (thread_data **)malloc(sizeof(thread_data *) * size);
        d.pending = (int *)malloc(sizeof(int) * size);

        for (long int _i = 0; _i < size; _i++)
        {
            t_data[_i] = (thread_data *)malloc(sizeof(thread_data));
            t_data[_i]->index_matrix = i;
            t_data[_i]->index_pending = _i;
        }

        int l = 0;
        for (long int _i = 0; _i < d.m_result[i].line; _i++)
        {
            for (long int _j = 0; _j < d.m_result[i].column; _j++)
            {
                t_data[l]->j = _j;
                t_data[l]->i = _i;
                l++;
            }
        }

        d.state = STATE_WAIT;
        init_pending(&d, i);
        for (long int th = 0; th < size; th++)
        {
            I_ASSERT_P(pthread_create(&mat_th[th], NULL, run, (void *)t_data[th]) != 0, "pthread_create(&mat_th[i], NULL, run, (void *)&t_data[i])");
        }

        //for (long int th = 0; th < size; th++)
        //{
        d.state = STATE_START;
        pthread_cond_broadcast(&d.cond);

        I_ASSERT_P(pthread_mutex_lock(&d.mutex) != 0, "pthread_mutex_lock(&d.mutex)");
        while (d.state != STATE_PRINT)
            I_ASSERT_P(pthread_cond_wait(&d.cond, &d.mutex) != 0, "pthread_cond_wait(&d.cond, &d.mutex)");
        I_ASSERT_P(pthread_mutex_unlock(&d.mutex) != 0, "pthread_mutex_unlock(&d.mutex)");

        fprintf(stderr, "IETRATION %ld : th\n", i);
        //}

        for (long int th = 0; th < size; th++)
            I_ASSERT_P(pthread_join(mat_th[th], NULL) != 0, "pthread_join(&mat_th[th], NULL)");
        for (long int _i = 0; _i < size; _i++)
        {
            free(t_data[_i]);
        }
        free(t_data);
        free(mat_th);
    }

    write_i(&d);
    I_ASSERT_P(pthread_cond_destroy(&d.cond) != 0, "pthread_cond_destroy(&d.cond)");
    I_ASSERT_P(pthread_mutex_destroy(&d.mutex) != 0, "pthread_mutex_destroy(&d.mutex)");
    destroy_matrices(&d);

    return EXIT_SUCCESS;
}