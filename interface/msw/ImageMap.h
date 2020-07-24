#ifndef __IMAGEMAP_H_INCLUDED__
#define __IMAGEMAP_H_INCLUDED__

#include "../../view/ListView.h"
#include "../ImageMapBase.h"
class CImageMap : public CImageMapBase
{
public:
	CImageMap(CListView* pListView);
	virtual ~CImageMap();
	
public:
	virtual wxThread::ExitCode Entry() wxOVERRIDE;
	
	virtual void SetThreadTerminate();
	virtual void IconReadStart();
	
};

#endif