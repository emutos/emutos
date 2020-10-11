/*
 * lod2c: convert simple .LOD files to C array initialization string
 *
 * Copyright (C) 2020 The EmuTOS Development Team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * important: this code assumes that there will only be one _DATA section
 *
 * compile with:
 *      m68k-atari-mint-gcc -o LOD2C.TTP lod2c.c
 *
 * usage:
 *      lod2c <infile> <outfile>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION         "0.3"
#define DSP_WORD_LEN    3

FILE *infp = NULL, *outfp = NULL;
char *inbuf, *outbuf;


/* convert two hex digits to unsigned byte */
static unsigned char hex(char *p)
{
    int i;
    unsigned char n;

    for (i = 0, n = 0; i < 2; i++, p++)
    {
        n <<= 4;
        if ((*p >= '0') && (*p <= '9'))
            n += *p - '0';
        else if ((*p >= 'A') && (*p <= 'F'))
            n += *p - 'A' + 10;
        else if ((*p >= 'a') && (*p <= 'f'))
            n += *p - 'a' + 10;
    }

    return n;
}

/* skip spaces */
static char *skipspaces(char *p)
{
    while(*p == ' ')
        p++;

    return p;
}

/* skip up to eol character */
static char *skiptoeol(char *p)
{
    while (*p != '\n')
        p++;

    return p;
}

static void convert_data(FILE *fp, char *p)
{
    int i, j;

    fprintf(fp, "   ");

    /* convert rest of file to binary */
    for ( ; *p; p++)
    {
        /* done if _END is found */
        if (strncmp(p, "_END", 4) == 0)
            break;
        if ((*p == '_') || (*p == '\r'))
        {
            p = skiptoeol(p);
            continue;
        }
        for (i = 0; *p != '\n'; i++)
        {
            for (j = 0; j < DSP_WORD_LEN; j++, p += 2)
                fprintf(fp, " 0x%02x,", hex(p));
            if (i & 1)
                fprintf(fp, "\n   ");
            p = skipspaces(p);
            if (*p == '\r')
                p++;
        }
    }
    if (j == 1)
        fprintf(fp,"\n");
}

static void quit(void)
{
    if (infp)
        fclose(infp);
    if (outfp)
        fclose(outfp);

    printf("press Return to exit\n");
    getchar();
    exit(1);
}

static void usage(void)
{
    printf("usage: lod2c <infile> <outfile>\n");
    quit();
}

static void write_intro(char *filename)
{
    fprintf(outfp, "/*\n");
    fprintf(outfp, " * DSP binary file generated from %s\n", filename);
    fprintf(outfp, " *\n");
    fprintf(outfp, " * Copyright (c) 2020 by The EmuTOS Development Team\n");
    fprintf(outfp, " *\n");
    fprintf(outfp, " * This file is distributed under the GPL, version 2 or at your\n");
    fprintf(outfp, " * option any later version.  See doc/license.txt for details.\n");
    fprintf(outfp, " */\n");
}

static long filelength(FILE *fp)
{
    long filepos;

    if (fseek(fp, 0L, SEEK_END) < 0)
        return -1L;

    filepos = ftell(fp);
    if (filepos < 0L)
        return -1L;

    if (fseek(fp, 0L, SEEK_SET) < 0)
        return -1L;

    return filepos;
}

int main(int argc, char **argv)
{
    int found;
    long filesize;
    char *p;

    if (argc != 3)
        usage();

    infp = fopen(argv[1], "rb");
    if (!infp)
    {
        fprintf(stderr, "can't open input file %s\n", argv[1]);
        quit();
    }

    filesize = filelength(infp);
    if (filesize < 0L)
    {
        fprintf(stderr, "can't find size of input file %s\n", argv[1]);
        quit();
    }

    inbuf = malloc(filesize+1);
    if (!inbuf)
    {
        fprintf(stderr, "can't malloc input buffer\n");
        quit();
    }

    if (fread(inbuf, filesize, 1, infp) != 1)
    {
        fprintf(stderr, "can't read file %s\n", argv[1]);
        quit();
    }
    inbuf[filesize] = 0x00;

    fclose(infp);
    infp = NULL;

    outfp = fopen(argv[2], "wb");
    if (!outfp)
    {
        fprintf(stderr, "can't create file %s\n", argv[2]);
        quit();
    }

    /* look for _DATA and decode it */
    for (p = inbuf, found = 0; *p; p++)
    {
        /* done if _END is found */
        if (strncmp(p, "_END", 4) == 0)
            break;
        if (strncmp(p, "_DATA ", 6) == 0)
        {
            p = skiptoeol(p);
            write_intro(argv[1]);
            convert_data(outfp, p);
            found++;
            break;
        }
        p = skiptoeol(p);
    }

    if (!found)
    {
        fprintf(stderr, "can't find _DATA section in %s\n", argv[1]);
        quit();
    }

    fclose(outfp);

    return 0;
}
