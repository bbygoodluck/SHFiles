#ifndef __SHCD_VIEW_H__
#define __SHCD_VIEW_H__

class CDirTree;
class CSHCDView : public wxWindow
{
public:
//	explicit CSHCDView(CSHCDPanel* parent, const int nID, const wxSize& sz, const wxString& strTitle);
	explicit CSHCDView(wxWindow* parent, const int nID, const wxSize& sz, const wxString& strTitle);
	virtual ~CSHCDView();

	void SetLineCount(int iLineCount) { m_iLineCount = iLineCount; }
	void SetSignal() {
		m_lock.getCondition()->Signal();
	}

	NODE* GetCurrentNode();
	void SetInit();
	void DeleteDir();
	void AddDir();
	void RenameDir();
protected:
	void Renderer(wxDC* pDC);

private:
	void Initialize();

	void CalcItemCountInView(wxDC* pDC);
	void CalcDispStr(wxDC* pDC, const wxString& strSource, wxString& strOut, const int iCheckWidth);//const wxRect& rcName, bool bCheckRect);

	void DisplayItems(wxDC* pDC);
	void DisplayParentPath(wxDC* pDC, NODE* pNode, int iDispRowIndex);
	void DisplayTopParentPath(wxDC* pDC, NODE* pNode, int iDispRowIndex);
	void DrawParentPath(wxDC* pDC, NODE* pNode, int iDispRowIndex, const wxColour& color);

	void DrawRowLine(wxDC* pDC, wxVector<NODE *>& vecDatas);
	void DrawColLineDown(wxDC* pDC, wxVector<NODE *>::iterator iterCurr);
	void DrawColLineDownToRoot(wxDC* pDC, NODE* pNode);
	void DrawColLineUp(wxDC* pDC, wxVector<NODE *>& vecDatas);

	void ProcessKeyEvent(const int nKeyCode);
	void CalcStartIndex();
	void CalcStartIndex(int iKeyCode, NODE* pNode);
	void FindChild(int iKeyCode, NODE* pNode);

	void CalcKeyDown();
	void CalcKeyUp();
	void CalcKeyLeft();
	void CalcKeyRight();

	void MoveVerticalScroll();
	void MoveHorizontalScroll(int iPos);

	void SetFullPath();
	void MoveDir();
	void SetScrolledCurrentNode(int iRowIndex);
	void ScrollVerticalProcess(wxEventType evtType, bool bWheel, int iThumbTrackPos = 0);
	void ScrollHorzProcess(wxEventType evtType, int iThumbTrackPos = 0);

	bool FindMouseClickItem(const wxPoint& pt);

private:
	wxRect m_viewRect;
	bool m_bLoadOK = false;
	bool m_bDriveLoad = false;

	wxIcon m_icoDrive;

	int m_iLineCount;
	int m_iWidthItemCountCanDraw;
	int m_iHeightItemCountCanDraw;
	int m_iStartIndex;
	int m_iLastIndex;
	int m_iLastRowIndex;
	int m_iTotalItemsCount; 
	int m_iCurrentRowIndex;
	int m_iRowIndex;
	int m_iCurrentLevel;
	int m_iCurrentLevelPage;
	int m_iLevelPageDiff;
	int m_iHeightPageCnt;
	int m_iPrevHeightPageCnt;
	int m_iDispCount;
	int m_iThumbTrackPos;
	int m_iStartLevel;
	int m_iEndLevel;
	int m_iMaxLevel;
	bool m_bSizeChanged;

	wxSize m_szItem;
	wxSize m_szChagned;
	wxVector<NODE *>::iterator m_curIter;
	wxVector<NODE *>::iterator m_PrevIter;
	NODE* m_pCurrNode;
	
	std::unordered_map<int, wxVector<NODE *>> m_Datas;

	wxWindow* m_pParent;
	CLock m_lock;

	wxString m_strTreeInfo;

	wxColour m_crBackGround;
	wxColour m_crDefaultText;
	wxColour m_crSelectedText;
	wxColour m_crSelectedBackground;
	wxColour m_crSelectedParentBackground;
	wxColour m_crLine;

	//2019.04.19 ADD
	wxBitmap* m_pDoubleBuffer;
	wxFont m_font;

	//2019.08.18 ADD
	wxTextCtrl* m_pTxtCtrlForRename;
private:
	void OnPaint(wxPaintEvent& event);
	void OnErase(wxEraseEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnSetFocus(wxFocusEvent& event);
	void OnKillFocus(wxFocusEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	void OnChar(wxKeyEvent& event);
	void OnScroll(wxScrollWinEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnMouseLBClick(wxMouseEvent& event);
	void OnMouseLBDBClick(wxMouseEvent& event);
	void OnRefreshAll(wxCommandEvent& event);

	void OnTextCtrlEnter(wxCommandEvent& event);
	void OnCharHookFromTextCtrl(wxKeyEvent& event);

	DECLARE_EVENT_TABLE()
};
#endif