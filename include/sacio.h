#pragma ident "$Id: sacio.h,v 1.4 2015/12/04 23:00:26 dechavez Exp $"
/*======================================================================
 *
 *  Include file for sacio library.  Everything before the prototypes
 *  was lifted from Dennis O'Neil's SEED reader... thanks!
 *
 *====================================================================*/
#ifndef sac_h_included
#define sac_h_included

#include "platform.h"

#define SAC_HDRLEN 632
#define SAC_Kx_LEN 8
#define SAC_KEVNM_LEN 16

struct sac_header {
    float   delta;      /* RF time increment, sec    */
    float   depmin;     /*    minimum amplitude      */
    float   depmax;     /*    maximum amplitude      */
    float   scale;      /*    amplitude scale factor */
    float   odelta;     /*    observed time inc      */
    float   b;          /* RD initial value, ampl.   */
    float   e;          /* RD final value, amplitude */
    float   o;          /*    event start, sec > 0   */
    float   a;          /*    1st arrival time       */
    float   internal1;  /*    internal use           */
    float   t0;         /*    user-defined time pick */
    float   t1;         /*    user-defined time pick */
    float   t2;         /*    user-defined time pick */
    float   t3;         /*    user-defined time pick */
    float   t4;         /*    user-defined time pick */
    float   t5;         /*    user-defined time pick */
    float   t6;         /*    user-defined time pick */
    float   t7;         /*    user-defined time pick */
    float   t8;         /*    user-defined time pick */
    float   t9;         /*    user-defined time pick */
    float   f;          /*    event end, sec > 0     */
    float   resp0;      /*    instrument respnse parm*/
    float   resp1;      /*    instrument respnse parm*/
    float   resp2;      /*    instrument respnse parm*/
    float   resp3;      /*    instrument respnse parm*/
    float   resp4;      /*    instrument respnse parm*/
    float   resp5;      /*    instrument respnse parm*/
    float   resp6;      /*    instrument respnse parm*/
    float   resp7;      /*    instrument respnse parm*/
    float   resp8;      /*    instrument respnse parm*/
    float   resp9;      /*    instrument respnse parm*/
    float   stla;       /*  T station latititude     */
    float   stlo;       /*  T station longitude      */
    float   stel;       /*  T station elevation, m   */
    float   stdp;       /*  T station depth, m       */
    float   evla;       /*    event latitude         */
    float   evlo;       /*    event longitude        */
    float   evel;       /*    event elevation        */
    float   evdp;       /*    event depth            */
    float   unused1;    /*    reserved for future use*/
    float   user0;      /*    available to user      */
    float   user1;      /*    available to user      */
    float   user2;      /*    available to user      */
    float   user3;      /*    available to user      */
    float   user4;      /*    available to user      */
    float   user5;      /*    available to user      */
    float   user6;      /*    available to user      */
    float   user7;      /*    available to user      */
    float   user8;      /*    available to user      */
    float   user9;      /*    available to user      */
    float   dist;       /*    stn-event distance, km */
    float   az;         /*    event-stn azimuth      */
    float   baz;        /*    stn-event azimuth      */
    float   gcarc;      /*    stn-event dist, degrees*/
    float   internal2;  /*    internal use           */
    float   internal3;  /*    internal use           */
    float   depmen;     /*    mean value, amplitude  */
    float   cmpaz;      /*  T component azimuth      */
    float   cmpinc;     /*  T component inclination  */
    float   unused2;    /*    reserved for future use*/
    float   unused3;    /*    reserved for future use*/
    float   unused4;    /*    reserved for future use*/
    float   unused5;    /*    reserved for future use*/
    float   unused6;    /*    reserved for future use*/
    float   unused7;    /*    reserved for future use*/
    float   unused8;    /*    reserved for future use*/
    float   unused9;    /*    reserved for future use*/
    float   unused10;   /*    reserved for future use*/
    float   unused11;   /*    reserved for future use*/
    float   unused12;   /*    reserved for future use*/
    INT32   nzyear;     /*  F zero time of file, yr  */
    INT32   nzjday;     /*  F zero time of file, day */
    INT32   nzhour;     /*  F zero time of file, hr  */
    INT32   nzmin;      /*  F zero time of file, min */
    INT32   nzsec;      /*  F zero time of file, sec */
    INT32   nzmsec;     /*  F zero time of file, msec*/
    INT32   internal4;  /*    internal use           */
    INT32   internal5;  /*    internal use           */
    INT32   internal6;  /*    internal use           */
    INT32   npts;       /* RF number of samples      */
    INT32   internal7;  /*    internal use           */
    INT32   internal8;  /*    internal use           */
    INT32   unused13;   /*    reserved for future use*/
    INT32   unused14;   /*    reserved for future use*/
    INT32   unused15;   /*    reserved for future use*/
    INT32   iftype;     /* RA type of file           */
    INT32   idep;       /*    type of amplitude      */
    INT32   iztype;     /*    zero time equivalence  */
    INT32   unused16;   /*    reserved for future use*/
    INT32   iinst;      /*    recording instrument   */
    INT32   istreg;     /*    stn geographic region  */
    INT32   ievreg;     /*    event geographic region*/
    INT32   ievtyp;     /*    event type             */
    INT32   iqual;      /*    quality of data        */
    INT32   isynth;     /*    synthetic data flag    */
    INT32   unused17;   /*    reserved for future use*/
    INT32   unused18;   /*    reserved for future use*/
    INT32   unused19;   /*    reserved for future use*/
    INT32   unused20;   /*    reserved for future use*/
    INT32   unused21;   /*    reserved for future use*/
    INT32   unused22;   /*    reserved for future use*/
    INT32   unused23;   /*    reserved for future use*/
    INT32   unused24;   /*    reserved for future use*/
    INT32   unused25;   /*    reserved for future use*/
    INT32   unused26;   /*    reserved for future use*/
    INT32   leven;      /* RA data-evenly-spaced flag*/
    INT32   lpspol;     /*    station polarity flag  */
    INT32   lovrok;     /*    overwrite permission   */
    INT32   lcalda;     /*    calc distance, azimuth */
    INT32   unused27;   /*    reserved for future use*/
    char    kstnm[SAC_Kx_LEN+1]; /*  F station name           */
    char    kevnm[SAC_KEVNM_LEN+1];/*    event name             */
    char    khole[SAC_Kx_LEN+1]; /*    man-made event name    */
    char    ko[SAC_Kx_LEN+1];    /*    event origin time id   */
    char    ka[SAC_Kx_LEN+1];    /*    1st arrival time ident */
    char    kt0[SAC_Kx_LEN+1];   /*    time pick 0 ident      */
    char    kt1[SAC_Kx_LEN+1];   /*    time pick 1 ident      */
    char    kt2[SAC_Kx_LEN+1];   /*    time pick 2 ident      */
    char    kt3[SAC_Kx_LEN+1];   /*    time pick 3 ident      */
    char    kt4[SAC_Kx_LEN+1];   /*    time pick 4 ident      */
    char    kt5[SAC_Kx_LEN+1];   /*    time pick 5 ident      */
    char    kt6[SAC_Kx_LEN+1];   /*    time pick 6 ident      */
    char    kt7[SAC_Kx_LEN+1];   /*    time pick 7 ident      */
    char    kt8[SAC_Kx_LEN+1];   /*    time pick 8 ident      */
    char    kt9[SAC_Kx_LEN+1];   /*    time pick 9 ident      */
    char    kf[SAC_Kx_LEN+1];    /*    end of event ident     */
    char    kuser0[SAC_Kx_LEN+1];/*    available to user      */
    char    kuser1[SAC_Kx_LEN+1];/*    available to user      */
    char    kuser2[SAC_Kx_LEN+1];/*    available to user      */
    char    kcmpnm[SAC_Kx_LEN+1];/*  F component name         */
    char    knetwk[SAC_Kx_LEN+1];/*    network name           */
    char    kdatrd[SAC_Kx_LEN+1];/*    date data read         */
    char    kinst[SAC_Kx_LEN+1]; /*    instrument name        */
};

/* definitions of constants for SAC enumerated data values */

#define IREAL   0       /* undocumented              */
#define ITIME   1       /* file: time series data    */
#define IRLIM   2       /* file: real&imag spectrum  */
#define IAMPH   3       /* file: ampl&phas spectrum  */
#define IXY     4       /* file: gen'l x vs y data   */
#define IUNKN   5       /* x data: unknown type      */
                        /* zero time: unknown        */
                        /* event type: unknown       */
#define IDISP   6       /* x data: displacement (nm) */
#define IVEL    7       /* x data: velocity (nm/sec) */
#define IACC    8       /* x data: accel (cm/sec/sec)*/
#define IB      9       /* zero time: start of file  */
#define IDAY   10       /* zero time: 0000 of GMT day*/
#define IO     11       /* zero time: event origin   */
#define IA     12       /* zero time: 1st arrival    */
#define IT0    13       /* zero time: user timepick 0*/
#define IT1    14       /* zero time: user timepick 1*/
#define IT2    15       /* zero time: user timepick 2*/
#define IT3    16       /* zero time: user timepick 3*/
#define IT4    17       /* zero time: user timepick 4*/
#define IT5    18       /* zero time: user timepick 5*/
#define IT6    19       /* zero time: user timepick 6*/
#define IT7    20       /* zero time: user timepick 7*/
#define IT8    21       /* zero time: user timepick 8*/
#define IT9    22       /* zero time: user timepick 9*/
#define IRADNV 23       /* undocumented              */
#define ITANNV 24       /* undocumented              */
#define IRADEV 25       /* undocumented              */
#define ITANEV 26       /* undocumented              */
#define INORTH 27       /* undocumented              */
#define IEAST  28       /* undocumented              */
#define IHORZA 29       /* undocumented              */
#define IDOWN  30       /* undocumented              */
#define IUP    31       /* undocumented              */
#define ILLLBB 32       /* undocumented              */
#define IWWSN1 33       /* undocumented              */
#define IWWSN2 34       /* undocumented              */
#define IHGLP  35       /* undocumented              */
#define ISRO   36       /* undocumented              */
#define INUCL  37       /* event type: nuclear shot  */
#define IPREN  38       /* event type: nuke pre-shot */
#define IPOSTN 39       /* event type: nuke post-shot*/
#define IQUAKE 40       /* event type: earthquake    */
#define IPREQ  41       /* event type: foreshock     */
#define IPOSTQ 42       /* event type: aftershock    */
#define ICHEM  43       /* event type: chemical expl */
#define IOTHER 44       /* event type: other source  */
                        /* data quality: other problm*/
#define IGOOD  45       /* data quality: good        */
#define IGLCH  46       /* data quality: has glitches*/
#define IDROP  47       /* data quality: has dropouts*/
#define ILOWSN 48       /* data quality: low s/n     */
#define IRLDTA 49       /* data is real data         */
#define IVOLTS 50       /* file: velocity (volts)    */
#define INIV51 51       /* undocumented              */
#define INIV52 52       /* undocumented              */
#define INIV53 53       /* undocumented              */
#define INIV54 54       /* undocumented              */
#define INIV55 55       /* undocumented              */
#define INIV56 56       /* undocumented              */
#define INIV57 57       /* undocumented              */
#define INIV58 58       /* undocumented              */
#define INIV59 59       /* undocumented              */
#define INIV60 60       /* undocumented              */

/* Format strings for writing headers of SAC ASCII files */

#define SACWFCS "%15.7f%15.7f%15.7f%15.7f%15.7f\n"  /* for floats  */
#define SACWICS "%10d%10d%10d%10d%10d\n"       /* for longs   */
#define SACWCCS1 "%-8.8s%-8.8s%-8.8s\n"             /* for strings */
#define SACWCCS2 "%-8.8s%-16.16s\n"                 /* for strings */

/* Format strings for reading headers of SAC ASCII files */

#define SACRFCS "%f %f %f %f %f"   /* for floats  */
#define SACRICS "%d %d %d %d %d"   /* for ints   */
#define SACRCCS1 "%8s%8s%8s"       /* for strings */
#define SACRCCS2 "%8s%16s"         /* for strings */

/*  sacio library function prototypes  */

int sacio_rah(FILE *, struct sac_header *);
int sacio_rbh(FILE *, struct sac_header *);
int sacio_wad(FILE *, float *, INT32, INT32 *);
int sacio_wah(FILE *, struct sac_header *);
int sacio_wbh(FILE *, struct sac_header *);
double sacio_sttodt(struct sac_header *);
void   sacio_settime(struct sac_header *hdr, double tofs);

#endif

/* Revision History
 *
 * $Log: sacio.h,v $
 * Revision 1.4  2015/12/04 23:00:26  dechavez
 * modfied format strings to calm OS X compiles
 *
 * Revision 1.3  2012/02/14 21:45:16  dechavez
 * use portable integer types (eg, long to INT32, unsigned long to UINT32)
 *
 * Revision 1.2  2007/01/07 17:35:16  dechavez
 * used #define constants instead of hard-coded string lengths
 *
 * Revision 1.1.1.1  2000/02/08 20:20:22  dec
 * import existing IDA/NRTS sources
 *
 */
