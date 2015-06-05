#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char	buf[1024];
int main(int argc, char **argv)
{
	FILE	*fp;
	int	i;

	fprintf(stdout, "hello %d\n", 10);
	printf("hello again %d\n", 20);

	fp = fopen("text.txt", "wb");
	i = fprintf(stdout, "test File stream hooking, fileno(stdout) = %d\n", fileno(stdout));
	printf(" writting %d chars\n", i);
	snprintf(buf, 1024, "test File stream hooking, fileno(stdout) = %d\n", fileno(stdout));
	fwrite(buf, strlen(buf) + 1, 1, stdout);

	strcpy(buf, "writing by fwrite\n");
	for (i = 0; i < 3; i++) {
	    fprintf(fp, "test test %d\n", i);
	}
	fwrite(buf, strlen(buf) + 1, 1, fp);
	fclose(fp);
	for (i = 0; i < 3; i++) {
	    fprintf(stdout, "test test %d\n", i);
	}
	for (i = 0; i < 3; i++) {
	    fprintf(stderr, "test test %d\n", i);
	}

	return(0);
}
