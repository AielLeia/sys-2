#include "main.h"

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