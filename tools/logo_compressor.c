/*
 *  logo_compressor: Compress EmuTOS logo to save space
 *
 *  Copyright 2021 The EmuTOS team
 *
 *  Author: Vincent Barrilliot
 *
 *  This program is licensed under the GNU General Public License.
 *  Please see LICENSE.TXT for details.
 *
 *  This tool takes the logo, inlined in this file for convenience,
 *  and compresses it into a file logo.c, which can be included in
 *  initinfo.c, which also contains the code to uncompress and
 *  display it (print_art).
 *
 *  The format is the following:
 *  Input (e.g. see example below): number 0-7 for color number.
 *  \n should terminate each line.
 *
 *  Output: repeated characters are represented as a pair of bytes:
 *  the first byte is the neg''ed count of repetitions+1 (e.g. 3
 *  repetitions is 0xfe), and the second is the color to repeat. 
 *  \n means end of line and 0 terminates the file.
 *  Color numbers are offset by 1 (LOGO_COLOR_OFFSET), because we can't use
 *  0 (it's the string terminator).
 *  Eg. colour 1 is ''\002'.
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* Logo is defined here */
#define LOGO_LENGTH 34  /* max length of strings in EmuTOS logo */
static const char l01[] = "11111111111 7777777777  777   7777\n"; 
static const char l02[] = "1                  7   7   7 7\n";
static const char l03[] = "1111   1 1 1    1  7   7   7  777\n";
static const char l04[] = "1     1 1 1 1   1  7   7   7     7\n";
static const char l05[] = "11111 1   1  111   7    777  7777";

char logo[512];   /* That shall be enough */

#define LOGO_COLOR_OFFSET 1
#define LOGO_CRLF '\020'


/* Define this to debug the print_art */
#ifdef DEBUG
/* The following is to emulate the initinfo */
static void crlf(void)
{
    printf("\n");
}

static void set_margin(void)
{
}

static void cprintf(const char *s, char c)
{
    (void)s;
    printf("%c",c);
}

#define block "\001"
/* This is the same thing as in initinfo except we add '0' so we can see the color number */
static void print_art(void)
{
    char c;
    char *r = (char*)logo;

    set_margin();
    while ((c = *r++))
    {
        if (c < 0)
        {   
            while (c++)
                cprintf(block, *r - LOGO_COLOR_OFFSET + '0');
        }
        else 
        {
            if (c == LOGO_CRLF)
            {
                cprintf(block, 0); /* Restore bg colour */
                crlf();
                set_margin();
            }
            else
                cprintf(block, c - LOGO_COLOR_OFFSET + '0');
        }
    }
    cprintf(block, 0);
    crlf();
}
#endif

int main(int argc, char *argv[])
{
    char outbuf[512]; 
    char cur;         /* Character to repeat */
    FILE *f;          /* Output file */
    char *logo_end;   /* Address of end of logo */
    int  len;         /* Length of logo string */
    int  i;
    char *r;          /* Read pointer iterates through characters or the logo */
    char *w = outbuf; /* writes characters in the output buffer */
    int  first;

    if (argc < 3)
    {
        fprintf(stderr, "Usage:\n\tlogo_compressor <output_logo.c> <output_logo.h>");
        return -1;
    }

    strcpy(logo,"");
    strcat(logo,l01);
    strcat(logo,l02);
    strcat(logo,l03);
    strcat(logo,l04);
    strcat(logo,l05);

#ifdef DEBUG
    printf("Logo to process:\n%s\n", logo);
#endif

    /* Replace spaces with 0 (color of the background) */
    len = strlen(logo);
    for (i = 0; i < len; i++)
    {
        if (logo[i] == ' ')
            logo[i] = '0';

        if (logo[i] >= '0' && logo[i] <= '9')
            logo[i] -= '0' - LOGO_COLOR_OFFSET;

        if (logo[i] == '\n')
            logo[i] = LOGO_CRLF;
    }

    r = (char*)logo;
    *w = 0;
    cur = *r;
    logo_end = &logo[len];
    while (r < logo_end)
    {        
        while (r[1] && r[1] == cur)
        {
            (*w)--;
            r++;
        }

        if (*w < 0)
        {
            *++w = cur;
            w++; /* Skip repeat counter */
        }
        else
        {
            *w++ = cur;
        }
        *w = 0;
        cur = *++r;
    }
    *w = 0;


    f = fopen (argv[1], "w");
    fprintf(f, "const char logo[] = {");
    r = outbuf;
    first = 1;
    while (*r)
    {
        fprintf(f, first ? "0x%02x" : ", 0x%02x", (unsigned char)(*r++));
        first = 0;
    }
    fprintf(f, "};\n");
    fclose(f);

    f = fopen (argv[2], "w");
    fprintf(f, "#define LOGO_LENGTH %d\n", LOGO_LENGTH);
    fprintf(f, "#define LOGO_COLOR_OFFSET %d\n", LOGO_COLOR_OFFSET);
    fprintf(f, "#define LOGO_CRLF '\\020'\n");
    fprintf(f, "extern const char logo[];\n");
    fclose(f);

    printf("%s and %s are generated.\n", argv[1], argv[2]);

#ifdef DEBUG
    printf("Result:\n");
    print_art();
#endif

    return 0;
}