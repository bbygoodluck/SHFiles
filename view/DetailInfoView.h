#ifndef _INFO_DISPLAY_VIEW_H
#define _INFO_DISPLAY_VIEW_H
/********************************************************************************
// class Name : CInfoDisplayView
// 설      명 : 선택한 파일(또는 폴더)의 상세 정보 보기
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
	void DisplayDiskSpaceInfo(const wxString& strDrive, bool bUpdate = false);

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

	int m_nDiskspaceWidth = 70;
private:
	void Display(wxDC* pDC);
	wxString CalcDispStr(wxDC* pDC, const wxString& strSourceSave, const wxString& strSource, int iDispWidth);

private:
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnErase(wxEraseEvent& event);
	void OnMouseMove(wxMouseEvent& event);

	wxDECLARE_EVENT_TABLE();
};
#endif
