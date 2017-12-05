#pragma ident "$Id: convert.c,v 1.5 2015/12/04 22:31:16 dechavez Exp $"
/*======================================================================
 *
 * Convert from generic xfer format to other types
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include "xfer.h"

#ifdef NRTS_SUPPORT
int Xfer_CvtNRTS(cnf, wav, packet)
struct xfer_cnf *cnf;
struct xfer_wav *wav;
struct nrts_packet *packet;
{
static UINT32 host_order = 0;
struct xfer_wavgen1 *wavgen;
struct xfer_cnfgen1 *cnfptr;
int standx, chnndx;

    if (wav->format != XFER_WAVGEN1) return -1;

    if (host_order == 0) host_order = util_order();

    cnfptr = (struct xfer_cnfgen1 *) &cnf->type.gen1;
    wavgen = (struct xfer_wavgen1 *) &wav->type.gen1;

    standx = wavgen->standx;
    chnndx = wavgen->chnndx;
    strcpy(packet->sname, cnfptr->sta[standx].name);
    strcpy(packet->cname, cnfptr->sta[standx].chn[chnndx].name);
    strcpy(packet->instype, cnfptr->sta[standx].chn[chnndx].instype);
    packet->beg.time = wavgen->tofs;
    packet->beg.code = ' ';
    packet->beg.qual = 0;
    packet->tear     = (float) wavgen->tear;
    packet->sint     = cnfptr->sta[standx].chn[chnndx].sint;
    packet->calib    = cnfptr->sta[standx].chn[chnndx].calib;
    packet->calper   = cnfptr->sta[standx].chn[chnndx].calper;
    packet->vang     = cnfptr->sta[standx].chn[chnndx].vang;
    packet->vang     = cnfptr->sta[standx].chn[chnndx].vang;
    packet->hang     = cnfptr->sta[standx].chn[chnndx].hang;
    packet->wrdsiz   = cnfptr->sta[standx].chn[chnndx].wrdsiz;
    packet->nsamp    = wavgen->nsamp;
    packet->order    = host_order;   /* protocol guarantees this */
    packet->type     = 0;
    packet->hlen     = 0;
    packet->dlen     = wavgen->nbyte;
    packet->header   = (char *) wavgen->data;
    packet->data     = (char *) wavgen->data;
    packet->end.time = packet->beg.time +
                     ((float) (packet->nsamp - 1) * packet->sint);
    packet->end.code = ' ';
    packet->end.qual = 0;

    return 0;
}
#endif /* NRTS_SUPPORT */

int xfer_Convert(cnf, wav, packet)
struct xfer_cnf *cnf;
struct xfer_wav *wav;
struct xfer_packet *packet;
{
INT32 *ldata;
INT16 *sdata;
struct xfer_wavgen1 *wavgen;
struct xfer_cnfgen1 *cnfptr;
int standx, chnndx;
INT32 i, nbytes;

    if (wav->format != XFER_WAVGEN1) return -1;

    cnfptr = (struct xfer_cnfgen1 *) &cnf->type.gen1;
    wavgen = (struct xfer_wavgen1 *) &wav->type.gen1;

    standx = wavgen->standx;
    chnndx = wavgen->chnndx;

    strcpy(packet->sname, cnfptr->sta[standx].name);
    packet->lat   = cnfptr->sta[standx].lat;
    packet->lon   = cnfptr->sta[standx].lon;
    packet->elev  = cnfptr->sta[standx].elev;
    packet->depth = cnfptr->sta[standx].depth;

    strcpy(packet->cname,   cnfptr->sta[standx].chn[chnndx].name);
    strcpy(packet->instype, cnfptr->sta[standx].chn[chnndx].instype);
    packet->sint   = cnfptr->sta[standx].chn[chnndx].sint;
    packet->calib  = cnfptr->sta[standx].chn[chnndx].calib;
    packet->calper = cnfptr->sta[standx].chn[chnndx].calper;
    packet->vang   = cnfptr->sta[standx].chn[chnndx].vang;
    packet->hang   = cnfptr->sta[standx].chn[chnndx].hang;
    packet->beg    = wavgen->tofs;
    packet->tear   = wavgen->tear;
    packet->nsamp  = wavgen->nsamp;

    packet->end = packet->beg + ((packet->nsamp - 1) * packet->sint);

    if (cnfptr->sta[standx].chn[chnndx].wrdsiz == 4) {
        packet->data = (INT32 *) wavgen->data;
    } else if (cnfptr->sta[standx].chn[chnndx].wrdsiz == 2) {
        nbytes = packet->nsamp * cnfptr->sta[standx].chn[chnndx].wrdsiz;
        sdata = (INT16 *) wavgen->data;
        ldata = (INT32 *) packet->dbuf;
        for (i = 0; i < packet->nsamp; i++) {
            ldata[i] = (INT32) sdata[i];
        }
        packet->data = ldata;
    } else {
        return -1;
    }

    return 0;
}

/* Revision History
 *
 * $Log: convert.c,v $
 * Revision 1.5  2015/12/04 22:31:16  dechavez
 * casts, format fixes and benign bug fixes to calm OS X compiles
 *
 * Revision 1.4  2012/02/14 20:22:33  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.3  2006/12/06 23:48:49  dechavez
 * remove 8-byte data support (not possible in principle)
 *
 * Revision 1.2  2006/11/10 06:30:26  dechavez
 * limited support for 8-byte data
 *
 * Revision 1.1.1.1  2000/02/08 20:20:42  dec
 * import existing IDA/NRTS sources
 *
 */
