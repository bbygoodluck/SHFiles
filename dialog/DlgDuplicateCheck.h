#ifndef __DLG_DUPLICATE_CHECK_H_INCLUDED__
#define __DLG_DUPLICATE_CHECK_H_INCLUDED__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/radiobut.h>
#include <wx/checkbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_SKIP 1000
#define wxID_OVERWIRTE 1001
#define wxID_RENAME 1002

///////////////////////////////////////////////////////////////////////////////
/// Class DlgDuplicateCheck
///////////////////////////////////////////////////////////////////////////////
class DlgDuplicateCheck : public wxDialog
{
private:
	int m_iDeCompressID = 0;
	bool m_bAllSame = false;
	
protected:
	wxStaticText* m_staticText1;
	wxStaticLine* m_staticline1;
	wxStaticText* m_staticSameFile;
	wxStaticText* m_staticOldSize;
	wxStaticText* m_staticOldTime;
	wxStaticText* m_staticDecompressSize;
	wxStaticText* m_staticDecompressTime;
	wxStaticText* m_staticText7;
	wxRadioButton* m_radioBtnSkip;
	wxRadioButton* m_radioBtnOverwrite;
	wxRadioButton* m_radioBtnRename;
	wxStaticLine* m_staticline2;
	wxCheckBox* m_checkBoxAll;
	wxButton* m_btnConfirm;
	wxButton* m_btnCancel;

	// Virtual event handlers, overide them in your derived class
	virtual void OnInitDialog( wxInitDialogEvent& event );
	virtual void OnRadioCheck( wxCommandEvent& event );
	virtual void OnCheckAll( wxCommandEvent& event );
	virtual void OnConfirmClick( wxCommandEvent& event );
	virtual void OnCancel( wxCommandEvent& event );

public:
	DlgDuplicateCheck( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = theMsgManager->GetMessage(wxT("MSG_DECOMPRESS_DLG_TITLE")), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 650,361 ), long style = wxDEFAULT_DIALOG_STYLE );
	~DlgDuplicateCheck();

public:
	void SetDuplicateFile(const wxString& strFullPathName, const wxString& strFileName, unsigned long ulDosDate, unsigned long ulUncompressedSize);
	
};

#endif