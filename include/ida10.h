#pragma ident "$Id: ida10.h,v 1.59 2018/01/18 23:29:35 dechavez Exp $"
/*======================================================================
 *
 *  IDA rev 10 include file.
 *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Copyright (c) 1999 Regents of the University of California.
 * All rights reserved.
 *====================================================================*/
#ifndef ida10_h_included
#define ida10_h_included

#include "platform.h"
#include "sanio.h" /* for the SANIO_TT_STAT_XXX constants */
#include "util.h"  /* for the swap macros */
#include "qdp.h"   /* for the Q330 epoch time macro */
#include "ida.h"   /* for IDA_EXTRA */
#include "mseed.h" /* for MSEED 512-byte IDA10.12 payloads */

#ifdef __cplusplus
extern "C" {
#endif

/*  Record types  */

#define IDA10_TYPE_UNKNOWN  0x0000
#define IDA10_TYPE_TS       0x0001
#define IDA10_TYPE_CA       0x0002
#define IDA10_TYPE_CF       0x0004
#define IDA10_TYPE_LM       0x0008
#define IDA10_TYPE_ISPLOG   0x8000
#define IDA10_TYPE_ALL      0xffff

/* Format codes */

#define IDA10_FORMAT_CODE       10

#define IDA10_SUBFORMAT_0        0
#define IDA10_SUBFORMAT_1        1
#define IDA10_SUBFORMAT_2        2
#define IDA10_SUBFORMAT_3        3
#define IDA10_SUBFORMAT_4        4
#define IDA10_SUBFORMAT_5        5
#define IDA10_SUBFORMAT_6        6
#define IDA10_SUBFORMAT_7        7
#define IDA10_SUBFORMAT_8        8
#define IDA10_SUBFORMAT_9        9
#define IDA10_SUBFORMAT_10      10
#define IDA10_SUBFORMAT_11      11
#define IDA10_SUBFORMAT_12      12
#define IDA10_MAX_SUBFORMAT     IDA10_SUBFORMAT_12

#define ida10FormatCode(RawPacket)    (RawPacket[2])
#define ida10SubFormatCode(RawPacket) (RawPacket[3])

/* Misc lengths */

#define IDA10_ISPLOGLEN   1024 /* fixed forever */
#define IDA10_FIXEDHDRLEN   64 /* fixed header length */
#define IDA10_DEFDATALEN   960 /* default data length */
#define IDA10_FIXEDRECLEN (IDA10_FIXEDHDRLEN + IDA10_DEFDATALEN)
#define IDA10_CNAMLEN        6 /* max channel name length (less NULL) */
#define IDA10_SNAMLEN        6 /* max station name length (less NULL) */
#define IDA10_NNAMELEN       2 /* max network name length (less NULL) */

/* The following is used for implementation convenience.  It is not
 * part of the IDA 10.x format specification.  Increase as desired
 * to support larger records if needed.
 */

#define IDA10_MAXDATALEN 16320  /* max size of data region, all types   */

/* The following header lengths "happen" to be the same for all
 * subformats currently defined.  Define new subformats with
 * different lengths and you will need to modify the code that makes
 * use of these constants (see unpack.c).  Better to just not do it.
 */

#define IDA10_CMNHEADLEN  50
#define IDA10_TSHEADLEN   (IDA10_CMNHEADLEN + 14)
#define IDA10_LMHEADLEN   (IDA10_CMNHEADLEN +  2)
#define IDA10_CFHEADLEN   (IDA10_CMNHEADLEN +  4)

#define IDA10_MAXHEADLEN  IDA10_TSHEADLEN
#define IDA10_MAXRECLEN   (IDA10_MAXHEADLEN + IDA10_MAXDATALEN)

#define IDA10_FIXED_NBYTES ((IDA10_FIXEDHDRLEN - IDA10_CMNHEADLEN) + IDA10_DEFDATALEN)

#define ida10PacketLength(cmn) ((cmn)->nbytes + IDA10_CMNHEADLEN)

/* various function return codes */

#define IDA10_OK        0
#define IDA10_EINVAL   -1
#define IDA10_ESRATE   -2
#define IDA10_EMSGSIZE -3
#define IDA10_IOERR    -4
#define IDA10_DATAERR  -5
#define IDA10_EOF      -6
#define IDA10_ETTAG    -7

/* Timing system identifier */

#define IDA10_TIMER_SAN     0
#define IDA10_TIMER_OFIS    1
#define IDA10_TIMER_Q330    2
#define IDA10_TIMER_GENERIC 3
#define IDA10_TIMER_IDA9    4
#define IDA10_TIMER_OBS     5
#define IDA10_TIMER_OBS2    6
#define IDA10_TIMER_SEED    7

/* SAN time tag */

typedef struct {
    UINT16 raw;     /* raw status from digitizer, decoded as follows */
    UINT8 receiver; /* receiver specific status */
    UINT8 type;     /* receiver type */
    BOOL init;      /* FALSE until first good time string has received */
    BOOL pps;       /* TRUE if external PPS present */
    BOOL pll;       /* TRUE if hardware PLL enabled */
    BOOL avail;     /* TRUE if external time string was read */
    BOOL rtt_fail;  /* TRUE if RTT counter failure */
    BOOL suspect;   /* TRUE if time might be bogus */
    BOOL locked;    /* TRUE if locked (4 or more SV) */
} IDA10_SAN_CLOCK_STATUS;

typedef struct {
    UINT32 ext; /* GPS seconds since 1/1/1999 */
    struct {
        UINT32 sec;
        UINT16 msc;
    } sys, epoch;
    UINT16 pll;      /* current input to the PLL */
    UINT16 phase;    /* difference between 1Hz and GPS PPS, 1000/51200 secs */
    IDA10_SAN_CLOCK_STATUS status;
} IDA10_SANTTAG;

/* OFIS time tag */

#define OFIS_TT_STAT_EXTPPS_AVAIL    0x0001  /* external PPS present */
#define OFIS_TT_STAT_EXTTIME_AVAIL   0x0002  /* external time available (1) */
#define OFIS_TT_STAT_EXTTIME_NOINIT  0x0004  /* if set,a good time string has not yet been received*/
#define OFIS_TT_STAT_SUSPICIOUS_GPS  0x0008  /* GPS time may be bogus */
#define OFIS_TT_STAT_GPS_CODE(s)     (((s) >> 4) & 0xf) /* 4 bit time code directly from GPS receiver */
#define OFIS_TT_STAT_EXTTIME_QUALITY 0x0100  /* 0 = < 4 SV's (or unlocked), 1=>= 4 SVs (or locked)*/
#define OFIS_TT_STAT_LEAPSEC         0x0200  /* leap second in progress */

typedef struct {
    UINT16 raw;     /* raw status from digitizer, decoded as follows */
    UINT8 receiver; /* receiver specific status */
    BOOL init;      /* FALSE until first good time string has received */
    BOOL pps;       /* TRUE if external PPS present */
    BOOL avail;     /* TRUE if external time string was read */
    BOOL suspect;   /* TRUE if time might be bogus */
    BOOL locked;    /* TRUE if locked (4 or more SV) */
    BOOL leapsec;   /* TRUE if leap second in progress */
} IDA10_OFIS_CLOCK_STATUS;

typedef struct {
    UINT32 ext; /* GPS seconds since 1/1/1999 */
    UINT64 sys; /* system time, 10 usec tics */
    UINT64 pps; /* PPS time, 10 usec tics */
    IDA10_OFIS_CLOCK_STATUS status; /* clock status */
} IDA10_OFISTAG;

/* Q330 time tag */

typedef struct {
    UINT32 seqno;      /* DT_DATA record sequence number */
    struct {
        INT32 sec;     /* seconds offset */
        INT32 usec;    /* microseconds offset */
        INT32 index;   /* nanosecond index offset */
    } offset;
    INT32 delay;       /* filter delay in microseconds */
    UINT16 loss;       /* lock time */
    struct {
        UINT8 bitmap;  /* clock quality bitmap */
        UINT8 percent; /* Quanterra clock quality percentage */
    } qual;
} IDA10_Q330TAG;

/* Generic time tag */

typedef struct {
    UINT64 tstamp;  /* nanoseconds since 1/1/1999 */
    struct {
        UINT8 receiver; /* receiver specific clock status */
        UINT8 generic;  /* device independent status bitmap */
#define IDA10_GENTAG_LOCKED     0x01
#define IDA10_GENTAG_SUSPICIOUS 0x02
#define IDA10_GENTAG_DERIVED    0x04
    } status;
} IDA10_GENTAG;

/* OBS time tag */

typedef struct {
    UINT64 imei;   /* Iridium modem identifier */
    UINT16 momsn;  /* SBD message sequence number */
    UINT32 tstamp; /* SBD message time stamp (sec since 1/1/1970) */
} IDA10_SBDHDR;

typedef struct {
    UINT64 sys;       /* datum time stamp in system tics */
    UINT64 ref;       /* nanoseconds since 1/1/1999 when sys=0 */
    UINT32 ticrate;   /* tics per second */
    IDA10_SBDHDR sbd; /* SBD header excerpt */
} IDA10_OBSTAG;

typedef struct {
    UINT64 sys;       /* datum time stamp in system tics */
    UINT32 ref;       /* seconds since 1/1/1999 when sys=0 */
    INT32  delay;     /* filter delay, in microseconds */
    UINT32 ticrate;   /* tics per second */
    IDA10_SBDHDR sbd; /* SBD header excerpt */
} IDA10_OBSTAG2;

/* Timetag pulled from SEED FSDH and blockette 1001 */

typedef struct {
    UINT64 tstamp;  /* nanoseconds since 1/1/1999 */
    struct {
        INT8 percent;  /* blockette 1001 percentage, or -1 */
#define IDA10_TT_SEED_LOCKED     0x01
#define IDA10_TT_SEED_SUSPICIOUS 0x02
        UINT8 bitmap;  /* status bitmap */
    } status;
} IDA10_SEEDTAG;

/* Device independent clock status */

typedef struct {
    UINT8 receiver; /* receiver specific status */
    BOOL init;      /* FALSE until first good time string has received */
    BOOL avail;     /* TRUE if external time string was read */
    BOOL suspect;   /* TRUE if time might be bogus */
    BOOL locked;    /* TRUE if locked */
    BOOL derived;   /* TRUE if derived from other sources (eg, previous packet) */
    int  percent;   /* if positive then GPS clock quality as a percentage */
} IDA10_CLOCK_STATUS;

/* 10.[0-13] time tag  */

#define Q330_TO_SAN_EPOCH (QDP_EPOCH_TO_1970_EPOCH - SAN_EPOCH_TO_1970_EPOCH)

typedef struct {
    int type; /* IDA10_TIMER_SAN or IDA10_TIMER_OFIS */
    IDA10_SANTTAG san;  /* raw data from SAN time tag */
    IDA10_OFISTAG ofis; /* raw data from OFIS time tag */
    IDA10_Q330TAG q330; /* raw data from Q330 time tag */
    IDA10_GENTAG gen;   /* raw data from generic time tag */
    IDA10_OBSTAG obs;   /* raw data from OBS time tag */
    IDA10_OBSTAG2 obs2; /* raw data from OBS time tag, including filter delay */
    IDA10_SEEDTAG seed; /* raw data from SEED FSDH + blockette 1001 */
    IDA_TIME_TAG ida9;  /* raw data from IDA9 time tag */
    /* the following are derived from the raw data above */
    UINT64 ext;    /* external time in nanosec since SAN epoch (1999) */
    UINT64 sys;    /* system time in nanosec */
    UINT64 hz;     /* 1-Hz time in nanosec */
    UINT64 offset; /* eternal time minus 1-Hz time */
    REAL64 epoch;  /* if ok, 1999 epoch time */
    UINT64 nepoch; /* epoch time as nanoseconds */
    BOOL derived;  /* TRUE if this is a derived time (not read from a packet) */
    IDA10_CLOCK_STATUS status; /* device independent external time status */
} IDA10_TTAG;

/* This is used for storing history in ida10DumpTimeTag() */

typedef struct {
    UINT64 count;
    UINT64 offset;
    IDA10_TTAG ttag;
    UINT32 flags;
} IDA10_TTAG_HISTORY;

/* 10.[0-12] common header */

#define IDA10_PREAMBLE_LEN        4
#define IDA10_SUBFORMAT_0_HDRLEN  50
#define IDA10_SUBFORMAT_1_HDRLEN  50
#define IDA10_SUBFORMAT_2_HDRLEN  50
#define IDA10_SUBFORMAT_3_HDRLEN  50
#define IDA10_SUBFORMAT_4_HDRLEN  50
#define IDA10_SUBFORMAT_5_HDRLEN  50
#define IDA10_SUBFORMAT_6_HDRLEN  50
#define IDA10_SUBFORMAT_7_HDRLEN  50
#define IDA10_SUBFORMAT_8_HDRLEN  50
#define IDA10_SUBFORMAT_10_HDRLEN 50
#define IDA10_SUBFORMAT_11_HDRLEN 50
#define IDA10_SUBFORMAT_12_HDRLEN 50
#define IDA10_CMNHDRLEN           50 /* because are all "happen to be" the same size */

#define IDA101_RESERVED_BYTES 20
#define IDA102_RESERVED_BYTES 12
#define IDA103_RESERVED_BYTES 12
#define IDA104_RESERVED_BYTES  0
#define IDA105_RESERVED_BYTES 28
#define IDA106_RESERVED_BYTES  8
#define IDA107_RESERVED_BYTES  8
#define IDA108_RESERVED_BYTES (IDA105_RESERVED_BYTES - 8)
#define IDA1010_RESERVED_BYTES 2
#define IDA1011_RESERVED_BYTES 2
#define IDA1012_RESERVED_BYTES 11

#define IDA105_SNAME_LEN 4
#define IDA105_NNAME_LEN 2
#define IDA108_SNAME_LEN IDA105_SNAME_LEN
#define IDA108_NNAME_LEN IDA105_NNAME_LEN
#define IDA1012_SNAME_LEN 5
#define IDA1012_NNAME_LEN IDA108_NNAME_LEN

#define IDA10_IDENT_LEN 4

/* The following have meaning only for TS records */

#define IDA10_MAXSTREAMS    128  /* max number of unique streams per sta */

/* Masks for the 10.[0-3] TS format/status descriptor */

#define IDA10_MASK_COMP     0x03 /* - - - - - - 1 1 */
#define IDA10_MASK_A2DSIZE  0x0c /* - - - - 1 1 - - */
#define IDA10_MASK_TYPE     0x30 /* - - 1 1 - - - - */
#define IDA10_MASK_TRIG     0x40 /* - 1 - - - - - - */
#define IDA10_MASK_CAL      0x80 /* 1 - - - - - - - */

/*  Values for the 10.[0-3] TS compression codes */

#define IDA10_COMP_NONE   0
#define IDA10_COMP_IGPP   1
#define IDA10_COMP_STEIM1 2
#define IDA10_COMP_STEIM2 3

/* Values for 10.[0-3] TS digitizer word sizes */

#define IDA10_A2D_24   (0 << 2)
#define IDA10_A2D_16   (1 << 2)
#define IDA10_A2D_8    (2 << 2)
#define IDA10_A2D_32   (3 << 2)

/* Masks and enums for 10.[0-3] TS output sample type */

#define IDA10_MASK_INT32    (0 << 4)
#define IDA10_MASK_INT16    (1 << 4)
#define IDA10_MASK_INT8     (2 << 4) /* 10.0, 10.1, 10.2 */
#define IDA10_MASK_REAL64   (2 << 4) /* 10.3 */
#define IDA10_MASK_REAL32   (3 << 4)

#define IDA10_DATA_TYPE_ERR -1
#define IDA10_DATA_UNKNOWN  0
#define IDA10_DATA_INT32    1
#define IDA10_DATA_INT16    2
#define IDA10_DATA_INT8     3
#define IDA10_DATA_REAL64   4
#define IDA10_DATA_REAL32   5
#define IDA10_DATA_MSEED512 6

/* Values for 10.[0-3] TS trigger flag */

#define IDA10_CONT       (0 << 6)
#define IDA10_TRIG       (1 << 6)

/* Values for 10.[0-3] TS calibration flag */

#define IDA10_CAL_OFF    (0 << 7)
#define IDA10_CAL_ON     (1 << 7)

/* TS record */

typedef struct {

    int type;           /* IDA10_TYPE_xx */
    int format;         /* raw packet format identifier (should always be 10) */
    int subformat;      /* raw packet subformat identifer (0 - 3 currently supported) */
    UINT16 boxid;       /* if non-zero, original IDA10 box id */
    UINT64 serialno;    /* if non-zero 64-bit box id (eg, Q330 serial number) */
    char   ident[IDA10_IDENT_LEN+1]; /* either the 16-bit box id, low order bits of 64-bit serial number, or sname */
    struct {
        IDA10_TTAG beg; /* time stamp at begining of packet */
        IDA10_TTAG end; /* time stamp at end of packet */
    } ttag;
    int nbytes;         /* number of bytes to follow */

    REAL64 beg;         /* 1970 epoch time, derived from ttag */
    REAL64 end;         /* 1970 epoch time, derived from ttag */

    IDA_EXTRA extra;    /* optional seqno and timestamp */

} IDA10_CMNHDR;

typedef struct {
        char strm[IDA10_CNAMLEN+1]; /* stream name              */
        UINT8 format;               /* format/status descriptor */
        UINT8 gain;                 /* conversion gain          */
        UINT16 nsamp;               /* number of samples        */
        struct {
            INT16 factor;           /* sample rate factor       */
            INT16 multiplier;       /* sample rate multiplier   */
        } srate;
} IDA10_TSHDR_DL;

typedef struct {

    IDA10_CMNHDR cmn;               /* common header */

    /* This holds the raw information from the packet */

    IDA10_TSHDR_DL dl;

    /* These are derived from the above */

    char sname[IDA10_SNAMLEN+1];    /* station name */
    char cname[IDA10_CNAMLEN+1];    /* channel name (chnlc) */
    double tofs;                    /* time of first sample */
    double tols;                    /* time of last  sample */
    double sint;                    /* sample interval, seconds */
    UINT64 nsint;                   /* sample interval, nanoseconds */
    double srate;                   /* 1.0 / sint */
    int gain;                       /* conversion gain factor */
    int nsamp;                      /* number of data samples */
    int nbytes;                     /* number of data bytes */
    int unused;                     /* short record detection */
    int datatype;                   /* data type (IDA10_DATA_xx) */

    /* This is derived via comparison with other packets */

    struct {
        INT64 nsec;   /* time stamp increment error, nanoseconds */
        INT64 samp;   /* time stamp increment error, samples */
        BOOL ok;      /* TRUE if acceptable to ignore */
        BOOL fresh;   /* TRUE when clock ETA toggles from off to on */
    } incerr;

} IDA10_TSHDR;

#define IDA10_MAXNSAMP_INT8   (IDA10_MAXDATALEN / sizeof(INT8))
#define IDA10_MAXNSAMP_INT16  (IDA10_MAXDATALEN / sizeof(INT16))
#define IDA10_MAXNSAMP_INT32  (IDA10_MAXDATALEN / sizeof(INT32))
#define IDA10_MAXNSAMP_REAL32 (IDA10_MAXDATALEN / sizeof(REAL32))
#define IDA10_MAXNSAMP_REAL64 (IDA10_MAXDATALEN / sizeof(REAL64))

typedef union {
    INT8 int8[IDA10_MAXNSAMP_INT8];
    INT16 int16[IDA10_MAXNSAMP_INT16];
    INT32 int32[IDA10_MAXNSAMP_INT32];
    REAL32 real32[IDA10_MAXNSAMP_REAL32];
    REAL64 real64[IDA10_MAXNSAMP_REAL64];
} IDA10_TSDAT;

typedef struct {
    IDA10_TSHDR hdr;
    IDA10_TSDAT data;
} IDA10_TS;

/* LM record */

typedef struct {
    IDA10_CMNHDR cmn;            /* common header */
    char text[IDA10_MAXDATALEN]; /* null terminated ascii text    */

    /* derived from the common header */

    double tofs;                 /* time of first message (string) */
    double tols;                 /* time of last  message (string) */
    char sname[IDA10_SNAMLEN+1]; /* station name  */
} IDA10_LM;

/* CF record */

typedef struct {
    IDA10_CMNHDR cmn;            /* common header                 */
    int seqno;                   /* sequence number               */
    int done;                    /* done flag                     */
    int nbytes;                  /* number of bytes in data array */
    char data[IDA10_MAXDATALEN]; /* the data                      */

    /* derived from the common header */

    double tstamp;               /* time stamp    */
    char sname[IDA10_SNAMLEN+1]; /* station name  */
} IDA10_CF;

/* These come from the SAN20xx code (idadsp.h) */

#define TT_STAT_EXTPPS_AVAIL    1    /* external PPS present */
#define TT_STAT_PLL_ENABLE      2    /* PLL is enabled (1) or not */
#define TT_STAT_EXTTIME_AVAIL   4    /* external time available (1) */
#define TT_STAT_EXTTIME_QUALITY 8    /* 0 = < 4 SV's (or unlocked), 1 = >= 4 SV's (or locked) */
#define TT_STAT_EXTTIME_NOINIT  0x10 /* if set, a good time string has not yet been received */
#define TT_STAT_RTT_FAILURE     0x20 /* when set, indicates that RTT counter not synchronized to PPS */
#define TT_STAT_TYPEMASK        0xc0
#define TT_STAT_EXTTIME_TYPE(s) (((s) & TT_STAT_TYPEMASK) >> 6)
#define GPS_NMEA 0
#define GPS_TSIP 1
#define GPS_UNK0 2
#define GPS_UNK1 3
#define TT_STAT_SETTYPE(t)    (((t) << 6) & TT_STAT_TYPEMASK)
#define NGPSTYPE 4
#define TT_STAT_SUSPICIOUS_GPS    0x100    /* GPS time may be bogus */
#define TT_STAT_RCVR_SPECIFIC(s) ((s) << 9)
#define GPS_SUSPICIOUS_TIME    0x8000

/* For renaming things */

typedef struct {
    char *old;      /* original name */
    char *new;      /* replacement name */
} IDA10_NAME_MAP;

/* Function prototypes */

/* application supplied */
IDA *ida10AppSuppliedGetIdaHandle(void);

/* misc.c */
int ida10SeqnoOffset(UINT8 *packet);
int ida10TstampOffset(UINT8 *packet);
BOOL ida10InsertSeqno32(UINT8 *packet, UINT32 seqno);
BOOL ida10HaveOrigSeqno(IDA10_CMNHDR *hdr);

/* mseed.c */
BOOL ida10ToMseed(UINT8 *src, int srclen, UINT32 seqno, char qcode, char *sta, char *net, LISS_MSEED_HDR *dest);

/* pack.c */
int ida10PackTSHdr(UINT8 *start, IDA10_TSHDR *tshdr);
int ida10PackTS(UINT8 *start, IDA10_TS *ts);

/* sint.c */
void ida10SintToFactMult(REAL64 sint, INT16 *fact, INT16 *mult);
REAL64 ida10FactMultToSint(INT16 fact, INT16 mult);

/* read.c */
int ida10ReadRecord(FILE *fp, UINT8 *buffer, int buflen, int *pType, BOOL TSheaders);
int ida10ReadGz(gzFile *gz, UINT8 *buffer, int buflen, int *pType, BOOL TSheaders);

/* rename.c */
char *ida10Cname(char *cname, LNKLST *map);
int ida10ReplaceCname(UINT8 *payload, LNKLST *map);
int ida10ReplaceSname(UINT8 *payload, char *new);
LNKLST *ida10BuildChannelMap(char *input);

/* reorder.c */
VOID ida10ReorderData(IDA10_TS *ts);

/* string.c */
char *ida10ErrorString(int code);
char *ida10DataTypeString(IDA10_TSHDR *hdr);
char *ida10CMNHDRtoString(IDA10_CMNHDR *hdr, char *msgbuf);
char *ida10TSHDRtoString(IDA10_TSHDR *hdr, char *msgbuf);
char *ida10TStoString(IDA10_TS *ts, char *msgbuf);
char *ida10TtagToString(IDA10_TTAG *ttag, char *msgbuf);
char *ida10PacketString(UINT8 *raw, char *msgbuf);

/* unpack.c */
int ida10PackGenericTtag(UINT8 *start, IDA10_GENTAG *gen);
int  ida10UnpackCmnHdr(UINT8 *start, IDA10_CMNHDR *out);
int  ida10UnpackTtag(UINT8 *start, IDA10_TTAG *out);
int  ida10UnpackTSHdr(UINT8 *start, IDA10_TSHDR *out);
int  ida10Type(UINT8 *raw);
BOOL ida10UnpackTS(UINT8 *start, IDA10_TS *out);
BOOL ida10UnpackLM(UINT8 *start, IDA10_LM *out);
BOOL ida10UnpackCF(UINT8 *start, IDA10_CF *out);

/* ttag.c */
INT64 ida10SanPhaseToNanosec(UINT16 phase);
void ida10CompleteTimeTag(IDA10_TTAG *ttag);
void ida10SetEndTime(IDA10_CMNHDR *cmnhdr, UINT64 duration);
int ida10TtagIncrErr(IDA10_TSHDR *phdr, IDA10_TSHDR *chdr, int *sign, UINT64 *errptr);
int ida10ClockQuality(IDA10_TTAG *ttag);
void ida10PrintTSttag(FILE *fp, IDA10_TS *ts, IDA10_TTAG_HISTORY *history);
void ida10InitTtagHistory(IDA10_TTAG_HISTORY *history);

/* version.c */
char *ida10VersionString(VOID);
VERSION *ida10Version(VOID);

#ifdef __cplusplus
}
#endif

#endif /* ida10_h_included */

/* Revision History
 *
 * $Log: ida10.h,v $
 * Revision 1.59  2018/01/18 23:29:35  dechavez
 * reworked IDA10.12 support (new definition includes serialno, SEED_TAG and longer sname)
 *
 * Revision 1.58  2017/10/20 01:30:53  dauerbach
 * added support for IDA 10.12 MSEED 512-byte payloads
 * removed include of liss.h
 *
 * Revision 1.57  2014/08/29 20:38:08  dechavez
 * updated prototypes, include mseed.h
 *
 * Revision 1.56  2014/08/26 17:08:47  dechavez
 * added nepoch to IDA10_TTAG
 *
 * Revision 1.55  2014/08/08 15:32:41  dechavez
 * added ida10Cname() prototype
 *
 * Revision 1.54  2014/04/30 15:44:31  dechavez
 * added percent field to IDA10_CLOCK_STATUS
 *
 * Revision 1.53  2013/09/20 16:19:28  dechavez
 * changed IDA10_MAX_SUBFORMAT to IDA10_SUBFORMAT_11
 *
 * Revision 1.52  2013/07/19 17:50:10  dechavez
 * IDA10_OBSTAG2 and IDA10.11 support
 *
 * Revision 1.51  2013/05/11 22:49:38  dechavez
 * IDA10.10 support
 *
 * Revision 1.50  2013/02/07 18:00:54  dechavez
 * added ida10PackTSHdr() and ida10PackTS() prototypes
 *
 * Revision 1.49  2012/12/13 21:42:53  dechavez
 * added IDA10_NAME_MAP and rename.c prototypes
 *
 * Revision 1.48  2011/10/28 21:54:18  dechavez
 * changed mseed converter prototype
 *
 * Revision 1.47  2011/10/24 19:35:33  dechavez
 * added mseed converter prototype
 *
 * Revision 1.46  2011/10/10 19:53:41  dechavez
 * defined IDA10x_RESERVED_BYTES
 *
 * Revision 1.45  2011/02/25 19:40:42  dechavez
 * added TSheader option to ida10ReadGz() and ida10ReadRecord()
 *
 * Revision 1.44  2011/02/24 17:27:13  dechavez
 * added IDA10_SUBFORMAT_6_HDRLEN thrugh IDA10_SUBFORMAT_8_HDRLEN
 * added ida10ReadGz() prototype
 *
 * Revision 1.43  2011/01/26 00:12:43  dechavez
 * fixed nasty bug in defining IDA10_TSDAT (multiplied instead of divided by datum size)
 *
 * Revision 1.42  2011/01/25 21:38:21  dechavez
 * IDA10_MAXDATALEN increased to 16320
 *
 * Revision 1.41  2010/09/30 17:36:14  dechavez
 * checkpoint with preliminary IDA9 support
 *
 * Revision 1.40  2010/09/10 22:43:08  dechavez
 * added IDA10_SUBFORMAT_6 through 8, ida10HaveOrigSeqno() prototype
 *
 * Revision 1.39  2009/02/03 22:24:12  dechavez
 * added IDA10_MAX_SUBFORMAT
 *
 * Revision 1.38  2008/08/20 18:18:28  dechavez
 * added fresh bit to incerr
 *
 * Revision 1.37  2008/01/07 22:00:39  dechavez
 * updated prototypes
 *
 * Revision 1.36  2007/12/14 21:38:17  dechavez
 * added IDA10.5 support (generic timer)
 *
 * Revision 1.35  2007/06/21 22:47:25  dechavez
 * added 'derived' clock status
 *
 * Revision 1.34  2007/06/01 19:01:54  dechavez
 * replaced explicit structure for extra field in IDA10_CMNHDR with IDA_EXTRA
 *
 * Revision 1.33  2007/02/20 02:17:20  dechavez
 * aap (2007-02-19)
 *
 * Revision 1.32  2007/01/04 23:31:20  dechavez
 * Changes to accomodate OpenBSD builds
 *
 * Revision 1.31  2007/01/04 17:51:04  dechavez
 * added nanosecond sample interval and incerr structure to TS header
 *
 * Revision 1.30  2006/12/22 02:27:19  dechavez
 * revised Q330TTAG, updated prototypes
 *
 * Revision 1.29  2006/12/08 17:39:54  dechavez
 * added IDA10_SUBFORMAT_4_HDRLEN
 *
 * Revision 1.28  2006/12/08 17:28:01  dechavez
 * 10.4 support
 *
 * Revision 1.27  2006/11/10 06:24:24  dechavez
 * defined generic, OFIS and SAN specific clock structures and status
 * removed IDA10_TT_STAT_x macros (use explicit SANIO_TT_x as needed)
 * added REAl64 data descriptor support for 10.3
 * added and updated a whole slew of prototypes for all the functions that
 * handle all the above
 *
 * Revision 1.26  2006/08/19 02:17:10  dechavez
 * further 10.3 support
 *
 * Revision 1.25  2006/08/15 00:48:35  dechavez
 * 10.3 (OFIS) support, and removed premature and incorrect support for 64 bit datatypes
 *
 * Revision 1.24  2005/10/06 19:35:27  dechavez
 * added format and subformat fields to common header structure
 *
 * Revision 1.23  2005/08/26 18:10:58  dechavez
 * added 10.2 support
 *
 * Revision 1.22  2005/03/23 21:17:12  dechavez
 * initial subformat 2 support
 *
 * Revision 1.21  2004/12/10 18:20:24  dechavez
 * updated prototypes
 *
 * Revision 1.20  2004/12/10 17:45:25  dechavez
 * updated prototypes
 *
 * Revision 1.19  2004/06/25 18:34:56  dechavez
 * C++ compatibility
 *
 * Revision 1.18  2003/12/10 05:35:26  dechavez
 * added includes and prototypes to calm the Solaris cc compiler
 *
 * Revision 1.17  2003/11/13 19:27:43  dechavez
 * removed uneeded includes
 *
 * Revision 1.16  2003/10/16 17:46:53  dechavez
 * updated prototypes, renamed the WORDSIZE constants to the more appropriate
 * TYPE versions
 *
 * Revision 1.15  2003/05/05 23:15:02  dechavez
 * increased IDA10_MAXSTREAMS to 128
 *
 * Revision 1.14  2002/11/05 17:42:52  dechavez
 * added "unused" flag derived field to TS header structure
 *
 * Revision 1.13  2002/09/09 21:01:04  dec
 * added sint.c prototypes (ida10SintToFactMult() & ida10FactMultToSint())
 *
 * Revision 1.12  2002/05/15 20:15:50  dec
 *  added IDA10_TT_xxx macros, and updated prototypes
 *
 * Revision 1.11  2002/05/15 18:23:43  dec
 * added ida10GetOffset() prototype and IDA10_TT_STAT_xxx defines
 *
 * Revision 1.10  2002/04/29 17:27:47  dec
 * added new string prototypes
 *
 * Revision 1.9  2002/03/15 22:34:37  dec
 * added IDA10.1 (and hence variable length packet) support,
 * ida10ReadRecord() and ida10ErrorString() prototypes
 *
 * Revision 1.8  2002/01/25 19:13:33  dec
 * define ida10TtagIncrErr return codes
 *
 * Revision 1.7  2001/09/09 01:16:14  dec
 * eliminate notion of fixed length TS records and allow any size up
 * to IDA10_MAXDATALEN, currently defined at 1600
 *
 * Revision 1.6  2001/05/07 22:36:10  dec
 * replace stdtypes.h with platform.h
 *
 * Revision 1.5  2000/11/08 01:56:13  dec
 * added CF support
 *
 * Revision 1.4  2000/11/02 20:19:05  dec
 * add IDA10_TYPE_ISPLOG definition
 *
 * Revision 1.3  2000/06/23 21:23:15  dec
 * removed demo change, back to original version
 *
 * Revision 1.2  2000/04/25 23:21:43  nobody
 * added change to demonstrate CVS under NT
 *
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
