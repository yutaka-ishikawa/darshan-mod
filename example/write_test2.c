#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char	buf[1024*10240];

void
try(int count)
{
    FILE	*fp;

    fp = fopen("file.txt", "w+");
    fwrite(buf, 1024, count, fp);
    fclose(fp);
}

int main(int argc, char **argv)
{
    try(10);
    try(100);
    try(1);
    return(0);
}
