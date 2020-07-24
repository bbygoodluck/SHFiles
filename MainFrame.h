#ifndef __MAINFRAME_H_INCLUDED__
#define __MAINFRAME_H_INCLUDED__

class CEvtHandler;
class CCustomStatusBar;

class CMainFrame : public wxFrame
{
public:
	CMainFrame(const wxString& strTitle);
	virtual ~CMainFrame();

private:
	void Initialize();
	void RegisterEventHandler();
	void CreateToolBar();
	
private:
	wxAuiManager	m_mgr;
	bool m_bMaximize = false;
	
	CEvtHandler*	m_pSHFEventHandler;
	std::unique_ptr<wxToolBar> m_pToolBar;
	std::unique_ptr<CCustomStatusBar> m_pStatusBar;
private:
#ifdef __WXMSW__
	virtual WXLRESULT MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
#endif
	void OnClose(wxCloseEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMaximized(wxMaximizeEvent& event);
	void OnIconized(wxIconizeEvent& event);

	wxDECLARE_EVENT_TABLE();

};

#endif
