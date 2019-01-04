#include <stdio.h>
#include <strings.h>

char	buf[128];
char	*lptr[128];
size_t	sz;
int
main(int argc, char **argv)
{
    char	ch = 1;

#ifdef __USE_EXTERN_INLINES
    printf("__USE_EXTERN_INLINES is defined\n");
#endif

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

    getc_unlocked(stdin);
    getchar_unlocked();
    putc_unlocked('h', stdout);
    putchar_unlocked('h');
    fgetc_unlocked(stdin);
    fputc_unlocked('i', stdout);

    fread_unlocked(buf, 10, 1, stdin);
    fwrite_unlocked(buf, 10,1, stdout);
    fgets_unlocked(buf, 128, stdin);
    fputs_unlocked(buf, stdout);
    return 0;
}
