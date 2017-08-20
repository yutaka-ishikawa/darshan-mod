#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COUNT	1024

char	buf[1024];
int main(int argc, char **argv)
{
    FILE	*fp;
    long	cnt = COUNT;
    long	i;

    if (argc == 2) {
	cnt = atol(argv[1]);
    }
    fprintf(stdout, "%f M times writting to /dev/null\n",
	    (float)cnt/(1000.0*1000.0));

    fp = fopen("./null", "w");
    for (i = 0; i < cnt; i++) {
	fprintf(fp, "test test %ld\n", i);
    }
    fclose(fp);
    return 0;
}
