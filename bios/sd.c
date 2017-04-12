/*
 * sd.c - SD/MMC card routines
 *
 * Copyright (C) 2013-2017 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"

#include "disk.h"
#include "asm.h"
#include "blkdev.h"
#include "delay.h"
#include "gemerror.h"
#include "mfp.h"
#include "sd.h"
#include "spi.h"
#include "string.h"
#include "tosvars.h"

#include "kprint.h"

#if CONF_WITH_SDMMC

/*
 *  SD card commands
 */
                            /* Basic command class */
#define CMD0        0           /* GO_IDLE_STATE: response type R1 */
#define CMD1        1           /* SEND_OP_COND: response type R1 */
#define CMD8        8           /* SEND_IF_COND: response type R7 */
#define CMD9        9           /* SEND_CSD: response type R1 */
#define CMD10       10          /* SEND_CID: response type R1 */
#define CMD12       12          /* STOP_TRANSMISSION: response type R1B */
#define CMD13       13          /* SEND_STATUS: response type R2 */
#define CMD58       58          /* READ_OCR: response type R3 */
                            /* Block read command class */
#define CMD16       16          /* SET_BLOCKLEN: response type R1 */
#define CMD17       17          /* READ_SINGLE_BLOCK: response type R1 */
#define CMD18       18          /* READ_MULTIPLE_BLOCK: response type R1 */
                            /* Block write command class */
#define CMD24       24          /* WRITE_BLOCK: response type R1 */
#define CMD25       25          /* WRITE_MULTIPLE_BLOCK: response type R1 */
                            /* Application-specific command class */
#define CMD55       55          /* APP_CMD: response type R1 */
#define ACMD13      13          /* SD_STATUS: response type R2 (in SPI mode only!) */
#define ACMD41      41          /* SD_SEND_OP_COND: response type R1 */
#define ACMD51      51          /* SEND_SCR: response type R1 */

/*
 *  SD card response types
 */
#define R1          1
#define R1B         2
#define R2          3
#define R3          4
#define R7          5

/*
 *  SD error code bits
 */
#define SD_ERR_IDLE_STATE       0x01
#define SD_ERR_ILLEGAL_CMD      0x04

/*
 *  SD timeouts
 */
                            /* a useful macro */
#define msec_to_ticks(msec)     ((msec*CLOCKS_PER_SEC+999)/1000)
                            /* these are byte-count timeout values */
#define SD_CMD_TIMEOUT          8       /* between sending crc & receiving response */
#define SD_CSD_TIMEOUT          8       /* between sending SEND_CSD cmd & receiving data */
                            /* these are millisecond timeout values (see SD specifications v4.10) */
#define SD_POWERUP_DELAY_MSEC   1       /* minimum power-up time */
#define SD_INIT_TIMEOUT_MSEC    1000    /* waiting for card to become ready */
#define SD_READ_TIMEOUT_MSEC    100     /* waiting for start bit of data block */
#define SD_WRITE_TIMEOUT_MSEC   500     /* waiting for end of busy */
                            /* these are derived timeout values in TOS ticks */
#define SD_POWERUP_DELAY_TICKS  msec_to_ticks(SD_POWERUP_DELAY_MSEC)
#define SD_INIT_TIMEOUT_TICKS   msec_to_ticks(SD_INIT_TIMEOUT_MSEC)
#define SD_READ_TIMEOUT_TICKS   msec_to_ticks(SD_READ_TIMEOUT_MSEC)
#define SD_WRITE_TIMEOUT_TICKS  msec_to_ticks(SD_WRITE_TIMEOUT_MSEC)

/*
 *  SD data tokens
 */
#define DATAERROR_TOKEN_MASK    0xf0    /* for reads: error token is 0000EEEE */
#define DATARESPONSE_TOKEN_MASK 0x1f    /* for writes: data response token is xxx0sss1 */
#define START_MULTI_WRITE_TOKEN 0xfc
#define STOP_TRANSMISSION_TOKEN 0xfd
#define START_BLOCK_TOKEN       0xfe

/*
 *  miscellaneous
 */
#define SDV2_CSIZE_MULTIPLIER   1024    /* converts C_SIZE to sectors */
#define DELAY_1_MSEC            delay_loop(loopcount_1_msec)

/*
 *  structures
 */
struct cardinfo {
    UBYTE type;
#define CARDTYPE_UNKNOWN    0
#define CARDTYPE_MMC        1
#define CARDTYPE_SD         2
    UBYTE version;
    UBYTE features;
#define BLOCK_ADDRESSING    0x02
#define MULTIBLOCK_IO       0x01
};

/*
 *  globals
 */
static struct cardinfo card;
static UBYTE response[5];

/*
 *  function prototypes
 */
static void sd_cardtype(struct cardinfo *info);
static int sd_command(UBYTE cmd,ULONG argument,UBYTE crc,UBYTE resptype,UBYTE *resp);
static ULONG sd_calc_capacity(UBYTE *csd);
static LONG sd_check(UWORD drv);
static void sd_features(struct cardinfo *info);
static UBYTE sd_get_dataresponse(void);
static int sd_mbtest(void);
static LONG sd_read(UWORD drv,ULONG sector,UWORD count,UBYTE *buf);
static int sd_receive_data(UBYTE *buf,UWORD len,UWORD special);
static int sd_send_data(UBYTE *buf,UWORD len,UBYTE token);
static int sd_special_read(UBYTE cmd,UBYTE *data);
static int sd_wait_for_not_busy(LONG timeout);
static int sd_wait_for_not_idle(UBYTE cmd,ULONG arg);
static int sd_wait_for_ready(LONG timeout);
static LONG sd_write(UWORD drv,ULONG sector,UWORD count,UBYTE *buf);


/*
 *  initialise sd/mmc bus
 */
void sd_init(void)
{
    sd_check(0);    /* just drive 0 to check */
}

/*
 *  read/write interface
 */
LONG sd_rw(WORD rw,LONG sector,WORD count,UBYTE *buf,WORD dev)
{
LONG ret;
UBYTE *p = buf;
int i;

    if (dev)
        return EUNDEV;

    if (count == 0)
        return 0;

    /*
     * retry at most once (to handle reinitialisation)
     */
    for (i = 0; i < 2; i++) {
        /* see if we need to (re)initialise the card after a previous error */
        if (card.type == CARDTYPE_UNKNOWN) {
            if (sd_check(dev)) {
                ret = EDRVNR;
                break;
            }
            if (!(rw & RW_NOMEDIACH)) {
                ret = E_CHNG;
                break;
            }
        }

        ret = (rw&RW_RW) ? sd_write(dev,sector,count,p) : sd_read(dev,sector,count,p);

        if (ret == 0L)
            break;

        if (ret > 0L) {         /* error but not timeout */
            ret = (rw&RW_RW) ? EWRITF : EREADF;
            break;
        }

        /* at this point, it must be a timeout.  we set the return
         * code in case the retry has failed
         */
        ret = (rw & RW_NOMEDIACH) ? EDRVNR : E_CHNG;

        /* for timeouts, card.type should already be set to
         * CARDTYPE_UNKNOWN, but we'll set it here anyway
         * (this is what forces the reinitialisation on retry)
         */
        card.type = CARDTYPE_UNKNOWN;
    }

    if (ret < 0)
        KDEBUG(("sd_rw(%d,%ld,%d,%p,%d) rc=%ld\n",rw,sector,count,p,dev,ret));

    return ret;
}

/*
 *  perform miscellaneous non-data-transfer functions
 */
LONG sd_ioctl(UWORD drv,UWORD ctrl,void *arg)
{
LONG rc = 0;
ULONG *info = arg;
UBYTE cardreg[16];

    if (drv)
        return EUNDEV;

    switch(ctrl) {
    case GET_DISKINFO:
        if (sd_special_read(CMD9,cardreg)) {    /* medium could have changed */
            sd_check(drv);                      /* so try to reset & retry */
            if (sd_special_read(CMD9,cardreg)) {
                card.type = CARDTYPE_UNKNOWN;
                rc = E_CHNG;
                break;
            }
        }
        info[0] = sd_calc_capacity(cardreg);
        info[1] = SECTOR_SIZE;
        break;
    case GET_DISKNAME:
        if (sd_special_read(CMD10,cardreg)) {   /* medium could have changed */
            sd_check(drv);                      /* so try to reset & retry */
            if (sd_special_read(CMD10,cardreg)) {
                card.type = CARDTYPE_UNKNOWN;
                rc = E_CHNG;
                break;
            }
        }
        if (card.type == CARDTYPE_SD) {
            cardreg[8] = '\0';
            strcpy(arg,(const char *)cardreg+1);
        } else {    /* must be MMC */
            cardreg[9] = '\0';
            strcpy(arg,(const char *)cardreg+3);
        }
        break;
    case GET_MEDIACHANGE:
        if (sd_special_read(CMD9,cardreg) == 0)
            rc = MEDIANOCHANGE;
        else {
            if (sd_check(drv))  /*  attempt to reset device  */
                card.type = CARDTYPE_UNKNOWN;
            rc = MEDIACHANGE;
        }
        break;
    default:
        rc = ERR;
    }

    return rc;
}

/*
 *  perform special read commands for ioctl
 */
static int sd_special_read(UBYTE cmd,UBYTE *data)
{
UWORD special = (cmd==CMD9) ? 1 : 0;
int rc = ERR;

    spi_cs_assert();

    if (sd_command(cmd,0L,0,R1,response) == 0)
        if (sd_receive_data(data,16,special) == 0)
            rc = 0;

    spi_cs_unassert();

    return rc;
}

/*
 *  check drive for card present and re-initialise to handle it
 */
static LONG sd_check(UWORD drv)
{
int i, rc;

    if (drv)
        return EUNDEV;

    spi_initialise();
    spi_clock_ident();

    /* wait at least 1msec */
    DELAY_1_MSEC;

    /* send at least 74 dummy clocks with CS unasserted (high) */
    spi_cs_unassert();
    for (i = 0; i < 10; i++)
        spi_send_byte(0xff);

    spi_cs_assert();

    /*
     *  if CMD0 doesn't cause a switch to idle state, there's
     *  probably no card inserted, so exit with error
     */
    rc = sd_command(CMD0,0L,0x95,R1,response);
    if ((rc < 0) || !(rc&SD_ERR_IDLE_STATE)) {
        KDEBUG(("CMD0 failed, rc=%d, response=0x%02x\n",rc,response[0]));
        card.type = CARDTYPE_UNKNOWN;
        spi_cs_unassert();
        return EDRVNR;
    }

    /*
     *  determine card type, version, features
     */
    sd_cardtype(&card);
    sd_features(&card);

    /*
     *  force block length to SECTOR_SIZE if byte addressing
     */
    if (card.type != CARDTYPE_UNKNOWN)
        if (!(card.features&BLOCK_ADDRESSING))
            if (sd_command(CMD16,SECTOR_SIZE,0,R1,response) != 0)
                card.type = CARDTYPE_UNKNOWN;

    spi_cs_unassert();

    KDEBUG(("Card info: type %d, version %d, features 0x%02x\n",
            card.type,card.version,card.features));

    switch(card.type) {
    case CARDTYPE_SD:
        spi_clock_sd();
        break;
    case CARDTYPE_MMC:
        spi_clock_mmc();
        break;
    default:
        return EDRVNR;
    }

    return 0L;
}

/*
 *  read one or more blocks
 */
static LONG sd_read(UWORD drv,ULONG sector,UWORD count,UBYTE *buf)
{
LONG i, rc, rc2;
LONG posn, incr;

    spi_cs_assert();

    /*
     *  handle byte/block addressing
     */
    if (card.features&BLOCK_ADDRESSING) {
        posn = sector;
        incr = 1;
    } else {
        posn = sector * SECTOR_SIZE;
        incr = SECTOR_SIZE;
    }

    rc = 0L;

    /*
     *  can we use multi sector reads?
     */
    if ((count > 1) && (card.features&MULTIBLOCK_IO)) {
        rc = sd_command(CMD18,posn,0,R1,response);
        if (rc == 0L) {
            for (i = 0; i < count; i++, buf += SECTOR_SIZE) {
                rc = sd_receive_data(buf,SECTOR_SIZE,0);
                if (rc)
                    break;
            }
            rc2 = sd_command(CMD12,0L,0,R1B,response);
            if (rc == 0)
                rc = rc2;
        }
    } else {            /* use single sector */
        for (i = 0; i < count; i++, posn += incr, buf += SECTOR_SIZE) {
            rc = sd_command(CMD17,posn,0,R1,response);
            if (rc == 0L)
                rc = sd_receive_data(buf,SECTOR_SIZE,0);
            if (rc)
                break;
        }
    }

    spi_cs_unassert();

    return rc;
}

/*
 *  write one or more blocks
 *
 *  note: we don't use the pre-erase function, since
 *  it doesn't seem to improve performance
 */
static LONG sd_write(UWORD drv,ULONG sector,UWORD count,UBYTE *buf)
{
LONG i, rc, rc2;
LONG posn, incr;

    spi_cs_assert();

    /*
     *  handle byte/block addressing
     */
    if (card.features&BLOCK_ADDRESSING) {
        posn = sector;
        incr = 1;
    } else {
        posn = sector * SECTOR_SIZE;
        incr = SECTOR_SIZE;
    }

    rc = 0L;

    /*
     *  can we use multi sector writes?
     */
    if ((count > 1) && (card.features&MULTIBLOCK_IO)) {
        rc = sd_command(CMD25,posn,0,R1,response);
        if (rc == 0L) {
            for (i = 0; i < count; i++, buf += SECTOR_SIZE) {
                rc = sd_send_data(buf,SECTOR_SIZE,START_MULTI_WRITE_TOKEN);
                if (rc)
                    break;
            }
            rc2 = sd_send_data(NULL,0,STOP_TRANSMISSION_TOKEN);
            if (rc == 0)
                rc = rc2;
        }
    } else {            /* use single sector write */
        for (i = 0; i < count; i++, posn += incr, buf += SECTOR_SIZE) {
            rc = sd_command(CMD24,posn,0,R1,response);
            if (rc == 0L)
                rc = sd_send_data(buf,SECTOR_SIZE,START_BLOCK_TOKEN);
            if (rc)
                break;
        }
    }

    spi_cs_unassert();

    return rc;
}

/*
 *  send a command to the SD card in SPI mode
 *
 *  returns -1  timeout or bad response type
 *          0   OK
 *          >0  error status from response[0]
 */
static int sd_command(UBYTE cmd,ULONG argument,UBYTE crc,UBYTE resp_type,UBYTE *resp)
{
    int i, resp_length;

    /*
     *  set up response length
     */
    switch(resp_type) {
    case R1:
    case R1B:
        resp_length = 1;
        break;
    case R2:
        resp_length = 2;
        break;
    case R3:
    case R7:
        resp_length = 5;
        break;
    default:
        return -1;
    }

    /*
     *  the following test serves two functions:
     *  1. it ensures that at least one byte is clocked out before sending
     *     the command.  some cards seem to require this, at least during
     *     the initialisation sequence.
     *  2. it cleans up any residual data that the card may be sending as
     *     a result of a previous command that experienced problems.
     */
    if (sd_wait_for_ready(SD_READ_TIMEOUT_TICKS) < 0)
        return -1;

    /* Send the command byte, argument, crc */
    spi_send_byte((cmd & 0x3f) | 0x40);

    spi_send_byte((argument>>24)&0xff);
    spi_send_byte((argument>>16)&0xff);
    spi_send_byte((argument>>8)&0xff);
    spi_send_byte(argument&0xff);

    /* CRC is ignored by default in SPI mode ... but we always need a stop bit! */
    spi_send_byte(crc|0x01);

    if (cmd == CMD12)                   /* stop transmission: */
        spi_recv_byte();                /* always discard first byte */

    /* now we look for the response, which starts with a byte with the 0x80 bit clear */
    for (i = 0; i < SD_CMD_TIMEOUT; i++) {
        resp[0] = spi_recv_byte();
        if ((resp[0]&0x80) == 0)
            break;
    }
    if (i >= SD_CMD_TIMEOUT)            /* timed out */
        return -1;

    /*
     *  retrieve remainder of response iff command is legal
     *  (if it's illegal, it's effectively an R1 response type)
     */
    if ((resp[0] & SD_ERR_ILLEGAL_CMD) == 0) {
        for (i = 1; i < resp_length; i++)
            resp[i] = spi_recv_byte();
    }

    /*
     *  for R1B responses, we need to wait for the end of the busy state.
     *  R1B is only set by write-type commands (CMD12, CMD28, CMD29, CMD38)
     *  so we use the write timeout here.
     */
    if (resp_type == R1B)
        if (sd_wait_for_not_busy(SD_WRITE_TIMEOUT_TICKS) < 0)
            return -1;

    return resp[0];
}

/*
 *  receive data block
 *
 *  notes:
 *  1. if 'buf' is NULL, we throw away the received data
 *  2. if 'special' is non-zero, we use the special SD_CSD_TIMEOUT
 *     instead of the standard read timeout
 *
 *  returns -1 timeout or unexpected start token
 *          0   ok
 */
static int sd_receive_data(UBYTE *buf,UWORD len,UWORD special)
{
LONG i;
UBYTE token;

    /* wait for the token */
    if (special) {
        for (i = 0; i < SD_CSD_TIMEOUT; i++) {
            token = spi_recv_byte();
            if (token != 0xff)
                break;
        }
    } else {
        ULONG end = hz_200 + SD_READ_TIMEOUT_TICKS;
        while(hz_200 < end) {
            token = spi_recv_byte();
            if (token != 0xff)
                break;
        }
    }
    if (token == 0xff)
        return -1;

    /* check for valid token */
    if (token != START_BLOCK_TOKEN) {
        KDEBUG(("sd_receive_data() bad startblock token 0x%02x\n",token));
        return -1;
    }

    /*
     *  transfer data
     */
    if (buf) {
        for (i = 0; i < len; i++)
            *buf++ = spi_recv_byte();
    } else {
        for (i = 0; i < len; i++)
            spi_recv_byte();
    }

    spi_recv_byte();        /* discard crc */
    spi_recv_byte();

    return 0;
}

/*
 *  send data block
 *
 *  returns -1  timeout or bad response token
 *          0   ok
 */
static int sd_send_data(UBYTE *buf,UWORD len,UBYTE token)
{
LONG i;
UBYTE rtoken;

    spi_send_byte(token);
    if (token == STOP_TRANSMISSION_TOKEN) {
        spi_recv_byte();    /* skip a byte before testing for busy */
    } else {
        /* send the data */
        for (i = 0; i < len; i++)
            spi_send_byte(*buf++);
        spi_send_byte(0xff);        /* send dummy crc */
        spi_send_byte(0xff);

        /* check the data response token */
        rtoken = sd_get_dataresponse();
        if ((rtoken & DATARESPONSE_TOKEN_MASK) != 0x05) {
            KDEBUG(("sd_send_data() response token 0x%02x\n",rtoken));
            return -1;
        }
    }

    return sd_wait_for_not_busy(SD_WRITE_TIMEOUT_TICKS);
}

/*
 *  determine card type & version
 */
static void sd_cardtype(struct cardinfo *info)
{
int rc;
UBYTE csd[16];

    info->type = CARDTYPE_UNKNOWN;          /* defaults */
    info->version = 0;

    /*
     *  check first for SDv2
     */
    if ((sd_command(CMD8,0x000001aaL,0x87,R7,response) >= 0)
     && ((response[0]&SD_ERR_ILLEGAL_CMD) == 0)) {
        if ((response[3]&0x0f) != 0x01)     /* voltage not accepted */
            return;
        if (response[4] != 0xaa)            /* check pattern mismatch */
            return;
        if (sd_wait_for_not_idle(ACMD41,0x40000000L) != 0)
            return;
        info->type = CARDTYPE_SD;
        info->version = 2;
        return;
    }

    /*
     *  check for SDv1
     */
    rc = sd_wait_for_not_idle(ACMD41,0L);
    if (rc == 0) {
        info->type = CARDTYPE_SD;
        info->version = 1;
        return;
    }

    /*
     *  check for MMC
     */
    rc = sd_wait_for_not_idle(CMD1,0L);
    if (rc) {
        info->type = CARDTYPE_UNKNOWN;
        return;
    }
    info->type = CARDTYPE_MMC;

    /*
     *  determine MMC version from CSD
     */
    if (sd_command(CMD9,0L,0,R1,response) == 0)
        if (sd_receive_data(csd,16,1) == 0)
            info->version = (csd[0] >> 2) & 0x0f;
}

/*
 *  determine card features
 */
static void sd_features(struct cardinfo *info)
{
    info->features = 0;

    /*
     *  check SDv2 for block addressing
     */
    if ((info->type == CARDTYPE_SD) && (info->version == 2)) {
        if (sd_command(CMD58,0L,0,R3,response) != 0) {  /* shouldn't happen */
            info->type = CARDTYPE_UNKNOWN;
            info->version = 0;
            return;
        }
        if (response[1] & 0x40)
            info->features |= BLOCK_ADDRESSING;
    }

    /*
     *  all SD cards support multiple block I/O
     */
    if (info->type == CARDTYPE_SD) {
        info->features |= MULTIBLOCK_IO;
        return;
    }

    /*
     *  check MMC for multiple block I/O support
     *  v3 cards always have it ... but so do some v2 & v1 cards
     */
    if (info->type == CARDTYPE_MMC) {
        if (info->version == 3)
            info->features |= MULTIBLOCK_IO;
        else if (sd_mbtest() == 0)
            info->features |= MULTIBLOCK_IO;
    }
}

/*
 *  test if multiple block i/o works
 *  returns 0 iff true
 */
static int sd_mbtest(void)
{
    /*
     *  see if READ_MULTIPLE_BLOCK/STOP_TRANSMISSION work
     *  if they do, we assume the write stuff works too
     */
    if (sd_command(CMD18,0L,0,R1,response))
        return -1;

    sd_receive_data(NULL,SECTOR_SIZE,0);
    if (sd_command(CMD12,0L,0,R1B,response))
        return -1;

    return 0;
}

/*
 *  calculate card capacity in sectors
 */
static ULONG sd_calc_capacity(UBYTE *csd)
{
UBYTE read_bl_len, c_size_mult;
ULONG c_size;

    if ((card.type == CARDTYPE_SD) && ((csd[0]>>6) == 0x01)) {
        c_size = csd[7] & 0x3f;
        c_size = (c_size << 8) + csd[8];
        c_size = (c_size << 8) + csd[9];
        return c_size * SDV2_CSIZE_MULTIPLIER;
    }

    read_bl_len = csd[5] & 0x0f;
    c_size = csd[6] & 0x03;
    c_size = (c_size << 8) + csd[7];
    c_size = (c_size << 2) + ((csd[8] & 0xc0) >> 6);
    c_size_mult = ((csd[9] & 0x03) << 1) + ((csd[10] & 0x80) >> 7);

    /*
     * according to the specs, size (bytes)
     *   = (C_SIZE+1) * 2**(C_SIZE_MULT+2) * 2**READ_BL_LEN
     *
     * therefore, size in sectors is calculated as:
     *   (C_SIZE+1) * 2**(C_SIZE_MULT+2) * 2**READ_BL_LEN / 512
     */
    return (c_size+1) << (c_size_mult+2 + read_bl_len - 9);
}

/*
 *  get the data response.  although it *should* be the byte
 *  immediately after the data transfer, some cards miss the
 *  time frame by one or more bits, so we check bit-by-bit.
 *
 *  idea stolen from the linux driver mmc_spi.c
 */
static UBYTE sd_get_dataresponse(void)
{
ULONG pattern;

        pattern = (ULONG)spi_recv_byte() << 24;    /* accumulate 4 bytes */
        pattern |= (ULONG)spi_recv_byte() << 16;
        pattern |= (ULONG)spi_recv_byte() << 8;
        pattern |= (ULONG)spi_recv_byte();

        /* the first 3 bits are undefined */
        pattern |= 0xe0000000L;             /* first 3 bits are undefined */

        /* left-adjust to leading 0 bit */
        while(pattern & 0x80000000L)
            pattern <<= 1;

        /* right-adjust to put code into bits 4-0 */
        pattern >>= 27;

        return LOBYTE(pattern);
}

/*
 *  initialisation function:
 *      loops, issuing command & waiting for card to become "un-idle"
 *
 *  assumes that input cmd is 1 or 41 and, if it's 41,
 *  it's ACMD41 and so must be preceded by CMD55
 *
 *  returns 0   ok
 *          -1  timeout
 */
static int sd_wait_for_not_idle(UBYTE cmd,ULONG arg)
{
ULONG end = hz_200 + SD_INIT_TIMEOUT_TICKS;

    while(hz_200 < end) {
        if (cmd == ACMD41)
            if (sd_command(CMD55,0L,0,R1,response) < 0)
                break;
        if (sd_command(cmd,arg,0,R1,response) < 0)
            break;
        if ((response[0] & SD_ERR_IDLE_STATE) == 0)
            return 0;
    }

    return -1;
}

/*
 *  wait for not busy indication
 *
 *  note: timeout value is in ticks
 *
 *  returns -1  timeout
 *          0   ok
 */
static int sd_wait_for_not_busy(LONG timeout)
{
ULONG end = hz_200 + timeout;
UBYTE c;

    while(hz_200 < end) {
        c = spi_recv_byte();
        if (c != 0x00)
            return 0;
    }

    return -1;
}

/*
 *  wait for ready indication
 *
 *  note: timeout value is in ticks
 *
 *  returns -1  timeout
 *          0   ok
 */
static int sd_wait_for_ready(LONG timeout)
{
ULONG end = hz_200 + timeout;
UBYTE c;

    while(hz_200 < end) {
        c = spi_recv_byte();
        if (c == 0xff)
            return 0;
    }

    return -1;
}

#endif /* CONF_WITH_SDMMC */
