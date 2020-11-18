#ifndef __IMAGEMAP_BASE_H_INCLUDED__
#define __IMAGEMAP_BASE_H_INCLUDED__

class CListView;
class CImageMapBase : public wxThreadHelper
{
protected:
	typedef struct _FILEICON_INFO {
		int iIconIndex = 0;
		int iOverlayIndex = 0;
	} FILE_ICONINFO;
	
	typedef std::unordered_map<wxString, FILE_ICONINFO> ICON_MAP;
	enum {
		READ_STOP = false,
		READ_START = true
	};

public:
	explicit CImageMapBase(CListView *pListView);
	virtual ~CImageMapBase();
	
public:
	virtual wxThread::ExitCode Entry() wxOVERRIDE { return (wxThread::ExitCode)0; }
	
	void Wait() {
		m_Lock.getCondition()->Wait();
	}

	void Continue() {
		m_Lock.getCondition()->Signal();
	}
	
	void TerminateRead();
	void Clear();
	
	virtual void SetThreadTerminate() = 0;
	virtual void IconReadStart() = 0;
	void AddIcon(const wxString& strFullPath, const wxString& strName, bool isDrive = false);
	void Draw(wxDC* pDC, const wxString& strName, int x, int y, unsigned int flags, bool isDir = false, bool isDrive = false);
	bool IsIconReading() { return m_bReadStart == READ_START; }
	
	
protected:
	void DoDraw(wxDC* pDC, int iIconIndex, int x, int y, unsigned int flags);
	
protected:
	CLock m_Lock;
	ICON_MAP m_iconMap;
	bool m_bThreadTerminate = false;
	bool m_bReadStart = READ_STOP;
 
#ifdef __WXMSW__
	HANDLE m_hEvent;
#else
#endif

	CListView* m_pListView = nullptr;
};
#endif