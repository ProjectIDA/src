#pragma ident "$Id: ifconfig.c,v 1.5 2011/10/13 18:23:19 dechavez Exp $"
/*======================================================================
 *
 *  Request Physical Interfaces
 *
 *====================================================================*/
#include "q330cc.h"

#define UNDEFINED_IF -1
#define ENET_IF       0
#define SER1_IF       1
#define SER2_IF       2
#define SER3_IF       3
static struct {
    char *name;
    int code;
} ifmap[] = {{"enet", ENET_IF}, {"sp1", SER1_IF}, {"sp2", SER2_IF}, {"sp3", SER3_IF}, {NULL, UNDEFINED_IF}};

static int interface = UNDEFINED_IF;

void IfconfigHelp(char *name, int argc, char **argv)
{
    printf("usage: %s [ { %s | %s | %s | %s } dot.decimal.ip.address ]\n", name, ifmap[0].name, ifmap[1].name, ifmap[2].name, ifmap[3].name);
    printf("Without arguments, prints a physical interfaces report (C1_PHY).\n");
    printf("To set an IP address, give one of the interface tags shown above, followed by the new address.\n");
}

static BOOL SetInterfaceAddress(Q330 *q330, char *ifname, char *addr)
{
int i;

    for (i = 0; interface == UNDEFINED_IF && ifmap[i].name != NULL; i++) {
        if (strcasecmp(ifname, ifmap[i].name) == 0) interface = ifmap[i].code;
    }
    if (interface == UNDEFINED_IF) {
        IfconfigHelp(q330->cmd.name, 0, NULL);
        return FALSE;
    }
    q330->cmd.p32 = (UINT32) inet_addr(addr);
    return TRUE;
}

BOOL VerifyIfconfig(Q330 *q330)
{
    q330->cmd.code = Q330_CMD_IFCONFIG;
    switch (q330->cmd.arglst->count) {
      case 0: return TRUE;
      case 2: return SetInterfaceAddress(q330, (char *) q330->cmd.arglst->array[0], (char *) q330->cmd.arglst->array[1]);
    }

    IfconfigHelp(q330->cmd.name, 0, NULL);
    return FALSE;
}

void ifconfig(Q330 *q330)
{
int i;
QDP_PKT pkt;
QDP_TYPE_C1_PHY phy;

    if (!qdp_C1_RQPHY(q330->qdp, &phy)) return;
    if (interface != UNDEFINED_IF) printf("Current ");
    qdpPrint_C1_PHY(stdout, &phy);
    if (interface == UNDEFINED_IF) return;

    switch (interface) {
      case ENET_IF: phy.ethernet.ip  = htonl(q330->cmd.p32); break;
      case SER1_IF: phy.serial[0].ip = htonl(q330->cmd.p32); break;
      case SER2_IF: phy.serial[1].ip = htonl(q330->cmd.p32); break;
      case SER3_IF: phy.serial[2].ip = htonl(q330->cmd.p32); break;
      default:
        printf("'This error should never occur' error in file %s, line %d\n", __FILE__, __LINE__);
        GracefulExit(q330, 1);
    }

    qdpEncode_C1_PHY(&pkt, &phy);
    if (!qdpPostCmd(q330->qdp, &pkt, TRUE)) {
        printf("Set Physical Interfaces command failed\n");
        return;
    }

    if (!qdp_C1_RQPHY(q330->qdp, &phy)) return;
    if (interface != UNDEFINED_IF) printf("\nUpdated ");
    qdpPrint_C1_PHY(stdout, &phy);
    printf("Changes will not take effect until configuration is saved and the Q330 is rebooted.\n");
}

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2008 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: ifconfig.c,v $
 * Revision 1.5  2011/10/13 18:23:19  dechavez
 * GracefulExit()s
 *
 * Revision 1.4  2009/07/23 20:30:51  dechavez
 * fixed byte order problem setting IP address, added help support
 *
 * Revision 1.3  2009/02/04 17:47:19  dechavez
 * use new libqdp qdp_C1_RQx commands to retrieve information from digitizer
 *
 * Revision 1.2  2009/01/05 22:48:11  dechavez
 * allow option for setting IP address
 *
 * Revision 1.1  2008/10/02 23:04:26  dechavez
 * created
 *
 */
