/*======================================================================
 *
 * Signal handling thread.
 *
 *====================================================================*/
#include "isi330.h"

#define MY_MOD_ID ISI330_MOD_SIGNALS

static THREAD_FUNC SignalHandlerThread(void *dummy)
{
sigset_t set;
int sig;
static char *fid = "SignalHandlerThread";

    LogMsg("%s: Signal handler thread started", fid);

    sigfillset(&set); /* catch all signals defined by the system */

    while (1) {

        /* wait for a signal to arrive */
        sigwait(&set, &sig);

        /* process signals */

        switch (sig) {
          case SIGTERM:
            /* LogMsg("SIGTERM"); */
            SetExitStatus(-(MY_MOD_ID + sig));
            /* GracefulExit(-(MY_MOD_ID + sig)); */
            break;

          case SIGQUIT:
            /* LogMsg("SIGQUIT"); */
            /* GracefulExit(-(MY_MOD_ID + sig)); */
            SetExitStatus(-(MY_MOD_ID + sig));
            break;

          case SIGINT:
            /* LogMsg("SIGINT"); */
            /* GracefulExit(-(MY_MOD_ID + sig)); */
            SetExitStatus(-(MY_MOD_ID + sig));
            break;

          case SIGUSR1:
            /* LogMsg("SIGHUP"); */
            /* GracefulExit(-(MY_MOD_ID + sig)); */
            SetExitStatus(-(MY_MOD_ID + sig));
            break;

          case SIGHUP:
            /* LogMsg("SIGHUP"); */
            /* GracefulExit(-(MY_MOD_ID + sig)); */
            SetExitStatus(-(MY_MOD_ID + sig));
            break;

          default:
            /* LogMsg("signal %d ignored", sig); */
            break;
        }
    }
}

VOID StartSignalHandler(VOID)
{
int status;
THREAD tid;
sigset_t set;
static char *fid = "StartSignalHandler";

/* Block all signals */
/* We keep the pthread specific function in place because there is no
 * easy way to emulate this under Windows.  This should not be a problem
 * because this whole section is ifdef'd out under Windows and is known to
 * work under Solaris/Linux (where the THREAD macros are pthread based).
 */

    sigfillset(&set);
    pthread_sigmask(SIG_SETMASK, &set, NULL);

/* Create signal handling thread to catch all nondirected signals */

    if (!THREAD_CREATE(&tid, SignalHandlerThread, (void *) NULL)) {
        LogMsg("%s: THREAD_CREATE: %s", fid, strerror(errno));
        exit(1);
    }
}
/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2017 Regents of the University of California            |
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
