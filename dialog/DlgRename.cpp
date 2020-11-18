///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////
#include "../ginc.h"
#include "DlgRename.h"

///////////////////////////////////////////////////////////////////////////

DlgRename::DlgRename( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("현재 디렉토리 : "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer2->Add( m_staticText1, 0, wxALL, 5 );

	m_staticCurrentPath = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticCurrentPath->Wrap( -1 );
	bSizer2->Add( m_staticCurrentPath, 1, wxALL, 5 );


	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	m_txtName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_txtName, 1, wxALL, 5 );

	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer4->Add( m_staticText3, 0, wxALIGN_BOTTOM|wxBOTTOM, 5 );

	m_txtExt = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 80,-1 ), 0 );
	bSizer4->Add( m_txtExt, 0, wxALL, 5 );


	bSizer1->Add( bSizer4, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	m_btnOK = new wxButton( this, wxID_ANY, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );

	m_btnOK->SetBitmap( wxArtProvider::GetBitmap( wxART_GO_FORWARD, wxART_HELP_BROWSER ) );
	bSizer6->Add( m_btnOK, 0, wxALL, 5 );

	m_btnCancel = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );

	m_btnCancel->SetBitmap( wxArtProvider::GetBitmap( wxART_QUIT, wxART_HELP_BROWSER ) );
	bSizer6->Add( m_btnCancel, 0, wxALL, 5 );


	bSizer1->Add( bSizer6, 0, wxALIGN_RIGHT, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DlgRename::OnClose ) );
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DlgRename::OnInitDialog ) );
	m_btnOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgRename::OnBtnRenameOK ), NULL, this );
	m_btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgRename::OnBtnRenameCancel ), NULL, this );
}

DlgRename::~DlgRename()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( DlgRename::OnClose ) );
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DlgRename::OnInitDialog ) );
	m_btnOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgRename::OnBtnRenameOK ), NULL, this );
	m_btnCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgRename::OnBtnRenameCancel ), NULL, this );
}

void DlgRename::OnClose( wxCloseEvent& event )
{
	
}

void DlgRename::OnInitDialog( wxInitDialogEvent& event )
{
	m_staticCurrentPath->SetLabelText(m_strCurrentPath);
	m_txtName->SetLabelText(m_strName);
	m_txtExt->SetLabelText(m_strExt);
	
	m_txtName->SetSelection(0, m_strName.Len());
	m_txtName->SetFocus();
}

void DlgRename::OnBtnRenameOK( wxCommandEvent& event )
{
	
}

void DlgRename::OnBtnRenameCancel( wxCommandEvent& event )
{
	
}

void DlgRename::SetRenameInfo(const wxString& strCurrentPath, const wxString& strRenameInfo)
{
	m_strCurrentPath = strCurrentPath;
	
	wxString strTmp(strRenameInfo);
	wxString strExt = theCommonUtil->GetExt(strRenameInfo);
	wxString strName(strRenameInfo);
	
	int iExtLen = strExt.Len();
	if(!strExt.IsEmpty())
		strName = strName.Left(strRenameInfo.Left(iExtLen) - 1);

	
}

wxString DlgRename::GetRenameInfo()
{
	wxString strReturn(m_strCurrentPath);
	if(theCommonUtil->Compare(SLASH, m_strCurrentPath.Right(1)) == 0)
		strReturn += m_strName;
	else
	{
		strReturn += SLASH;
		strReturn += m_strName;
	}
	
	if(!m_strExt.IsEmpty())
	{
		strReturn += wxT(".");
		strReturn += m_strExt;
	}
	
	return strReturn;
}