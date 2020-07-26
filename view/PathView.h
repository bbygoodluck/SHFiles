#ifndef __PATH_VIEW_H_INCLUDED__
#define __PATH_VIEW_H_INCLUDED__

class CViewPanel;
class CPathView : public wxWindow
{
	friend class CViewPanel;
	class CPathItemData
	{
	public:
		explicit CPathItemData() {}
		virtual ~CPathItemData() {}

	public:
		void ReclacPolygon()
		{
			m_ptPolygon[1].x = m_rcDisp.GetRight() - 5;
			m_ptPolygon[1].y = m_rcDisp.GetTop();

			m_ptPolygon[2].x = m_rcDisp.GetRight();
			m_ptPolygon[2].y = m_rcDisp.GetHeight() / 2;

			m_ptPolygon[3].x = m_rcDisp.GetRight() - 5;
			m_ptPolygon[3].y = m_rcDisp.GetBottom() + 1;
		}
	public:
		wxString	m_strName;
		wxPoint		m_ptPolygon[6];
		wxRect		m_rcDisp;
	};

public:
	explicit CPathView();
	explicit CPathView(wxWindow* parent, const int nID, const wxPoint& pt, const wxSize& sz, long lStyle = CHILD_WINDOW_STYLE);
	~CPathView();

	enum path_op {
		unknown = -1,
		add,
		updir,
		root,
		changedrive
	};

	void AddPath(const wxString& strPath);
	
private:
	void ClearPathItems();
	void Init();
	void Renderer(wxDC* dc);
	void CalcPathPolygon(wxDC* pDC);
	void DisplayPathItem(wxDC* pDC);
	
#ifdef __WXMSW__
	void AddDriveItem();
#endif

	void AddPathPolygon(wxDC* pDC, const wxString& strPath, int iPrevIndex);
private:
	wxFont m_font;
	wxRect m_viewRect;
	wxIcon m_icoDrive;
	wxVector<CPathItemData> m_vecPathDatas;
	
	wxBitmap m_bitmapNext;
	wxBitmap m_bitmapNextDisable;
	wxRect   m_bmpRect;
	wxRect   m_rcClick = wxRect(0, 0, 0, 0);
	
	wxToolTip* m_pToolTip = nullptr;
	wxBitmap* m_pDoubleBuffer = nullptr;
	wxSize m_szChagned;
	
	wxString m_strCurrentDrive = wxT("");
	wxString m_strPath = wxT("");
	
	int m_iRED;
	int m_iGREEN;
	int m_iBLUE;
	int	m_iMouseOverIndex = -1;
	int m_iCurrentPathIndex = 0;
	int m_iDepthCount = 0;
	int	m_iPrevItemIndex = -1;
	int m_iTotalItemCount = 0;
	int m_iDispWidth = 0;
#ifdef __WXMSW__
int	 m_nDriveSelectedIndex = 0;
#endif
	bool m_bNextPath = false; 
	bool m_bMouseOver = false;
	bool m_bNextPathMouseOver = false;
	bool m_bChangedPath = false;
	bool m_bFounded = false;
	
	CViewPanel* m_pViewPanel;
	std::unique_ptr<wxTextCtrl> m_pTxtCtrl = nullptr;
private:
	void OnPaint(wxPaintEvent& event);
	void OnErase(wxEraseEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseLButtonClick(wxMouseEvent& event);
	void OnMouseDBClick(wxMouseEvent& event);
	void OnLeaveWindow(wxMouseEvent& event);
	void OnDriveMenuClick(wxCommandEvent& event);
	void OnDrvieMenuUpdate(wxUpdateUIEvent& event);
	
	//디렉토리 이동이벤트
	void OnEnterTextCtrl(wxCommandEvent& event);
	void OnKeyDownTextCtrl(wxKeyEvent& event);
	void OnKillFocusTxtCtrl(wxFocusEvent& event);
	wxDECLARE_EVENT_TABLE();
};

#endif
