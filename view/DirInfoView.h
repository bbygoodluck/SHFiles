#ifndef __DIRECTORY_INFO_VIEW_H__
#define __DIRECTORY_INFO_VIEW_H__

class CDirInfoView : public wxWindow
{
public:
	explicit CDirInfoView(wxWindow* parent, const int nID, const wxPoint& pt, const wxSize& sz, long lStyle = CHILD_WINDOW_STYLE);
	virtual ~CDirInfoView();

	void SetDirInfo(const wxString& strInfo);
	void SetKeyInputInfo(const wxString& strKeyInfo);

private:
	wxRect	m_viewRect;
	wxString m_strItemInfo;
	wxString m_strKeyInput;

	wxSize m_szChagned;
	wxFont m_font;
	wxBitmap* m_pDoubleBuffer = nullptr;

private:
	void Renderer(wxDC* pDC);

private:
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnErase(wxEraseEvent& event);

	wxDECLARE_EVENT_TABLE();
};
#endif