/* A program for changing the language setting of a multi-language
 * TOS ROM (EmuTOS) file.
 * 
 * Compile with:
 *      gcc -o tos-lang-change -O -Wall tos-lang-change.c
 * 
 * (w) 2005 by Eero Tamminen
 */

#include <stdio.h>
#include <netinet/in.h> /* big endian (network) / host endian conversions */

#define TOS_CONF_OFFSET 0x1C

typedef struct {
        uint16_t value;
        const char *name;
} country_t;

#define COUNTRY_ERROR 255

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
        { 14, "Holland" },
        { 15, "Czech Republic" },
        { 16, "Hungary" },
	{ 17, "Slovakia" },
	{ 18, "Greece" },
        {127, "Multilanguage (all countries are supported, TOS >= v4.0)" },
        {COUNTRY_ERROR, NULL }
};


static inline uint16_t conf2country(uint16_t conf)
{
        return conf >> 1;
}
static inline uint16_t country2conf(uint16_t conf, uint16_t country)
{
        return (country << 1) | (conf & 1);
}

/* return country name when given country TOS code, NULL if unrecognized code */
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

/* show all the TOS country alternatives and asks user for a country code
 * returns the user code if it's valid, otherwise COUNTRY_ERROR
 */
static uint16_t get_new_country_value(void)
{
        country_t *country;
        uint16_t value;

        printf("\nSelect new country code:\n");
        for (country = countries; country->name; country++) {
                printf("%3hu) %s\n", country->value, country->name);
        }
        printf("> ");
        if (scanf("%3hu", &value) == 1) {
                return value;
        } else {
                fprintf(stderr, "Error: code %ud is invalid!\n", value);
                return COUNTRY_ERROR;
        }
}

/* gets current OS conf bits, returns new (user given) bits */
static uint16_t get_new_conf_value(uint16_t conf)
{
        uint16_t value;
        const char *name;

        value = conf2country(conf);

        name = get_country_name(value);
        if (!name) {
        /* failed */
                fprintf(stderr, "Error: TOS ROM has unrecognized country code %d\n", value);
                return conf;
        }
        printf("Current TOS ROM country code:\n%3hu) %s\n", value, name);

        do {
                value = get_new_country_value();
        } while (value == COUNTRY_ERROR);

        name = get_country_name(value);
        printf("New TOS ROM country code:\n%3hu) %s\n", value, name);

        return country2conf(conf, value);
}

int main(int argc, char *argv[])
{
        uint16_t oldconf, newconf;
        FILE *fp;

        if (argc != 2) {
                printf("usage: %s <TOS ROM file>\n", *argv);
                printf("\nAllows changing the default country in a multilanguage Atari TOS ROM\n");
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
        printf("OS conf variable updated to the TOS ROM file.\n");

        return 0;
}

