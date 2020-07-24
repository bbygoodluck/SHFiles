#ifndef _INFO_DISPLAY_VIEW_H
#define _INFO_DISPLAY_VIEW_H
/********************************************************************************
// class Name : CInfoDisplayView
// ¼³      ¸í : ¼±ÅÃÇÑ ÆÄÀÏ(¶Ç´Â Æú´õ)ÀÇ »ó¼¼ Á¤º¸ º¸±â
*********************************************************************************/
#include <wx/tooltip.h>
class CDetailInfoView : public wxWindow
{
public:
	explicit CDetailInfoView();
	explicit CDetailInfoView(wxWindow* parent, const int nID, const wxPoint& pt, const wxSize& sz, long lStyle = CHILD_WINDOW_STYLE);
	virtual ~CDetailInfoView();

public:
	void SetDetailInfo(const wxString& strDetailInfo);
	void SetSelectedItemInfo(const wxString& strSelectedItemInfo);

private:
	wxFont m_font;
	wxRect m_viewRect;
	wxRect m_DiskSpaceRect;

	wxIcon m_icoInfo;

	wxString m_strCurrentDir;
	wxString m_strItemInfo;
	wxString m_strSelectedItemInfo;
	wxString m_strCurrentDrive;
	wxString m_strDispDiskSpace;
	wxString m_strTotalSpace;
	wxString m_strFreeSpace;
	wxString m_strSelItemDisp;

	double m_dblFreeSpace;
	double m_dblTotalSpace;

	wxToolTip* m_pToolTip;

	//2019.04.11 ADD
	wxBitmap* m_pDoubleBuffer;

	//2019.04.16 ADD
	bool m_bSizeChanged;
	wxSize m_szChagned;

private:
	void Display(wxDC* pDC);
	wxString CalcDispStr(wxDC* pDC, const wxString& strSourceSave, const wxString& strSource, int iDispWidth);

private:
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnErase(wxEraseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnDetailInfoView(wxCommandEvent& event);
	void OnSelectedItemDisplay(wxCommandEvent& event);
	void OnDiskSpaceDisplay(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};
#endif