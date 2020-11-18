#include "../ginc.h"
#include "../compress/CompressImpl.h"
#include "DlgDeCompress.h"

DlgDeCompress::DlgDeCompress( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	SetIcon(wxIcon("wxwin"));
	
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_DECOMPRESS_MSG")), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_staticText1->Wrap( -1 );
	bSizer2->Add( m_staticText1, 0, wxALL, 5 );

	m_staticSelCompressFile = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticSelCompressFile->Wrap( -1 );
	bSizer2->Add( m_staticSelCompressFile, 1, wxALL, 5 );


	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText3 = new wxStaticText( this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_DECOMPRESSING")), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer3->Add( m_staticText3, 0, wxALL, 5 );

	m_staticCurrentDecompress = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_MIDDLE );
	m_staticCurrentDecompress->Wrap( -1 );
	bSizer3->Add( m_staticCurrentDecompress, 1, wxALL, 5 );


	bSizer1->Add( bSizer3, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	m_ExtractProgress = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	m_ExtractProgress->SetValue( 0 );
	bSizer4->Add( m_ExtractProgress, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 5 );


	bSizer1->Add( bSizer4, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	m_staticPercent = new wxStaticText( this, wxID_ANY, wxT("100%"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticPercent->Wrap( -1 );
	bSizer5->Add( m_staticPercent, 0, wxALIGN_RIGHT|wxSHAPED, 5 );


	bSizer1->Add( bSizer5, 0, wxEXPAND|wxRIGHT, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_btnCancel = new wxButton( this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_CANCEL")), wxDefaultPosition, wxDefaultSize, 0 );

	m_btnCancel->SetBitmap( wxArtProvider::GetBitmap( wxART_CLOSE, wxART_BUTTON ) );
	bSizer6->Add( m_btnCancel, 0, wxALIGN_RIGHT|wxALL, 5 );


	bSizer1->Add( bSizer6, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DlgDeCompress::OnInitDialog ) );
	m_btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDeCompress::OnCancel ), NULL, this );
	
	Bind(wxEVT_THREAD, &DlgDeCompress::OnDeCompressThreadEnd, this);
	CenterOnScreen();
}

DlgDeCompress::~DlgDeCompress()
{
	// Disconnect Events
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DlgDeCompress::OnInitDialog ) );
	m_btnCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDeCompress::OnCancel ), NULL, this );
	Unbind(wxEVT_THREAD, &DlgDeCompress::OnDeCompressThreadEnd, this);
}

void DlgDeCompress::OnInitDialog( wxInitDialogEvent& event )
{
	m_ExtractProgress->SetRange(100);
	m_staticSelCompressFile->SetLabelText(m_strDecompressedFile);
}

void DlgDeCompress::OnCancel( wxCommandEvent& event )
{
	theCompress->DoCancel();
}

void DlgDeCompress::SetDecompressInfo(const wxString& strDecompressedFile, const wxString& strDecompressDir, _MENU_EVENT_TYPE _menuType)
{
	m_strDecompressedFile = strDecompressedFile;
	m_strDecompressDir = strDecompressDir;
	
	m_menyType = _menuType;
	DoNeedCreateDir();
	
	theCompress->SetUnCompressedInfo(m_strDecompressedFile, m_strDecompressDir);
	theCompress->DoStart(this);
}

void DlgDeCompress::DoNeedCreateDir()
{
	if(m_menyType == _MENU_DECOMPRESS_MK_FOLDER)
	{
		wxString strName = theCommonUtil->GetFileName(m_strDecompressedFile, false);
		bool bAddSlash = theCommonUtil->Compare(m_strDecompressDir.Right(1), SLASH) == 0 ? false : true;
		
		wxString strCreatePath = m_strDecompressDir + (bAddSlash ? SLASH + strName : strName);
		if(!wxDirExists(strCreatePath))
		{
			wxMkDir(strCreatePath);
			m_strDecompressDir = strCreatePath;
		}
	}
}

void DlgDeCompress::OnDeCompressThreadEnd(wxThreadEvent& event)
{
	theCompress->ClearCompressInfo();
	EndModal(wxID_OK);
}

void DlgDeCompress::SetExtractTotal(unsigned long ulTotal)
{
	m_ulTotal = ulTotal;
}

void DlgDeCompress::SetExtractCurrent(unsigned long ulCurrent)
{
	int iPercent = (ulCurrent * 100) / m_ulTotal;
	m_ExtractProgress->SetValue(iPercent);
	m_staticPercent->SetLabelText(wxString::Format(wxT("%d%"), iPercent));
}

void DlgDeCompress::SetExtractCurrentFile(const wxString& strFileName)
{
	m_staticCurrentDecompress->SetLabelText(strFileName);
}