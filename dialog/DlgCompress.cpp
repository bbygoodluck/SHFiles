///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include "../ginc.h"
#include "../compress/CompressImpl.h"
#include "DlgCompress.h"

///////////////////////////////////////////////////////////////////////////
DlgCompress::DlgCompress( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	SetIcon(wxIcon("wxwin"));
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Compress Lists : "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer7->Add( m_staticText4, 0, wxALL, 5 );

	m_lstDatas = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer7->Add( m_lstDatas, 1, wxALL|wxEXPAND, 5 );


	bSizer1->Add( bSizer7, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Compress Type : "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer8->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxArrayString m_cmbCompressTypeChoices;
	
	std::vector<wxString> vCompressType = theCompress->GetCompressList();
	for(auto item : vCompressType)
		m_cmbCompressTypeChoices.Add(item);
		
	m_cmbCompressType = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_cmbCompressTypeChoices, 0 );
	m_cmbCompressType->SetSelection( 0 );
	bSizer8->Add( m_cmbCompressType, 0, wxALIGN_CENTER_VERTICAL, 5 );


	bSizer1->Add( bSizer8, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Compressed File : "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer9->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_staticCompressedFile = new wxStaticText( this, wxID_ANY, wxT("C:\\aaa.zip"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticCompressedFile->Wrap( -1 );
	bSizer9->Add( m_staticCompressedFile, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );


	bSizer1->Add( bSizer9, 0, wxEXPAND, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	m_staticCurrentTxt = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticCurrentTxt->Wrap( -1 );
	bSizer2->Add( m_staticCurrentTxt, 0, wxALL|wxEXPAND, 5 );

	m_progressComTotal = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( -1,18 ), wxGA_HORIZONTAL );
	m_progressComTotal->SetValue( 0 );
	bSizer2->Add( m_progressComTotal, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	m_staticPercent = new wxStaticText( this, wxID_ANY, wxT("100%"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticPercent->Wrap( -1 );
	bSizer4->Add( m_staticPercent, 1, wxRIGHT, 5 );


	bSizer2->Add( bSizer4, 1, wxALIGN_RIGHT|wxEXPAND|wxSHAPED, 5 );


	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	m_progressTotal = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( -1,18 ), wxGA_HORIZONTAL );
	m_progressTotal->SetValue( 0 );
	bSizer5->Add( m_progressTotal, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_staticTotal = new wxStaticText( this, wxID_ANY, wxT("(10 / 30)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTotal->Wrap( -1 );
	bSizer6->Add( m_staticTotal, 1, 0, 5 );


	bSizer5->Add( bSizer6, 1, wxALIGN_RIGHT|wxSHAPED, 5 );


	bSizer1->Add( bSizer5, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_btnCancel = new wxButton( this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_CANCEL")), wxDefaultPosition, wxSize( -1,30 ), 0 );
	bSizer3->Add( m_btnCancel, 0, wxALL, 5 );


	bSizer1->Add( bSizer3, 0, wxALIGN_RIGHT, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DlgCompress::OnInitDialog ) );
	m_cmbCompressType->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DlgCompress::OnChoice ), NULL, this );
	m_btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgCompress::OnCancelClick ), NULL, this );
	
	Bind(wxEVT_THREAD, &DlgCompress::OnCompressThreadEnd, this);
	CenterOnScreen();
}

DlgCompress::~DlgCompress()
{
	// Disconnect Events
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DlgCompress::OnInitDialog ) );
	m_cmbCompressType->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DlgCompress::OnChoice ), NULL, this );
	m_btnCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgCompress::OnCancelClick ), NULL, this );
	Unbind(wxEVT_THREAD, &DlgCompress::OnCompressThreadEnd, this);
}

void DlgCompress::OnInitDialog( wxInitDialogEvent& event )
{
	m_progressComTotal->SetRange(100);
	m_progressTotal->SetRange(m_iTotalItems);
	
	wxString strCompressTotal = wxString::Format(wxT("(0 / %d)"), m_iTotalItems);
	m_staticTotal->SetLabelText(strCompressTotal);        
}

void DlgCompress::SetCompressInfo(const std::vector<wxString>& vCompressList, const wxString& strCompressedFile, const wxString& strCompressType)
{
	m_vCompressList = vCompressList;
	m_strCompressedFile = strCompressedFile;
	m_strCompressType = strCompressType;
	
	m_lstDatas->DeleteAllItems();
	m_lstDatas->AppendColumn(wxT("Compress Items"), wxLIST_FORMAT_LEFT, 760);
	
	int iIndex = 0;
	for(auto item : m_vCompressList)
	{
		m_lstDatas->InsertItem(iIndex, item);
		iIndex++;
	}
	
	m_iTotalItems = m_vCompressList.size();
	
	m_staticCompressedFile->SetLabelText(m_strCompressedFile);
	
	int iTypeCount = m_cmbCompressType->GetCount();
	for(int i = 0; i < iTypeCount; i++)
	{
		wxString strCompType = m_cmbCompressType->GetString(i);
		if(strCompType.CmpNoCase(m_strCompressType) == 0)
		{
			m_cmbCompressType->SetSelection(i);
			break;
		}
	}
	
	theCompress->SetCompressInfo(m_vCompressList, m_strCompressedFile);
	theCompress->DoStart(this);
}

void DlgCompress::OnChoice( wxCommandEvent& event )
{
	
}

void DlgCompress::OnCancelClick( wxCommandEvent& event )
{
	theCompress->DoCancel();
}

void DlgCompress::SetCurrentFile(const wxString& strCurrentFile)
{
	m_staticCurrentTxt->SetLabelText(strCurrentFile);
}

void DlgCompress::SetCurrentFileSize(unsigned long ulSize)
{
	int iPercent = ((ulSize * 100) / m_ulTotalSize);
	
	m_progressComTotal->SetValue(iPercent);
	
	wxString strFile = wxString::Format(wxT("%3d%"), iPercent);
	m_staticPercent->SetLabelText(strFile);//strSize);
}

void DlgCompress::SetCurrentFileTotalSize(unsigned long ulTotalSize)
{
	m_ulTotalSize = ulTotalSize;
}

void DlgCompress::SetCompressTotal(int iTotal)
{
	wxString strCompressTotal = wxString::Format(wxT("(%d / %d)"), iTotal, m_iTotalItems);
	m_staticTotal->SetLabelText(strCompressTotal);
	
	m_progressTotal->SetValue(iTotal);
}

void DlgCompress::OnCompressThreadEnd(wxThreadEvent& event)
{
	theCompress->ClearCompressInfo();
	EndModal(wxID_OK);
}