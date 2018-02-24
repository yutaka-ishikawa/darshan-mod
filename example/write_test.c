#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char	buf[1024];

void
try(int iter)
{
    int		i;
    FILE	*fp;

    fp = fopen("file.txt", "w+");
    for (i = 0; i < iter; i++) {
	fprintf(fp, "%s %s %s %d", "We", "are", "in", 2015);
	snprintf(buf, 1023, "%s %s %s %d", "We", "are", "in", 2015);
	fwrite(buf, strlen(buf)+1, 1, fp);
    }
    fclose(fp);
}

int main(int argc, char **argv)
{
	char hostname[1024];

	if (gethostname(hostname, sizeof(hostname)) < 0) {
		fprintf(stderr, "error: getting hostname\n");
		exit(1);
	}
	printf("I am %s and I am opening the file now\n", hostname);
	memset(buf, 0, 1024);
	try(2);
	try(1000);
	try(100);
	return(0);
}
