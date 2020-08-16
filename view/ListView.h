#ifndef __FILELISTVIEW_H__
#define __FILELISTVIEW_H__

#include "../engine/LocalFileSystem.h"
#include "../interface/PositionInfo.h"
#include "../interface/DirData.h"
#include "../interface/Sorting.h"

#ifdef __WXMSW__
	#include "../interface/msw/ImageMap.h"
#else
#endif

struct _HistoryItem
{
	int iCurrentIndex;
	int iStartIndex;
};

class CMyTooltipView;
class CViewPanel;
class CLocalWatcherMonitorHandler;

WX_DECLARE_STRING_HASH_MAP(_HistoryItem, _HISTORY);

class CListView : public wxWindow
{
	friend class CImageMap;
	friend class CViewPanel;

protected:
	typedef struct _SELITEM_INFO {
		int m_iSelIndex;
		bool m_bFile;
	} SELITEM_INFO;
	
	const int ICON_WIDTH              = 16;
	const int ICON_HEIGHT             = 16;
	const int ICON_SELECTED_WIDTH_GAP = 10;
	const int GAP_WIDTH               = 2;
	const int GAP_HEIGHT              = 2;
	
	wxIcon   m_icoUpDir;
	wxColour m_colDefault;
	wxColour m_colDrive;
	wxColour m_colDir;
	wxColour m_colSize;
	wxColour m_colTime;
	wxColour m_colAttr;
	wxColour m_colType;
	wxColour m_colMatch;
	
protected:
	class CColumnPoint
	{
	public:
		wxPoint pt1;
		wxPoint pt2;
	};
	
public:
	explicit CListView(wxWindow* parent, const int nID, const wxSize& sz);
	virtual ~CListView();

	wxString GetCurrentPath() {	return m_strCurrentPath; }
	wxString GetCurrentItem();
	
	void DoSelectedItemsClear();
	//FileSystemWatcher 함수들..
	virtual void DoCreate(const wxString& strName) = 0;
	virtual void DoModify(const wxString& strName) = 0;
	virtual void DoDelete(const wxString& strName) = 0;
	virtual void DoRename(const wxString& strOldName, const wxString& strNewName) = 0;
	void DoMyEventExecute(wxCommandEvent& event);
	
	wxString GetDetailInfo();
	wxString GetDirInfo();
	
	//생성/수정/삭제시 디렉토리 정보 변경
	void UpdateModificationTimeOfDir();
	void ShowCompress();
//공용함수
protected:
	void Clear();
	void Initialize();
	void DoSortStart();
	void ReCalcPage();
	virtual void LoadDir(const wxString& strPath) = 0;
	virtual void ProcessKeyEvent(const int nKeyCode);
	virtual void Renderer(wxDC* pDC) = 0;
	//이름변경(메뉴실행시)
	virtual void DoRenameFromMenu(const wxString& strOldPathName, const wxString& strNewPathName) = 0;
	//외부편집프로그램 실행
	virtual void ExecuteExternalProgramForEdit(int iIndex) = 0;
	//생성/수정/삭제시 디렉토리 정보 변경
	virtual void DoUpdateModificationTimeOfDir() = 0;
	//외부프로그램 실행
	void ExecFileEditProgram();
	//콘텍스트 메뉴 보기
	void DisplayContextMenu(const wxPoint& pt);
	//이름변경
	void DoRenameFromMenuPrepare();
	void DoRenameOn(const wxString& strRename);
	
	void DisplayItems(wxDC* pDC);
	void CalcColumn(wxDC* pDC);
	bool CalcAutoColumn(wxDC* pDC, const wxRect& viewRect);
	void DrawColumn(wxDC* pDC);
	void CalcPosition(wxDC* pDC);
	
	wxString CalcDispStr(wxDC* pDC, const wxString& strSourceSave, const wxString& strSource, bool isDrive);
	wxString FindMaxData(const wxVector<wxString>& vecData);
	wxString FindMaxData(const wxString& a, const wxString& b);
	
	void MoveLeftAndUpKey();
	void MoveRightAndDownKey();
	
	void GotoRoot();
	bool PressEnterKey();
	
	//아이템존재여부 확인
	wxVector<CDirData>::iterator GetItemExist(const wxString& strName, bool& bExist);
	//아이콘 읽기
	bool ReadIconThreadStop();
	void ReadIconThreadStart();
	void ReadIconThreadTerminate();
	//마우스이벤트 처리
	void DoMouseProcess(const wxPoint& pt, bool bDblClick = false);
	bool FindItemInMousePoint(const wxPoint& pt);
	
	//선택아이템 처리
	bool SetSelectedItem(int iKeyCode);
	//복사, 이동
	void MakeCopyOrMoveList(bool bUseClipboard, bool bMove, std::list<wxString>& lstItems);
	//삭제(휴지통, 완전삭제)
	bool MakeTrashOrDeleteData(std::list<wxString>& lstDatas, bool bTrash);
	int GetItemDistance(wxVector<CDirData>::iterator it) 
	{
		return std::distance(m_itemList.begin(), it);
	}
	
	CViewPanel* GetParentPanel() { return m_pViewPanel; }
	wxString MakeFullPathName(const wxString& strName);
	void DoSelectAllOrRelease(const wxEventType& evtType);
	void FindMatchItems();
	void DoMatchClear();

private:
	void AllClear();
	void ShowFavoriteMenu();
	
protected:
	// 화면 버퍼(더블버퍼링 이용)
	wxBitmap* m_pDoubleBuffer = nullptr;
	//변경사이즈(OnSize Event)
	wxSize m_szChagned = wxSize(0, 0);
	//화면 영역 크기
	wxRect m_viewRect = wxRect(0, 0, 0, 0);
	// 컬럼 라인 정보
	wxVector<CColumnPoint> m_ptList;
	// 포지션정보
	wxVector<CPositionInfo>	m_posList;
	// 화면표시이름
	std::unordered_map<wxString, wxString> m_dispNameInfoMap;
	
	// 현재 디렉토리
	wxString m_strCurrentPath = wxT("");
	// 가장긴 이름
	wxString m_strMaxName = wxT("");
	// 가장긴 파일유형
	wxString m_strMaxTypeName = wxT("");
	//드라이브 명
	wxString m_strVolume = wxT("");
	//이름변경
	wxString m_strItemToRename = wxT("");
	// 키 입력문자
	wxString m_strKeyInput = wxT("");
	// 데이터
	wxVector<CDirData> m_itemList;
	//디렉토리방문이력
	_HISTORY m_hashHistory;
	
	// 문자 높이
	int m_iCharHeight         = 0;
	// 컬럼당 표시가능한 아이템수
	int m_nItemCountInColumn  = 0;
	// 컬럼수
	int	m_nDispColumn         = 0;
	// 화면에 표시가능한 아이템수
	int	m_nDisplayItemInView  = 0;
	// 컬럼끝 포인트
	int	m_nDispColumnEndPoint = 0;
	// 전체아이템수
	int	m_nTotalItems         = 0;
	// 화면표시 아이템 시작위치
	int m_nStartIndex;
	// 현재커서위치
	int m_nCurrentItemIndex   = 0;
	// 현재포지션위치
	int	m_iCurrentPosition    = 0;
	// 디렉토리수
	int m_iDirCount           = 0;
	// 파일수
	int m_iFileCount          = 0;
	//디렉토리 이동히스토리 인덱스
	int m_iHistoryIndex       = 0;
	int m_iHistoryStartIndex  = 0;
	//히스토리 최대 카운터
	int	m_nMaxHistoryCount    = 100;
	//Path Depth
	int m_iPathDepth          = 1;
	//전체 포지션 카운터
	int m_iTotalPositionCnt   = 0;
	//선택 파일수
	int m_iSelFileCnt         = 0;
	//선택 디렉토리 수
	int m_iSelDirCnt          = 0;
	//폴더 사이즈
	double m_dblFileSizeInDir = 0.0;
	//디렉토리 Load 플래그
	bool m_bDirLoaded   = false;
	//화면 변경 플래그
	bool m_bSizeOrColumnChanged = false;
	//윈도우 포커스 플래그
	bool m_bSetFocus    = false;
	//컬림 계산플래그
	bool m_bCalcColumn  = false;
	// 화면에 표시가능한 아이템
	bool m_bDispFlag[5] = {false, };
	bool m_bMouseClickItemFound = false;
	bool m_bContextMenuFromMenuEvent = false;
	bool m_bIsDisplayDetailInfo = false;
	//파일/폴더 이미지리스트
	CImageMap* m_pImageMap = nullptr;
	CViewPanel* m_pViewPanel;
	//선택 아이템
	std::unordered_map<int, SELITEM_INFO> m_hashSelectedItem;
	
	// 이름변경 텍스트컨트롤
	std::unique_ptr<wxTextCtrl> m_pTxtCtrlForRename = nullptr;
	//tooltip window	
	CMyTooltipView* m_pMyTooltipView;
	CMyTooltipView* m_pMyTooltipKeyInput;
	//Match 아이템 정보
	wxVector<int> m_matchItems;
protected:
	void OnSetFocus(wxFocusEvent& event);
	void OnKillFocus(wxFocusEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	void OnChar(wxKeyEvent& event);
	//마우스 이벤트
	void OnMouseLBottonDown(wxMouseEvent& event);
	void OnMouseLButtonDBClick(wxMouseEvent& event);
	void OnMouseRButtonDown(wxMouseEvent& event);
	void OnMouseRButtonUp(wxMouseEvent& event);
	
	//파일편집(외부편집프로그램이 여러개인경우)
	void OnMenuFileEditProcess(wxCommandEvent& event);
	//압축 실행
	void OnCompress(wxCommandEvent& event);
	//이름변경 이벤트
	void OnEnterTextCtrl(wxCommandEvent& event);
	void OnKeyDownTextCtrl(wxKeyEvent& event);
	void OnKillFocusTxtCtrl(wxFocusEvent& event);
	wxDECLARE_EVENT_TABLE();
};
#endif