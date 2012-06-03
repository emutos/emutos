/*
 * mkrom.c - Create an EmuTOS ROM image
 *
 * Copyright (c) 2012 EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Riviere
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * This tool adds padding bytes to an EmuTOS binary image,
 * and also creates special ROM formats.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define SIZE_ERROR ((size_t)-1)
#define MIN(a, b) ((a)<=(b) ? (a) : (b))
#define BUFFER_SIZE (16*1024)

/* Command-line commands */
typedef enum
{
    CMD_NONE = 0,
    CMD_PAD
} CMD_TYPE;

/* Global variables */
const char* g_argv0; /* Program name */
unsigned char g_buffer[BUFFER_SIZE]; /* Global buffer to minimize stack usage */

/* Get an integer value from an integer string with a k, m, or g suffix */
size_t get_size_value(const char* strsize)
{
    unsigned long val;
    char suffix;
    char tail; /* Will only be affected if the suffix is too long */
    int ret;

    ret = sscanf(strsize, "%lu%c%c", &val, &suffix, &tail);
    if (ret == 1) /* No suffix */
        ; /* val is already a number of bytes */
    else if (ret == 2 && (suffix == 'k' || suffix == 'K'))
        val *= 1024;
    else if (ret == 2 && (suffix == 'm' || suffix == 'M'))
        val *= 1024 * 1024;
    else if (ret == 2 && (suffix == 'g' || suffix == 'G'))
        val *= 1024 * 1024 * 1024;
    else
    {
        fprintf(stderr, "%s: %s: invalid size.\n", g_argv0, strsize);
        return SIZE_ERROR;
    }

    return (size_t)val;
}

/* Get the size of an open file */
size_t get_file_size(FILE* file, const char* filename)
{
    long initial_pos; /* Initial file position */
    int err; /* Seek error */
    long end_pos; /* End file position */

    /* Backup the initial position */
    initial_pos = ftell(file);
    if (initial_pos == -1)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, filename, strerror(errno));
        return SIZE_ERROR;
    }

    /* Seek to end of file */
    err = fseek(file, 0, SEEK_END);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, filename, strerror(errno));
        return SIZE_ERROR;
    }

    /* Get the end file position */
    end_pos = ftell(file);
    if (end_pos == -1)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, filename, strerror(errno));
        return SIZE_ERROR;
    }

    /* Restore the initial file position */
    err = fseek(file, 0, initial_pos);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, filename, strerror(errno));
        return SIZE_ERROR;
    }

    /* The end position is the file size */
    return (size_t)end_pos;
}

/* Write a block of same bytes to a file */
int write_byte_block(FILE* outfile, const char* outfilename, unsigned char value, size_t count)
{
    size_t towrite; /* Number of bytes to write this time */
    size_t written; /* Number of bytes written this time */

    memset(g_buffer, value, MIN(BUFFER_SIZE, count));
    while (count > 0)
    {
        towrite = MIN(BUFFER_SIZE, count);
        written = fwrite(g_buffer, 1, towrite, outfile);
        if (written != towrite)
        {
            fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
            return 0;
        }

        count -= written;
    }

    return 1;
}

/* Copy a stream into another one */
int copy_stream(FILE* infile, const char* infilename,
                FILE* outfile, const char* outfilename,
                size_t count)
{
    size_t toread; /* Number of bytes to read this time */
    size_t towrite; /* Number of bytes to write this time */
    size_t written; /* Number of bytes written this time */

    for(;;)
    {
        toread = MIN(BUFFER_SIZE, count);
        if (toread == 0)
            break;

        towrite = fread(g_buffer, 1, toread, infile);
        if (towrite == 0)
        {
            if (ferror(infile))
            {
                fprintf(stderr, "%s: %s: %s\n", g_argv0, infilename, strerror(errno));
                return 0;
            }
            else
            {
                fprintf(stderr, "%s: %s: premature end of file.\n", g_argv0, infilename);
                return 0;
            }
        }

        written = fwrite(g_buffer, 1, towrite, outfile);
        if (written != towrite)
        {
            fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
            return 0;
        }

        count -= written;
    }

    return 1;
}

/* Copy and pad with zeroes up to target_size */
static int cmd_pad(FILE* infile, const char* infilename,
                   FILE* outfile, const char* outfilename,
                   size_t target_size)
{
    size_t source_size;
    size_t free_size;
    int ret; /* boolean return value: 0 == error, 1 == OK */

    printf("# Padding %s to %ld kB image into %s\n", infilename, ((long)target_size) / 1024, outfilename);

    /* Get the input file size */
    source_size = get_file_size(infile, infilename);
    if (source_size == SIZE_ERROR)
        return 0;

    /* Check if the input file size is not too big */
    if (source_size > target_size)
    {
        fprintf(stderr, "%s: %s is too big: %lu extra bytes\n", g_argv0, infilename, (unsigned long)(source_size - target_size));
        return 0;
    }

    /* Copy the input file */
    ret = copy_stream(infile, infilename, outfile, outfilename, source_size);
    if (!ret)
        return ret;

    /* Pad with zeroes */
    free_size = target_size - source_size;
    ret = write_byte_block(outfile, outfilename, 0, free_size);
    if (!ret)
        return ret;

    printf("# %s done (%lu bytes free)\n", outfilename, (unsigned long)free_size);

    return 1;
}

/* Main program */
int main(int argc, char* argv[])
{
    const char* infilename;
    FILE* infile;
    const char* outfilename;
    FILE* outfile;
    size_t target_size = 0;
    int err; /* stdio error: 0 == OK, EOF == error */
    int ret; /* boolean return value: 0 == error, 1 == OK */
    CMD_TYPE op = CMD_NONE;

    g_argv0 = argv[0]; /* Remember the program name */

    if (argc == 5 && !strcmp(argv[1], "pad"))
    {
        op = CMD_PAD;

        target_size = get_size_value(argv[2]);
        if (target_size == SIZE_ERROR)
            return 1;

        infilename = argv[3];
        outfilename = argv[4];
    }
    else
    {
        fprintf(stderr, "usage:\n");
        fprintf(stderr, "  # Generic zero padding\n");
        fprintf(stderr, "  %s pad <size> <source> <destination>\n", g_argv0);
        return 1;
    }

    /* Open the source file */
    infile = fopen(infilename, "rb");
    if (infile == NULL)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, infilename, strerror(errno));
        return 1;
    }

    /* Open the destination file */
    outfile = fopen(outfilename, "wb");
    if (outfile == NULL)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 1;
    }

    switch (op)
    {
        case CMD_PAD:
            ret = cmd_pad(infile, infilename, outfile, outfilename, target_size);
        break;

        default:
            abort(); /* Should not happen */
        break;
    }

    if (!ret)
    {
        /* Error message already written */
        fclose(outfile);
        remove(outfilename);
        return 1;
    }

    /* Close the output file */
    err = fclose(outfile);
    if (err != 0)
    {
        remove(outfilename);
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 1;
    }

    /* Close the input file */
    err = fclose(infile);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, infilename, strerror(errno));
        return 1;
    }

    return 0;
}
