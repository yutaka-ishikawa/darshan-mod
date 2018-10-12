#include <stdio.h>

char	buf[128];
char	*lptr[128];
size_t	sz;
int
main(int argc, char **argv)
{
    char	ch = 1;
    fputc(ch, stdout);
    fputs("hello", stdout);	// call fwrite@PLT
    fputs(argv[0], stdout);	// call fputs
    putc(ch, stdout);		// call	_IO_putc@PLT
    putchar(ch);
    puts("hello");
    fgetc(stdin);
    getc(stdin);		// call _IO_getc@PLT
    ch = getchar();
    ungetc(ch, stdin);
    gets(buf);
    getline(lptr, &sz, stdin);
    getdelim(lptr, &sz, '.', stdin);
    return 0;
}
