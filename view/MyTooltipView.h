#ifndef __MY_TOOLTIP_VIEW_H_INCLUDED
#define __MY_TOOLTIP_VIEW_H_INCLUDED

class CMyTooltipView : public wxWindow
{
public:
	CMyTooltipView(wxWindow* pParent);
	~CMyTooltipView();
	
public:
	void SetTooltipText(const wxString& strTooltip);
	
private:
	void Renderer(wxDC* pDC);
	
private:
	// 화면 버퍼(더블버퍼링 이용)
	wxBitmap* m_pDoubleBuffer = nullptr;
	//변경사이즈(OnSize Event)
	wxSize m_szChagned = wxSize(0, 0);
	//화면 영역 크기
	wxRect m_viewRect = wxRect(0, 0, 0, 0);
	
	wxString m_strTooltip = wxT("");
private:
	void OnPaint(wxPaintEvent& event);
	void OnErase(wxEraseEvent& event);
	void OnSize(wxSizeEvent& event);
	wxDECLARE_EVENT_TABLE();
};

#endif