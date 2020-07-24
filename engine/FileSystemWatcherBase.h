#ifndef __FILESYSTEM_WATCHER_BASE_INCLUDED__
#define __FILESYSTEM_WATCHER_BASE_INCLUDED__

#include <queue>

class CWatcherDir
{
public:
	CWatcherDir() 
		: bSubTree(false), m_strDir(wxT("")) 
	{
		wxZeroMemory(m_Buffer);
	}
	
	~CWatcherDir() {}
	
	enum
	{
		BUFFER_SIZE = 4096
	};

public:
	void* GetBuffer() { return m_Buffer; }

public:
	bool		bSubTree;
	wxString	m_strDir;
	char m_Buffer[BUFFER_SIZE];
};

class CFileSystemWatcherBase : public wxThreadHelper
{
public:
	typedef struct WatchingData
	{
		WatchingData(const int iAction, const wxString& strOldName, const wxString& strNewName, const wxString& strFullPath)
			: m_iAction(iAction)
			, m_strOldName(strOldName)
			, m_strNewName(strNewName)
			, m_strFullPath(strFullPath)
		{
		}

		wxString m_strNewName = wxT("");
		wxString m_strOldName = wxT("");
		wxString m_strFullPath = wxT("");
		int m_iAction;
	} WATCHDIR_INFO;

public:
	CFileSystemWatcherBase();
	virtual ~CFileSystemWatcherBase();
	
public:
	int GetQueueSize() { return m_queue.size(); }
	bool IsQueueEmpty() { return m_queue.empty(); }
	WATCHDIR_INFO GetWatchItem() {
		return m_queue.front();
	}

	void QueuePop() { m_queue.pop(); }
	
protected:
	virtual wxThread::ExitCode Entry() = 0;
	virtual void Clear() = 0;
	virtual int AddPath(const wxString& strPath, unsigned long ulNotifyFilter, bool bSubTree) = 0;
	
	void DoWatchDirectory();
	int Native2WatcherFlags(int flags);
	
protected:
	std::queue<WATCHDIR_INFO> m_queue;
};

#endif