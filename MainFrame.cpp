#include "ginc.h"
#include "EvtHandler.h"
#include "CustomStatusBar.h"
#include "MainFrame.h"

wxBEGIN_EVENT_TABLE(CMainFrame, wxFrame)
	EVT_SIZE(CMainFrame::OnSize)
	EVT_CLOSE(CMainFrame::OnClose)
	EVT_MAXIMIZE(CMainFrame::OnMaximized)
	EVT_ICONIZE(CMainFrame::OnIconized)
wxEND_EVENT_TABLE()


CMainFrame::CMainFrame(const wxString& strTitle)
	: wxFrame(NULL, wxID_ANY, strTitle, wxPoint(theJsonConfig->GetXPos(), theJsonConfig->GetYPos()),
		                                wxSize(theJsonConfig->GetWidth(), theJsonConfig->GetHeight()))
{
	_gMainFrame = this;
	Initialize();

	SetIcon(wxIcon("wxwin"));
	wxMenuBar* pMenuBar = wxXmlResource::Get()->LoadMenuBar(this, wxT("SHCMenu"));
	if (pMenuBar == nullptr)
	{
		wxMessageBox(wxT("Failed to load MenuBar resource"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}

	SetMenuBar(pMenuBar);
	RegisterEventHandler();
	
	CreateToolBar();
	
	m_pStatusBar = std::make_unique<CCustomStatusBar>(this);
	SetStatusBar(m_pStatusBar.get());
	
	m_mgr.SetManagedWindow(this);
	
	theSplitterManager->Init();
	theSplitterManager->GetSplitter()->SetSashGravity(1.0);
	theSplitterManager->DoSplitter();

	m_mgr.AddPane(theSplitterManager->GetSplitter(), wxAuiPaneInfo().CenterPane().PaneBorder(false));
	m_mgr.Update();

	if (theJsonConfig->IsMaximized())
		((wxTopLevelWindow*)wxTheApp->GetTopWindow())->Maximize(true);
	else
		CenterOnScreen();
}

CMainFrame::~CMainFrame()
{
	_gAppActivated = true;
}

void CMainFrame::Initialize()
{
	int iMonitors = wxDisplay::GetCount();

	int nCurrDisp = wxDisplay::GetFromWindow(this);
	if (nCurrDisp < 0)
		nCurrDisp = 0;

	wxDisplay disp(nCurrDisp);

	if (iMonitors == 1)
	{	//모니터가 하나일경우 기존 모니터 인덱스와 다를 경우는 Default로 셋팅
		int iMonitorPos = theJsonConfig->GetMonitorPos();
		if (nCurrDisp != iMonitorPos)
		{
			wxRect rcDefault = theJsonConfig->GetDefaultRect();

			theJsonConfig->SetChangeGlobalVal(wxT("DispMonitor"), 0);
			theJsonConfig->SetChangeGlobalVal(wxT("XPos"), rcDefault.GetLeft());
			theJsonConfig->SetChangeGlobalVal(wxT("YPos"), rcDefault.GetRight());
			theJsonConfig->SetChangeGlobalVal(wxT("Width"), rcDefault.GetWidth());
			theJsonConfig->SetChangeGlobalVal(wxT("Height"), rcDefault.GetHeight());

			if (!theJsonConfig->IsMaximized())
				this->SetSize(rcDefault);
		}
	}

	if (theJsonConfig->IsMaximized())
		this->Maximize();
}

void CMainFrame::OnClose(wxCloseEvent& event)
{
	m_mgr.UnInit();
	
	PopEventHandler(true);
	Destroy();
}

void CMainFrame::OnSize(wxSizeEvent& event)
{
	wxSize szFrame = event.GetSize();
	
	if (!this->IsMaximized())
	{	//최대화에서 원래상태로 복원되었을 경우
		if (m_bMaximize)
		{
			int nCurrDisp = wxDisplay::GetFromWindow(this);
			if (nCurrDisp < 0)
				nCurrDisp = 0;

			wxDisplay disp(nCurrDisp);

			theJsonConfig->SetChangeGlobalVal(wxT("Maximized"), 0);
			if (theJsonConfig->IsCenterOnScreen())
				CenterOnScreen();
		}
		else
		{
			theJsonConfig->SetChangeGlobalVal(wxT("Width"), szFrame.GetWidth());
			theJsonConfig->SetChangeGlobalVal(wxT("Height"), szFrame.GetHeight());
			
			theSplitterManager->DoSplitter(szFrame);
		}

		m_bMaximize = false;
	}
}

void CMainFrame::OnMaximized(wxMaximizeEvent& event)
{
	m_bMaximize = true;
	theJsonConfig->SetChangeGlobalVal(wxT("Maximized"), 1);
}

void CMainFrame::OnIconized(wxIconizeEvent& event)
{

}

void CMainFrame::RegisterEventHandler()
{
	m_pSHFEventHandler = new CEvtHandler(this);
	PushEventHandler(m_pSHFEventHandler);
		
	m_pSHFEventHandler->LoadBookmark(false);
}

#ifdef __WXMSW__
WXLRESULT CMainFrame::MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
	WXLRESULT rc;
	switch (nMsg)
	{
		default:
			break;
	}
	
	rc = ::DefWindowProc(this->GetHWND(), nMsg, wParam, lParam);
	return rc;
}
#endif

void CMainFrame::CreateToolBar()
{
	m_pToolBar = std::make_unique<wxToolBar>(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxTB_FLAT | wxTB_HORZ_TEXT);//.reset(new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxTB_FLAT | wxTB_HORZ_TEXT));
	
	m_pToolBar->AddTool(TOOLBAR_ID_REFRESH, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_REFRESH_ALL")), wxBitmap(wxT("./img/refresh.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_REFRESH_ALL")), wxEmptyString, NULL);
	m_pToolBar->AddSeparator();
	m_pToolBar->AddTool(TOOLBAR_ID_FILE_EDIT, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_EDIT")), wxArtProvider::GetBitmap(wxART_EDIT, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_EDIT")), wxEmptyString, NULL);
	m_pToolBar->AddTool(TOOLBAR_ID_FIND, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_FIND")), wxArtProvider::GetBitmap(wxART_FIND, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_FIND")), wxEmptyString, NULL);
	m_pToolBar->AddSeparator();
	m_pToolBar->AddTool(TOOLBAR_ID_DIR_MANAGER, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_MCD")), wxBitmap(wxT("./img/MakeFolder.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_MCD")), wxEmptyString, NULL);
	m_pToolBar->AddTool(TOOLBAR_ID_BOOKMARK, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_FAVORITE")), wxArtProvider::GetBitmap(wxART_ADD_BOOKMARK, wxART_BUTTON), wxNullBitmap, wxITEM_DROPDOWN, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_BOOKMARK")), wxEmptyString, NULL);
	m_pToolBar->AddSeparator();
	m_pToolBar->AddTool(TOOLBAR_ID_ENV_SETTING, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_ENVIRONMENT")), wxBitmap(wxT("./img/environment.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_ENVIRONMENT")), wxEmptyString, NULL);
	m_pToolBar->AddTool(TOOLBAR_ID_ENV_SAVE, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_ENV_SAVE")), wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_ENV_SAVE")), wxEmptyString, NULL);
	m_pToolBar->AddStretchableSpace();
	m_pToolBar->AddTool(TOOLBAR_ID_THIS_PROGRAM, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_THISPG")), wxArtProvider::GetBitmap(wxART_HELP_PAGE, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, theMsgManager->GetMessage(wxT("MSG_TOOLBAR_THISPG")), wxEmptyString, NULL);
	m_pToolBar->Realize();

	SetToolBar(m_pToolBar.get());
	
	m_pSHFEventHandler->SetBookmarkDropdown();
	m_pSHFEventHandler->RegisterToolbarEvent();
}