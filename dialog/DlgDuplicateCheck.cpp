#include "../ginc.h"
#include "DlgDuplicateCheck.h"

DlgDuplicateCheck::DlgDuplicateCheck( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	SetIcon(wxIcon("wxwin"));
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_MAIN")), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer2->Add( m_staticText1, 0, wxALL|wxEXPAND, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer2->Add( m_staticline1, 0, wxEXPAND|wxTOP, 5 );


	bSizer1->Add( bSizer2, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	m_staticSameFile = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticSameFile->Wrap( -1 );
	m_staticSameFile->SetForegroundColour( wxColour( 0, 0, 255 ) );

	bSizer3->Add( m_staticSameFile, 0, wxALL|wxEXPAND, 5 );


	bSizer1->Add( bSizer3, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_EXIST_FILE_TITLE")) ), wxVERTICAL );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	m_staticOldSize = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticOldSize->Wrap( -1 );
	bSizer5->Add( m_staticOldSize, 0, wxALL|wxEXPAND, 5 );

	m_staticOldTime = new wxStaticText( sbSizer1->GetStaticBox(), wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticOldTime->Wrap( -1 );
	bSizer5->Add( m_staticOldTime, 0, wxALL|wxEXPAND, 5 );


	sbSizer1->Add( bSizer5, 1, wxEXPAND, 5 );


	bSizer4->Add( sbSizer1, 1, wxEXPAND, 5 );


	bSizer4->Add( 10, 0, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_DECOMPRESS_TITLE")) ), wxVERTICAL );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_staticDecompressSize = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticDecompressSize->Wrap( -1 );
	bSizer6->Add( m_staticDecompressSize, 0, wxALL|wxEXPAND, 5 );

	m_staticDecompressTime = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticDecompressTime->Wrap( -1 );
	bSizer6->Add( m_staticDecompressTime, 0, wxALL|wxEXPAND, 5 );


	sbSizer2->Add( bSizer6, 1, wxEXPAND, 5 );


	bSizer4->Add( sbSizer2, 1, wxEXPAND, 5 );


	bSizer1->Add( bSizer4, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	m_staticText7 = new wxStaticText( this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_TO_DO_WORK")), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	bSizer7->Add( m_staticText7, 0, wxALL, 5 );

	m_radioBtnSkip = new wxRadioButton( this, wxID_SKIP, theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_TO_DO_WORK_SKIP")), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_radioBtnSkip, 0, wxALL, 5 );

	m_radioBtnOverwrite = new wxRadioButton( this, wxID_OVERWIRTE, theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_TO_DO_WORK_OVERWRITE")), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioBtnOverwrite->SetValue( true );
	bSizer7->Add( m_radioBtnOverwrite, 0, wxALL, 5 );

	m_radioBtnRename = new wxRadioButton( this, wxID_RENAME, theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_TO_DO_WORK_RENAME")), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_radioBtnRename, 0, wxALL, 5 );


	bSizer1->Add( bSizer7, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer8->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );


	bSizer1->Add( bSizer8, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );

	m_checkBoxAll = new wxCheckBox( this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_ALL_FILE_SAME")), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer10->Add( m_checkBoxAll, 0, wxALL, 5 );


	bSizer9->Add( bSizer10, 1, wxALL, 5 );

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );

	m_btnConfirm = new wxButton( this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_CONFIRM")), wxDefaultPosition, wxDefaultSize, 0 );

	m_btnConfirm->SetBitmap( wxArtProvider::GetBitmap( wxART_GO_FORWARD, wxART_TOOLBAR ) );
	bSizer11->Add( m_btnConfirm, 0, wxALL, 5 );

	m_btnCancel = new wxButton( this, wxID_ANY, theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_CANCEL")), wxDefaultPosition, wxDefaultSize, 0 );

	m_btnCancel->SetBitmap( wxArtProvider::GetBitmap( wxART_CLOSE, wxART_TOOLBAR ) );
	bSizer11->Add( m_btnCancel, 0, wxALL, 5 );


	bSizer9->Add( bSizer11, 0, 0, 5 );


	bSizer1->Add( bSizer9, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DlgDuplicateCheck::OnInitDialog ) );
	m_radioBtnSkip->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( DlgDuplicateCheck::OnRadioCheck ), NULL, this );
	m_radioBtnOverwrite->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( DlgDuplicateCheck::OnRadioCheck ), NULL, this );
	m_radioBtnRename->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( DlgDuplicateCheck::OnRadioCheck ), NULL, this );
	m_checkBoxAll->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DlgDuplicateCheck::OnCheckAll ), NULL, this );
	m_btnConfirm->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDuplicateCheck::OnConfirmClick ), NULL, this );
	m_btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDuplicateCheck::OnCancel ), NULL, this );
	
	CenterOnScreen();
}

DlgDuplicateCheck::~DlgDuplicateCheck()
{
	// Disconnect Events
	this->Disconnect( wxEVT_INIT_DIALOG, wxInitDialogEventHandler( DlgDuplicateCheck::OnInitDialog ) );
	m_radioBtnSkip->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( DlgDuplicateCheck::OnRadioCheck ), NULL, this );
	m_radioBtnOverwrite->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( DlgDuplicateCheck::OnRadioCheck ), NULL, this );
	m_radioBtnRename->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( DlgDuplicateCheck::OnRadioCheck ), NULL, this );
	m_checkBoxAll->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( DlgDuplicateCheck::OnCheckAll ), NULL, this );
	m_btnConfirm->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDuplicateCheck::OnConfirmClick ), NULL, this );
	m_btnCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DlgDuplicateCheck::OnCancel ), NULL, this );

}

void DlgDuplicateCheck::OnInitDialog( wxInitDialogEvent& event )
{
	m_iDeCompressID = wxID_OVERWIRTE;
}

void DlgDuplicateCheck::OnRadioCheck( wxCommandEvent& event )
{
	m_iDeCompressID = event.GetId();
}

void DlgDuplicateCheck::OnCheckAll( wxCommandEvent& event )
{
	bool bChecked = event.IsChecked();
	m_bAllSame = bChecked;
}

void DlgDuplicateCheck::OnConfirmClick( wxCommandEvent& event )
{
	switch(m_iDeCompressID)
	{
		case wxID_SKIP:
			theCompress->SetDeCompressType(COMPTYPE_UNZIP_SKIP);
			break;
			
		case wxID_OVERWIRTE:
			theCompress->SetDeCompressType(COMPTYPE_UNZIP_OVERWRITE);
			break;
			
		case wxID_RENAME:
			theCompress->SetDeCompressType(COMPTYPE_UNZIP_RENAME);
			break;
	}
	
	theCompress->SetAllDeCompressSame(m_bAllSame);
	EndModal(wxID_OK);
}

void DlgDuplicateCheck::OnCancel( wxCommandEvent& event )
{
	int iRetID = wxMessageBox(theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_DECOMPRESS_STOP")), PROGRAM_FULL_NAME, wxICON_QUESTION | wxYES_NO);
	if(iRetID != wxYES)
		return;
		
	EndModal(wxID_CANCEL);
}

void DlgDuplicateCheck::SetDuplicateFile(const wxString& strFullPathName, const wxString& strFileName, unsigned long ulDosDate, unsigned long ulUncompressedSize)
{
	m_staticSameFile->SetLabelText(strFullPathName);
	wxFileName fn(strFullPathName);
	
	wxString strExt = fn.GetExt();
	wxString strName = theCommonUtil->GetFileName(strFullPathName, false);
	
	strName = strName + wxT("(2).") + strExt;
	m_radioBtnRename->SetLabelText(m_radioBtnRename->GetLabelText() + wxT(" : ") + strName);
	
	unsigned long ulExistedFileSize = fn.GetSize().ToULong();
	wxString strExistedFile(wxT(""));
	wxString strSizeType(wxT(""));
	wxColour color(wxColour(192, 192, 192));
	
	theCommonUtil->GetSizeInfo((double)ulExistedFileSize, strExistedFile, strSizeType, color);
	
	wxDateTime dtModify = fn.GetModificationTime();

	int iYear  = dtModify.GetYear();
	int iMonth = dtModify.GetMonth() + 1;
	int iDay   = dtModify.GetDay();
	int iHour  = dtModify.GetHour();
	int iMin   = dtModify.GetMinute();
	int iSec   = dtModify.GetSecond();
	
	wxString strOldTime = wxString::Format(wxT("%04d-%02d-%02d %02d:%02d:%02d"), iYear, iMonth, iDay, iHour, iMin, iSec);
	
	m_staticOldSize->SetLabelText(strExistedFile + wxT(" ") + strSizeType);
	m_staticOldTime->SetLabelText(strOldTime);
	
	wxDateTime dtCompress;
	dtCompress.SetFromDOS(ulDosDate);
	
	wxString strCompressedFile(wxT(""));
	wxString strCompressedFileType(wxT(""));
	
	theCommonUtil->GetSizeInfo((double)ulUncompressedSize, strCompressedFile, strCompressedFileType, color);
	
	iYear  = dtCompress.GetYear();
	iMonth = dtCompress.GetMonth() + 1;
	iDay   = dtCompress.GetDay();
	iHour  = dtCompress.GetHour();
	iMin   = dtCompress.GetMinute();
	iSec   = dtCompress.GetSecond();
	
	wxString strCompressTime = wxString::Format(wxT("%04d-%02d-%02d %02d:%02d:%02d"), iYear, iMonth, iDay, iHour, iMin, iSec);
	
	m_staticDecompressSize->SetLabelText(strCompressedFile + wxT(" ") + strCompressedFileType);
	m_staticDecompressTime->SetLabelText(strCompressTime);
}