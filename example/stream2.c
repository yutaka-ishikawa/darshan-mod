#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char	buf[1024];
int main(int argc, char **argv)
{
	FILE	*fp;
	int	i;

	fp = fopen("text.txt", "wb");
	if (fp == NULL) {
	    fprintf(stderr, "Cannot open text.txt\n"); exit(-1);
	}
	snprintf(buf, 1024, "test stream hooking, fileno(stdout) = %d\n", fileno(stdout));
	i = fwrite(buf, strlen(buf), 1, fp);
	i *= strlen(buf);
	printf("%d characters are written by fwrite\n", i);
	fclose(fp);

	fp = fopen("text.txt", "r");
	if (fp == NULL) {
	    fprintf(stderr, "Cannot open text.txt\n"); exit(-1);
	}
	while ((i = fgetc(fp)) != EOF) {
	    printf("%c", i);
	}
	fclose(fp);
	fp = fopen("text.txt", "r");
	while ((i = getc(fp)) != EOF) {
	    printf("%c", i);
	}
	fclose(fp);
	return(0);
}
