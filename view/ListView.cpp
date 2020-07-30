#include "../ginc.h"
#include "ListView.h"
#include "../ViewPanel.h"
#include "MyToolTipView.h"

#ifdef __WXMSW__
	#include "../interface/msw/MSWContextMenuHandler.h"
#else
#endif

wxBEGIN_EVENT_TABLE(CListView, wxWindow)
	EVT_SET_FOCUS(CListView::OnSetFocus)
	EVT_KILL_FOCUS(CListView::OnKillFocus)
	EVT_SIZE(CListView::OnSize)
	EVT_KEY_DOWN(CListView::OnKeyDown)
	EVT_CHAR(CListView::OnChar)
	EVT_LEFT_DOWN(CListView::OnMouseLBottonDown)
	EVT_LEFT_DCLICK(CListView::OnMouseLButtonDBClick)
	EVT_RIGHT_DOWN(CListView::OnMouseRButtonDown)
	EVT_RIGHT_UP(CListView::OnMouseRButtonUp)
	EVT_MY_CUSTOM_COMMAND(wxEVT_EXEC_FILE_EDIT, wxID_ANY, CListView::DoMyEventExecute)
	EVT_MY_CUSTOM_COMMAND(wxEVT_EXEC_CONTEXT_MENU, wxID_ANY, CListView::DoMyEventExecute)
	EVT_MY_CUSTOM_COMMAND(wxEVT_DIR_FILE_RENAME, wxID_ANY, CListView::DoMyEventExecute)
	EVT_MY_CUSTOM_COMMAND(wxEVT_ITEM_ALL_SELECT, wxID_ANY, CListView::DoMyEventExecute)
	EVT_MY_CUSTOM_COMMAND(wxEVT_ITEM_ALL_RELEASE, wxID_ANY, CListView::DoMyEventExecute)
	EVT_MENU_RANGE(EXTERNAL_PROGRAM_START_ID, EXTERNAL_PROGRAM_END_ID, CListView::OnMenuFileEditProcess)
wxEND_EVENT_TABLE()

CListView::CListView(wxWindow* parent, const int nID, const wxSize& sz)
	: wxWindow(parent, nID, wxDefaultPosition, sz, FILELISTVIEW_STYLE)
	, m_pViewPanel((CViewPanel *)parent)
{
	//상위폴더 이동 이미지
	m_icoUpDir = wxArtProvider::GetIcon(wxART_GO_DIR_UP, wxART_OTHER, wxSize(16, 16));
	
	m_colDefault = theJsonConfig->GetDefaultColor();
	m_colDrive   = theJsonConfig->GetDriveColor();
	m_colDir     = theJsonConfig->GetDirColor();
	m_colSize    = theJsonConfig->GetDefaultColor();
	m_colTime    = theJsonConfig->GetDefaultColor();
	m_colAttr    = theJsonConfig->GetAttributeColor();
	m_colType    = theJsonConfig->GetDescriptionColor();
	m_colMatch   = theJsonConfig->GetMatchColor();
	
	m_pImageMap = new CImageMap(this);
	
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	
	m_pTxtCtrlForRename = std::make_unique<wxTextCtrl>(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxBORDER_THEME);
	m_pTxtCtrlForRename->SetBackgroundColour(wxColour(220, 220, 220));
	m_pTxtCtrlForRename->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_pTxtCtrlForRename->SetFont(*_gFont);
	m_pTxtCtrlForRename->Show(false);
	
	m_pTxtCtrlForRename->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(CListView::OnKeyDownTextCtrl), NULL, this);
	m_pTxtCtrlForRename->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(CListView::OnEnterTextCtrl), NULL, this);
	m_pTxtCtrlForRename->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(CListView::OnKillFocusTxtCtrl), NULL, this);
	
	m_pMyTooltipView = new CMyTooltipView(this);
	m_pMyTooltipView->SetSize(60, 20);
	m_pMyTooltipView->Show(false);
}

CListView::~CListView()
{
	ReadIconThreadTerminate();
	
	if (m_pDoubleBuffer)
		delete m_pDoubleBuffer;
	
	m_pDoubleBuffer = nullptr;
	AllClear();
	
	m_pTxtCtrlForRename->Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(CListView::OnKeyDownTextCtrl), NULL, this);
	m_pTxtCtrlForRename->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(CListView::OnEnterTextCtrl), NULL, this);
	m_pTxtCtrlForRename->Disconnect(wxEVT_KILL_FOCUS, wxFocusEventHandler(CListView::OnKillFocusTxtCtrl), NULL, this);
}

void CListView::AllClear()
{
	Clear();
	
	wxVector<CDirData>().swap(m_itemList);
	m_itemList.reserve(0);
	
	wxVector<CColumnPoint>().swap(m_ptList);
	m_ptList.reserve(0);
	
	wxVector<CPositionInfo>().swap(m_posList);
	m_posList.reserve(0);
	
	wxVector<int>().swap(m_matchItems);
	m_matchItems.reserve(0);
}

void CListView::Clear()
{
	m_posList.clear();
	m_ptList.clear();
	m_itemList.clear();
	m_dispNameInfoMap.clear();
	m_hashSelectedItem.clear();
}

void CListView::Initialize()
{
	Clear();
	
	wxVector<CPositionInfo>().swap(m_posList);
	m_posList.reserve(450);
	
	wxVector<CColumnPoint>().swap(m_ptList);
	m_ptList.reserve(250);
	
	wxVector<CDirData>().swap(m_itemList);
	m_itemList.reserve(300);
		
	//디렉토리 수
	m_iDirCount = 0;
	//파일수
	m_iFileCount = 0;
	//용량
	m_dblFileSizeInDir = 0.0;
	//아이템 전체수
	m_nTotalItems = 0;
	//화면표시 시작 인덱스
	m_nStartIndex = 0;
	//현재 인덱스
	m_nCurrentItemIndex = 0;
	//현재 디렉토리
	m_strCurrentPath = wxT("");
	//Max Name
	m_strMaxName     = wxT("");
	//Max Type Name
	m_strMaxTypeName = wxT("");
	
	//디렉토리 Load 플래그
	m_bDirLoaded = false;
	//화면 변경 플래그
	m_bSizeOrColumnChanged = false;
	//상세정보표시 플래그
	m_bIsDisplayDetailInfo = false;
}

bool CListView::ReadIconThreadStop()
{
	bool bReturn = true;
	if (m_pImageMap)
	{
		if (m_pImageMap->IsIconReading())
		{
			m_pImageMap->TerminateRead();
			m_pImageMap->Wait();
		}
	}
	else
		bReturn = false;
		
	return bReturn;
}

void CListView::ReadIconThreadStart()
{
	//쓰레드 실행
	m_pImageMap->IconReadStart();
}

void CListView::ReadIconThreadTerminate()
{
	if (m_pImageMap)
	{
		m_pImageMap->SetThreadTerminate();

		delete m_pImageMap;
		m_pImageMap = NULL;
	}
}

void CListView::CalcColumn(wxDC* pDC)
{
	if (m_nTotalItems <= 0)
		return;
		
	if(!m_bSizeOrColumnChanged && !m_bDirLoaded)
		return;
	
	m_dispNameInfoMap.clear();
	
	int nRight = m_viewRect.GetRight();
	int nBottom = m_viewRect.GetBottom();

	m_iCharHeight = pDC->GetCharHeight() + 2;

	m_nItemCountInColumn = (m_viewRect.GetHeight() / m_iCharHeight);
	if (m_nItemCountInColumn == 0)
		return;
	
	if(m_bSizeOrColumnChanged)
		m_ptList.clear();
		
	m_nDispColumn = theJsonConfig->GetColumnCount();
	if (m_nDispColumn == 0)
	{
		//자동컬럼인경우
		if (!CalcAutoColumn(pDC, m_viewRect))
			return;
	}
	//화면안에 표시될 아이템 수
	m_nDisplayItemInView = m_nItemCountInColumn * m_nDispColumn;

	int nDispWidth = static_cast<int>(nRight / m_nDispColumn);
	if (m_nDispColumn > 1)
	{
		//컬럼구분선 좌표
		wxPoint pt1(0, 0);
		wxPoint pt2(0, 0);
		//컬럼 좌표 설정
		for (int nColumnCount = 0; nColumnCount < (m_nDispColumn - 1); nColumnCount++)
		{
			CColumnPoint colPoint;
			colPoint.pt1.x = nDispWidth * (nColumnCount + 1);
			colPoint.pt1.y = 0;

			colPoint.pt2.x = nDispWidth * (nColumnCount + 1);
			colPoint.pt2.y = nBottom;

			m_ptList.push_back(colPoint);
		}
	}

	m_nDispColumnEndPoint = nDispWidth;
	if(m_bDirLoaded)
		ReadIconThreadStart();
		
	//아이템 표시좌표 계산
	CalcPosition(pDC);
}

bool CListView::CalcAutoColumn(wxDC* pDC, const wxRect& viewRect)
{
	//전체건수 / 컬럼당 아이템수 <= 0 이면 컬럼은 1
	m_nDispColumn = m_nTotalItems / m_nItemCountInColumn;
	if (m_nDispColumn <= 0)
		m_nDispColumn = 1;
	else
	{	//컬럼당 표시아이템수 > 전체 아이템수 ==> 컬럼은 1
		if (m_nItemCountInColumn > m_nTotalItems)
			m_nDispColumn = 1;
		else
		{	//표시가능 컬럼수
			int iAvalibleDispItem = 0;

			wxSize szEng = pDC->GetTextExtent(wxString("A"));
			wxSize szHan = pDC->GetTextExtent(wxString("가"));
			//한글과 영문의 평균 Width
			int iAvgCharWidth = (szEng.GetWidth() + szHan.GetWidth()) / 2;
			//최대 표시가능 Width
			int iMaxDispWidth = viewRect.GetWidth();
			//자동컬럼시 표시글자수의 최대값은 16
			int iMaxDispCharWidth = iAvgCharWidth * 16;
			int iDispColumn = 0;

			while (1)
			{	//전체 표시가능 Width > 최대 표시가능 Width
				if (iMaxDispWidth > iMaxDispCharWidth)
				{	//표시컬럼 증가
					iDispColumn++;
					//표시가능 아이템수 증가
					iAvalibleDispItem += m_nItemCountInColumn;
					//전체 표시가능 Width -= 최대표시가능Width
					iMaxDispWidth -= iMaxDispCharWidth;

					if (iAvalibleDispItem >= m_nTotalItems)
						break;
				}
				else
					break;
			}

			//자동 컬럼 선택시 최대 9개의 컬럼
			m_nDispColumn = iDispColumn >= 9 ? 9 : iDispColumn;
		}
	}

	if (m_nDispColumn <= 0)
		m_nDispColumn = 1;
		
	return true;
}

void CListView::CalcPosition(wxDC* pDC)
{
	m_posList.clear();

	m_bDispFlag[0] = false;
	m_bDispFlag[1] = false;
	m_bDispFlag[2] = false;
	m_bDispFlag[3] = false;
	m_bDispFlag[4] = false;

	wxString strAttr(wxT("RHSA"));
	wxString strTime(wxT("0000-00-00 00:00"));
	wxString strSize(wxT("123.45 MB"));
	wxString strFileSize(wxT("123.45 "));
	wxString strFileSizeType(wxT("MB"));

	wxSize sizeMaxName = pDC->GetTextExtent(m_strMaxName);
	wxSize sizeMaxTypeName = pDC->GetTextExtent(m_strMaxTypeName);
	wxSize sizeAttr = pDC->GetTextExtent(strAttr);
	wxSize sizeTime = pDC->GetTextExtent(strTime);
	wxSize sizeSize = pDC->GetTextExtent(strSize);
	wxSize sizeFileSize = pDC->GetTextExtent(strFileSize);
	wxSize szFileSizeType = pDC->GetTextExtent(strFileSizeType);

	int iNameWidth = sizeMaxName.GetWidth();
	int iSizeWidth = sizeSize.GetWidth();
	int iTimeWidth = sizeTime.GetWidth();
	int iAttrWidth = sizeAttr.GetWidth();
	int iTypeWidth = sizeMaxTypeName.GetWidth();

	unsigned int arrWidth[5] = { 0, };
	arrWidth[0] = iNameWidth;
	arrWidth[1] = iSizeWidth;
	arrWidth[2] = iTimeWidth;
	arrWidth[3] = iAttrWidth;
	arrWidth[4] = iTypeWidth;

	int iSizeSum = 0;
	int iAvaliableWidth = m_nDispColumnEndPoint - (ICON_SELECTED_WIDTH_GAP + (GAP_WIDTH * 21));

	for (int i = 0; i < 5; i++)
	{
		iSizeSum += arrWidth[i];
		if (iAvaliableWidth > iSizeSum)
			m_bDispFlag[i] = true;
	}

	m_bDispFlag[0] = true;
	if (!m_bDispFlag[1])
		iAvaliableWidth = m_nDispColumnEndPoint - (ICON_SELECTED_WIDTH_GAP + (GAP_WIDTH * 12));

	int iTempColumn = 0;
	int iDispIndex = 0;
	int iStartX = 1;
	int iStartY = 0;

	int iDisp_x1 = 0;
	int iDisp_y1 = 0;
	int iDisp_x2 = 0;
	int iDisp_y2 = 0;

	//2019.04.17 ADD
	int iGapWidth = GAP_WIDTH * 2;

	for (int nIndex = 0; nIndex < m_nDisplayItemInView; nIndex++)
	{
		if (nIndex > (m_nItemCountInColumn - 1))
		{
			if ((nIndex % m_nItemCountInColumn) == 0)
			{
				iTempColumn++;
				iDispIndex = 0;
			}
		}

		int x1 = iStartX + (iTempColumn * m_nDispColumnEndPoint);
		int y1 = (iStartY + 1) + (m_iCharHeight * iDispIndex);
		int x2 = m_nDispColumnEndPoint - 1;// (iStartX + ((iTempColumn + 1) * m_nDispColumnEndPoint)) - 1;
		int y2 = m_iCharHeight;// iStartY + (iHeight * (iDispIndex + 1));

		wxRect rect(x1, y1, x2, y2);

		CPositionInfo posInfo;
		posInfo.m_mainRect = rect;

		int iIcon_x1 = x1 + ICON_SELECTED_WIDTH_GAP + GAP_WIDTH;
		int iIcon_y1 = y1 + 2;
		int iIcon_x2 = ICON_WIDTH;
		int iIcon_y2 = y2;

		wxRect rcIcon(iIcon_x1, iIcon_y1, iIcon_x2, iIcon_y2);
		posInfo.m_iconRect = rcIcon;

		// 이름표시
		if (m_bDispFlag[0])
		{
			iDisp_x1 = rcIcon.GetRight() + (GAP_WIDTH * 3);
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iNameWidth;
			iDisp_y2 = y2;

			if (!m_bDispFlag[1])
				iDisp_x2 = iAvaliableWidth;

			wxRect rcName(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_nameRect = rcName;
		}

		//설명
		if (m_bDispFlag[4])
		{
			iDisp_x1 = rect.GetRight() - (iTypeWidth + iGapWidth);
			iDisp_y1 = y1;
			iDisp_x2 = iTypeWidth;
			iDisp_y2 = y2;

			wxRect rcType(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_typeNameRect = rcType;
		}
	
		//속성
		if (m_bDispFlag[3])
		{
		//	iDisp_x1 = rect.GetRight() - (iAttrWidth + iGapWidth);// m_bDispFlag[4] == true ? posInfo.m_typeNameRect.GetLeft() - (iAttrWidth + iGapWidth) : rect.GetRight() - (iAttrWidth + iGapWidth);
			iDisp_x1 = m_bDispFlag[4] == true ? posInfo.m_typeNameRect.GetLeft() - (iAttrWidth + iGapWidth * 2) : rect.GetRight() - (iAttrWidth + iGapWidth);
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iAttrWidth;
			iDisp_y2 = y2;

			wxRect rcAttr(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_attrRect = rcAttr;
		}

		//수정시간
		if (m_bDispFlag[2])
		{
			iDisp_x1 = m_bDispFlag[3] == true ? posInfo.m_attrRect.GetLeft() - (iTimeWidth + iGapWidth * 2) : rect.GetRight() - (iTimeWidth + iGapWidth);
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iTimeWidth;
			iDisp_y2 = y2;

			wxRect rcTime(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_timeRect = rcTime;
		}

		//사이즈 표시
		if (m_bDispFlag[1])
		{
			iDisp_x1 = m_bDispFlag[2] == true ? posInfo.m_timeRect.GetLeft() - (iSizeWidth + iGapWidth * 2) : rect.GetRight() - (iSizeWidth + iGapWidth);
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iSizeWidth;
			iDisp_y2 = y2;

			wxRect rcSize(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_sizeRect = rcSize;

			int iFileSizeWidth = sizeFileSize.GetWidth();

			iDisp_x1 = posInfo.m_sizeRect.GetLeft();
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iFileSizeWidth;
			iDisp_y2 = y2;

			wxRect rcFileSize(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_sizeRectFile = rcFileSize;

			int iSizeTypeWidth = szFileSizeType.GetWidth();

			iDisp_x1 = rcFileSize.GetRight() + iGapWidth;
			iDisp_y1 = y1 + 1;
			iDisp_x2 = iSizeTypeWidth;
			iDisp_y2 = y2;

			wxRect rcFileSizeType(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
			posInfo.m_sizeRectType = rcFileSizeType;
		}
		/*	
		}
		else
		{
			//사이즈 표시
			if (m_bDispFlag[1])
			{
				iDisp_x1 = posInfo.m_nameRect.GetRight() + (GAP_WIDTH * 2);
				iDisp_y1 = y1 + 1;
				iDisp_x2 = iSizeWidth;
				iDisp_y2 = y2;

				wxRect rcSize(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
				posInfo.m_sizeRect = rcSize;

				int iFileSizeWidth = sizeFileSize.GetWidth();

				iDisp_x1 = posInfo.m_nameRect.GetRight() + (GAP_WIDTH * 2);
				iDisp_y1 = y1 + 1;
				iDisp_x2 = iFileSizeWidth;
				iDisp_y2 = y2;

				wxRect rcFileSize(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
				posInfo.m_sizeRectFile = rcFileSize;

				int iSizeTypeWidth = szFileSizeType.GetWidth();

				iDisp_x1 = rcFileSize.GetRight() + (GAP_WIDTH * 2);
				iDisp_y1 = y1 + 1;
				iDisp_x2 = iSizeTypeWidth;
				iDisp_y2 = y2;

				wxRect rcFileSizeType(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
				posInfo.m_sizeRectType = rcFileSizeType;
			}

			//수정시간
			if (m_bDispFlag[2])
			{
				iDisp_x1 = posInfo.m_sizeRect.GetRight() + (GAP_WIDTH * 4);
				iDisp_y1 = y1 + 1;
				iDisp_x2 = iTimeWidth;
				iDisp_y2 = y2;

				wxRect rcTime(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
				posInfo.m_timeRect = rcTime;
			}

			//속성
			if (m_bDispFlag[3])
			{
				iDisp_x1 = posInfo.m_timeRect.GetRight() + (GAP_WIDTH * 4);
				iDisp_y1 = y1 + 1;
				iDisp_x2 = iAttrWidth;
				iDisp_y2 = y2;

				wxRect rcAttr(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
				posInfo.m_attrRect = rcAttr;
			}

			//설명
			if (m_bDispFlag[4])
			{
				iDisp_x1 = posInfo.m_attrRect.GetRight() + (GAP_WIDTH * 3);
				iDisp_y1 = y1 + 1;
				iDisp_x2 = iTypeWidth;
				iDisp_y2 = y2;

				wxRect rcType(iDisp_x1, iDisp_y1, iDisp_x2, iDisp_y2);
				posInfo.m_typeNameRect = rcType;
			}
		}
		*/
		m_posList.push_back(posInfo);
		iDispIndex++;
	}
}

void CListView::ReCalcPage()
{
	int nTempPage = 1;
	int nTempColumnPos = 1;
	int nTempItemIndex = m_nCurrentItemIndex;

	int nStartIndex = 0;
	while (1)
	{
		if ((m_nCurrentItemIndex >= nStartIndex) && (m_nCurrentItemIndex <= (nStartIndex + m_nDisplayItemInView)))
			break;

		nStartIndex += m_nDisplayItemInView;
		if (nStartIndex > m_nTotalItems)
		{
			nStartIndex = m_nTotalItems - m_nDisplayItemInView;
			break;
		}
	}

	m_nStartIndex = nStartIndex;
	if (m_nStartIndex < 0)
		m_nStartIndex = 0;

	if (m_nCurrentItemIndex < 0)
		m_nCurrentItemIndex = 0;
}


void CListView::DrawColumn(wxDC* pDC)
{
	pDC->SetPen(wxPen(theJsonConfig->GetColumnColor()));

	for (auto &colItem : m_ptList)
		pDC->DrawLine(colItem.pt1, colItem.pt2);
}

void CListView::DisplayItems(wxDC* pDC)
{
	if (m_nTotalItems <= 0)
		return;

	int nDisplayItemCount = m_nStartIndex + m_nDisplayItemInView;
	if (nDisplayItemCount >= m_nTotalItems)
		nDisplayItemCount = m_nTotalItems;

	if (nDisplayItemCount <= 0)
		return;
		
	int nPosIndex = 0;
	m_iTotalPositionCnt = m_posList.size();
	
	wxColour dispColor;

	wxString strName(wxT(""));
	wxString strSrcName(wxT(""));
	wxString strFullPathName(wxT(""));
	wxString strSpace(wxT(""));
	
	for (int nIndex = m_nStartIndex; nIndex < nDisplayItemCount; nIndex++)
	{
		if (nIndex >= m_nTotalItems)
			nIndex = m_nTotalItems - 1;

		if (nPosIndex >= m_iTotalPositionCnt)
			nPosIndex = m_iTotalPositionCnt - 1;
			
		CDirData itemData = m_itemList.at(nIndex);
		CPositionInfo posInfo = m_posList.at(nPosIndex);

		strSrcName = itemData.GetName();
		strName = itemData.GetName();

		bool isDrive = itemData.IsDrive() ? true : false;
		bool isDir = itemData.IsDir() ? true : false;
		bool isFile = itemData.IsFile() ? true : false;
		bool isCut = itemData.IsCut() ? true : false;

		bool bSelected = itemData.IsItemSelected();
		bool bMatched = itemData.IsMatch();

		//표시 색상
		if (isDrive)     dispColor = m_colDrive;
		else if (isDir)  dispColor = m_colDir;
		else
		{
			wxString strExt(theCommonUtil->GetExt(strName).Upper());
			dispColor = theJsonConfig->GetExtColor(strExt);
		}

	//	if ((iMatchCount > 0) && !isDrive)
	//		dispColor = wxColour(90, 90, 90);

		if (bMatched)
			dispColor = m_colMatch;

		if (bSelected)
			dispColor = wxColour(255, 255, 80);// wxColour(172, 89, 255);
			
		//아이템이 선택되었을경우
		if (m_nCurrentItemIndex == nIndex)
		{
			if (!m_bIsDisplayDetailInfo)
			{
				m_pViewPanel->TransferInfomation(TRANSFER_LISTVIEW_DETAILINFO_TO_DETAILVIEW, GetDetailInfo());
				m_bIsDisplayDetailInfo = true;
			}

			wxRect rcFillRect(posInfo.m_mainRect);
			rcFillRect.SetBottom(rcFillRect.GetBottom() + 1);

			wxPen   pen;//(dispColor);
			wxBrush brush;//(dispColor);

			//포커스가 없는경우
			if (!m_bSetFocus)
			{
				pen = wxPen(wxColour(30, 30, 30), 1);
				brush = wxColour(30, 30, 30);
			}
			else
			{
				pen = dispColor;
				brush = dispColor;
			}

			pDC->SetPen(pen);
			pDC->SetBrush(brush);

			pDC->DrawRoundedRectangle(rcFillRect, -0.08);
			//포커스가 없는경우
			if (!m_bSetFocus)
				dispColor = wxColour(90, 90, 90);
			else
				dispColor = wxColour(0, 0, 0);
		}
		else
		{
			if (bSelected || bMatched)
			{
				wxRect rcFillRect(posInfo.m_mainRect);
				rcFillRect.SetBottom(rcFillRect.GetBottom() + 1);

				wxPen   pen(wxColour(30, 30, 30));
				wxBrush brush(wxColour(19, 102, 142));

				pDC->SetPen(pen);
				pDC->SetBrush(brush);

				pDC->DrawRectangle(rcFillRect);

				pDC->SetPen(wxNullPen);
				pDC->SetBrush(wxNullBrush);
			}
		}
		
		wxColour colSelected = (m_nCurrentItemIndex == nIndex) ? dispColor : m_colDefault;
		wxString strDisp(wxT(""));
#ifdef __WXMSW__
		strSrcName.MakeLower();
#endif

		strDisp = CalcDispStr(pDC, strSrcName, strName, isDrive);
		pDC->SetTextForeground(dispColor);
		pDC->DrawLabel(strDisp, posInfo.m_nameRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		
		if(!isDrive)
		{
			if (m_bDispFlag[1])
			{
				if (isDir)
					pDC->DrawLabel(theMsgManager->GetMessage(wxT("MSG_DIR_FILESIZE_POS")), posInfo.m_sizeRect, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				else
				{
					wxString strFileSize(itemData.GetSizeString());// (wxT(""));
					wxString strFileSizeType(itemData.GetSizeTypeString());// (wxT(""));
					wxColour colorType(itemData.GetSizeColor());// (wxColour(192, 192, 192));
					wxColour colDispColor;

					if (itemData.GetAttribute() & ATTR_RDONLY)
					{
						colDispColor = *wxYELLOW;
						colorType = *wxYELLOW;
					}
					else
						colDispColor = m_colDefault;

					pDC->SetTextForeground(m_nCurrentItemIndex == nIndex ? dispColor : colDispColor);

					if (strFileSizeType.Cmp(_T("")) == 0)	//사이즈가 KB이하인경우
						pDC->DrawLabel(strFileSize, posInfo.m_sizeRect, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
					else
					{	//사이즈가 KB이상인경우
						pDC->DrawLabel(strFileSize, posInfo.m_sizeRectFile, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
						pDC->SetTextForeground(m_nCurrentItemIndex == nIndex ? dispColor : colorType);
						pDC->DrawLabel(strFileSizeType, posInfo.m_sizeRectType, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
					}
				}
			}
			
			//시간정보
			if (m_bDispFlag[2])
			{
				wxString strTime = itemData.GetDateTimeToString();
				pDC->SetTextForeground(colSelected);
				pDC->DrawLabel(strTime, posInfo.m_timeRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
			}
			//속성정보
			if (m_bDispFlag[3])
			{
				wxString strAttr = itemData.GetAttributeToString();
				pDC->SetTextForeground(colSelected);
				pDC->DrawLabel(strAttr, posInfo.m_attrRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
			}
			
			//type정보
			if (m_bDispFlag[4])
			{
				wxString strDesc = itemData.GetTypeName();
				pDC->SetTextForeground(m_nCurrentItemIndex == nIndex ? dispColor : m_colType);
				pDC->DrawLabel(strDesc, posInfo.m_typeNameRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
			}
		}
		
		if (bSelected)
		{	//아이템이 선택되었을때 선택표시를 Polygon으로 처리(▶)
			wxPoint ptSel[3];

			ptSel[0].x = 0;
			ptSel[0].y = 0;

			ptSel[1].x = 0;
			ptSel[1].y = 0;

			ptSel[2].x = 0;
			ptSel[2].y = 0;

			//Polygon 보정
			wxRect rcMain(posInfo.m_mainRect);

			ptSel[0].x = rcMain.GetLeft() + 3;
			ptSel[0].y = rcMain.GetTop() + 3;

			ptSel[1].x = rcMain.GetLeft() + 9;
			ptSel[1].y = rcMain.GetTop() + 9;

			ptSel[2].x = rcMain.GetLeft() + 3;
			ptSel[2].y = rcMain.GetBottom() - 2;

			wxPen penSel(wxColour(225, 113, 0));
			wxBrush brushSel(wxColour(225, 113, 0));

			pDC->SetPen(penSel);
			pDC->SetBrush(brushSel);

			pDC->DrawPolygon(3, ptSel, -1);

			pDC->SetPen(wxNullPen);
			pDC->SetBrush(wxNullBrush);
		}
		
		if (theCommonUtil->Compare(itemData.GetName(), wxT("..")) == 0)
			pDC->DrawIcon(m_icoUpDir, wxPoint(posInfo.m_iconRect.GetLeft(), posInfo.m_iconRect.GetTop()));
		else
		{
#ifdef __WXMSW__
			int iImageFlag = ILD_NORMAL | ILC_MASK;// | ILD_TRANSPARENT | ILD_IMAGE | ILC_MASK;

			if (isDir || isFile)
			{
				if (isCut)
					iImageFlag |= ILD_BLEND25;

				if (itemData.GetAttribute() & ATTR_HIDDEN)
					iImageFlag |= ILD_BLEND25;
			}

			int iImageX = posInfo.m_iconRect.GetLeft();
			int iImageY = posInfo.m_iconRect.GetTop();

			if (isDrive)
				strName = itemData.GetDriveName();

			m_pImageMap->Draw(pDC, strName, iImageX, iImageY, iImageFlag, isDir, isDrive);
#else
#endif
		}
			
		nPosIndex++;
	}
}

wxString CListView::FindMaxData(const wxVector<wxString>& vecData)
{
	wxClientDC clientDC(this);

	clientDC.SetFont(*_gFont);
	wxVector<wxString> vecTmp(vecData);

	wxString strItem(wxT(""));
	wxVector<wxString>::iterator iter = std::max_element(vecTmp.begin(), vecTmp.end(), [&clientDC](const wxString& a, const wxString& b)
																						{
																							wxString strA(a);
																							wxString strB(b);

																							wxSize szA = clientDC.GetTextExtent(strA);
																							wxSize szB = clientDC.GetTextExtent(strB);

																							return szA.GetWidth() < szB.GetWidth();
																						}
	);

	strItem = *iter;
	clientDC.SetFont(wxNullFont);

	return strItem;
}

wxString CListView::FindMaxData(const wxString& a, const wxString& b)
{
	wxClientDC clientDC(this);
	clientDC.SetFont(*_gFont);

	wxSize szA = clientDC.GetTextExtent(a);
	wxSize szB = clientDC.GetTextExtent(b);
	wxString strMaxData = szA.GetWidth() < szB.GetWidth() ? b : a;
	
	clientDC.SetFont(wxNullFont);
	return strMaxData;
}

wxString CListView::CalcDispStr(wxDC* pDC, const wxString& strSourceSave, const wxString& strSource, bool isDrive)
{
	CPositionInfo posInfo = m_posList.at(0);

	wxString strName(strSource);
	wxString strDispName(wxT(""));
	wxString strSrcSave(strSourceSave);
	wxString strSrcName(strSource);
	wxString strDisp(wxT(""));

	wxSize szNameSize = pDC->GetTextExtent(strName);

	int iDispWidth = isDrive ? posInfo.m_mainRect.GetWidth() - 35 : posInfo.m_nameRect.GetWidth();
	int iNameWidth = szNameSize.GetWidth();

	int iLen = strName.length();

	if (iNameWidth > iDispWidth)
	{
		std::unordered_map<wxString, wxString>::const_iterator fIter = m_dispNameInfoMap.find(strSrcSave);
		if (fIter != m_dispNameInfoMap.end())
			return fIter->second;

		for (int iIndex = 0; iIndex < iLen; iIndex++)
		{
			strDisp = strName.Left(iIndex + 1);
			wxSize sizeText = pDC->GetTextExtent(wxString(strDisp + wxT("...")));
			if ((sizeText.GetWidth()) > iDispWidth)
			{
				//	strDisp = strName.Left(iIndex + (isDrive ? 1 : 3));
				strDisp = strName.Left(iIndex);
				break;
			}
		}

		strDisp += wxT("...");

		std::unordered_map<wxString, wxString>::value_type val(strSrcSave, strDisp);
		m_dispNameInfoMap.insert(val);
	}
	else
		strDisp = strName;

	return strDisp;
}

void CListView::OnSetFocus(wxFocusEvent& event)
{
	m_bSetFocus = true;
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CListView::OnKillFocus(wxFocusEvent& event)
{
	_gAppActivated = false;
	m_bSetFocus = false;
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CListView::OnSize(wxSizeEvent& event)
{
	wxSize size = event.GetSize();
	if ((size.x == 0) || (size.y == 0))
		return;
	
	m_bSizeOrColumnChanged = true;
		
	m_szChagned = size;
	if(m_pDoubleBuffer)
		delete m_pDoubleBuffer;

	m_pDoubleBuffer = new wxBitmap(m_szChagned.x, m_szChagned.y);
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CListView::OnChar(wxKeyEvent& event)
{
	int iKeyCode = event.GetKeyCode();
	if (theSkipKeyMap->IsExistSkipKey(iKeyCode))
		return;
		
	bool bShift = wxIsShiftDown();
	bool bControl = wxIsCtrlDown();
	
	if(bShift || bControl)
		return;
	
	wxString strKeyName(theCommonUtil->GetKeyName(event));
	bool bRefresh = false;
	int iLenKeyInput = 0;
	if(iKeyCode == WXK_BACK)
	{
		if(!m_strKeyInput.IsEmpty())
		{
			iLenKeyInput = m_strKeyInput.Len();
			m_strKeyInput = m_strKeyInput.Left(iLenKeyInput - 1);
		}
	}
	else
	{
		m_strKeyInput += strKeyName;
		iLenKeyInput = m_strKeyInput.Len();
	}
	
	if(iLenKeyInput > 0)
	{
		DoMatchClear();
		
		if(!m_strKeyInput.IsEmpty())
		{
			wxClientDC dc(this);
			wxSize sztip = dc.GetTextExtent(m_strKeyInput);
			wxSize szTooltip;
			
			wxRect rcWindow =this->GetClientRect();
			wxPoint ptTooltip(rcWindow.GetRight() / 2 - sztip.GetWidth(), rcWindow.GetHeight() / 2); 
			
			szTooltip.SetWidth(sztip.GetWidth() + 10);
			szTooltip.SetHeight(sztip.GetHeight() + 5);
			
			m_pMyTooltipView->SetTooltipText(m_strKeyInput);
			m_pMyTooltipView->SetThemeEnabled(true);
			m_pMyTooltipView->SetPosition(ptTooltip);
			m_pMyTooltipView->SetSize(szTooltip);
			m_pMyTooltipView->Show(true);
			
			FindMatchItems();
			bRefresh = true;
		}
		else
		{
			//클리어
			bRefresh = true;
			m_pMyTooltipView->Show(false);			
		}
	}
	else
	{	
		//클리어
		DoMatchClear();
		m_pMyTooltipView->Show(false);
	}
	
	if(bRefresh)
		theCommonUtil->RefreshWindow(this, m_viewRect);
		
	event.Skip();
}

void CListView::DoMatchClear()
{
	wxVector<int>::const_iterator fIter = m_matchItems.begin();
	while(fIter != m_matchItems.end())
	{
		CDirData* pData = (CDirData *)&m_itemList.at(*fIter);
		pData->SetMatch(false);
		
		fIter++;
	}		
}

void CListView::FindMatchItems()
{
	m_matchItems.clear();
	
	wxString strKeyInput(m_strKeyInput);
#ifdef __WXMSW__
	strKeyInput.MakeLower();
#endif

	wxString strRegEx(wxT("(^") + strKeyInput + wxT(")"));
	wxRegEx _regex(strRegEx);

	wxString strName;
	for (int iIndex = 0; iIndex < m_nTotalItems; iIndex++)
	{
		CDirData* pData = (CDirData *)&m_itemList.at(iIndex);
		strName = pData->GetName();

		if ((strName == wxT("..")) || pData->IsDrive())
			continue;

#ifdef __WXMSW__
		strName.MakeLower();
#endif
		if (!strKeyInput.IsEmpty())
		{
			if (_regex.Matches(strName))
			{
				pData->SetMatch(true);
				m_matchItems.push_back(iIndex);
			}
			else
				pData->SetMatch(false);
		}
		else
			pData->SetMatch(false);
	}

	if (m_matchItems.size() > 0)
	{
		m_nCurrentItemIndex = m_matchItems.at(m_matchItems.size() - 1) + 1;
		ReCalcPage();
	}
}

void CListView::OnKeyDown(wxKeyEvent& event)
{
	int iKeyCode = event.GetKeyCode();

	bool bControlDown = event.ControlDown();
	bool bAltDown     = event.AltDown();
	bool bShiftDown   = event.ShiftDown();

	wxString strKeyName(theCommonUtil->GetKeyName(event));
	
	if (bShiftDown)
	{
		if (theJsonConfig->IsShift_IME_KOR_MoveDrive())
			theCommonUtil->SetImeModeToEnglish(this);
		
		wxString strDriveName = strKeyName + wxT(":") + SLASH;
		if (theDriveInfo->IsExistDrive(strDriveName))
		{
			if(CLocalFileSystem::IsCanReadDir(strDriveName))
			{
				if(!theCommonUtil->IsSameDrive(strDriveName, m_strCurrentPath))
				{	
					LoadDir(strDriveName);
					theSplitterManager->ChangeTabPagePathName(strDriveName);
			
					theCommonUtil->RefreshWindow(this, m_viewRect);
				}
			}
		}
	}
	else if (bControlDown)
	{
	//	if (iKeyCode != WXK_DOWN && iKeyCode != WXK_UP)
	//	{
	//		if (iKeyCode >= WXK_F1 && iKeyCode <= WXK_F12)
	//			ShowVisitDir(true, iKeyCode);
	//	}
	}
	else if (bAltDown)
	{
		//	if (iKeyCode >= 48 && iKeyCode <= 57)  //48(0) ~ 57(9)
		//	{
		//		int iColumnCount = iKeyCode - 48;
				//	theConfig->setColumnCount(iColumnCount);
				//	theJsonConfig->SetColumnCount(iColumnCount);
		//		theJsonConfig->SetChangeGlobalVal(wxT("DispColumn"), iColumnCount);
		//		theCommonUtil->ExecPostCommandEvent2EventHandler(wxEVT_CHANGE_VIEW_COLUMN, wxT(""), iColumnCount);
		//	}
	}
	else
	{
	//	if (theSkipKeyMap->IsExistSkipKey(iKeyCode))
	//	{
			ProcessKeyEvent(iKeyCode);
		//	return;
	//	}
	}
	
	event.Skip();
}

void CListView::ProcessKeyEvent(const int nKeyCode)
{
	switch (nKeyCode)
	{
		case WXK_F4:
			ShowFavoriteMenu();
			break;
		
		case WXK_REVERSE_SLASH:
			GotoRoot();
			break;

		case WXK_TAB:
			theSplitterManager->ChangeActiveTab();
			break;

		case WXK_SPACE:
			if (!SetSelectedItem(WXK_SPACE))
				return;
			
			m_nCurrentItemIndex++;
			MoveRightAndDownKey();
			break;

		case wxKEY_DOT:
		{
			//최상위 Path
			if(m_iPathDepth == 1)
				return;
				
			m_nCurrentItemIndex = 0;
			if (!PressEnterKey())
				return;
		}
		break;

		case WXK_RETURN:
			if (!PressEnterKey())
				return;
			
			break;

		case WXK_LEFT:
			if (m_nDispColumn > 1)
			{
				m_nCurrentItemIndex -= m_nItemCountInColumn;
				MoveLeftAndUpKey();
			}

			break;

		case WXK_RIGHT:
			if (m_nDispColumn > 1)
			{
				m_nCurrentItemIndex += m_nItemCountInColumn;
				MoveRightAndDownKey();
			}
			break;

		case WXK_UP:
			if (wxGetKeyState(WXK_SHIFT))
			{
				if (!SetSelectedItem(WXK_UP))
					return;
			}
			else
			{
				m_nCurrentItemIndex--;
				MoveLeftAndUpKey();
			}

			break;

		case WXK_DOWN:
			if (wxGetKeyState(WXK_SHIFT))
			{
				if (!SetSelectedItem(WXK_DOWN))
					return;
			}
			else
			{
				m_nCurrentItemIndex++;
				MoveRightAndDownKey();
			}

			break;

		case WXK_PAGEDOWN:
			m_nCurrentItemIndex += m_nDisplayItemInView;

			if ((m_nStartIndex + (m_nItemCountInColumn * m_nDispColumn)) < m_nTotalItems)
				m_nStartIndex += (m_nItemCountInColumn * m_nDispColumn);

			if (m_nCurrentItemIndex >= (m_nTotalItems - 1))
				m_nCurrentItemIndex = m_nTotalItems - 1;

			break;

		case WXK_PAGEUP:
			m_nCurrentItemIndex -= (m_nItemCountInColumn * m_nDispColumn);
			m_nStartIndex -= (m_nItemCountInColumn * m_nDispColumn);

			if (m_nStartIndex <= 0)
				m_nStartIndex = 0;
			
			if (m_nCurrentItemIndex <= 0)
				m_nCurrentItemIndex = 0;

			break;

		case WXK_HOME:
			m_nCurrentItemIndex = 0;
			m_nStartIndex = 0;
			break;

		case WXK_END:
			if (m_nTotalItems > m_nDisplayItemInView)
			{
				int iMod = m_nTotalItems % m_nDisplayItemInView;
				m_nStartIndex = m_nTotalItems - iMod;

				if (m_nStartIndex <= 0)
					m_nStartIndex = 0;
			}

			m_nCurrentItemIndex = m_nTotalItems - 1;
			break;

		default:
			 return;
	}
	
	switch(nKeyCode)
	{
		case WXK_SPACE:
		case wxKEY_DOT:
		case WXK_RETURN:
		case WXK_LEFT:
		case WXK_RIGHT:
		case WXK_UP:
		case WXK_DOWN:
		case WXK_PAGEDOWN:
		case WXK_PAGEUP:
		case WXK_HOME:
		case WXK_END:
			m_bIsDisplayDetailInfo = false;
			break;
	}
	
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CListView::MoveLeftAndUpKey()
{
	if (!((m_nCurrentItemIndex >= m_nStartIndex) && (m_nCurrentItemIndex <= (m_nStartIndex + m_nDisplayItemInView))))
		m_nStartIndex -= m_nItemCountInColumn;

	if (m_nCurrentItemIndex <= 0)
	{
		m_nCurrentItemIndex = 0;
		m_nStartIndex = 0;
	}
}

void CListView::MoveRightAndDownKey()
{
	if ((m_nStartIndex + m_nDisplayItemInView) <= (m_nTotalItems - 1))
	{
		if (m_nCurrentItemIndex > ((m_nStartIndex + m_nDisplayItemInView) - 1))
			m_nStartIndex += m_nItemCountInColumn;
	}

	if (m_nCurrentItemIndex >= (m_nTotalItems - 1))
		m_nCurrentItemIndex = m_nTotalItems - 1;
}

bool CListView::PressEnterKey()
{
	CDirData pCurData = m_itemList.at(m_nCurrentItemIndex);
	wxString strName(pCurData.GetName());
	wxString strSearchDir(wxT(""));
	
	if (pCurData.IsDir())
	{
		if (strName == wxT(".."))
		{
			strSearchDir = theCommonUtil->GetParentPath(pCurData.GetPath());

			if (strSearchDir.IsEmpty())
				return false;

			if(m_hashHistory.size())
			{
				_HistoryItem historyItem = m_hashHistory[strSearchDir];

				m_iHistoryIndex      = historyItem.iCurrentIndex;
				m_iHistoryStartIndex = historyItem.iStartIndex;
			}
		}
		else
		{
			strSearchDir = m_strCurrentPath + (m_iPathDepth == 1 ? strName : SLASH + strName);

			int nHistoryCount = m_hashHistory.size();
			if (nHistoryCount < m_nMaxHistoryCount)
			{
				_HistoryItem historyItem;
				historyItem.iCurrentIndex = m_nCurrentItemIndex;
				historyItem.iStartIndex = m_nStartIndex;

				m_hashHistory[m_strCurrentPath] = historyItem;
			}

			if (!CLocalFileSystem::IsCanReadDir(strSearchDir))
				return false;
		}
	}
	else if (pCurData.IsFile())
	{
		theCommonUtil->LaunchAndExec(pCurData.GetFullPath(), m_strCurrentPath);
		return false;
	}
	else
	{
		strSearchDir = pCurData.GetDriveName();
		if (!CLocalFileSystem::IsCanReadDir(strSearchDir))
			return false;
	}
	
	if(pCurData.IsDir() || pCurData.IsDrive())
	{
		if(pCurData.IsDrive())
		{
			if(theCommonUtil->IsSameDrive(strSearchDir, m_strCurrentPath))
				return false;
		}
		
		LoadDir(strSearchDir);
		theSplitterManager->ChangeTabPagePathName(strSearchDir);
	}
		
	return true;
}

void CListView::GotoRoot()
{
#ifdef __WXMSW__
	if(!CLocalFileSystem::IsCanReadDir(m_strVolume))
	{
		wxString strMsg;
		strMsg = m_strVolume + theMsgManager->GetMessage(wxT("MSG_IS")) + wxT(" ") + theMsgManager->GetMessage(wxT("MSG_DIREDTORY_READ_FAIL"));
		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		return;
	}
	
	//이동 루트드라이브 == 현재디렉토리
	if (m_strVolume.CmpNoCase(m_strCurrentPath) == 0)
		return;
	
	_HistoryItem historyItem = m_hashHistory[m_strVolume];

	m_nCurrentItemIndex = historyItem.iCurrentIndex;
	m_nStartIndex = historyItem.iStartIndex;
	
	LoadDir(m_strVolume);	
	theSplitterManager->ChangeTabPagePathName(m_strVolume);
#else
#endif
}

void CListView::DoMouseProcess(const wxPoint& pt, bool bDblClick)
{
	m_bMouseClickItemFound = false;
	if (FindItemInMousePoint(pt))
	{
		if (bDblClick)
		{
			PressEnterKey();
			return;
		}
		
		m_bMouseClickItemFound = true;
		theCommonUtil->RefreshWindow(this, m_viewRect);
	}
}

bool CListView::FindItemInMousePoint(const wxPoint& pt)
{
	bool bFoundOK = false;
	wxVector<CPositionInfo>::const_iterator fIter = m_posList.begin();
	
	int iClickPosIndex = 0;
	while(fIter != m_posList.end())
	{
		if(fIter->m_mainRect.Contains(pt))
		{	//아이템 시작인덱스 + 클릭인덱스 < 전체 아이템수
			bFoundOK = (m_nStartIndex + iClickPosIndex) < m_nTotalItems ? true : false;
			break;
		}
		
		iClickPosIndex++;
		fIter++;
	}
	
	if(bFoundOK)
		m_nCurrentItemIndex = m_nStartIndex + iClickPosIndex;
	
	return bFoundOK;
}

void CListView::OnMouseLBottonDown(wxMouseEvent& event)
{
	DoMouseProcess(event.GetPosition());
	
	if(!m_bSetFocus) theSplitterManager->ChangeActiveTab();
}

void CListView::OnMouseLButtonDBClick(wxMouseEvent& event)
{
	DoMouseProcess(event.GetPosition(), true);
}

void CListView::OnMouseRButtonDown(wxMouseEvent& event)
{
	DoMouseProcess(event.GetPosition());
}

void CListView::OnMouseRButtonUp(wxMouseEvent& event)
{
	DisplayContextMenu(event.GetPosition());
}
	
bool CListView::SetSelectedItem(int iKeyCode)
{
	int iTotalItem = m_itemList.size();
	if(m_nCurrentItemIndex >= iTotalItem - 1)
		return false;
		
	wxVector<CDirData>::iterator fIter = m_itemList.begin() + m_nCurrentItemIndex;
	
	if((theCommonUtil->Compare(fIter->GetName(), wxT("..")) != 0) && !fIter->IsDrive())
	{	
		//선택이 되어 있는 상태면 선택이 안된상태로 변경
		bool bNewSelected = fIter->IsItemSelected() ? false : true; 
		fIter->SetItemSelected(bNewSelected);
		
		if (bNewSelected) //아이템 선택
		{
			SELITEM_INFO _Info;
			_Info.m_iSelIndex = m_nCurrentItemIndex;
			_Info.m_bFile = fIter->IsFile();
			
			std::unordered_map<int, SELITEM_INFO>::value_type valsel(m_nCurrentItemIndex, _Info);
			m_hashSelectedItem.insert(valsel);
			
			if(fIter->IsFile())
				m_iSelFileCnt++;
			else
				m_iSelDirCnt++;
		}
		else
		{
			std::unordered_map<int, SELITEM_INFO>::const_iterator findKey = m_hashSelectedItem.find(m_nCurrentItemIndex);
			if (findKey != m_hashSelectedItem.end())
			{
				SELITEM_INFO _Info = findKey->second;
				if(_Info.m_bFile)
					m_iSelFileCnt--;
				else
					m_iSelDirCnt--;
					
				m_hashSelectedItem.erase(m_nCurrentItemIndex);
			}
		}
		
		if(m_hashSelectedItem.size() > 0)
		{
			m_pMyTooltipView->Show(false);
			
			wxString strSelString(theMsgManager->GetMessage(wxT("MSG_DETAILINFO_VIEW_SELINFO")));
			wxString strSelItems = strSelString + wxString::Format(theMsgManager->GetMessage(wxT("MSG_DETAILINFO_VIEW_ITEM_SELECT")), m_iSelDirCnt, m_iSelFileCnt);
			
			wxClientDC dc(this);
			wxSize sztip = dc.GetTextExtent(strSelItems);
			wxSize szTooltip;
	
			szTooltip.SetWidth(sztip.GetWidth() + 10);
			szTooltip.SetHeight(sztip.GetHeight() + 5);
	
			m_pMyTooltipView->SetTooltipText(strSelItems);
			m_pMyTooltipView->SetThemeEnabled(true);
			m_pMyTooltipView->SetPosition(wxPoint(m_viewRect.GetRight() - szTooltip.GetWidth(), m_viewRect.GetBottom() - szTooltip.GetHeight()));
			m_pMyTooltipView->SetSize(szTooltip);
			m_pMyTooltipView->Show(true);
		}
		else
		{
			m_pMyTooltipView->Show(false);
			m_iSelDirCnt = 0;
			m_iSelFileCnt = 0;
		}
	}
	
	return true;
}

void CListView::MakeCopyOrMoveList(bool bUseClipboard, bool bMove, std::list<wxString>& lstItems)
{
	int iSelectedItems = m_hashSelectedItem.size();
	if (iSelectedItems != 0)
	{
		std::unordered_map<int, SELITEM_INFO>::const_iterator iter;
		for (iter = m_hashSelectedItem.begin(); iter != m_hashSelectedItem.end(); ++iter)
		{
			SELITEM_INFO selItem = iter->second;
			CDirData* pItem = (CDirData *)&m_itemList.at(selItem.m_iSelIndex);
			lstItems.push_back(pItem->GetFullPath());

			if (bUseClipboard)
			{
				pItem->SetCut(false);

				if (bMove == true)
					pItem->SetCut(true);
			}
		}
	}
	else
	{
		CDirData* pItem = (CDirData *)&m_itemList.at(m_nCurrentItemIndex);
		if (pItem->IsDrive())
		{
			wxMessageBox(theMsgManager->GetMessage(wxT("MSG_COPY_MOVE_NOT_SUPPORT_DRIVE")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
			return;
		}
		else
		{
			lstItems.push_back(pItem->GetFullPath());
			if (bUseClipboard)
			{
				pItem->SetCut(false);
				
				if (bMove == true)
					pItem->SetCut(true);
			}
		}
	}
	
	if(bUseClipboard)
		theCommonUtil->RefreshWindow(this, m_viewRect);
}

bool CListView::MakeTrashOrDeleteData(std::list<wxString>& lstDatas, bool bTrash)
{
	bool bDel = false;
	int iSelectedItems = m_hashSelectedItem.size();

	wxString strMsg(wxT(""));
	wxString strGoTrash = theMsgManager->GetMessage(wxT("MSG_DLG_DELETE_TRASH"));
	wxString strDelComplete = theMsgManager->GetMessage(wxT("MSG_DLG_DELETE_NOT_TRASH"));

	bool bGoTrash = bTrash;
	int iRetValue = 0;
	bool bOpenCheck = false;
	if (iSelectedItems == 0)
	{
		CDirData selItem = m_itemList.at(m_nCurrentItemIndex);
		if (theCommonUtil->Compare(selItem.GetName(), wxT("..")) == 0)
			return false;
			
		if (selItem.IsFile())
			bOpenCheck = CLocalFileSystem::IsCheckedFileOpen(selItem.GetFullPath());
		
		strMsg = selItem.GetFullPath();
		
		if (bOpenCheck)
		{
			wxMessageBox(strMsg + wxT(" is opened!. you can't delete operation"), wxT("Delete...."), wxICON_ERROR, this);
			return false;
		}
		
		strMsg += wxT("\n");
		strMsg += bGoTrash ? strGoTrash : strDelComplete;

		wxString strDelItem(selItem.GetFullPath());
		lstDatas.push_back(strDelItem);
		
		iRetValue = wxMessageBox(strMsg, wxT("Delete...."), wxYES_NO | wxICON_EXCLAMATION, this);
	}
	else
	{
		std::unordered_map<int, SELITEM_INFO>::const_iterator iter = m_hashSelectedItem.begin();
		strMsg = wxT("");
		strMsg += wxString::Format(wxT("%d "), iSelectedItems);
		strMsg += theMsgManager->GetMessage(wxT("MSG_DLG_DELETE_SELITEM"));
		strMsg += wxT("\n\n");

		wxString strItem(wxT(""));
		for (iter; iter != m_hashSelectedItem.end(); iter++)
		{
			bOpenCheck = false;

			SELITEM_INFO _info = iter->second;
			CDirData itemSel = m_itemList.at(_info.m_iSelIndex);
			
			strItem = itemSel.GetFullPath();

			if (itemSel.IsFile())
			{
				bOpenCheck = CLocalFileSystem::IsCheckedFileOpen(strItem);
				if (bOpenCheck)
				{
					int iRet = wxMessageBox(strItem + wxT(" is opend another program!. this file to be skipped! \n Continue?"), wxT("Delete...."), wxYES_NO | wxICON_ERROR, this);
					if (iRet == wxYES)
						continue;
					else
						return false;
				}
			}

			lstDatas.push_back(strItem);

			strMsg += strItem;
			strMsg += wxT("\n");
		}

		strMsg += wxT("\n");
		strMsg += bGoTrash ? strGoTrash : strDelComplete;

		iRetValue = wxMessageBox(strMsg, wxT("Delete...."), wxYES_NO | wxICON_EXCLAMATION, this);			
	}
	
	if (iRetValue == wxYES)
	{
		if (lstDatas.size() > 0)
		{
			bDel = true;
			m_hashSelectedItem.clear();
		}
	}
	
	return bDel;
}

wxVector<CDirData>::iterator CListView::GetItemExist(const wxString& strName, bool& bExist)
{
	bExist = false;
	auto it = std::find_if(m_itemList.begin(), m_itemList.end(), [&strName](const CDirData& data) {
		int iCmp = -1;
#ifdef __WXMSW__ //윈도우의 경우는 대소문자를 구분하지 않음
		iCmp = data.GetName().CmpNoCase(strName);
#else
		iCmp = data.GetName().Cmp(strName);
#endif
		return (iCmp == 0);
	});

	if (it != m_itemList.end())
		bExist = true;

	return it;
}

void CListView::DoSelectedItemsClear()
{
	std::unordered_map<int, SELITEM_INFO>::iterator iTer = m_hashSelectedItem.begin();
	while(iTer != m_hashSelectedItem.end())
	{
		SELITEM_INFO _Info = iTer->second;
		CDirData* data = (CDirData *)&m_itemList.at(_Info.m_iSelIndex);
		
		data->SetItemSelected(false);
		iTer++;
	}
	
	m_hashSelectedItem.clear();
	m_pMyTooltipView->Show(false);
	
	m_iSelDirCnt = 0;
	m_iSelFileCnt = 0;
}

void CListView::DoMyEventExecute(wxCommandEvent& event)
{
	wxEventType evtType = event.GetEventType();
	if(evtType == wxEVT_EXEC_FILE_EDIT)
	{
		ExecFileEditProgram();
	}
	else if(evtType == wxEVT_EXEC_CONTEXT_MENU)
	{
		m_bContextMenuFromMenuEvent = true;
		int iItemPosition = m_nCurrentItemIndex % m_nDisplayItemInView;
		CPositionInfo posInfo = m_posList.at(iItemPosition);

		wxPoint pt(posInfo.m_nameRect.GetRight() / 4, posInfo.m_nameRect.GetTop() + ICON_HEIGHT);

		DisplayContextMenu(pt);
		
		m_bContextMenuFromMenuEvent = false;
	}
	else if(evtType == wxEVT_DIR_FILE_RENAME)
	{
		DoRenameFromMenuPrepare();
	}
	else if(evtType == wxEVT_ITEM_ALL_SELECT || evtType == wxEVT_ITEM_ALL_RELEASE)
	{
		DoSelectAllOrRelease(evtType);
	}
}

void CListView::DisplayContextMenu(const wxPoint& pt)
{
	int iSelectedItemIndex = m_nCurrentItemIndex;
	int iItemCount = m_itemList.size();

	if (iItemCount < m_nCurrentItemIndex)
		m_nCurrentItemIndex = iSelectedItemIndex;

	wxArrayString arrString;
	int iSelectedItems = m_hashSelectedItem.size();
	if (iSelectedItems != 0)
	{
		std::unordered_map<int, SELITEM_INFO>::const_iterator iter = m_hashSelectedItem.begin();
		while(iter != m_hashSelectedItem.end())
		{
			SELITEM_INFO _ItemInfo = iter->second;
			int iSelItem = _ItemInfo.m_iSelIndex;

			CDirData selItem = m_itemList.at(iSelItem);
			arrString.Add(selItem.GetFullPath());
			
			iter++;
		}
	}
	else
	{
		wxString strContextPath = m_strCurrentPath;
		if(m_bMouseClickItemFound || m_bContextMenuFromMenuEvent)
		{
			CDirData item = m_itemList.at(m_nCurrentItemIndex);
			if (item.GetName().Cmp(wxT("..")) == 0)
				return;
#ifdef __WXMSW__
			strContextPath = item.IsDrive() ? item.GetDriveName() : item.GetFullPath();
#else
			strContextPath = item.GetFullPath();
#endif
		}
		
		arrString.Add(strContextPath);
	}
	
	CContextMenuHandler cMenu;
	cMenu.SetObject(arrString);
	cMenu.ShowContextMenu(this, pt);
}

void CListView::OnMenuFileEditProcess(wxCommandEvent& event)
{
	int iID = event.GetId();
	int iIndex = iID - EXTERNAL_PROGRAM_START_ID;
	
	ExecuteExternalProgramForEdit(iIndex);
}

void CListView::ExecFileEditProgram()
{
	int iExternalPGCount = _gExternalPGList.size();
	if(iExternalPGCount == 0)
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_FILE_MENU_EDIT_NOT_SET")), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
		return;
	}
	
	int iSelCount = m_hashSelectedItem.size();
	if(iSelCount == 0)
	{
		CDirData data = m_itemList.at(m_nCurrentItemIndex);
		if (data.IsDir() || data.IsDrive())
		{
			wxString strMsg = data.IsDrive() ? wxT("MSG_FILE_MENU_EDIT_NOT_SUPPORT_DRV") : wxT("MSG_FILE_MENU_EDIT_NOT_SUPPORT_DIR");
			wxMessageBox(theMsgManager->GetMessage(strMsg), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
			return;
		}
	}
	else
	{
		bool bIncludedDir = false;
		std::unordered_map<int, SELITEM_INFO>::const_iterator fIter = m_hashSelectedItem.begin();
		while(fIter != m_hashSelectedItem.end())
		{
			SELITEM_INFO _info = fIter->second;
			CDirData itemSel = m_itemList.at(_info.m_iSelIndex);
			if(itemSel.IsDir())
			{
				bIncludedDir = true;
				break;
			}
			
			fIter++;
		}
		
		if(bIncludedDir)
		{
			wxMessageBox(wxT("The directory is included in the selection"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
			return;
		}
	}
	
	if(iExternalPGCount > 1)
	{
		int iItemPosition = m_nCurrentItemIndex - m_nStartIndex;//% m_nDisplayItemInView;
		CPositionInfo posInfo = m_posList.at(iItemPosition);
		
		wxPoint pt(posInfo.m_nameRect.GetLeft() + 5, posInfo.m_nameRect.GetTop() + ICON_HEIGHT + 5);
		wxMenu menu;
		
		for (int i = 0; i < iExternalPGCount; i++)
		{
			EXTERNAL_EDIT_PROGRAM extInfo = _gExternalPGList.at(i);
			wxMenuItem* pMenuItem = menu.Append(EXTERNAL_PROGRAM_START_ID + i, extInfo._strPGName);

			int iIconIndex = theCommonUtil->GetIconIndex(extInfo._strPGPath);
			HICON hIcon = ImageList_GetIcon(_gImageList, iIconIndex, ILD_NORMAL);
			wxIcon ico;
			ico.CreateFromHICON(hIcon);

			if (ico.IsOk())
			{
				wxBitmap bmp;
				bmp.CopyFromIcon(ico);

				if(bmp.IsOk())
					pMenuItem->SetBitmap(bmp);
				else
					pMenuItem->SetBitmap(_gExternalPG);
			}
			else
				pMenuItem->SetBitmap(_gExternalPG);

			DestroyIcon(hIcon);
		}
		
		this->PopupMenu(&menu, pt);
	}
	else
	{
		//프로그램 실행
		ExecuteExternalProgramForEdit(0);
	}
}

wxString CListView::GetDirInfo()
{
	wxString strFileSize(wxT(""));
	wxString strFileSizeType(wxT(""));
	
	wxColour colorType(wxColour(192, 192, 192));
	theCommonUtil->GetSizeInfo(m_dblFileSizeInDir, strFileSize, strFileSizeType, colorType);
	if (strFileSizeType.IsEmpty())
		strFileSizeType = wxT("Bytes");
				
	wxString strDirInfo = wxString::Format(theMsgManager->GetMessage(wxT("MSG_DETAILINFO_VIEW_FORMAT"))
										, m_iDirCount
										, m_iFileCount
										, strFileSize
										, strFileSizeType);

	return strDirInfo;
}

wxString CListView::GetDetailInfo()
{
	wxVector<CDirData>::const_iterator iter = m_itemList.begin() + m_nCurrentItemIndex;
	wxString strDetailInfo = iter->GetName();
	strDetailInfo.append(wxT("  |  "));
#ifdef __WXMSW__
	if(iter->IsDrive())
	{
		wxString strDriveName = iter->GetDriveName();
		CDriveItem* pDriveItem = theDriveInfo->GetDriveItem(strDriveName);
		strDetailInfo = pDriveItem->GetDisplayName();
		strDetailInfo += wxT(" ");
		strDetailInfo += pDriveItem->GetSpace();
	}
	else
	{
#endif	
		if(iter->IsFile())
		{
			strDetailInfo += theCommonUtil->SetComma(iter->GetSize().ToString());
			strDetailInfo += wxT(" Bytes");
			strDetailInfo.append(wxT("  |  "));
		}
		
		wxString strAttr = iter->GetAttributeToString();
	
		strDetailInfo.append(strAttr);
		strDetailInfo.append(wxT("  |  "));
		

		wxString strTime = iter->GetDateTimeToString();
		strDetailInfo.append(strTime);
		strDetailInfo.append(wxT("  |  "));
		strDetailInfo.append(iter->GetTypeName());
#ifdef __WXMSW__
	}
#endif

	return strDetailInfo;
}

wxString CListView::MakeFullPathName(const wxString& strName)
{
#ifdef __WXMSW__
	return m_iPathDepth == 1 ? m_strCurrentPath + strName : m_strCurrentPath + SLASH + strName;
#else
#endif
}

void CListView::UpdateModificationTimeOfDir()
{
	DoUpdateModificationTimeOfDir();
}

void CListView::ShowFavoriteMenu()
{
	if (theBookmark->GetBookmarkSize() == 0)
	{
		wxMessageBox(wxT("Registered bookmark item is nothing"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
		return;
	}
	
	int iItemPosition = m_nCurrentItemIndex % m_nDisplayItemInView;
	CPositionInfo posInfo = m_posList.at(iItemPosition);

	wxRect rcPos(posInfo.m_nameRect);
	rcPos.SetLeft(posInfo.m_nameRect.GetLeft() + 10);
	rcPos.SetBottom(posInfo.m_nameRect.GetBottom() + 1);
	
	int iFavoriteRight = posInfo.m_nameRect.GetRight() + 3;
	int iViewRight = m_viewRect.GetRight();

	if (iFavoriteRight >= iViewRight)
		rcPos.SetLeft(rcPos.GetLeft() - 20);

	wxPoint pt(rcPos.GetLeft() , rcPos.GetTop() + rcPos.GetHeight());
	wxMenu menuFavorite;
	theBookmark->CreateBookmarkMenu(&menuFavorite);

	this->PopupMenu(&menuFavorite, pt);
}

void CListView::DoSortStart()
{
	std::sort(m_itemList.begin(), m_itemList.end(), CSorting::DirSortOfName);
//	std::qsort(&m_itemList.at(0), m_itemList.size() - 1, sizeof(m_itemList.at(0)), CSorting::DirSortOfName2);
}

void CListView::DoSelectAllOrRelease(const wxEventType& evtType)
{
	bool bAllSelect = evtType == wxEVT_ITEM_ALL_SELECT ? true : false;
	wxVector<CDirData>::iterator fIter = m_itemList.begin();
	
	while(fIter != m_itemList.end())
	{
		if(!fIter->IsDrive())
			fIter->SetItemSelected(bAllSelect);
		
		fIter++;
	}
	
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 이름변경 관련 함수
void CListView::DoRenameFromMenuPrepare()
{
	wxVector<CDirData>::const_iterator fIter = m_itemList.begin() + m_nCurrentItemIndex;
	m_strItemToRename = fIter->GetName();
	if(fIter->IsDrive() || (theCommonUtil->Compare(wxT(".."), m_strItemToRename) == 0))
		return;
	
	DoRenameOn(m_strItemToRename);
}

void CListView::OnEnterTextCtrl(wxCommandEvent& event)
{
	wxString strNewRename = m_pTxtCtrlForRename->GetValue();
	
	wxString strOldPathName = MakeFullPathName(m_strItemToRename);
	wxString strNewPathName = MakeFullPathName(strNewRename);

	bool bExist = false;
	GetItemExist(strNewRename, bExist);
	if (bExist)
	{
		wxString strMsg = strNewPathName + theMsgManager->GetMessage(wxT("MSG_DLG_ADD_DIR_EXIST"));
		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		return;
	}
	
	if(!theCommonUtil->IsCreatableDirOrFileName(strNewRename))
	{
		DoRenameOn(strNewRename);
		return;
	}
	
	DoRenameFromMenu(strOldPathName, strNewPathName);
	
	m_pTxtCtrlForRename->SetValue(wxT(""));
	m_pTxtCtrlForRename->Show(false);
	m_pMyTooltipView->Show(false);
	
	event.Skip();
}

void CListView::DoRenameOn(const wxString& strRename)
{
	int iCurrentPosition = m_nCurrentItemIndex % m_nDisplayItemInView;
	CPositionInfo posInfo = m_posList.at(iCurrentPosition);
	
	int iPosX1 = posInfo.m_mainRect.GetLeft();
	int iPosY1 = posInfo.m_mainRect.GetTop();
	int iPosX2 = posInfo.m_mainRect.GetWidth();
	int iPosY2 = posInfo.m_mainRect.GetHeight() + 1;

	int iLength = strRename.Len();

	m_pTxtCtrlForRename->SetSize(wxRect(iPosX1, iPosY1, iPosX2, iPosY2));
	m_pTxtCtrlForRename->SetLabelText(strRename);
	m_pTxtCtrlForRename->SetSelection(0, iLength);
	m_pTxtCtrlForRename->Show(true);
	m_pTxtCtrlForRename->SetFocus();
	
	wxString strDontUse(theMsgManager->GetMessage(wxT("MSG_INFO_RENAME_DONTUSE")) + theMsgManager->GetMessage(wxT("MSG_INFO_RENAME_DONTUSE_STRING")));
	wxClientDC dc(this);
	wxSize sztip = dc.GetTextExtent(strDontUse);
	wxSize szTooltip;
	
	szTooltip.SetWidth(sztip.GetWidth() + 10);
	szTooltip.SetHeight(sztip.GetHeight() + 5);
	
	m_pMyTooltipView->SetTooltipText(strDontUse);
	m_pMyTooltipView->SetThemeEnabled(true);
	m_pMyTooltipView->SetPosition(wxPoint(iPosX1 + 30, iPosY1 + iPosY2));
	m_pMyTooltipView->SetSize(szTooltip);
	m_pMyTooltipView->Show(true);
}

void CListView::OnKeyDownTextCtrl(wxKeyEvent& event)
{
	int iKeyCode = event.GetKeyCode();
	event.Skip();
	
	if (iKeyCode == WXK_ESCAPE)
	{
		m_pTxtCtrlForRename->SetLabelText(wxT(""));
		m_pTxtCtrlForRename->Show(false);
		m_pMyTooltipView->Show(false);
	}
}
	
void CListView::OnKillFocusTxtCtrl(wxFocusEvent& event)
{
	event.Skip();
	
	m_pTxtCtrlForRename->SetLabelText(wxT(""));
	m_pTxtCtrlForRename->Show(false);
	m_pMyTooltipView->Show(false);
}