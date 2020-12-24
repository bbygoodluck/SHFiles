#include "../../ginc.h"
#include "LocalWatcherMonitorHandler.h"


wxBEGIN_EVENT_TABLE(CLocalWatcherMonitorHandler, wxEvtHandler)
//	EVT_TIMER(TIMER_ID, CLocalWatcherMonitorHandler::OnTimer)
	EVT_MY_CUSTOM_COMMAND(wxEVT_FILE_SYSTEM_WATCH, wxID_ANY, CLocalWatcherMonitorHandler::OnFileSystemWatchEvent)
wxEND_EVENT_TABLE()

CLocalWatcherMonitorHandler::CLocalWatcherMonitorHandler(CListView* pOwner)
	: CWatcherMonitorHandler(pOwner)
	, m_pLCWatcher(nullptr)
{
//	m_timer.SetOwner(this, TIMER_ID);
	m_pLCWatcher = std::make_unique<CLocalFileSystemWatcher>();
	m_pLCWatcher->SetOwner(this);
}

CLocalWatcherMonitorHandler::~CLocalWatcherMonitorHandler()
{
//	if(m_timer.IsRunning())
//		m_timer.Stop();

//	if(m_pLCWatcher)
//		delete m_pLCWatcher;

//	m_pLCWatcher = nullptr;
}

void CLocalWatcherMonitorHandler::AddWatchDir(const wxString& strPath)
{
	if (m_timer.IsRunning())
		m_timer.Stop();

	wxString strWatchPath(strPath);

	strWatchPath = strPath;

//	m_pLCWatcher->SetOwner(this);
	m_pLCWatcher->AddPath(strWatchPath);

//	m_timer.Start(800);
}
/*
void CLocalWatcherMonitorHandler::OnTimer(wxTimerEvent& event)
{
	if (m_pLCWatcher->IsQueueEmpty())
		return;

	while (!m_pLCWatcher->IsQueueEmpty())
	{
		CFileSystemWatcherBase::WATCHDIR_INFO info = m_pLCWatcher->GetWatchItem();

		_FS_WATCHER_TYPE watchType = (_FS_WATCHER_TYPE)info.m_iAction;
		wxString strNewName = info.m_strNewName;
		wxString strOldName = info.m_strOldName;

		switch (watchType)
		{
			case FS_WATCHER_CREATE:
				m_pListView->DoCreate(strNewName);
				break;

			case FS_WATCHER_MODIFY:
				m_pListView->DoModify(strNewName);
				break;

			case FS_WATCHER_DELETE:
				m_pListView->DoDelete(strNewName);
				break;

			case FS_WATCHER_RENAME:
				m_pListView->DoRename(strOldName, strNewName);
				break;

			default:
				break;
		}

		m_iFileOperationCount++;
		m_pLCWatcher->QueuePop();
	}

	m_pLCWatcher->UnLock();
	//생성/수정/삭제/이름변경에 대한 처리 후 디렉토리 정보에 대한 변경시간 갱신
	if(m_iFileOperationCount > 0)
	{
		m_pListView->UpdateModificationTimeOfDir();
		m_iFileOperationCount = 0;
	}
}
*/
void CLocalWatcherMonitorHandler::OnFileSystemWatchEvent(wxCommandEvent& event)
{
	if(m_pLCWatcher->IsQueueEmpty())
		return;

	while (!m_pLCWatcher->IsQueueEmpty())
	{
		CFileSystemWatcherBase::WATCHDIR_INFO info = m_pLCWatcher->GetWatchItem();

		_FS_WATCHER_TYPE watchType = (_FS_WATCHER_TYPE)info.m_iAction;

		switch (watchType)
		{
			case FS_WATCHER_CREATE:
				m_pListView->DoCreate(info.m_strNewName);
				break;

			case FS_WATCHER_MODIFY:
				m_pListView->DoModify(info.m_strNewName);
				break;

			case FS_WATCHER_DELETE:
				m_pListView->DoDelete(info.m_strNewName);
				break;

			case FS_WATCHER_RENAME:
				m_pListView->DoRename(info.m_strOldName, info.m_strNewName);
				break;

			default:
				break;
		}

		m_iFileOperationCount++;
		m_pLCWatcher->QueuePop();
	}

	m_pLCWatcher->UnLock();
	//생성/수정/삭제/이름변경에 대한 처리 후 디렉토리 정보에 대한 변경시간 갱신
	if(m_iFileOperationCount > 0)
	{
		m_pListView->UpdateModificationTimeOfDir();
		m_iFileOperationCount = 0;
	}
}
