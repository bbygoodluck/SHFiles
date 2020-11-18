#include "../ginc.h"
#include "../interface/SHCD.h"
#include "DlgMCDFind.h"
#include "../view/ListView.h"
#include "../engine/LocalFileSystem.h"
#include <wx/regex.h>

DlgMCDFind::DlgMCDFind(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
	: wxDialog(parent, id, title, pos, size, style)
{
	SetIcon(wxIcon("wxwin"));
	CenterOnScreen();

	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText1 = new wxStaticText(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_SCD_FIND_DIR_MSG")), wxPoint(-1, -1), wxDefaultSize, 0);
	m_staticText1->Wrap(-1);
	bSizer2->Add(m_staticText1, 0, wxLEFT | wxTOP, 8);

	m_txtFind = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxPoint(-1, -1), wxSize(-1, 23), wxTE_PROCESS_ENTER | wxBORDER_SIMPLE);
	m_txtFind->SetFont(*_gFont);// wxFont(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString));

	bSizer2->Add(m_txtFind, 1, wxBOTTOM | wxLEFT | wxRIGHT | wxTOP, 5);

	m_btnFind = new wxButton(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_FIND_SEARCH_GO")), wxDefaultPosition, wxSize(-1, -1), 0);

	m_btnFind->SetBitmap(wxArtProvider::GetBitmap(wxART_FIND, wxART_MENU));
	bSizer2->Add(m_btnFind, 0, wxRIGHT, 5);

	m_btnSearchCancel = new wxButton(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_FIND_SEARCH_CANCEL")), wxDefaultPosition, wxDefaultSize, 0);

	m_btnSearchCancel->SetBitmap(wxArtProvider::GetBitmap(wxART_MISSING_IMAGE, wxART_BUTTON));
	bSizer2->Add(m_btnSearchCancel, 0, 0, 5);


	bSizer1->Add(bSizer2, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer(wxHORIZONTAL);

	m_radioBtnInclude = new wxRadioButton(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_FIND_INPUT_INC")), wxDefaultPosition, wxDefaultSize, 0);
	bSizer3->Add(m_radioBtnInclude, 0, wxALL, 5);

	m_radioBtnEqual = new wxRadioButton(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_FIND_INPUT_MATCH")), wxDefaultPosition, wxDefaultSize, 0);
	bSizer3->Add(m_radioBtnEqual, 0, wxALL, 5);

	m_staticFindingText = new wxStaticText(this, wxID_ANY, wxT("111"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticFindingText->SetFont(*_gFont);
	m_staticFindingText->Wrap(-1);
	m_staticFindingText->SetForegroundColour(wxColour(14, 64, 101));

	bSizer3->Add(m_staticFindingText, 1, wxALL, 5);

	bSizer1->Add(bSizer3, 0, wxEXPAND, 5);

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer(wxVERTICAL);

	m_listBox1 = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL | wxLB_SINGLE);
	m_listBox1->SetFont(*_gFont);
	m_listBox1->SetForegroundColour(wxColour(70, 70, 70));
	m_listBox1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_SCROLLBAR));

	bSizer4->Add(m_listBox1, 1, wxALL | wxEXPAND, 5);


	bSizer1->Add(bSizer4, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxHORIZONTAL);

	m_btnClose = new wxButton(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_OPTION_BTN_CLOSE")), wxDefaultPosition, wxDefaultSize, 0);

	m_btnClose->SetBitmap(wxArtProvider::GetBitmap(wxART_QUIT, wxART_BUTTON));
	bSizer6->Add(m_btnClose, 0, wxALL, 5);


	bSizer5->Add(bSizer6, 0, wxALIGN_RIGHT, 5);


	bSizer1->Add(bSizer5, 0, wxEXPAND, 5);


	this->SetSizer(bSizer1);
	this->Layout();

	this->Centre(wxBOTH);

	// Connect Events
	this->Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(DlgMCDFind::OnInitDialog));
	this->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(DlgMCDFind::OnKeyDown));
	m_txtFind->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(DlgMCDFind::OnTextEnter), NULL, this);
	m_btnFind->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DlgMCDFind::OnFind), NULL, this);
	m_txtFind->Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(DlgMCDFind::OnTextCharHook), NULL, this);
	m_btnSearchCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DlgMCDFind::OnFindCancel), NULL, this);
	m_radioBtnInclude->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(DlgMCDFind::OnRdo_IncludeClick), NULL, this);
	m_radioBtnEqual->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(DlgMCDFind::OnRdo_EqualClick), NULL, this);
	m_btnClose->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DlgMCDFind::OnClose), NULL, this);
	m_listBox1->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(DlgMCDFind::m_listBox1OnLeftDClick), NULL, this);
	m_listBox1->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(DlgMCDFind::OnLstBoxKeyDown), NULL, this);
}

DlgMCDFind::~DlgMCDFind()
{
	// Disconnect Events
	this->Disconnect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(DlgMCDFind::OnInitDialog));
	this->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(DlgMCDFind::OnKeyDown));
	m_txtFind->Disconnect(wxEVT_CHAR_HOOK, wxKeyEventHandler(DlgMCDFind::OnTextCharHook), NULL, this);
	m_txtFind->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(DlgMCDFind::OnTextEnter), NULL, this);
	m_btnFind->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DlgMCDFind::OnFind), NULL, this);
	m_btnSearchCancel->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DlgMCDFind::OnFindCancel), NULL, this);
	m_radioBtnInclude->Disconnect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(DlgMCDFind::OnRdo_IncludeClick), NULL, this);
	m_radioBtnEqual->Disconnect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(DlgMCDFind::OnRdo_EqualClick), NULL, this);
	m_btnClose->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DlgMCDFind::OnClose), NULL, this);
	m_listBox1->Disconnect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(DlgMCDFind::m_listBox1OnLeftDClick), NULL, this);
	m_listBox1->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(DlgMCDFind::OnLstBoxKeyDown), NULL, this);
}

void DlgMCDFind::OnInitDialog(wxInitDialogEvent& event)
{
	m_bFindCancel = false;
	m_bClose = false;
	m_bMoveDir = false;

	m_bFindInclude = true;
	m_radioBtnInclude->SetValue(true);
	m_radioBtnEqual->SetValue(false);
	m_btnSearchCancel->Enable(false);
	
	m_staticFindingText->SetLabelText(wxT(""));
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		wxMessageBox(wxT("Could not create the worker thread!"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR, this);
		return;
	}

	GetThread()->Run();
}

void DlgMCDFind::OnTextEnter(wxCommandEvent& event)
{
	SearchStart();
}

void DlgMCDFind::OnFind(wxCommandEvent& event)
{
	SearchStart();
}

void DlgMCDFind::SearchStart()
{
	int iLstItemCnt = m_listBox1->GetCount();
	for (int iIndex = 0; iIndex < iLstItemCnt; iIndex++)
		m_listBox1->Delete(0);

	m_strFindDirName = m_txtFind->GetLineText(1);
	if (m_strFindDirName.IsEmpty())
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_DLG_FIND_INPUT_SEARCHWORD")), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
		return;
	}

	m_staticFindingText->SetLabelText(wxT("[ ") + m_strFindDirName + wxT(" ]") + wxT(" is Searching......."));
	m_btnFind->Enable(false);
	m_btnSearchCancel->Enable(true);
	m_btnClose->Enable(false);

	wxString strRegEx(wxT(""));
	if (m_bFindInclude)
		strRegEx = wxT(".*");

	m_strRegEx = m_strFindDirName + strRegEx;
	m_Lock.getCondition()->Signal();
}

void DlgMCDFind::OnFindCancel(wxCommandEvent& event)
{
	m_bFindCancel = true;
	m_txtFind->SetLabelText(wxT(""));

	m_staticFindingText->SetLabelText(m_strFindDirName + wxT(" is canceled"));
}

void DlgMCDFind::OnRdo_IncludeClick(wxCommandEvent& event)
{
	m_bFindInclude = true;
	m_radioBtnInclude->SetValue(true);
	m_radioBtnEqual->SetValue(false);
}

void DlgMCDFind::OnRdo_EqualClick(wxCommandEvent& event)
{
	m_bFindInclude = false;
	m_radioBtnInclude->SetValue(false);
	m_radioBtnEqual->SetValue(true);

}
void DlgMCDFind::OnClose(wxCommandEvent& event)
{
	m_bClose = true;
	m_Lock.getCondition()->Signal();

	if (GetThread() && GetThread()->IsRunning())
		GetThread()->Wait();

	EndDialog(wxID_CLOSE);
}

wxThread::ExitCode DlgMCDFind::Entry()
{
	while (true)
	{
		m_Lock.getCondition()->Wait();
		if (m_bClose)
			break;

		wxRegEx re(m_strRegEx);
		wxVector<NODE *>::iterator iter = theSHCD->GetBegin();
		wxString strName(wxT(""));
		wxString strFullPath(wxT(""));

		for (iter; iter != theSHCD->GetEnd(); ++iter)
		{
			strName = (*iter)->_Name;
			if (re.Matches(strName))
			{
				strFullPath = theSHCD->GetFullPath((*iter));
				m_listBox1->Append(strFullPath);
			}

			if (m_bFindCancel)
				break;
		}

		m_txtFind->SetLabelText(wxT(""));
		m_staticFindingText->SetLabelText(wxT("[ ") + m_strFindDirName + wxT(" ]") + wxT(" Searching is completed......."));
		
		m_bFindCancel = false;
		m_btnFind->Enable(true);
		m_btnClose->Enable(true);
		m_btnSearchCancel->Enable(false);
	}

	return (wxThread::ExitCode)0;
}

void DlgMCDFind::m_listBox1OnLeftDClick(wxMouseEvent& event)
{
	m_bMoveDir = false;
	int iSel = m_listBox1->GetSelection();
	wxString strPath = m_listBox1->GetString(iSel);
	if (!CLocalFileSystem::IsCanReadDir(strPath))
	{
		wxMessageBox(strPath + wxT(" cannot read directory"), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}

	wxString strMsg = wxString::Format(theMsgManager->GetMessage(wxT("MSG_MCD_FIND_AND_MOVE_DIR")), strPath);
	int iRet = wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxICON_QUESTION | wxYES_NO);
	if (iRet == wxYES)
	{
		m_bMoveDir = true;
		theSplitterManager->GetActiveTab()->GetActiveViewPanel()->ReloadPathOfView(strPath);
		EndDialog(wxID_CLOSE);
	}
}

void DlgMCDFind::OnKeyDown(wxKeyEvent& event)
{
	ProcessKeyEvent(event.GetKeyCode());
	event.Skip();
}

void DlgMCDFind::OnTextCharHook(wxKeyEvent& event)
{
	ProcessKeyEvent(event.GetKeyCode());
	event.Skip();
}

void DlgMCDFind::ProcessKeyEvent(int iKeyCode)
{
	if (iKeyCode == WXK_ESCAPE)
	{
		m_bMoveDir = false;
		EndDialog(wxID_CLOSE);
	}
}

void DlgMCDFind::OnLstBoxKeyDown(wxKeyEvent& event)
{
	ProcessKeyEvent(event.GetKeyCode());
	event.Skip();
}