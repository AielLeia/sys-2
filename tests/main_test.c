#include "../main.h"

int test_build_matrices_function()
{
    return d.m_1[0].line != 0 && d.m_2[0].line != 0 && d.m_result[0].line != 0;
}

int test_pending_function()
{
    size_t size = d.m_result[0].column * d.m_result[0].line;
    for (long int i = 0; i < size; i++)
        if (d.pending[i] == 0)
            return 0;
    return 1;
}

int test_destroy_matrices_function()
{
    return d.m_1 == NULL && d.m_2 == NULL && d.m_result == NULL;
}

int main(int argc, char *argv[])
{
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

    printf("Test destroy: %9s", "");
    destroy_matrices(&d);
    if (test_destroy_matrices_function() != 1)
        printf("NO\n");
    else
        printf("OK\n");

    free(d.pending);
    return EXIT_SUCCESS;
}