#include "../ginc.h"
#include "DlgMakeSHCDInfo.h"
//#include "DirectoryManagerDlg.h"

wxBEGIN_EVENT_TABLE(DlgMakeSHCDInfo, wxDialog)
	EVT_THREAD(SCD_MAKE_TREE_INFO_THREAD_EVENT, DlgMakeSHCDInfo::OnMakeTreeInfoThreadEvent)
wxEND_EVENT_TABLE()

DlgMakeSHCDInfo::DlgMakeSHCDInfo(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) 
	: wxDialog(parent, id, title, pos, size, style)
	, m_pParent(parent)
	, m_bClose(false)
{
	SetIcon(wxIcon("wxwin"));
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxVERTICAL);

	m_staticText1 = new wxStaticText(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_READING_DIRECTORYINFO")), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1->Wrap(-1);
	bSizer1->Add(m_staticText1, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxHORIZONTAL);

	m_btnCancel = new wxButton(this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DLG_OPTION_BTN_CANCEL")), wxDefaultPosition, wxDefaultSize, 0);

	m_btnCancel->SetBitmap(wxArtProvider::GetBitmap(wxART_MISSING_IMAGE, wxART_BUTTON));
	bSizer2->Add(m_btnCancel, 0, wxALL, 5);


	bSizer1->Add(bSizer2, 1, wxALIGN_RIGHT, 5);


	this->SetSizer(bSizer1);
	this->Layout();

	this->Centre(wxBOTH);

	// Connect Events
	this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(DlgMakeSHCDInfo::OnClose));
	this->Connect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(DlgMakeSHCDInfo::OnInitDialog));
	m_btnCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DlgMakeSHCDInfo::OnBtnCancelClick), NULL, this);
}

DlgMakeSHCDInfo::~DlgMakeSHCDInfo()
{
	// Disconnect Events
	this->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(DlgMakeSHCDInfo::OnClose));
	this->Disconnect(wxEVT_INIT_DIALOG, wxInitDialogEventHandler(DlgMakeSHCDInfo::OnInitDialog));
	m_btnCancel->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DlgMakeSHCDInfo::OnBtnCancelClick), NULL, this);
}

void DlgMakeSHCDInfo::OnInitDialog(wxInitDialogEvent& event)
{
	m_bClose = false;
}

void DlgMakeSHCDInfo::OnClose(wxCloseEvent& event)
{
	m_bClose = true;
}

void DlgMakeSHCDInfo::OnMakeTreeInfoThreadEvent(wxThreadEvent& event)
{
	int iNum = event.GetInt();
	if (iNum == -1)
	{
		wxMilliSleep(100);
		EndDialog(wxID_CLOSE);
	}
}

void DlgMakeSHCDInfo::OnBtnCancelClick(wxCommandEvent& event)
{
	m_bClose = true;
}