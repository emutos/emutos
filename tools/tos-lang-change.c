/* A program for changing the language setting of a multi-language
 * TOS ROM (EmuTOS) file.
 *
 * Compile with:
 *      gcc -o tos-lang-change -O -Wall tos-lang-change.c
 *
 * Copyright 2005-2016 Eero Tamminen
 * Copyright 2022      Christian Zietz
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include <stdio.h>
#include <stdint.h>

/* big endian (network) / host endian conversions */
#ifdef _WIN32
#include <winsock.h>
#else
#include <netinet/in.h>
#endif

#define TOS_CONF_OFFSET 0x1C

#define MAX_INPUTLEN    20      /* for user input country code */

typedef struct {
        int16_t value;
        const char *name;
} country_t;

#define COUNTRY_ERROR   -1

static country_t countries[] = {
        {  0, "USA" },
        {  1, "Germany" },
        {  2, "France" },
        {  3, "United Kingdom" },
        {  4, "Spain" },
        {  5, "Italy" },
        {  6, "Sweden" },
        {  7, "Switzerland (French)" },
        {  8, "Switzerland (German)" },
        {  9, "Turkey" },
        { 10, "Finland" },
        { 11, "Norway" },
        { 12, "Denmark" },
        { 13, "Saudi Arabia" },
        { 14, "Netherlands" },
        { 15, "Czech Republic" },
        { 16, "Hungary" },
        { 17, "Poland" },
        { 19, "Russia" },
        { 31, "Greece" },
        {127, "Multilanguage (all countries are supported, TOS >= v4.0)" },
        {COUNTRY_ERROR, NULL }
};


static uint16_t conf2country(uint16_t conf)
{
        return conf >> 1;
}
static uint16_t country2conf(uint16_t conf, uint16_t country)
{
        return (country << 1) | (conf & 1);
}

/* returns country name when given country TOS code, NULL if unrecognized code */
static const char *get_country_name(uint16_t value)
{
        country_t *country;

        for (country = countries; country->name; country++) {
                if (value == country->value) {
                        break;
                }
        }
        return country->name;
}

/* return 1 iff passed value is in country table */
static int valid_country_value(uint16_t value)
{
        country_t *country;

        for (country = countries; country->name; country++) {
                if (value == country->value) {
                        return 1;
                }
        }

        return 0;
}

/* show all the TOS country alternatives and ask user for a country code.
 * returns the user code if it's valid, otherwise COUNTRY_ERROR
 */
static int16_t get_new_country_value(void)
{
        char s[MAX_INPUTLEN];
        country_t *country;
        uint16_t value;

        printf("\nSelect new country code:\n");
        for (country = countries; country->name; country++) {
                printf("%3hu) %s\n", country->value, country->name);
        }
        printf("> ");
        if (!fgets(s, MAX_INPUTLEN, stdin))
                return COUNTRY_ERROR;

        if (sscanf(s, "%3hu\n", &value) == 1) {
                if (valid_country_value(value))
                        return value;
        }

        fprintf(stderr, "Error: invalid code!\n");
        return COUNTRY_ERROR;
}

/* gets current OS conf bits, returns new (user given) bits */
static uint16_t get_new_conf_value(uint16_t conf)
{
        int16_t value;
        const char *name;

        value = conf2country(conf);

        name = get_country_name(value);
        if (!name) {
                /* failed */
                fprintf(stderr, "Error: TOS ROM has unrecognized country code %d\n", value);
                return conf;
        }
        printf("Current TOS video sync mode:\n %s\n", (conf & 1) ? "PAL" : "NTSC");
        printf("Current TOS ROM country code:\n%3hu) %s\n", value, name);

        do {
                value = get_new_country_value();
        } while (value == COUNTRY_ERROR);

        name = get_country_name(value);
        conf = country2conf(conf, value);
        /* us -> NTSC (0), any other -> PAL (1) */
        if (value) {
                conf |= 1;
        } else {
                conf &= ~1;
        }

        printf("\nNew TOS video sync mode:\n %s\n", conf&1 ? "PAL" : "NTSC");
        printf("New TOS ROM country code:\n%3hu) %s\n", value, name);
        return conf;
}

int main(int argc, char *argv[])
{
        uint16_t oldconf, newconf;
        FILE *fp;

        if (argc != 2) {
                printf("usage: %s <TOS ROM file>\n", *argv);
                printf("\nAllows changing the default country in a multilanguage Atari TOS ROM.\n");
                printf("\nUSA implies NTSC, any other country implies PAL video sync mode.\n");
                return 1;
        }

        printf("Opening TOS ROM file '%s' for reading/writing...\n", argv[1]);

        /* get current OS configuration value */
        if (!(fp = fopen(argv[1], "rb+"))) {
                perror("Error opening TOS ROM file");
                return -1;
        }
        if (fseek(fp, TOS_CONF_OFFSET, SEEK_SET) != 0) {
                perror("Error seeking to OS conf var offset");
                return -1;
        }
        if (fread(&oldconf, sizeof(oldconf), 1, fp) != 1) {
                perror("Error reading OS conf variable");
                return -1;
        }

        /* get new configuration value from the user and convert between
         * host & big (network) endian values
         */
        newconf = htons(get_new_conf_value(ntohs(oldconf)));
        if (newconf == oldconf) {
                printf("\nOS conf variable not changed.\n");
                return 0;
        }

        /* write new OS configuration value */
        if (fseek(fp, TOS_CONF_OFFSET, SEEK_SET) != 0) {
                perror("Error seeking to OS conf var offset");
                return -1;
        }
        if (fwrite(&newconf, sizeof(newconf), 1, fp) != 1) {
                perror("Error writing OS conf variable");
                return -1;
        }
        fclose(fp);
        printf("\nOS conf variable updated in TOS ROM file '%s'.\n", argv[1]);

        return 0;
}
