#pragma ident "$Id: packet.c,v 1.4 2012/02/14 20:22:33 dechavez Exp $"
/*======================================================================
 *
 *  Read/write xfer_packets.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "platform.h"
#include "xfer.h"
#include "util.h"

static size_t slen = XFER_SNAMLEN + 1;
static size_t clen = XFER_CNAMLEN + 1;
static size_t ilen = XFER_INAMLEN + 1;
static size_t Dlen = sizeof(double);
static size_t Flen = sizeof(float);
static size_t Ilen = sizeof(int);
static size_t Llen = sizeof(INT32);
static size_t Slen = sizeof(INT16);
static size_t Ulen = sizeof(UINT32);

int Xfer_WrtPacket(fp, packet)
FILE *fp;
struct xfer_packet *packet;
{
size_t dlen;

    dlen = packet->nsamp * sizeof(INT32);

    if (fwrite(packet->sname,     1, slen, fp) != slen) return  -1;
    if (fwrite(&packet->lat,      1, Flen, fp) != Flen) return  -2;
    if (fwrite(&packet->lon,      1, Flen, fp) != Flen) return  -3;
    if (fwrite(&packet->elev,     1, Flen, fp) != Flen) return  -4;
    if (fwrite(&packet->depth,    1, Flen, fp) != Flen) return  -5;
    if (fwrite(packet->cname,     1, clen, fp) != clen) return  -6;
    if (fwrite(packet->instype,   1, ilen, fp) != ilen) return  -7;
    if (fwrite(&packet->sint,     1, Flen, fp) != Flen) return  -8;
    if (fwrite(&packet->calib,    1, Flen, fp) != Flen) return  -9;
    if (fwrite(&packet->calper,   1, Flen, fp) != Flen) return -10;
    if (fwrite(&packet->vang,     1, Flen, fp) != Flen) return -11;
    if (fwrite(&packet->hang,     1, Flen, fp) != Flen) return -12;
    if (fwrite(&packet->beg,      1, Dlen, fp) != Dlen) return -13;
    if (fwrite(&packet->end,      1, Dlen, fp) != Dlen) return -13;
    if (fwrite(&packet->tear,     1, Ilen, fp) != Ilen) return -14;
    if (fwrite(&packet->nsamp,    1, Llen, fp) != Llen) return -15;
    if (fwrite(packet->data,      1, dlen, fp) != dlen) return -16;

    return 0;
}

int Xfer_RdPacket(fp, packet)
FILE *fp;
struct xfer_packet *packet;
{
size_t dlen;

    if (fread(packet->sname,     1, slen, fp) != slen) return  -1;
    if (fread(&packet->lat,      1, Flen, fp) != Flen) return  -2;
    if (fread(&packet->lon,      1, Flen, fp) != Flen) return  -3;
    if (fread(&packet->elev,     1, Flen, fp) != Flen) return  -4;
    if (fread(&packet->depth,    1, Flen, fp) != Flen) return  -5;
    if (fread(packet->cname,     1, clen, fp) != clen) return  -6;
    if (fread(packet->instype,   1, ilen, fp) != ilen) return  -7;
    if (fread(&packet->sint,     1, Flen, fp) != Flen) return  -8;
    if (fread(&packet->calib,    1, Flen, fp) != Flen) return  -9;
    if (fread(&packet->calper,   1, Flen, fp) != Flen) return -10;
    if (fread(&packet->vang,     1, Flen, fp) != Flen) return -11;
    if (fread(&packet->hang,     1, Flen, fp) != Flen) return -12;
    if (fread(&packet->beg,      1, Dlen, fp) != Dlen) return -13;
    if (fread(&packet->end,      1, Dlen, fp) != Dlen) return -13;
    if (fread(&packet->tear,     1, Ilen, fp) != Ilen) return -14;
    if (fread(&packet->nsamp,    1, Llen, fp) != Llen) return -15;
    if (packet->nsamp > XFER_MAXDAT) return -16;

    dlen = packet->nsamp * sizeof(INT32);

    if (fread(packet->dbuf, 1, dlen, fp) != dlen) return -17;
    packet->data = (INT32 *) packet->dbuf;

    return 0;
}

/* Revision History
 *
 * $Log: packet.c,v $
 * Revision 1.4  2012/02/14 20:22:33  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.3  2005/05/25 22:41:56  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.2  2005/05/23 21:02:24  dechavez
 * cleaned up includes (05-23 update AAP)
 *
 * Revision 1.1.1.1  2000/02/08 20:20:43  dec
 * import existing IDA/NRTS sources
 *
 */
