/*
 * mkrom.c - Create an EmuTOS ROM image
 *
 * Copyright (C) 2012-2024 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * This tool adds padding bytes to an EmuTOS binary image,
 * and also creates special ROM formats.
 */

#define DBG_MKROM 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define SIZE_ERROR ((size_t)-1)
#define MIN(a, b) ((a)<=(b) ? (a) : (b))
#define BUFFER_SIZE (16*1024)

/* Command-line commands */
typedef enum
{
    CMD_NONE = 0,
    CMD_PAD,
    CMD_PAK3,
    CMD_STC,
    CMD_AMIGA,
    CMD_AMIGA_KICKDISK,
    CMD_AMIGA_FLOPPY,
    CMD_LISA_BOOT_FLOPPY,
    CMD_LISA_DATA_FLOPPY
} CMD_TYPE;

/* Global variables */
static const char* g_argv0; /* Program name */
static uint8_t g_buffer[BUFFER_SIZE]; /* Global buffer to minimize stack usage */

/* Read a big endian long */
static uint32_t read_big_endian_long(const uint32_t* p)
{
    union
    {
        uint32_t l;
        uint8_t b[4];
    } u;

    u.l = *p;

    return ((uint32_t)u.b[0]) << 24
         | ((uint32_t)u.b[1]) << 16
         | ((uint32_t)u.b[2]) << 8
         |  (uint32_t)u.b[3];
}

/* Write a big endian long */
static void write_big_endian_long(uint32_t* p, uint32_t value)
{
    union
    {
        uint32_t l;
        char b[4];
    } u;

    u.b[0] = (value >> 24);
    u.b[1] = (value >> 16);
    u.b[2] = (value >> 8);
    u.b[3] = value;

    *p = u.l;
}

/* Read a big endian short */
static uint16_t read_big_endian_short(const uint16_t* p)
{
    union
    {
        uint16_t s;
        uint8_t b[2];
    } u;

    u.s = *p;

    return ((uint16_t)u.b[0]) << 8
         |  (uint16_t)u.b[1];
}

/* Write a big endian short */
static void write_big_endian_short(uint16_t* p, uint16_t value)
{
    union
    {
        uint16_t s;
        uint8_t b[2];
    } u;

    u.b[0] = (value >> 8);
    u.b[1] = value;

    *p = u.s;
}

/* Add two longs, and the carry, if any */
static uint32_t add_with_carry(uint32_t a, uint32_t b)
{
    uint32_t sum = a + b;

    if (sum < a) /* Overflow */
        sum++; /* Add carry */

    return sum;
}

/* Get an integer value from an integer string with a k, m, or g suffix */
static size_t get_size_value(const char* strsize)
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
static size_t get_file_size(FILE* file, const char* filename)
{
    long initial_pos; /* Initial file position */
    int err; /* Seek error */
    long end_pos; /* End file position */

    /* Remember the initial position */
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
    err = fseek(file, initial_pos, SEEK_SET);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, filename, strerror(errno));
        return SIZE_ERROR;
    }

    /* The end position is the file size */
    return (size_t)end_pos;
}

/* Write a block of identical bytes to a file */
static int write_byte_block(FILE* outfile, const char* outfilename, uint8_t value, size_t count)
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
static int copy_stream(FILE* infile, const char* infilename,
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

/* Append a file into a stream, and pad it with zeros */
static int append_and_pad(FILE* infile, const char* infilename,
                          FILE* outfile, const char* outfilename,
                          size_t target_size, size_t *psource_size)
{
    size_t free_size;
    int ret; /* boolean return value: 0 == error, 1 == OK */

    /* Get the input file size */
    *psource_size = get_file_size(infile, infilename);
    if (*psource_size == SIZE_ERROR)
        return 0;

    /* Check if the input file size is not too big */
    if (*psource_size > target_size)
    {
        fprintf(stderr, "%s: %s is too big: %lu extra bytes\n", g_argv0, infilename, (unsigned long)(*psource_size - target_size));
        return 0;
    }

    /* Copy the input file */
    ret = copy_stream(infile, infilename, outfile, outfilename, *psource_size);
    if (!ret)
        return ret;

    /* Pad with zeros */
    free_size = target_size - *psource_size;
    ret = write_byte_block(outfile, outfilename, 0, free_size);
    if (!ret)
        return ret;

    return 1;
}

/* Copy and pad with zeros up to target_size */
static int cmd_pad(FILE* infile, const char* infilename,
                   FILE* outfile, const char* outfilename,
                   size_t target_size)
{
    size_t source_size;
    size_t free_size;
    int ret; /* boolean return value: 0 == error, 1 == OK */

    printf("# Padding %s to %ld KB image into %s\n", infilename, ((long)target_size) / 1024, outfilename);

    ret = append_and_pad(infile, infilename, outfile, outfilename, target_size, &source_size);
    if (!ret)
        return ret;

    free_size = target_size - source_size;
    printf("# %s done (%lu bytes free)\n", outfilename, (unsigned long)free_size);

    return 1;
}

/* PAK/3 512kB image */
static int cmd_pak3(FILE* infile, const char* infilename,
                   FILE* outfile, const char* outfilename)
{
    size_t source_size;
    size_t max_size = 256 * 1024;       /* input must be a 256KB image */
    size_t target_size = 512 * 1024;    /* which we pad to 512KB (and patch) */
    long jmp_address = 0x40030L;
    const unsigned char jmp_instr[] = { 0x4e, 0xf9, 0x00, 0xe0, 0x00, 0x00 };
    int ret; /* boolean return value: 0 == error, 1 == OK */

    printf("# Padding %s to %ld KB image into %s\n", infilename, ((long)target_size) / 1024, outfilename);

    /* Get the input file size */
    source_size = get_file_size(infile, infilename);
    if (source_size == SIZE_ERROR)
        return 0;

    /* Check if the input file size is too big */
    if (source_size > max_size)
    {
        fprintf(stderr, "%s: %s is too big: %lu extra bytes\n", g_argv0, infilename, (unsigned long)(source_size - max_size));
        return 0;
    }

    ret = append_and_pad(infile, infilename, outfile, outfilename, target_size, &source_size);
    if (!ret)
        return ret;

    /* Rewind to the patch address */
    if (fseek(outfile, jmp_address, SEEK_SET) != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Write the 'JMP' instruction */
    if (fwrite(jmp_instr, 1, sizeof jmp_instr, outfile) != sizeof jmp_instr)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    printf("# %s done\n", outfilename);

    return 1;
}

/* Steem Engine cartridge image */
static int cmd_stc(FILE* infile, const char* infilename,
                   FILE* outfile, const char* outfilename)
{
    size_t source_size;
    size_t target_size = 128 * 1024;
    size_t free_size;
    int ret; /* boolean return value: 0 == error, 1 == OK */

    printf("# Padding %s to %ld KB Steem Engine cartridge image into %s\n", infilename, ((long)target_size) / 1024, outfilename);

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

    /* Insert a long zero at the beginning */
    ret = write_byte_block(outfile, outfilename, 0, 4);
    if (!ret)
        return ret;

    /* Copy the input file */
    ret = copy_stream(infile, infilename, outfile, outfilename, source_size);
    if (!ret)
        return ret;

    /* Pad with zeros */
    free_size = target_size - source_size;
    ret = write_byte_block(outfile, outfilename, 0, free_size);
    if (!ret)
        return ret;

    printf("# %s done (%lu bytes free)\n", outfilename, (unsigned long)free_size);

    return 1;
}

/* Structure at the end of an Amiga ROM */
struct amiga_rom_footer
{
    uint32_t checksum_fixup; /* Fixup to get checksum == 0xffffffff */
    uint32_t romsize; /* Size of the ROM, in bytes */
    uint16_t vectors[8]; /* Autovectors exception numbers */
};

/* Compute the checksum of an Amiga ROM */
static int compute_amiga_checksum(FILE* file, const char* filename, size_t filesize, uint32_t* pchecksum)
{
    uint32_t big_endian_val;
    uint32_t value;
    uint32_t checksum = 0;
    size_t nread;
    int err; /* Seek error */
    size_t total_read = 0;

    /* Rewind to the start of the file */
    err = fseek(file, 0, SEEK_SET);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, filename, strerror(errno));
        return 0;
    }

    while (total_read < filesize)
    {
        nread = fread(&big_endian_val, sizeof big_endian_val, 1, file);
        if (nread == 0)
        {
            if (ferror(file))
            {
                fprintf(stderr, "%s: %s: %s\n", g_argv0, filename, strerror(errno));
                return 0;
            }
            else
            {
                fprintf(stderr, "%s: %s: premature end of file.\n", g_argv0, filename);
                return 0;
            }
        }

        total_read += sizeof big_endian_val;

        value = read_big_endian_long(&big_endian_val);
        checksum = add_with_carry(checksum, value);
    }

    *pchecksum = checksum;
    return 1;
}

/* Amiga ROM image */
static int cmd_amiga(FILE* infile, const char* infilename,
                     FILE* outfile, const char* outfilename)
{
    int i;
    size_t nwrite;
    size_t source_size;
    size_t target_size = 256 * 1024;
    struct amiga_rom_footer footer;
    size_t max_size = target_size - sizeof footer;
    size_t free_size;
    int ret; /* boolean return value: 0 == error, 1 == OK */
    int err; /* Seek error */
    uint32_t checksum;

    printf("# Padding %s to %ld KB Amiga ROM image into %s\n", infilename, ((long)target_size) / 1024, outfilename);

    /* Get the input file size */
    source_size = get_file_size(infile, infilename);
    if (source_size == SIZE_ERROR)
        return 0;

    /* Check if the input file size is not too big */
    if (source_size > max_size)
    {
        fprintf(stderr, "%s: %s is too big: %lu extra bytes\n", g_argv0, infilename, (unsigned long)(source_size - max_size));
        return 0;
    }

    /* Copy the input file */
    ret = copy_stream(infile, infilename, outfile, outfilename, source_size);
    if (!ret)
        return ret;

    /* Pad with zeros */
    free_size = max_size - source_size;
    ret = write_byte_block(outfile, outfilename, 0, free_size);
    if (!ret)
        return ret;

    /* Set up ROM footer */
    /* The checksum fixup will be overwritten in a second pass */
    write_big_endian_long(&footer.checksum_fixup, 0);
    write_big_endian_long(&footer.romsize, target_size);
    for (i = 0; i < 8; i++)
        write_big_endian_short(&footer.vectors[i], 0x18 + i);

    /* Write the footer with temporary checksum_fixup */
    nwrite = fwrite(&footer, 1, sizeof footer, outfile);
    if (nwrite != sizeof footer)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Compute the adequate checksum_fixup */
    ret = compute_amiga_checksum(outfile, outfilename, target_size, &checksum);
    if (!ret)
        return ret;
#if DBG_MKROM
    printf("# checksum before fixup = 0x%08lx\n", (unsigned long)checksum);
#endif
    write_big_endian_long(&footer.checksum_fixup, 0xffffffff - checksum);

    /* Seek to the footer location again */
    err = fseek(outfile, max_size, SEEK_SET);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Write the footer a second time with the fixed checksum */
    nwrite = fwrite(&footer, 1, sizeof footer, outfile);
    if (nwrite != sizeof footer)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

#if DBG_MKROM
    ret = compute_amiga_checksum(outfile, outfilename, target_size, &checksum);
    if (!ret)
        return ret;
    printf("# checksum  after fixup = 0x%08lx\n", (unsigned long)checksum);
#endif

    printf("# %s done (%lu bytes free)\n", outfilename, (unsigned long)free_size);

    return 1;
}

/* Amiga Kickstart-like floppy for Amiga 1000 */
static int cmd_amiga_kickdisk(FILE* infile, const char* infilename,
                              FILE* outfile, const char* outfilename)
{
    int ret; /* boolean return value: 0 == error, 1 == OK */
    size_t written; /* Number of bytes written this time */
    size_t source_size;
    size_t bootblock_size = 512;
    size_t rom_size = 256 * 1024;
    size_t target_size = 880 * 1024;
    size_t pad_size;

    printf("# Padding %s to Amiga 1000 Kickstart disk into %s\n", infilename, outfilename);

    /* Get the input file size */
    source_size = get_file_size(infile, infilename);
    if (source_size == SIZE_ERROR)
        return 0;

    /* Check if the input file size is not too big */
    if (source_size != rom_size)
    {
        fprintf(stderr, "%s: %s has invalid size: %lu bytes (should be %lu bytes)\n", g_argv0, infilename, (unsigned long)source_size, (unsigned long)rom_size);
        return 0;
    }

    /* Write the Kickstart bootblock */
    strncpy((char*)g_buffer, "KICK", bootblock_size);
    written = fwrite(g_buffer, 1, bootblock_size, outfile);
    if (written != bootblock_size)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Copy the input file */
    ret = copy_stream(infile, infilename, outfile, outfilename, source_size);
    if (!ret)
        return ret;

    /* Pad with zeros */
    pad_size = target_size - bootblock_size - source_size;
    ret = write_byte_block(outfile, outfilename, 0, pad_size);
    if (!ret)
        return ret;

    printf("# %s done\n", outfilename);

    return 1;
}

/* Amiga boot floppy */
static int cmd_amiga_floppy(FILE* bootfile, const char* bootfilename,
                            FILE* ramtosfile, const char* ramtosfilename,
                            FILE* outfile, const char* outfilename)
{
    int ret; /* boolean return value: 0 == error, 1 == OK */
    size_t bootblock_size = 1024;
    size_t floppy_size = 880 * 1024;
    size_t max_ramtos_size = floppy_size - bootblock_size;
    size_t boot_size;
    size_t ramtos_size;
    size_t free_size;
    uint32_t checksum;
    uint32_t big_endian_long;
    int err; /* Seek error */
    size_t nwrite;

    printf("# Padding %s and %s to Amiga boot floppy into %s\n", bootfilename, ramtosfilename, outfilename);

    /* Append and pad boot */
    ret = append_and_pad(bootfile, bootfilename, outfile, outfilename, bootblock_size, &boot_size);
    if (!ret)
        return ret;

    /* Append and pad ramtos */
    ret = append_and_pad(ramtosfile, ramtosfilename, outfile, outfilename, max_ramtos_size, &ramtos_size);
    if (!ret)
        return ret;
    free_size = max_ramtos_size - ramtos_size;

    /* Seek to the ramtos size location (must match amigaboot.S ramtos_size) */
    err = fseek(outfile, 14, SEEK_SET);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Write the ramtos size */
    write_big_endian_long(&big_endian_long, ramtos_size);
    nwrite = fwrite(&big_endian_long, 1, sizeof big_endian_long, outfile);
    if (nwrite != sizeof big_endian_long)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Compute the adequate bootblock checksum fixup */
    ret = compute_amiga_checksum(outfile, outfilename, bootblock_size, &checksum);
    if (!ret)
        return ret;
#if DBG_MKROM
    printf("# checksum before fixup = 0x%08lx\n", (unsigned long)checksum);
#endif

    /* Seek to the checksum location */
    err = fseek(outfile, 4, SEEK_SET);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Write the checksum fixup */
    write_big_endian_long(&big_endian_long, 0xffffffff - checksum);
    nwrite = fwrite(&big_endian_long, 1, sizeof big_endian_long, outfile);
    if (nwrite != sizeof big_endian_long)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

#if DBG_MKROM
    ret = compute_amiga_checksum(outfile, outfilename, bootblock_size, &checksum);
    if (!ret)
        return ret;
    printf("# checksum  after fixup = 0x%08lx\n", (unsigned long)checksum);
#endif

    /* Seek to end of bootblock */
    err = fseek(outfile, bootblock_size, SEEK_SET);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    printf("# %s done (%lu bytes free)\n", outfilename, (unsigned long)free_size);

    return 1;
}

/* Header of Apple Disk Copy 4.2 disk image.
 * https://wiki.68kmla.org/DiskCopy_4.2_format_specification
 * Each sector has a size of 512 bytes,
 * plus an extra area of 12 bytes called "tag".
 */
struct dc42_header
{
    char pascal_name[64]; /* Image name, first byte is length, padded with 0 */
    uint32_t data_size; /* Size of Data Block */
    uint32_t tag_size; /* Size of Tag Block */
    uint32_t data_checksum; /* Checksum of Data Block */
    uint32_t tag_checksum; /* Checksum of Tag Block */
    uint8_t encoding; /* Disk encoding */
    uint8_t format; /* Format byte */
    uint16_t magic; /* Magic number */
};

#define DC42_ENCODING_GCR_SSDD 0x00 /* GCR 400 KB */
#define DC42_FORMAT_MAC400K 0x02
#define DC42_MAGIC 0x0100

/* Write a C string to a Pascal buffer.
 * First Pascal byte will be the string length, followed by string characters.
 * Return 1 if success, or 0 if error.
 */
static int pascal_strncpy(char* pascal_dest, size_t dest_size, const char* str)
{
    size_t maxlength = dest_size - 1;
    size_t length = strlen(str);

    if (length > maxlength)
    {
        fprintf(stderr, "error: String too long: %s", str);
        return 0;
    }

    pascal_dest[0] = (char)length;
    strncpy(pascal_dest + 1, str, maxlength);

    return 1;
}

/* Compute the checksum for Apple Disk Copy 4.2 data area or tag area.
 * Due to a bug in the original software, data and tag checksums need to
 * be computed differently.
 * Set the "tag" parameter to 1 when computing the checksum for the tag area,
 * or 0 for the data area.
 */
static int compute_dc42_checksum(FILE* file, const char* filename, size_t datasize, uint32_t* pchecksum, int tag)
{
    uint16_t big_endian_val;
    uint16_t value;
    uint32_t checksum = 0;
    size_t nread;
    size_t total_read = 0;

    while (total_read < datasize)
    {
        nread = fread(&big_endian_val, sizeof big_endian_val, 1, file);
        if (nread == 0)
        {
            if (ferror(file))
            {
                fprintf(stderr, "%s: %s: %s\n", g_argv0, filename, strerror(errno));
                return 0;
            }
            else
            {
                fprintf(stderr, "%s: %s: premature end of file.\n", g_argv0, filename);
                return 0;
            }
        }

        total_read += sizeof big_endian_val;

        if (tag)
        {
            /* Bug in Disk Copy 4.2 format for tags checksum!
             * First 12 bytes must be skipped. */
            if (total_read <= 12)
                continue;
        }

        value = read_big_endian_short(&big_endian_val);
        checksum += value;
        checksum = (checksum >> 1) | ((checksum & 1) << 31); /* ROR32 */
    }

    *pchecksum = checksum;
    return 1;
}

/* Write a single tag */
static int write_tag(FILE* file, const char* filename, const uint16_t w[6])
{
    size_t written; /* Number of bytes written this time */
    size_t size = sizeof(uint16_t) * 6;

    written = fwrite(w, 1, size, file);
    if (written != size)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, filename, strerror(errno));
        return 0;
    }

    return 1;
}

/* Write tags */
static int write_tags(FILE* file, const char* filename,
                      size_t total_sectors, size_t used_sectors)
{
    int ret; /* boolean return value: 0 == error, 1 == OK */
    uint16_t w[6];
    size_t sector;

    /* This mimics the LisaOS boot floppy.
     * Certainly overkill, as the boot ROM only checks for the 0xaaaa signature
     * in the first sector. So most of this code could certainly be removed.
     */

    /* Used sectors */
    write_big_endian_short(&w[0], 0); /* ??? */
    write_big_endian_short(&w[1], 2); /* ??? */
    write_big_endian_short(&w[4], 0x87ff); /* ??? */
    write_big_endian_short(&w[5], 0x07ff); /* ??? */
    for (sector = 0; sector < used_sectors; sector++)
    {
        /* The first sector *must* be marked as bootable */
        uint16_t type = (sector == 0) ? 0xaaaa : 0xbbbb;
        write_big_endian_short(&w[2], type);
        write_big_endian_short(&w[3], sector);

        ret = write_tag(file, filename, w);
        if (!ret)
            return ret;
    }

    /* Empty sectors */
    memset(w, 0, sizeof w);
    for (; sector < total_sectors; sector++)
    {
        ret = write_tag(file, filename, w);
        if (!ret)
            return ret;
    }

    return 1;
}

/* Apple Lisa boot floppy. Single sided, GCR format. */
static int cmd_lisa_boot_floppy(FILE* bootfile, const char* bootfilename,
                                FILE* ramtosfile, const char* ramtosfilename,
                                FILE* outfile, const char* outfilename)
{
    int ret; /* boolean return value: 0 == error, 1 == OK */
    size_t written; /* Number of bytes written this time */
    struct dc42_header header;
    size_t bootblock_size = 512;
    size_t floppy_size = 400 * 1024;
    size_t total_sectors = floppy_size / 512;
    size_t tag_size = 12 * total_sectors;
    size_t max_ramtos_size = floppy_size - bootblock_size;
    size_t boot_size;
    size_t ramtos_size;
    size_t free_size;
    size_t used_sectors;
    uint32_t checksum;
    uint32_t big_endian_long;
    int err; /* Seek error */
    size_t nwrite;

    printf("# Padding %s and %s to Lisa boot floppy into %s\n", bootfilename, ramtosfilename, outfilename);

    /* Set up the Disk Copy 4.2 header */
    memset(&header, 0, sizeof header);
    /*pascal_strncpy(header.pascal_name, sizeof header.pascal_name, "-not a Macintosh disk-");*/
    pascal_strncpy(header.pascal_name, sizeof header.pascal_name, "EmuTOS for Lisa");
    write_big_endian_long(&header.data_size, floppy_size);
    write_big_endian_long(&header.tag_size, tag_size);
    header.encoding = DC42_ENCODING_GCR_SSDD;
    header.format = DC42_FORMAT_MAC400K;
    write_big_endian_short(&header.magic, DC42_MAGIC);

    /* Write the header */
    written = fwrite(&header, 1, sizeof header, outfile);
    if (written != sizeof header)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Append and pad boot */
    ret = append_and_pad(bootfile, bootfilename, outfile, outfilename, bootblock_size, &boot_size);
    if (!ret)
        return ret;

    /* Append and pad ramtos */
    ret = append_and_pad(ramtosfile, ramtosfilename, outfile, outfilename, max_ramtos_size, &ramtos_size);
    if (!ret)
        return ret;
    free_size = max_ramtos_size - ramtos_size;
    used_sectors = (bootblock_size + ramtos_size + 511) / 512;

    /* Append tags */
    ret = write_tags(outfile, outfilename, total_sectors, used_sectors);
    if (!ret)
        return ret;

    /* Seek to the ramtos size location (must match lisaboot.S ramtos_size) */
    err = fseek(outfile, sizeof header + 2, SEEK_SET);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Write the ramtos size */
    write_big_endian_long(&big_endian_long, ramtos_size);
    nwrite = fwrite(&big_endian_long, 1, sizeof big_endian_long, outfile);
    if (nwrite != sizeof big_endian_long)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Rewind to the start of data */
    err = fseek(outfile, sizeof header, SEEK_SET);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Compute the data checksum */
    ret = compute_dc42_checksum(outfile, outfilename, floppy_size, &checksum, 0);
    if (!ret)
        return ret;
    write_big_endian_long(&header.data_checksum, checksum);

    /* Compute the tag checksum */
    ret = compute_dc42_checksum(outfile, outfilename, tag_size, &checksum, 1);
    if (!ret)
        return ret;
    write_big_endian_long(&header.tag_checksum, checksum);

    /* Seek to the header location again */
    err = fseek(outfile, 0, SEEK_SET);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Write the header a second time with the checksums */
    nwrite = fwrite(&header, 1, sizeof header, outfile);
    if (nwrite != sizeof header)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    printf("# %s done (%lu bytes free)\n", outfilename, (unsigned long)free_size);

    return 1;
}

/* Write tags filled with zero */
static int write_zero_tags(FILE* file, const char* filename,
                           size_t total_sectors)
{
    int ret; /* boolean return value: 0 == error, 1 == OK */
    uint16_t w[6];
    size_t sector;

    memset(w, 0, sizeof w);
    for (sector = 0; sector < total_sectors; sector++)
    {
        ret = write_tag(file, filename, w);
        if (!ret)
            return ret;
    }

    return 1;
}

/* Convert an ST floppy image to an Apple Lisa floppy image. Must be 400 KB */
static int cmd_lisa_data_floppy(FILE* infile, const char* infilename,
                                FILE* outfile, const char* outfilename)
{
    int ret; /* boolean return value: 0 == error, 1 == OK */
    size_t written; /* Number of bytes written this time */
    struct dc42_header header;
    size_t floppy_size = 400 * 1024;
    size_t total_sectors = floppy_size / 512;
    size_t tag_size = 12 * total_sectors;
    uint32_t checksum;
    int err; /* Seek error */
    size_t nwrite;
    size_t source_size;

    printf("# Converting %s to Lisa data floppy into %s\n", infilename, outfilename);

    /* Get the input file size */
    source_size = get_file_size(infile, infilename);
    if (source_size == SIZE_ERROR)
        return 0;

    /* Check input file */
    if (source_size != floppy_size)
    {
        fprintf(stderr, "%s: %s has wrong size (%lu bytes), must be exactly %lu bytes\n", g_argv0, infilename, (unsigned long)source_size, (unsigned long)floppy_size);
        return 0;
    }

    /* Set up the Disk Copy 4.2 header */
    memset(&header, 0, sizeof header);
    pascal_strncpy(header.pascal_name, sizeof header.pascal_name, "-not a Macintosh disk-");
    write_big_endian_long(&header.data_size, floppy_size);
    write_big_endian_long(&header.tag_size, tag_size);
    header.encoding = DC42_ENCODING_GCR_SSDD;
    header.format = DC42_FORMAT_MAC400K;
    write_big_endian_short(&header.magic, DC42_MAGIC);

    /* Write the header */
    written = fwrite(&header, 1, sizeof header, outfile);
    if (written != sizeof header)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Copy the input file */
    ret = copy_stream(infile, infilename, outfile, outfilename, floppy_size);
    if (!ret)
        return ret;

    /* Append tags filled with zeros */
    ret = write_zero_tags(outfile, outfilename, total_sectors);
    if (!ret)
        return ret;

    /* Rewind to the start of data */
    err = fseek(outfile, sizeof header, SEEK_SET);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Compute the data checksum */
    ret = compute_dc42_checksum(outfile, outfilename, floppy_size, &checksum, 0);
    if (!ret)
        return ret;
    write_big_endian_long(&header.data_checksum, checksum);

    /* Compute the tag checksum */
    ret = compute_dc42_checksum(outfile, outfilename, tag_size, &checksum, 1);
    if (!ret)
        return ret;
    write_big_endian_long(&header.tag_checksum, checksum);

    /* Seek to the header location again */
    err = fseek(outfile, 0, SEEK_SET);
    if (err != 0)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    /* Write the header a second time with the checksums */
    nwrite = fwrite(&header, 1, sizeof header, outfile);
    if (nwrite != sizeof header)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, outfilename, strerror(errno));
        return 0;
    }

    printf("# %s done\n", outfilename);

    return 1;
}

/* Main program */
int main(int argc, char* argv[])
{
    const char* bootfilename = NULL;
    FILE* bootfile = NULL;
    const char* infilename;
    FILE* infile;
    const char* outfilename;
    FILE* outfile;
    size_t target_size = 0;
    int err; /* stdio error: 0 == OK, EOF == error */
    int ret; /* boolean return value: 0 == error, 1 == OK */
    CMD_TYPE op = CMD_NONE;
    const char* outmode = "wb"; /* By default, write only */

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
    else if (argc == 4 && !strcmp(argv[1], "pak3"))
    {
        op = CMD_PAK3;
        infilename = argv[2];
        outfilename = argv[3];
    }
    else if (argc == 4 && !strcmp(argv[1], "stc"))
    {
        op = CMD_STC;
        infilename = argv[2];
        outfilename = argv[3];
    }
    else if (argc == 4 && !strcmp(argv[1], "amiga"))
    {
        op = CMD_AMIGA;
        infilename = argv[2];
        outfilename = argv[3];
        outmode = "w+b"; /* Computing the checksum requires read/write */
    }
    else if (argc == 4 && !strcmp(argv[1], "amiga-kickdisk"))
    {
        op = CMD_AMIGA_KICKDISK;
        infilename = argv[2];
        outfilename = argv[3];
    }
    else if (argc == 5 && !strcmp(argv[1], "amiga-floppy"))
    {
        op = CMD_AMIGA_FLOPPY;
        bootfilename = argv[2];
        infilename = argv[3];
        outfilename = argv[4];
        outmode = "w+b"; /* Computing the checksum requires read/write */
    }
    else if (argc == 5 && !strcmp(argv[1], "lisa-boot-floppy"))
    {
        op = CMD_LISA_BOOT_FLOPPY;
        bootfilename = argv[2];
        infilename = argv[3];
        outfilename = argv[4];
        outmode = "w+b"; /* Computing the checksum requires read/write */
    }
    else if (argc == 4 && !strcmp(argv[1], "lisa-data-floppy"))
    {
        op = CMD_LISA_DATA_FLOPPY;
        infilename = argv[2];
        outfilename = argv[3];
        outmode = "w+b"; /* Computing the checksum requires read/write */
    }
    else
    {
        fprintf(stderr, "usage:\n");
        fprintf(stderr, "  # Generic zero padding\n");
        fprintf(stderr, "  %s pad <size> <source> <destination>\n", g_argv0);
        fprintf(stderr, "\n");
        fprintf(stderr, "  # Steem Engine cartridge image\n");
        fprintf(stderr, "  %s stc <source.img> <destination.stc>\n", g_argv0);
        fprintf(stderr, "\n");
        fprintf(stderr, "  # Amiga ROM image\n");
        fprintf(stderr, "  %s amiga <source.img> <destination.rom>\n", g_argv0);
        fprintf(stderr, "\n");
        fprintf(stderr, "  # Amiga 1000 Kickstart disk\n");
        fprintf(stderr, "  %s amiga-kickdisk <source.rom> <destination.adf>\n", g_argv0);
        fprintf(stderr, "\n");
        fprintf(stderr, "  # Amiga boot floppy\n");
        fprintf(stderr, "  %s amiga-floppy <bootfile.img> <source.img> <destination.adf>\n", g_argv0);
        fprintf(stderr, "\n");
        fprintf(stderr, "  # Apple Lisa boot floppy\n");
        fprintf(stderr, "  %s lisa-boot-floppy <bootfile.img> <source.img> <destination.dc42>\n", g_argv0);
        fprintf(stderr, "\n");
        fprintf(stderr, "  # Apple Lisa data floppy from Atari 400 KB floppy (1 side, 80 tracks, 10 sectors)\n");
        fprintf(stderr, "  %s lisa-data-floppy <source.st> <destination.dc42>\n", g_argv0);
        fprintf(stderr, "\n");
        fprintf(stderr, "  # PAK/3 image\n");
        fprintf(stderr, "  %s pak3 <source.img> <destination.img>\n", g_argv0);
        return 1;
    }

    /* Open the boot file (if present) */
    if (bootfilename)
    {
        bootfile = fopen(bootfilename, "rb");
        if (bootfile == NULL)
        {
            fprintf(stderr, "%s: %s: %s\n", g_argv0, bootfilename, strerror(errno));
            return 1;
        }
    }

    /* Open the source file */
    infile = fopen(infilename, "rb");
    if (infile == NULL)
    {
        fprintf(stderr, "%s: %s: %s\n", g_argv0, infilename, strerror(errno));
        return 1;
    }

    /* Open the destination file */
    outfile = fopen(outfilename, outmode);
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

        case CMD_PAK3:
            ret = cmd_pak3(infile, infilename, outfile, outfilename);
        break;

        case CMD_STC:
            ret = cmd_stc(infile, infilename, outfile, outfilename);
        break;

        case CMD_AMIGA:
            ret = cmd_amiga(infile, infilename, outfile, outfilename);
        break;

        case CMD_AMIGA_KICKDISK:
            ret = cmd_amiga_kickdisk(infile, infilename, outfile, outfilename);
        break;

        case CMD_AMIGA_FLOPPY:
            ret = cmd_amiga_floppy(bootfile, bootfilename, infile, infilename, outfile, outfilename);
        break;

        case CMD_LISA_BOOT_FLOPPY:
            ret = cmd_lisa_boot_floppy(bootfile, bootfilename, infile, infilename, outfile, outfilename);
        break;

        case CMD_LISA_DATA_FLOPPY:
            ret = cmd_lisa_data_floppy(infile, infilename, outfile, outfilename);
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

    /* Close the boot file (if present) */
    if (bootfilename)
    {
        err = fclose(bootfile);
        if (err != 0)
        {
            fprintf(stderr, "%s: %s: %s\n", g_argv0, bootfilename, strerror(errno));
            return 1;
        }
    }

    return 0;
}
