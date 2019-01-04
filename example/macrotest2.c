#include <stdio.h>
#include <strings.h>

char	buf[128];
char	*lptr[128];
size_t	sz;
int
main(int argc, char **argv)
{
    char	ch = 1;
    ch = getc_unlocked(stdin);
    printf("ch = %c\n", ch);
    ch = getchar_unlocked();
    printf("ch = %c\n", ch);
    return 0;
}
