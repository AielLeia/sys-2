#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>

#include <errno.h>

#define I_ASSERT_P(assertion, msg) \
    if (assertion)                 \
    {                              \
        perror(msg);               \
        exit(EXIT_FAILURE);        \
    }

#define I_ASSERT_I(assertion, msg) \
    if (assertion)                 \
    {                              \
        fprintf(stderr, msg);      \
        exit(EXIT_FAILURE);        \
    }

int main(int argc, char *argv[])
{
    I_ASSERT_I(argc < 2, "Usage: ./prog file_name");
    int fd;
    void *m_mapped;
    struct stat f_stat;

    fd = open(argv[1], O_RDONLY);
    I_ASSERT_P(fd == -1, "Error file descriptor");

    fstat(fd, &f_stat);

    m_mapped = mmap(NULL, f_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    I_ASSERT_P(m_mapped == MAP_FAILED, "Error mmap");

    printf("%s\n", m_mapped);

    munmap(m_mapped, f_stat.st_size);
    close(fd);
    return EXIT_SUCCESS;
}