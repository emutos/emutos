#ifndef _XHDI_H
#define _XHDI_H

#define XHGETVERSION    0
#define XHINQTARGET     1
#define XHRESERVE       2
#define XHLOCK          3
#define XHSTOP          4
#define XHEJECT         5
#define XHDRVMAP        6
#define XHINQDEV        7
#define XHINQDRIVER     8
#define XHNEWCOOKIE     9
#define XHREADWRITE     10
#define XHINQTARGET2    11
#define XHINQDEV2       12
#define XHDRIVERSPECIAL 13
#define XHGETCAPACITY   14
#define XHMEDIUMCHANGED 15
#define XHMINTINFO      16
#define XHDOSLIMITS     17
#define XHLASTACCESS    18
#define XHREACCESS      19

long XHInqTarget(UWORD major, UWORD minor, ULONG *blocksize,
                 ULONG *deviceflags, char *productname);

long XHGetCapacity(UWORD major, UWORD minor, ULONG *blocks, ULONG *blocksize);

long XHReadWrite(UWORD major, UWORD minor, UWORD rw, ULONG sector,
                 UWORD count, void *buf);
#endif /* _XHDI_H */
