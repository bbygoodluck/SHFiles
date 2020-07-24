///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DLG_RENAME_H__INCLUDED__
#define __DLG_RENAME_H__INCLUDED__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class DlgRename
///////////////////////////////////////////////////////////////////////////////
class DlgRename : public wxDialog
{
private:
	wxString m_strName = wxT("");
	wxString m_strExt  = wxT("");
	wxString m_strCurrentPath = wxT("");
	
protected:
	wxStaticText* m_staticText1;
	wxStaticText* m_staticCurrentPath;
	wxTextCtrl* m_txtName;
	wxStaticText* m_staticText3;
	wxTextCtrl* m_txtExt;
	wxButton* m_btnOK;
	wxButton* m_btnCancel;

	// Virtual event handlers, overide them in your derived class
	virtual void OnClose( wxCloseEvent& event );
	virtual void OnInitDialog( wxInitDialogEvent& event );
	virtual void OnBtnRenameOK( wxCommandEvent& event );
	virtual void OnBtnRenameCancel( wxCommandEvent& event );

public:
	DlgRename( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Rename"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 466,125 ), long style = wxDEFAULT_DIALOG_STYLE );
	~DlgRename();
		
public:
	void SetRenameInfo(const wxString& strCurrentPath, const wxString& strRenameInfo);
	wxString GetRenameInfo();

};

#endif