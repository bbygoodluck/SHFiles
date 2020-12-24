#ifndef __LOCAL_FILESYSTEM_WATCHER_H_INCLUDED__
#define __LOCAL_FILESYSTEM_WATCHER_H_INCLUDED__

#include "../FileSystemWatcherBase.h"
class CLocalFileSystemWatcher : public CFileSystemWatcherBase
{
	class CLocalWatcherDir : public CWatcherDir
	{
		public:
			CLocalWatcherDir() : hFile(NULL), m_hIOCP(INVALID_HANDLE_VALUE) {
				wxZeroMemory(PollingOverlap);
			}
			virtual ~CLocalWatcherDir() {}

		public:
			OVERLAPPED	PollingOverlap;
			HANDLE		hFile;
			HANDLE		m_hIOCP;
			ULONG_PTR   CompletionKey;
	};

public:
	CLocalFileSystemWatcher();
	virtual ~CLocalFileSystemWatcher();

	void Lock();
	void UnLock();
protected:
	virtual wxThread::ExitCode Entry() override;
	virtual void Clear() override;

public:
	virtual int AddPath(const wxString& strPath,
						unsigned long ulNotifyFilter = FILE_NOTIFY_CHANGE_CREATION |
						                             FILE_NOTIFY_CHANGE_SIZE       |
									                 FILE_NOTIFY_CHANGE_LAST_WRITE |
													 FILE_NOTIFY_CHANGE_DIR_NAME   |
													 FILE_NOTIFY_CHANGE_ATTRIBUTES |
													 FILE_NOTIFY_CHANGE_FILE_NAME,
						bool bSubTree = false) override;

private:
	CLocalWatcherDir  m_watchDir;
	unsigned long	  m_ulNotifyFilters;
	int	m_nLastError = 0;
	bool m_bRemove = false;
};
#endif
