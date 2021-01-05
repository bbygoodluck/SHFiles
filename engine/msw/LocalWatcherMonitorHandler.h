#ifndef __LOCAL_WATCHER_MONITOR_HANDLER_H_INCLUDED__
#define __LOCAL_WATCHER_MONITOR_HANDLER_H_INCLUDED__

#include "../WatcherMonitorHandler.h"
#include "LocalFileSystemWatcher.h"

class CLocalWatcherMonitorHandler : public CWatcherMonitorHandler
{
protected:
	enum {
		TIMER_ID = 5000
	};
public:
	explicit CLocalWatcherMonitorHandler(CListView* pOwner);
	virtual ~CLocalWatcherMonitorHandler();

	virtual void AddWatchDir(const wxString& strPath) override;

private:
	std::unique_ptr<CLocalFileSystemWatcher> m_pLCWatcher;
	int m_iTimerID;

private:
	void DoWatchProcess(bool bSetTimer);

private:
	void OnTimer(wxTimerEvent& event);
	void OnFileSystemWatchEvent(wxCommandEvent& event) override;
	wxDECLARE_EVENT_TABLE();
};
#endif
