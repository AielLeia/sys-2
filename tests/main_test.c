#include "../main.h"

#define SIZE_OF(type) ((int)(&type + 1) - (int)(&type))

int test_build_matrices_function()
{
    return d.m_1[0].line != 0 && d.m_2[0].line != 0 && d.m_result[0].line != 0;
}

int test_pending_function()
{
    size_t size = d.m_result[0].column * d.m_result[0].line;
    for (size_t i = 0; i < size; i++)
        if (d.pending[i] == 0)
            return 0;
    return 1;
}

int test_destroy_matrices_function()
{
    return d.m_1 == NULL && d.m_2 == NULL && d.m_result == NULL;
}

int test_get_pending_function()
{
    return get_pending(&d, 0) == 6;
}

int main(int argc, char *argv[])
{
    I_ASSERT_I(argc < 2, "Usage: ./prog filetest");
    int result = build_matrices(&d, argv[1]);

    printf("Test build:   %9s", "");
    I_ASSERT_P(result == -1, "build_matrice");
    I_ASSERT_I(result == -2, "build_matrice");

    if (test_build_matrices_function() != 1)
        printf("NO\n");
    else
        printf("OK\n");

    d.pending = (int *)malloc(sizeof(int) * (d.m_result[0].column * d.m_result[0].line));
    printf("Test pending: %9s", "");
    init_pending(&d, 0);
    if (test_pending_function() != 1)
        printf("NO\n");
    else
        printf("OK\n");

    printf("Test get pending: %5s", "");
    if (test_get_pending_function() != 1)
        printf("NO\n");
    else
        printf("OK\n");

    int line = 0;
    int column = 0;
    long int result_mat = 0;
    for (long int _i = 0; _i < d.m_1[0].column; _i++)
        result_mat += (d.m_1[0].mat[line][_i] * d.m_2[0].mat[_i][column]);

    printf("%ld\n", result_mat);

    printf("Test destroy: %9s", "");
    destroy_matrices(&d);
    if (test_destroy_matrices_function() != 1)
        printf("NO\n");
    else
        printf("OK\n");

    free(d.pending);
    return EXIT_SUCCESS;
}