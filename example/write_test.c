#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char	buf[1024];
int main(int argc, char **argv)
{
	FILE *fp;
	char hostname[1024];

	if (gethostname(hostname, sizeof(hostname)) < 0) {
		fprintf(stderr, "error: getting hostname\n");
		exit(1);
	}

	printf("I am %s and I am opening the file now\n", hostname);

	fp = fopen("file.txt", "w+");
	fprintf(fp, "%s %s %s %d", "We", "are", "in", 2015);

	memset(buf, 0, 1024);
	snprintf(buf, 1023, "%s %s %s %d", "We", "are", "in", 2015);
	fwrite(buf, strlen(buf)+1, 1, fp);

	fclose(fp);

	return(0);
}
