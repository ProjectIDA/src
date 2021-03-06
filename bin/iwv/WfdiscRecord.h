#pragma ident "$Id: WfdiscRecord.h,v 1.2 2011/03/17 20:47:46 dechavez Exp $"

#if !defined(_WFDISCRECORD_H_INCLUDED_)
#define _WFDISCRECORD_H_INCLUDED_
#include "platform.h"
#include "cssio.h"

class CWfdiscRecord  
	{
public:
	CWfdiscRecord();
	CWfdiscRecord(struct cssio_wfdisc wfd);
	long GetCount(int n);
	bool CloseDataSet();
	bool OpenDataSet();


	virtual ~CWfdiscRecord();
//*********************************
	long LoadData(int,int nMaxCounts=0);

	struct cssio_wfdisc wfd;
	long *lData;

	unsigned long nCacheFirstCount, nCacheSize;
	unsigned long nCacheMaximumSize;
	int ncssFile;



	};

#endif // !defined(_WFDISCRECORD_H_INCLUDED_)

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2010 Regents of the University of California            |
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
 * $Log: WfdiscRecord.h,v $
 * Revision 1.2  2011/03/17 20:47:46  dechavez
 * use the new CSSIO_, cssio_, prefixed names from libcssio 2.2.0
 *
 * Revision 1.1  2010/05/11 18:25:00  akimov
 * initial production release
 *
 */
