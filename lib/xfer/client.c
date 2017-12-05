#pragma ident "$Id: client.c,v 1.8 2015/12/04 22:31:16 dechavez Exp $"
/*======================================================================
 * 
 * IDA data exchange protocols, client side routines.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1997 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#include <errno.h>
#include <memory.h>
#include "xfer.h"

static int slen = sizeof(UINT16);
static int llen = sizeof(UINT32);
static UINT16 sval;
static UINT32 lval;

/*============================ internal use ==========================*/

/* Round off sample interval to reduce protocol induced(!) error */

static float round_sint(float sint)
{
#ifdef DONT_DO_ROUNDOFF
    return sint;
#else
double dsintmsc;
INT32   lsintmsc;

    dsintmsc = (double) sint * (double) 1000.0;
    lsintmsc = (INT32) dsintmsc;
    if (dsintmsc - (double) lsintmsc >= (double) 0.5) lsintmsc++;
    dsintmsc = (double) lsintmsc;
    return (float) (dsintmsc / 1000.0);
#endif
}

/*  Decompress waveform data */

static int xfer_Decompress(cnf, wav)
struct xfer_cnf *cnf; /* disk loop configuration               */
struct xfer_wav *wav; /* original waveform message from server */
{
UINT32 order;
static union {
   INT16 s[XFER_MAXDAT];
   INT32 l[XFER_MAXDAT];
} buffer;
UINT8 *data, *output;
int nsamp, wrdsiz, standx, chnndx;
INT32 *nbyte;
int *comp;
IDA ida;
static char *fid = "xfer_Decompress";

/* The following formats don't have protocol compression */

    if (
        wav->format == XFER_WAVRAW    ||
        wav->format == XFER_WAVSEED   ||
        wav->format == XFER_WAVPASSCAL
    ) return XFER_OK;

/* Pull out the necessary pieces from the waveform record */

    switch (wav->format) {

      case XFER_WAVGEN1:

        standx =  wav->type.gen1.standx;
        chnndx =  wav->type.gen1.chnndx;
        nbyte  = &wav->type.gen1.nbyte;
        nsamp  =  wav->type.gen1.nsamp;
        data   =  wav->type.gen1.data;
        comp   = &wav->type.gen1.comp;

        break;

#ifdef IDA_SUPPORT
      case XFER_WAVIDA:

        if (*(comp = &wav->type.ida.comp) == XFER_CMPNONE) return XFER_OK;

        nbyte = &wav->type.ida.nbyte;
        data  =  wav->type.ida.data;

        idaInitHandle(&ida); ida.rev.value = wav->type.ida.rev;
        output = ida_decompress(&ida, comp, nbyte, data);
        idaClearHandle(&ida);

        if (output == NULL || output == data) {
            util_log(1, "%s: ida_decompress failed", fid);
            xfer_errno = XFER_EINVAL;
            return XFER_ERROR;
        }

        memcpy(data, output, *nbyte);

        return XFER_OK;
#endif /* IDA_SUPPORT */

      default:
        util_log(1, "%s: unsupported waveform format ", fid);
        xfer_errno = XFER_EFORMAT;
        return XFER_ERROR;
    }

/* Pull out the necessary pieces from the configuration record */

    switch (cnf->format) {

      case XFER_CNFGEN1:

        wrdsiz = cnf->type.gen1.sta[standx].chn[chnndx].wrdsiz;
        order  = cnf->type.gen1.sta[standx].chn[chnndx].order;
        break;

      case XFER_CNFNRTS:

        wrdsiz = cnf->type.nrts.sta[standx].chn[chnndx].wrdsiz;
        order  = cnf->type.nrts.sta[standx].chn[chnndx].order;
        break;

      default:

        util_log(1, "%s: unsupported configuration format code %d",
            fid, wav->format
        );
        xfer_errno = XFER_EFORMAT;
        return XFER_ERROR;
    }

/* Sanity checks */

	if (wrdsiz == 8) {
		*comp = XFER_CMPNONE;
	} else if (wrdsiz != 2 && wrdsiz != 4) {
        util_log(1, "%s: unexpected wrdsiz `%d'", fid, wrdsiz);
        xfer_errno = XFER_EINVAL;
        return XFER_ERROR;
    }


/* Decompress */

    switch (*comp) {

      case XFER_CMPIGPP:

        if (wrdsiz == 4) {
            util_ldcmp(buffer.l, data, nsamp);
            output = (void *) buffer.l;
        } else {
            util_sdcmp(buffer.s, data, nsamp);
            output = (void *) buffer.s;
        }
        break;

      case XFER_CMPNONE:

        if (order != util_order()) {
            if (wrdsiz == 4) {
                util_lswap((INT32 *) data, nsamp);
            } else if (wrdsiz == 8) {
                util_llswap((UINT64 *) data, nsamp);
            } else {
                util_sswap((INT16 *) data, nsamp);
            }
        }
        output = data;
        break;

      default:

        util_log(1, "%s: unrecognized compression code %d", fid, *comp);
        xfer_errno = XFER_EINVAL;
        return XFER_ERROR;
    }

    *comp  = XFER_CMPNONE;
    *nbyte = nsamp * wrdsiz;
    memcpy(data, output, *nbyte);

    return XFER_OK;
}

/* Encode a channel portion of a station portion of a waveform request */

static INT32 xfer_EncodeChnReq(start_of_message, info, protocol)
char *start_of_message;
void *info;
int protocol;
{
char *ptr;
struct xfer_time *xtime;
struct xfer01_chnreq *ver01;
static char *fid = "xfer_EncodeChnReq";

    ptr = start_of_message;

    switch (protocol) {

      case 0x01:

        ver01 = (struct xfer01_chnreq *) info;

        memcpy(ptr, ver01->name, XFER01_CNAMLEN + 1);
        ptr += XFER01_CNAMLEN + 1;

        xtime = xfer_time(ver01->beg);
        lval = htonl((UINT32) xtime->sec);
        memcpy(ptr, &lval, llen);
        ptr += llen;
        lval = htonl(xtime->usec);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        xtime = xfer_time(ver01->end);
        lval = htonl((UINT32) xtime->sec);
        memcpy(ptr, &lval, llen);
        ptr += llen;
        lval = htonl(xtime->usec);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        break;

      default:
        xfer_errno = XFER_EPROTOCOL;
        return XFER_ERROR;
    }

    return (INT32) (ptr - start_of_message);
}

/* Encode a station portion of a waveform request */

static INT32 xfer_EncodeStaReq(start_of_message, info, protocol)
char *start_of_message;
void *info;
int protocol;
{
int i;
INT32 len;
char *ptr;
struct xfer01_stareq *ver01;
static char *fid = "xfer_EncodeStaReq";

    ptr = start_of_message;

    switch (protocol) {

      case 0x01:

        ver01 = (struct xfer01_stareq *) info;

        memcpy(ptr, ver01->name, XFER01_SNAMLEN + 1);
        ptr += XFER01_SNAMLEN + 1;

        sval = htons((UINT16) ver01->nchn);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        for (i = 0; i < ver01->nchn; i++) {
            len = xfer_EncodeChnReq(ptr, (void *)(ver01->chn+i), protocol);
            if (len <= 0) return XFER_ERROR;
            ptr += len;
        }

        break;

      default:
        xfer_errno = XFER_EPROTOCOL;
        return XFER_ERROR;
    }

    return (INT32) (ptr - start_of_message);
}

/* Encode a waveform request */

static INT32 xfer_EncodeWavReq(start_of_message, req, protocol)
char *start_of_message;
union xfer_wavreq *req;
int protocol;
{
int i;
INT32 len;
char *ptr;
static char *fid = "xfer_EncodeWavReq";

    ptr = start_of_message;

    switch (protocol) {

      case 0x01:

        sval = htons((UINT16) req->ver01.format);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        sval = htons((UINT16) req->ver01.keepup);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        sval = htons((UINT16) req->ver01.comp);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        sval = htons((UINT16) req->ver01.nsta);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        for (i = 0; i < req->ver01.nsta; i++) {
            len = xfer_EncodeStaReq(ptr,(void *)(req->ver01.sta+i),protocol);
            if (len <= 0) return XFER_ERROR;
            ptr += len;
        }

        break;

      default:
        xfer_errno = XFER_EPROTOCOL;
        return XFER_ERROR;
    }

    return (INT32) (ptr - start_of_message);
}

/* Encode a request preamble */

static INT32 xfer_EncodePreamble(start_of_message, preamble, protocol)
char *start_of_message;
union xfer_preamble *preamble;
int protocol;
{
char *ptr;
static char *fid = "xfer_EncodePreamble";

    ptr = start_of_message;

    switch (protocol) {

      case 0x01:

        lval = htonl((UINT32) preamble->ver01.client_id);
        memcpy(ptr, &lval, llen);
        ptr += llen;

        sval = htons((UINT16) preamble->ver01.format);
        memcpy(ptr, &sval, slen);
        ptr += slen;

        break;

      default:
        xfer_errno = XFER_EPROTOCOL;
        return XFER_ERROR;
    }

    return (INT32) (ptr - start_of_message);
}

/* Encode an arbitrary request */

static INT32 xfer_EncodeReq(start_of_message, req)
char *start_of_message;
struct xfer_req *req;
{
INT32 len;
char *ptr;
static char *fid = "xfer_EncodeReq";

    ptr = start_of_message;

/* ALL requests include the protocol as the first two bytes */

    sval = htons((UINT16) req->protocol);
    memcpy(ptr, &sval, slen);
    ptr += slen;

/* AND followed by 4 bytes of request code */

    lval = htonl((UINT32) req->type);
    memcpy(ptr, &lval, llen);
    ptr += llen;

/* AND followed by 2 bytes of socket I/O timeout */

    sval = htons((UINT16) req->timeout);
    memcpy(ptr, &sval, slen);
    ptr += slen;

/* AND followed by 4 bytes of client send (server recv) buffer size */

    lval = htonl((UINT32) req->sndbuf);
    memcpy(ptr, &lval, llen);
    ptr += llen;

/* AND followed by 4 bytes of client recv (server send) buffer size */

    lval = htonl((UINT32) req->rcvbuf);
    memcpy(ptr, &lval, llen);
    ptr += llen;

/* Everything else that follows depends on the protocol version */

    switch (req->protocol) {

      case 0x01:

        len = xfer_EncodePreamble(ptr, &req->preamble, req->protocol);
        if (len < 0) return XFER_ERROR;
        ptr += len;

        switch (req->type) {

          case XFER_CNFREQ:
            /* nothing further is required for a configuration request */
            break;

          case XFER_WAVREQ:
            len = xfer_EncodeWavReq(ptr, &req->request.wav, req->protocol);
            if (len < 0) return XFER_ERROR;
            ptr += len;

            break;

          default:
            xfer_errno = XFER_EREQUEST;
            return XFER_ERROR;
        }

        break;

      default:
        xfer_errno = XFER_EPROTOCOL;
        return XFER_ERROR;
    }

    return (INT32) (ptr - start_of_message);
}

/* Decode a channel descriptor */

static int xfer_DecodeChnCnf(info, src, format)
void *info;
char **src;
int format;
{
INT32 ltemp;
float factor;
struct xfer_time xtime;
struct xfer_gen1chncnf *gen1;
struct xfer_nrtschncnf *nrts;
static char *fid = "xfer_DecodeChnCnf";

    switch (format) {

      case XFER_CNFGEN1:

        gen1 = (struct xfer_gen1chncnf *) info;

        memcpy(gen1->name, *src, XFER01_CNAMLEN + 1);
        *src += XFER01_CNAMLEN + 1;

        memcpy(gen1->instype, *src, XFER01_INAMLEN + 1);
        *src += XFER01_INAMLEN + 1;

        memcpy(&sval, *src, slen);
        *src += slen;
        gen1->wrdsiz = (int) ntohs(sval);

        memcpy(&lval, *src, llen);
        *src += llen;
        gen1->order = (UINT32) ntohl(lval);

        memcpy(&lval, *src, llen);
        *src += llen;
        gen1->sint = (float) (((double) ntohl(lval)) / (double) 1000000.0);
        gen1->sint = round_sint(gen1->sint);

        memcpy(&lval, *src, llen);
        *src += llen;
        factor = (float) ((INT32) ntohl(lval));

        memcpy(&lval, *src, llen);
        *src += llen;
        gen1->calib = (float) ((INT32) ntohl(lval)) / factor;

        memcpy(&lval, *src, llen);
        *src += llen;
        gen1->calper = (float) (((INT32) ntohl(lval)) / 100000.0);

        memcpy(&lval, *src, llen);
        *src += llen;
        gen1->vang = (float) (((INT32) ntohl(lval)) / 1000000.0);

        memcpy(&lval, *src, llen);
        *src += llen;
        gen1->hang = (float) (((INT32) ntohl(lval)) / 1000000.0);

        memcpy(&lval, *src, llen);
        *src += llen;
        xtime.sec = (INT32) ntohl(lval);

        memcpy(&lval, *src, llen);
        *src += llen;
        xtime.usec = (UINT32) ntohl(lval);
        gen1->beg = xfer_dtime(&xtime);

        memcpy(&lval, *src, llen);
        *src += llen;
        xtime.sec = (INT32) ntohl(lval);

        memcpy(&lval, *src, llen);
        *src += llen;
        xtime.usec = (UINT32) ntohl(lval);
        gen1->end = xfer_dtime(&xtime);

        break;

#ifdef NRTS_SUPPORT

      case XFER_CNFNRTS:

        nrts = (struct xfer_nrtschncnf *) info;

        memcpy(nrts->name, *src, NRTS_SNAMLEN + 1);
        *src += NRTS_SNAMLEN + 1;
        util_log(3, "%s: name = %s", fid, nrts->name);

        memcpy(nrts->instype, *src, NRTS_INAMLEN + 1);
        *src += NRTS_INAMLEN + 1;
        util_log(3, "%s: instype = %s", fid, nrts->instype);

        memcpy(&sval, *src, slen);
        *src += slen;
        nrts->wrdsiz = (int) ntohs(sval);
        util_log(3, "%s: wrdsiz = %d", fid, nrts->wrdsiz);

        memcpy(&lval, *src, llen);
        *src += llen;
        nrts->order = (UINT32) ntohl(lval);
        util_log(3, "%s: order = 0x%08x", fid, nrts->order);

        memcpy(&lval, *src, llen);
        *src += llen;
        nrts->sint = (float) (((INT32) ntohl(lval)) / 1000000.0);
        nrts->sint = round_sint(nrts->sint);
        util_log(3, "%s: sint = %.3f", fid, nrts->sint);

        memcpy(&lval, *src, llen);
        *src += llen;
        ltemp = (INT32) ntohl(lval);
        util_log(3, "%s: factor = %ld", fid, (float) ltemp);

        memcpy(&lval, *src, llen);
        *src += llen;
        nrts->calib = (float) ((INT32) ntohl(lval)) / (float) ltemp;
        util_log(3, "%s: calib = %.3f", fid, nrts->calib);

        memcpy(&lval, *src, llen);
        *src += llen;
        nrts->calper = (float) (((INT32) ntohl(lval)) / 100000.0);
        util_log(3, "%s: calper = %.3f", fid, nrts->calper);

        memcpy(&lval, *src, llen);
        *src += llen;
        nrts->vang = (float) (((INT32) ntohl(lval)) / 1000000.0);
        util_log(3, "%s: vang = %.3f", fid, nrts->vang);

        memcpy(&lval, *src, llen);
        *src += llen;
        nrts->hang = (float) (((INT32) ntohl(lval)) / 1000000.0);
        util_log(3, "%s: hang = %.3f", fid, nrts->hang);

        memcpy(&lval, *src, llen);
        *src += llen;
        xtime.sec = (INT32) ntohl(lval);

        memcpy(&lval, *src, llen);
        *src += llen;
        xtime.usec = (UINT32) ntohl(lval);
        nrts->beg = xfer_dtime(&xtime);
        util_log(3, "%s: beg = %s", fid, util_dttostr(nrts->beg,0));

        memcpy(&lval, *src, llen);
        *src += llen;
        xtime.sec = (INT32) ntohl(lval);

        memcpy(&lval, *src, llen);
        *src += llen;
        xtime.usec = (UINT32) ntohl(lval);
        nrts->end = xfer_dtime(&xtime);
        util_log(3, "%s: end = %s", fid, util_dttostr(nrts->end,0));

        memcpy(&sval, *src, slen);
        *src += slen;
        nrts->type = (int) ntohs(sval);
        util_log(3, "%s: type = %hd", fid, nrts->type);

        memcpy(&sval, *src, slen);
        *src += slen;
        nrts->hlen = (int) ntohs(sval);
        util_log(3, "%s: hlen = %hd", fid, nrts->hlen);

        memcpy(&sval, *src, slen);
        *src += slen;
        nrts->dlen = (int) ntohs(sval);
        util_log(3, "%s: dlen = %hd", fid, nrts->dlen);

        memcpy(&lval, *src, llen);
        *src += llen;
        nrts->nrec = (INT32) ntohl(lval);
        util_log(3, "%s: nrec = %ld", fid, nrts->nrec);

        memcpy(&lval, *src, llen);
        *src += llen;
        nrts->nhide = (INT32) ntohl(lval);
        util_log(3, "%s: nhide = %ld", fid, nrts->nhide);

        memcpy(&lval, *src, llen);
        *src += llen;
        nrts->latency = (time_t) ntohl(lval);
        util_log(3, "%s: latency = %ld", fid, nrts->latency);

        break;
#endif /* NRTS_SUPPORT */

      default:
        xfer_errno = XFER_EFORMAT;
        return XFER_ERROR;
    }

    return XFER_OK;
}

/* Decode a station descriptor */

static int xfer_DecodeStaCnf(info, src, format)
void *info;
char **src;
int format;
{
int i, retval;
struct xfer_gen1stacnf *gen1;
struct xfer_nrtsstacnf *nrts;
static char *fid = "xfer_DecodeStaCnf";

    switch (format) {

      case XFER_CNFGEN1:

        gen1 = (struct xfer_gen1stacnf *) info;

        memcpy(gen1->name, *src, XFER01_SNAMLEN + 1);
        *src += XFER01_SNAMLEN + 1;

        memcpy(&lval, *src, llen);
        *src += llen;
        gen1->lat = (float) (((INT32) ntohl(lval)) / 1000000.0);

        memcpy(&lval, *src, llen);
        *src += llen;
        gen1->lon = (float) (((INT32) ntohl(lval)) / 1000000.0);

        memcpy(&lval, *src, llen);
        *src += llen;
        gen1->elev = (float) (((INT32) ntohl(lval)) / 1000000.0);

        memcpy(&lval, *src, llen);
        *src += llen;
        gen1->depth = (float) (((INT32) ntohl(lval)) / 1000000.0);

        memcpy(&sval, *src, slen);
        *src += slen;
        gen1->nchn = (int) ntohs(sval);

        if (gen1->nchn > XFER_MAXCHN) {
            util_log(1, "%s: increase XFER_MAXCHN (%d) to %d",
                fid, XFER_MAXCHN, gen1->nchn
            );
            xfer_errno = XFER_ELIMIT;
            return XFER_ERROR;
        }

        for (i = 0; i < gen1->nchn; i++) {
            retval = xfer_DecodeChnCnf(gen1->chn + i, src, format);
            if (retval != XFER_OK) return retval;
        }

        break;

#ifdef NRTS_SUPPORT
      case XFER_CNFNRTS:

        nrts = (struct xfer_nrtsstacnf *) info;

        memcpy(nrts->name, *src, XFER01_SNAMLEN + 1);
        *src += XFER01_SNAMLEN + 1;
        util_log(3, "%s: name = %s", fid, nrts->name);

        memcpy(&lval, *src, llen);
        *src += llen;
        nrts->lat = (float) (((INT32) ntohl(lval)) / 1000000.0);
        util_log(3, "%s: lat = %.3f", fid, nrts->lat);

        memcpy(&lval, *src, llen);
        *src += llen;
        nrts->lon = (float) (((INT32) ntohl(lval)) / 1000000.0);
        util_log(3, "%s: lon = %.3f", fid, nrts->lon);

        memcpy(&lval, *src, llen);
        *src += llen;
        nrts->elev = (float) (((INT32) ntohl(lval)) / 1000000.0);
        util_log(3, "%s: elev = %.3f", fid, nrts->elev);

        memcpy(&lval, *src, llen);
        *src += llen;
        nrts->depth = (float) (((INT32) ntohl(lval)) / 1000000.0);
        util_log(3, "%s: depth = %.3f", fid, nrts->depth);

        memcpy(&sval, *src, slen);
        *src += slen;
        nrts->nchn = (int) ntohs(sval);

        if (nrts->nchn > XFER_MAXCHN) {
            util_log(1, "%s: increase XFER_MAXCHN (%d) to %d",
                fid, XFER_MAXCHN, nrts->nchn
            );
            xfer_errno = XFER_ELIMIT;
            return XFER_ERROR;
        }

        for (i = 0; i < nrts->nchn; i++) {
            retval = xfer_DecodeChnCnf(nrts->chn + i, src, format);
            if (retval != 0) return XFER_OK;
        }

        break;
#endif /* NRTS_SUPPORT */

      default:
        xfer_errno = XFER_EFORMAT;
        return XFER_ERROR;
    }

    return XFER_OK;
}

/* Decode a configuration message */

static int xfer_DecodeCnf(cnf, src)
struct xfer_cnf *cnf;
char *src;
{
int i, retval;
static char *fid = "xfer_DecodeCnf";

    memcpy(&sval, src, slen);
    src += slen;
    cnf->format = (int) ntohs(sval);

    switch (cnf->format) {

      case XFER_CNFGEN1:

        memcpy(&lval, src, llen);
        src += llen;
        cnf->type.gen1.order = (UINT32) ntohl(lval);

        memcpy(&sval, src, slen);
        src += slen;
        cnf->type.gen1.nsta = (int) ntohs(sval);

        if (cnf->type.gen1.nsta > XFER_MAXSTA) {
            util_log(1, "%s: increase XFER_MAXSTA (%d) to %d",
                fid, XFER_MAXSTA, cnf->type.gen1.nsta
            );
            xfer_errno = XFER_ELIMIT;
            return XFER_ERROR;
        }

        for (i = 0; i < cnf->type.gen1.nsta; i++) {
            retval = xfer_DecodeStaCnf(
                cnf->type.gen1.sta + i, &src, cnf->format
            );
            if (retval != XFER_OK) return retval;
        }

        break;

#ifdef NRTS_SUPPORT
      case XFER_CNFNRTS:

        memcpy(&lval, src, llen);
        src += llen;
        cnf->type.nrts.order = (UINT32) ntohl(lval);

        memcpy(&sval, src, slen);
        src += slen;
        cnf->type.nrts.nsta = (int) ntohs(sval);

        if (cnf->type.nrts.nsta > XFER_MAXSTA) {
            util_log(1, "%s: increase XFER_MAXSTA (%d) to %d",
                fid, XFER_MAXSTA, cnf->type.nrts.nsta
            );
            xfer_errno = XFER_ELIMIT;
            return XFER_ERROR;
        }

        for (i = 0; i < cnf->type.nrts.nsta; i++) {
            retval = xfer_DecodeStaCnf(
                cnf->type.nrts.sta + i, &src, cnf->format
            );
            if (retval != XFER_OK) return retval;
        }

        break;
#endif /* NRTS_SUPPORT */

      default:
        xfer_errno = XFER_EFORMAT;
        return XFER_ERROR;
    }

    return XFER_OK;
}

/* Receive a remote configuration */

int xfer_RecvCnf(sd, cnf)
int sd;
struct xfer_cnf *cnf;
{
int retval;
INT32 len;
static char *fid = "xfer_RecvCnf";

   retval = xfer_RecvMsg(sd, Xfer_Buffer, Xfer_Buflen, &len);
   if (retval != XFER_OK) return XFER_ERROR;

/* If a break (zero length message) was received, follow up with
 * a second read to get the cause code.
 */

    if (len == 0) {
        retval = xfer_RecvMsg(sd, Xfer_Buffer, Xfer_Buflen, &len);
        if (retval != XFER_OK) return XFER_ERROR;
        memcpy(&lval, Xfer_Buffer, llen);
        xfer_errno = (int) ntohl(lval);
        return XFER_ERROR;
    }

/* Otherwise decode the presumably OK configuration message */

    retval = xfer_DecodeCnf(cnf, Xfer_Buffer);
    return retval;
}

/* Decode a waveform packet */

int xfer_DecodeWav(cnf, wav, src)
struct xfer_cnf *cnf;
struct xfer_wav *wav;
char *src;
{
struct xfer_time xtime;
static char *fid = "xfer_DecodeWav";
#ifdef DEBUG
static char buf[1024];
#endif

    memcpy(&sval, src, slen);
    src += slen;
    wav->format = (int) ntohs(sval);

    switch (wav->format) {

      case XFER_WAVGEN1:

        memcpy(&sval, src, slen);
        src += slen;
        wav->type.gen1.standx = (int) ntohs(sval);

        memcpy(&sval, src, slen);
        src += slen;
        wav->type.gen1.chnndx = (int) ntohs(sval);

        memcpy(&lval, src, llen);
        src += llen;
        xtime.sec = (INT32) ntohl(lval);

        memcpy(&lval, src, llen);
        src += llen;
        xtime.usec = (UINT32) ntohl(lval);
        wav->type.gen1.tofs = xfer_dtime(&xtime);

        memcpy(&sval, src, slen);
        src += slen;
        wav->type.gen1.tear = (int) ntohs(sval);

        memcpy(&sval, src, slen);
        src += slen;
        wav->type.gen1.comp = (int) ntohs(sval);

        memcpy(&lval, src, llen);
        src += llen;
        wav->type.gen1.nsamp = (INT32) ntohl(lval);

        memcpy(&lval, src, llen);
        src += llen;
        wav->type.gen1.nbyte = (INT32) ntohl(lval);

        if (wav->type.gen1.nbyte > XFER_MAXDAT) {
            util_log(1, "%s: increase XFER_MAXDAT (%d) to %d",
                fid, XFER_MAXDAT, wav->type.gen1.nbyte
            );
            xfer_errno = XFER_ELIMIT;
            return XFER_ERROR;
        }

        if (wav->type.gen1.nbyte < 0) {
            util_log(1, "%s: illegal gen1.nbyte %d",
                fid, wav->type.gen1.nbyte
            );
            xfer_errno = XFER_EINVAL;
            return XFER_ERROR;
        }

        memcpy(wav->type.gen1.data, src, wav->type.gen1.nbyte);

        break;

      case XFER_WAVRAW:

        memcpy(&lval, src, llen);
        src += llen;
        wav->type.raw.nbyte = (INT32) ntohl(lval);

        if (wav->type.raw.nbyte > XFER_MAXDAT) {
            util_log(1, "%s: increase XFER_MAXDAT (%d) to %d",
                fid, XFER_MAXDAT, wav->type.raw.nbyte
            );
            xfer_errno = XFER_ELIMIT;
            return XFER_ERROR;
        }

        if (wav->type.raw.nbyte < 0) {
            util_log(1, "%s: illegal raw.nbyte %d",
                fid, wav->type.raw.nbyte
            );
            xfer_errno = XFER_EINVAL;
            return XFER_ERROR;
        }

        memcpy(wav->type.raw.data, src, wav->type.raw.nbyte);

        break;

#ifdef IDA_SUPPORT
      case XFER_WAVIDA:

        memcpy(&sval, src, slen);
        src += slen;
        wav->type.ida.rev = (int) ntohs(sval);

        memcpy(&sval, src, slen);
        src += slen;
        wav->type.ida.comp = (int) ntohs(sval);

        memcpy(&lval, src, llen);
        src += llen;
        wav->type.ida.nbyte = (INT32) ntohl(lval);

        if (wav->type.ida.nbyte > XFER_MAXDAT) {
            util_log(1, "%s: increase XFER_MAXDAT (%d) to %d",
                fid, XFER_MAXDAT, wav->type.ida.nbyte
            );
            xfer_errno = XFER_ELIMIT;
            return XFER_ERROR;
        }

        if (wav->type.ida.nbyte < 0) {
            util_log(1, "%s: illegal ida.nbyte %d",
                fid, wav->type.ida.nbyte
            );
            xfer_errno = XFER_EINVAL;
            return XFER_ERROR;
        }
#ifdef DEBUG
sprintf(buf, "%s: %d %d %ld: ", fid, wav->type.ida.rev, wav->type.ida.comp, wav->type.ida.nbyte);
for (i = 0; i < 80; i++) sprintf(buf+strlen(buf), "%c", src[i] >= 0x20 && src[i] <= 0x7e ? src[i] : '.');
util_log(1, buf);
#endif
        memcpy(wav->type.ida.data, src, wav->type.ida.nbyte);

        break;
#endif /* IDA_SUPPORT */

#ifdef SEED_SUPPORT
      case XFER_WAVSEED:
        memcpy(&lval, src, llen);
        src += llen;
        wav->type.seed.nbyte = (INT32) ntohl(lval);

        if (wav->type.seed.nbyte > SEED_PAKLEN) {
            util_log(1, "%s: increase SEED_PAKLEN (%d) to %d",
                fid, SEED_PAKLEN, wav->type.seed.nbyte
            );
            xfer_errno = XFER_ELIMIT;
            return XFER_ERROR;
        }

        if (wav->type.seed.nbyte < 0) {
            util_log(1, "%s: illegal seed.nbyte %d",
                fid, wav->type.seed.nbyte
            );
            xfer_errno = XFER_EINVAL;
            return XFER_ERROR;
        }

        memcpy(wav->type.seed.data, src, wav->type.seed.nbyte);

        break;
#endif /* SEED_SUPPORT */

#ifdef PASSCAL_SUPPORT
      case XFER_WAVPASSCAL:
        memcpy(&lval, src, llen);
        src += llen;
        wav->type.pas.nbyte = (INT32) ntohl(lval);

        if (wav->type.pas.nbyte > PASSCAL_PAKLEN) {
            util_log(1, "%s: increase PASSCAL_PAKLEN (%d) to %d",
                fid, PASSCAL_PAKLEN, wav->type.pas.nbyte
            );
            xfer_errno = XFER_ELIMIT;
            return XFER_ERROR;
        }

        if (wav->type.pas.nbyte < 0) {
            util_log(1, "%s: illegal pas.nbyte %d",
                fid, wav->type.pas.nbyte
            );
            xfer_errno = XFER_EINVAL;
            return XFER_ERROR;
        }

        memcpy(wav->type.pas.data, src, wav->type.pas.nbyte);

        break;
#endif /* PASSCAL_SUPPORT */

      default:
        util_log(1, "%s: unsupported waveform format code %d",
            fid, wav->format
        );
        xfer_errno = XFER_EFORMAT;
        return XFER_ERROR;
    }

    return xfer_Decompress(cnf, wav);
}

/* Send an arbitrary request */

int xfer_SendReq(sd, req)
int sd;
struct xfer_req *req;
{
INT32 len;
static char *fid = "xfer_SendReq";

    len = xfer_EncodeReq(Xfer_Buffer+4, req);
    if (len < 0) return XFER_ERROR;

    return xfer_SendMsg(sd, Xfer_Buffer, len);
}

/*============================ external use ==========================*/

/* Receive a waveform packet */

int Xfer_RecvWav(sd, cnf, wav)
int sd;
struct xfer_cnf *cnf;
struct xfer_wav *wav;
{
int retval = XFER_OK;
INT32 len = -1;
static char *fid = "Xfer_RecvWav";

    while (1) {

        retval = xfer_RecvMsg(sd, Xfer_Buffer, Xfer_Buflen, &len);
        if (retval != XFER_OK) {
            return XFER_ERROR;
        }

        if (len > 0) return xfer_DecodeWav(cnf, wav, Xfer_Buffer);

    /* If a break (zero length message) was received, follow up with
     * a second read to get the cause code.
     */
        retval = xfer_RecvMsg(sd, Xfer_Buffer, Xfer_Buflen, &len);
        if (retval != XFER_OK) {
            return XFER_ERROR;
        }
        memcpy(&lval, Xfer_Buffer, llen);
        retval = (int) ntohl(lval);

        if (retval == XFER_FINISHED) {
            xfer_errno = 0;
            return retval;
        } else if (retval != XFER_HEARTBEAT) {
            xfer_errno = retval;
            return XFER_ERROR;
        } else {
            util_log(2, "HEARTBEAT received");
        }
    }
}

/* Revision History
 *
 * $Log: client.c,v $
 * Revision 1.8  2015/12/04 22:31:16  dechavez
 * casts, format fixes and benign bug fixes to calm OS X compiles
 *
 * Revision 1.7  2012/02/14 20:22:32  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.6  2006/11/10 06:30:26  dechavez
 * limited support for 8-byte data
 *
 * Revision 1.5  2006/02/09 20:01:48  dechavez
 * IDA handle support
 *
 * Revision 1.4  2005/05/25 22:41:56  dechavez
 * mods to calm Visual C++ warnings
 *
 * Revision 1.3  2004/09/28 22:49:47  dechavez
 * improved error handling
 *
 * Revision 1.2  2000/07/24 19:17:36  dec
 * fixed typo in round_sint and changed #defines so that it is invoked
 *
 * Revision 1.1.1.1  2000/02/08 20:20:42  dec
 * import existing IDA/NRTS sources
 *
 */
