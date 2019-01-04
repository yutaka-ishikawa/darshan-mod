#define __USE_GNU
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MSG	"Hello.World 20bytes\n"	// 20 bytes
char	buf[128];
char	*lptr;
size_t	sz;

int
output(FILE *fp, char *msg)
{
    char	ch = 'a';

    /*
     * 10 I/O operations here.  putc_unlock and fputc_unlock cannot be
     * captured with -O or higher optimization levels.  That is, 8 I/O
     * operations can be observed in this case.  fwrite_unlocked might
     * also cannot be captured.
     *	See /usr/include/x86_64-linux-gnu/bits/stdi.h
     */
    fputc(ch, fp); fputc('\n', fp);	// 2 bytes
    fputs(MSG, fp);			// 22 bytes, call fwrite@PLT
    fputs(msg, fp);			// 42 bytes, call fputs
    ch = 'b';
    putc(ch, fp); putc('\n', fp);	// 44 bytes, call	_IO_putc@PLT
    putc_unlocked('c', fp);		// 45 bytes, _IO_putc_unlocked
    fputc_unlocked('\n', fp);		// 46 bytes, _IO_putc_unlocked
    fwrite_unlocked(msg, strlen(msg), 1, fp); // 66 bytes
    fputs_unlocked(msg, fp);	// 86 bytes
    return 86;
}

int
input(FILE *fp)
{
    int		tot = 2;
    ssize_t	ret;

    /*
     * 9 I/O operations here.  getc_unlcoked and fread_unlocked cannot
     * be captured with -O or higher optimization levels.  That is, 7
     * I/O operations are observed in this case.
     *   See /usr/include/x86_64-linux-gnu/bits/stdi.h
     */
    fgetc(fp);				// 1 bytes
    getc(fp);				// 2 bytes, call _IO_getc@PLT
    ret = getline(&lptr, &sz, fp);		// 22 bytes
    tot += ret;
    ret = getdelim(&lptr, &sz, '\n', fp);	// 42 bytes
    tot += ret;
    getc_unlocked(fp);			// 43 bytes _IO_getc_unlocked
    tot++;
    fread_unlocked(buf, 3, 1, fp);	// 46 Cannot capture with optimization
    tot +=3;
    fgets_unlocked(buf, 128, fp);	// 66 bytes
    tot += strlen(buf);
    fgets_unlocked(buf, 128, fp);	// 86 bytes
    tot += strlen(buf);
    ungetc('\n', fp);
    return tot;
}

int
main(int argc, char **argv)
{
    FILE	*fp;
    int		osz, isz;

    sz = 128;
    lptr = malloc(sz);
    fp = fopen("./tdata", "w");
    if (fp == NULL) {
	fprintf(stderr, "Cannot open/create the ./tdata file\n");
	exit(-1);
    }
    osz = output(fp, MSG);
    fclose(fp);
    fp = fopen("./tdata", "r");
    if (fp == NULL) {
	fprintf(stderr, "Cannot open/create the ./tdata file\n");
	exit(-1);
    }
    isz = input(fp);
    /* stdout only */
    if (osz == isz) {
	putchar_unlocked('S'); putchar('u');
	puts("ccess.  (20bytes)"); // 20 bytes in total
    } else {
	printf("Error: write %d bytes but read %d bytes\n", osz, isz);
    }
#if 0
    int		ch;
    /* stdin only */
    ch = getchar();			//
    gets(buf);
    getchar_unlocked();			// _IO_getc_unlocked
    fgetc_unlocked(stdin);		// _IO_getc_unlocked
#endif
    return 0;
}
