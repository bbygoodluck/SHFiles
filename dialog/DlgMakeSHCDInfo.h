#ifndef __DLG_MAKE_SHCDINFO_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class DlgMakeSHCDInfo
///////////////////////////////////////////////////////////////////////////////
//class DirectoryManagerDlg;
class CSCDFrame;
class DlgMakeSHCDInfo : public wxDialog
{
protected:
	wxStaticText* m_staticText1;
	wxButton* m_btnCancel;

	// Virtual event handlers, overide them in your derived class
	virtual void OnClose(wxCloseEvent& event);
	virtual void OnInitDialog(wxInitDialogEvent& event);
	virtual void OnBtnCancelClick(wxCommandEvent& event);

public:
	DlgMakeSHCDInfo(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Make Directory Information....."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(511, 100), long style = wxDEFAULT_DIALOG_STYLE);
	~DlgMakeSHCDInfo();

public:
	bool IsClose() { return m_bClose; }
	bool IsCancel() { return m_bClose; }

private:
	wxWindow* m_pParent;

	bool m_bClose;
private:
	void OnMakeTreeInfoThreadEvent(wxThreadEvent& event);
	wxDECLARE_EVENT_TABLE();
};

#endif