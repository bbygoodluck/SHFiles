#ifndef __WATCHER_MONITOR_HANLDER_INCLUDED__
#define __WATCHER_MONITOR_HANLDER_INCLUDED__

#include "../view/ListView.h"
class CWatcherMonitorHandler : public wxEvtHandler
{
public:
	explicit CWatcherMonitorHandler(CListView* pListView) 
		: m_pListView(pListView)
	{
			
	}
	
	virtual ~CWatcherMonitorHandler() {}
	virtual void AddWatchDir(const wxString& strPath) = 0;
	int m_iFileOperationCount = 0;
protected:
	wxTimer m_timer;
	CListView* m_pListView;
	
};
#endif