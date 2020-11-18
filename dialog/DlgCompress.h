///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __DLG_COMPRESS_H_INCLUDED
#define __DLG_COMPRESS_H_INCLUDED

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listctrl.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/statline.h>
#include <wx/gauge.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////////
/// Class DlgCompress
///////////////////////////////////////////////////////////////////////////////
class DlgCompress : public wxDialog
{
	private:
		std::vector<wxString> m_vCompressList;
		wxString m_strCompressedFile = wxT("");
		wxString m_strCompressType = wxT("");
		unsigned long m_ulTotalSize = 0;
		int m_iTotalItems = 0;
	protected:
		wxStaticText* m_staticText4;
		wxListCtrl* m_lstDatas;
		wxStaticText* m_staticText5;
		wxChoice* m_cmbCompressType;
		wxStaticText* m_staticText6;
		wxStaticText* m_staticCompressedFile;
		wxStaticLine* m_staticline1;
		wxStaticText* m_staticCurrentTxt;
		wxGauge* m_progressComTotal;
		wxStaticText* m_staticPercent;
		wxGauge* m_progressTotal;
		wxStaticText* m_staticTotal;
		wxButton* m_btnCancel;

		// Virtual event handlers, overide them in your derived class
		virtual void OnInitDialog( wxInitDialogEvent& event );
		virtual void OnChoice( wxCommandEvent& event );
		virtual void OnCancelClick( wxCommandEvent& event );


	public:
		DlgCompress( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Compress"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 879,430 ), long style = wxDEFAULT_DIALOG_STYLE );
		~DlgCompress();
		
public:
	void SetCompressInfo(const std::vector<wxString>& vCompressList, const wxString& strCompressedFile, const wxString& strCompressType);
	void SetCurrentFile(const wxString& strCurrentFile);
	void SetCurrentFileSize(unsigned long ulSize);
	void SetCurrentFileTotalSize(unsigned long ulTotalSize);
	void SetCompressTotal(int iTotal);
	
private:
	void OnCompressThreadEnd(wxThreadEvent& event);
};

#endif