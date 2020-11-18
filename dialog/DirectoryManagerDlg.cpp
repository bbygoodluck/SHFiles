#include "../ginc.h"
#include "../interface/DirSearchThread.h"
#include "../interface/MakeTreeInfoThread.h"
#include "../view/SHCDView.h"
#include "../engine/LocalFileSystem.h"

#include "DlgSearchDir.h"
#include "DlgMakeSHCDInfo.h"
#include "DlgMCDFind.h"
#include "DirectoryManagerDlg.h"

#include <iostream>
#include <fstream>
#include <streambuf>
/*
wxBEGIN_EVENT_TABLE(DirectoryManagerDlg, wxDialog)
	EVT_THREAD(SCD_MAKE_TREE_INFO_THREAD_EVENT, DirectoryManagerDlg::OnMakeTreeInfoThreadEvent)
wxEND_EVENT_TABLE()
*/
DirectoryManagerDlg::DirectoryManagerDlg(wxWindow* parent, 
									wxWindowID id, 
									const wxString& title, 
									const wxPoint& pos, 
									const wxSize& size, 
									long style)
	: wxDialog(parent, id, title, pos, wxSize(theJsonConfig->GetWidth() - 50, theJsonConfig->GetHeight() - 50), style)
{
	SetIcon(wxIcon("wxwin"));
	CenterOnScreen();
	
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxHORIZONTAL);

	m_scdToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_HORZ_TEXT);
	m_toolRefreshAll = m_scdToolBar->AddTool(wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_REFRESH_ALL")), wxBitmap(wxT("./img/refresh_11.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

	m_toolRefreshDir = m_scdToolBar->AddTool(wxID_SCD_REFRESH, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_REFRESH_DIR")), wxBitmap(wxT("./img/refresh.bmp"), wxBITMAP_TYPE_ANY), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

	m_scdToolBar->AddSeparator();

	m_toolAddFolder = m_scdToolBar->AddTool(wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_DIR_ADD")), wxArtProvider::GetBitmap(wxART_PLUS, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

	m_toolRenameFolder = m_scdToolBar->AddTool(wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_DIR_RENAME")), wxArtProvider::GetBitmap(wxART_FIND_AND_REPLACE, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

	m_toolDelete = m_scdToolBar->AddTool(wxID_DIR_DELETE, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_DIR_DEL")), wxArtProvider::GetBitmap(wxART_CUT, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

	m_scdToolBar->AddSeparator();

	m_toolFind = m_scdToolBar->AddTool(wxID_DIR_FIND, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_DIR_FIND")), wxArtProvider::GetBitmap(wxART_FIND, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

	m_scdToolBar->AddStretchSpacer();

	m_toolClose = m_scdToolBar->AddTool(wxID_DIR_MNG_CLOSE, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_EXIT")), wxArtProvider::GetBitmap(wxART_QUIT, wxART_BUTTON), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL);

	m_scdToolBar->Realize();

	bSizer6->Add(m_scdToolBar, 1, 0, 5);


	bSizer4->Add(bSizer6, 1, wxEXPAND, 5);


	bSizer1->Add(bSizer4, 0, wxEXPAND, 5);

	m_staticline1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	bSizer1->Add(m_staticline1, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText2 = new wxStaticText(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_BOOKMARK_PATH")) + wxT(" "), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2->Wrap(-1);
	bSizer2->Add(m_staticText2, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

	m_staticText31 = new wxStaticText(this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText31->Wrap(-1);
	m_staticText31->SetFont(*_gFont);
	m_staticText31->SetForegroundColour(wxColour(80, 80, 80));

	bSizer2->Add(m_staticText31, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);


	bSizer1->Add(bSizer2, 0, wxBOTTOM | wxEXPAND | wxTOP, 5);

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxVERTICAL);

	m_pSHCDView = std::make_unique<CSHCDView>(this, ++wxFILFELIST_VIEWID, wxDefaultSize, wxT(""));//.reset(new CSHCDView(this, ++wxFILFELIST_VIEWID, wxDefaultSize, wxT("")));
	bSizer3->Add(m_pSHCDView.get(), 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

	bSizer1->Add(bSizer3, 1, wxEXPAND, 5);

	this->SetSizer(bSizer1);
	this->Layout();

	this->Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(DirectoryManagerDlg::OnInitDialog));
	this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(DirectoryManagerDlg::OnClose));

	// Connect Events
	this->Connect(m_toolRefreshAll->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DirectoryManagerDlg::OnRefreshAll));
	this->Connect(m_toolRefreshDir->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DirectoryManagerDlg::OnSCDRefreshDir));
	this->Connect(m_toolAddFolder->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DirectoryManagerDlg::OnAddDir));
	this->Connect(m_toolRenameFolder->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DirectoryManagerDlg::OnRenameDir));
	this->Connect(m_toolDelete->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DirectoryManagerDlg::OnDeleteDir));
	this->Connect(m_toolFind->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DirectoryManagerDlg::OnFindDir));
	this->Connect(m_toolClose->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DirectoryManagerDlg::OnDirManagerClose));
	
	this->Maximize(true);
}

DirectoryManagerDlg::~DirectoryManagerDlg()
{
	this->Disconnect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(DirectoryManagerDlg::OnInitDialog));
	this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(DirectoryManagerDlg::OnClose));

	// Disconnect Events
	this->Disconnect(m_toolRefreshAll->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DirectoryManagerDlg::OnRefreshAll));
	this->Disconnect(m_toolRefreshDir->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DirectoryManagerDlg::OnSCDRefreshDir));
	this->Disconnect(m_toolAddFolder->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DirectoryManagerDlg::OnAddDir));
	this->Disconnect(m_toolRenameFolder->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DirectoryManagerDlg::OnRenameDir));
	this->Disconnect(m_toolDelete->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DirectoryManagerDlg::OnDeleteDir));
	this->Disconnect(m_toolFind->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DirectoryManagerDlg::OnFindDir));
	this->Disconnect(m_toolClose->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DirectoryManagerDlg::OnDirManagerClose));

	theSHCD->Delete();
}

void DirectoryManagerDlg::OnClose(wxCloseEvent& event)
{
	EndDialog(wxID_CLOSE);
//	Destroy();
}

void DirectoryManagerDlg::OnInitDialog(wxInitDialogEvent& event)
{
	LoadSCDData(true);
}

void DirectoryManagerDlg::SetNodePath(const wxString& strFullPath)
{
	m_staticText31->SetLabelText(strFullPath);
}

void DirectoryManagerDlg::LoadSCDData(bool bFirst, bool bSetFocus)
{
	if(!bFirst)
		theSHCD->Delete();

	if (!bFirst) m_pSHCDView->Freeze();

	wxString strSCDFilePath(m_strSCDFilePath);
	wxString strCurrPath(m_strCurrPath);
	if (strSCDFilePath.IsEmpty() && strCurrPath.IsEmpty())
		return;

	if (!wxFileExists(strSCDFilePath))
	{
		DlgSearchDir dlg(_gMainFrame);

		CDirSearchThread* m_pDirSearchThread = new CDirSearchThread(&dlg);

		if (m_pDirSearchThread->Create() != wxTHREAD_NO_ERROR)
		{
			wxLogError(wxT("Can't create thread!"));
		}

		m_pDirSearchThread->SetSearchCurrentDirectory(strCurrPath, strSCDFilePath);
		m_pDirSearchThread->Run();

		dlg.ShowModal();
		dlg.Destroy();

		m_pDirSearchThread->Delete();
		delete m_pDirSearchThread;

		m_pDirSearchThread = nullptr;
	}

	std::ifstream inFile;
	inFile.open(strSCDFilePath.char_str());
	
	size_t lineCount = std::count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n'); //inFile.GetLineCount();// 

	DlgMakeSHCDInfo dlgMakeInfo(_gMainFrame);

	m_pTreeInfoThread = new CMakeTreeInfoThread(&dlgMakeInfo);
	if (m_pTreeInfoThread->Create() != wxTHREAD_NO_ERROR)
	{
		wxLogError(wxT("Can't create thread!"));
	}

	m_pTreeInfoThread->SetLoadFromFile(true);
	m_pTreeInfoThread->SetSHCDFile(strSCDFilePath, strCurrPath, lineCount);
	m_pTreeInfoThread->Run();

	dlgMakeInfo.ShowModal();
	dlgMakeInfo.Destroy();

	m_pTreeInfoThread->Delete();
	
	delete m_pTreeInfoThread;
	m_pTreeInfoThread = nullptr;

	if(!bFirst)   m_pSHCDView->Thaw();
	if(!bFirst)   m_pSHCDView->SetInit();
	if(bSetFocus) m_pSHCDView->SetFocus();
}

void DirectoryManagerDlg::LoadPartFolder(const wxString& strPath, NODE* pNode)
{
	DlgMakeSHCDInfo dlgMakeInfo(_gMainFrame);

	m_pTreeInfoThread = new CMakeTreeInfoThread(&dlgMakeInfo);
	if (m_pTreeInfoThread->Create() != wxTHREAD_NO_ERROR)
	{
		wxLogError(wxT("Can't create thread!"));
	}

	m_pTreeInfoThread->SetLoadFromFile(false);
	m_pTreeInfoThread->SetCurrentLevel(pNode);
	m_pTreeInfoThread->SetSHCDFile(wxT(""), strPath);
	m_pTreeInfoThread->Run();
	
	dlgMakeInfo.ShowModal();
	dlgMakeInfo.Destroy();
	
	m_pTreeInfoThread->Delete();
	delete m_pTreeInfoThread;
	m_pTreeInfoThread = nullptr;

	m_pSHCDView->SetInit();
	m_pSHCDView->SetFocus();
}

void DirectoryManagerDlg::OnRefreshAll(wxCommandEvent& event)
{
	RefreshAll();
}

void DirectoryManagerDlg::RefreshAll()
{
	m_strCurrPath = m_staticText31->GetLabelText();

	wxString strSCDFilePath(m_strSCDFilePath);

	if (wxFileExists(strSCDFilePath))
	{
		int iRet = 0;
		if (!CLocalFileSystem::RecursiveDelete(strSCDFilePath, this, false, iRet))
		{
			wxString strMsg(strSCDFilePath);
			strMsg += theMsgManager->GetMessage(wxT("MSG_DLG_SCD_FILE_DEL_ERR"));

			wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
			return;
		}

		m_pSHCDView->Freeze();
		
		LoadSCDData(false, false);
		
		m_pSHCDView->Thaw();
		m_pSHCDView->SetInit();
		m_pSHCDView->SetFocus();
	}
}

void DirectoryManagerDlg::OnDirManagerClose(wxCommandEvent& event)
{
	EndDialog(wxID_CLOSE);
}

void DirectoryManagerDlg::OnRenameDir(wxCommandEvent& event)
{
	m_pSHCDView->RenameDir();
}

void DirectoryManagerDlg::OnDeleteDir(wxCommandEvent& event)
{
	m_pSHCDView->DeleteDir();
}

void DirectoryManagerDlg::OnFindDir(wxCommandEvent& event)
{
	FindDir();
}

void DirectoryManagerDlg::FindDir()
{
	DlgMCDFind dlgFind(_gMainFrame);
	dlgFind.ShowModal();
	bool bMoveDir = dlgFind.IsMoveDir();
	dlgFind.Destroy();

	if (bMoveDir)
		EndDialog(wxID_CLOSE);
}

void DirectoryManagerDlg::OnAddDir(wxCommandEvent& event)
{
	m_pSHCDView->AddDir();
}