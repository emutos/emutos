/* Kernel printf (usable only by OS task) */

#include "portab.h"

extern void printout(char *);

extern char * output;	/* This output is the result max 1024 c */


static void  prt_dec(int num, char * str)
{
    int	i;
    char	temp[6];

    temp[0] = '\0';
    for(i = 1; i <= 5; i++)  {
        temp[i] = num % 10 + '0';
        num /= 10;
    }
    for(i = 5; temp[i] == '0'; i--);
    if( i == 0 )
        i++;
    while( i >= 0 )
        *str++ = temp[i--];
}

static void  prt_hex(LONG num, char * str)
{
    int	i;
    char	temp[5];

    temp[0] = '\0';
    for(i = 1; i <= 4; i++)  {
        temp[i] = "0123456789abcdef"[num & 0x0f];
        num = num >> 4;
    }
    for(i = 4; temp[i] == '0'; i--);
    if( i == 0 )
        i++;
    while( i >= 0 )
        *str++ = temp[i--];
}

static void  prt_bin(LONG num, char * str)
{
    int	i;
    char	temp[17];

    temp[0] = '\0';
    for(i = 1; i <= 16; i++)  {
        temp[i] = ((num%2) == 0) ? '0' : '1';
        num = num >> 1;
    }
    for(i = 16; temp[i] == '0'; i--);
    if( i == 0 )
        i++;
    while( i >= 0 )
        *str++ = temp[i--];
}


/*------------------------------------------------------------------------
 *  kprintf  --  kernel printf: formatted, unbuffered output to emulator
 *----------------------------------------------------------------------*/
void    kprint(char * fmt)
{
    int c;
    char * out;

    out = output;

    while (1) {
        /* Echo characters until '%' or end of fmt string */
        c = *fmt++;
        *out++=(char)c;
        if( c == '\0' ) {
            break;
        }
    }
    printout(output);
}

void    kprint1(char * fmt, int * arg1)
{
    int	        c;
    int	        f;		/* The format character (comes after %) */

    int         num;            /* arg as number */
    char        *str;		/* Running pointer in string		*/
    char        string[20];  	/* Space for number -> string convers.  */
                                /*  from number conversion		*/
    char	sign;		/* Set to '-' for negative decimals	*/
    char        *out;            /* This points to actual char in output */

    out = output;               /* Set output pointer to start */
    output[1023]='\0';

    while((c = *fmt++)) {
        if( c == '%' ) {
            /* Echo "...%%..." as '%' */
            if( *fmt == '%' ) {
                *out++=*fmt++;
                continue;
            }

            if( (f= *fmt++) == '\0' ) {
                *out++='%';
                break;
            }
            sign = '\0';	/* sign == '-' for negative decimal */

            switch( f ) {
            case 'c' :
                *out++ = (char) *arg1;
                break;

            case 's' :
                str = (char *) arg1;
                while(*str != '\0') {
                    *out++=*str++;
                }
                break;

            case 'i' :
            case 'd' :
                num = *arg1;        /* add short/int conversion??? */
                if ( num < 0 ) {
                    *out++ = '-';
                    num = -num;
                }
            case 'u':
                str = string;
                num = *arg1;        /* add short/int conversion??? */
                prt_dec(num, str);
                while(*str != '\0') {
                    *out++=*str++;
                }
                break;

            case 'x' :
                str = string;
                num = *arg1;        /* add short/int conversion??? */
                prt_hex(num, str);
                while(*str != '\0') {
                    *out++=*str++;
                }
                break;

            case 'b' :
                str = string;
                num = *arg1;        /* add short/int conversion??? */
                prt_bin(num, str);
                while(*str != '\0') {
                    *out++=*str++;
                }
                break;

            default :
                *out++=(char)f;
                break;
            }

        } else {
            *out++=(char)c;
        }
    }
    printout(output);
}


void    kprint2(char * fmt, int * arg1, int * arg2)
{
    printout(output);
}

void    kprint3(char * fmt, int * arg1, int * arg2, int * arg3)
{
    printout(output);
}

