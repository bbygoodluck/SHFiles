#include "../ginc.h"
#include "../interface/SHCD.h"
#include "../EvtHandler.h"
#include "SHCDView.h"
#include "ListView.h"
#include "MyToolTipView.h"
#include "../engine/LocalFileSystem.h"
#include "../dialog/DirectoryManagerDlg.h"
#include "../dialog/DlgDelete.h"
#include "../dialog/DlgAddDir.h"

#define FULL_ITEM_SELECTED 0

const int SCD_ICONWIDTH = 16;
const int SCD_ICONHEIGHT = 16;
const int MARGIN = 1;
const int DRAWNODE_VGAP = 2;
const int DRAWNODE_HGAP = 2;
const int MAX_WIDTH = 130;

BEGIN_EVENT_TABLE(CSHCDView, wxWindow)
	EVT_ERASE_BACKGROUND(CSHCDView::OnErase)
	EVT_PAINT(CSHCDView::OnPaint)
	EVT_SIZE(CSHCDView::OnSize)
	EVT_SET_FOCUS(CSHCDView::OnSetFocus)
	EVT_KILL_FOCUS(CSHCDView::OnKillFocus)
	EVT_MOUSEWHEEL(CSHCDView::OnMouseWheel)
	EVT_LEFT_DOWN(CSHCDView::OnMouseLBClick)
	EVT_LEFT_DCLICK(CSHCDView::OnMouseLBDBClick)
	EVT_MY_CUSTOM_COMMAND(wxEVT_DIRMANAGER_REFRESH_ALL, wxID_ANY, CSHCDView::OnRefreshAll)
END_EVENT_TABLE()

CSHCDView::CSHCDView(wxWindow* parent, const int nID, const wxSize& sz, const wxString& strTitle)
	: wxWindow(parent, nID, wxDefaultPosition, sz, CHILD_WINDOW_STYLE | wxWANTS_CHARS | wxVSCROLL | wxHSCROLL)
	, m_pParent(parent)
	, m_viewRect(0, 0, 0, 0)
	, m_iLineCount(0)
	, m_iWidthItemCountCanDraw(0)
	, m_iHeightItemCountCanDraw(0)
	, m_iStartIndex(0)
	, m_iLastIndex(0)
	, m_iLastRowIndex(0)
	, m_iTotalItemsCount(0)
	, m_iCurrentRowIndex(0)
	, m_iRowIndex(0)
	, m_iCurrentLevel(0)
	, m_iCurrentLevelPage(0)
	, m_iLevelPageDiff(0)
	, m_iHeightPageCnt(0)
	, m_iPrevHeightPageCnt(0)
	, m_iDispCount(0)
	, m_iThumbTrackPos(0)
	, m_iStartLevel(0)
	, m_iEndLevel(0)
	, m_iMaxLevel(0)
	, m_bSizeChanged(false)
	, m_szItem(0, 0)
	, m_PrevIter(nullptr)
	, m_crBackGround(wxColour(200, 200, 200))
	, m_crDefaultText(wxColour(70, 70, 70))
	, m_crSelectedText(wxColour(255, 51, 51))
	, m_crSelectedBackground(wxColour(60, 60, 60))
	, m_crSelectedParentBackground(wxColour(60, 60, 60))
	, m_crLine(wxColour(90, 90, 90))
	, m_pDoubleBuffer(nullptr)
{
	m_icoDrive = wxArtProvider::GetIcon(wxART_HARDDISK, wxART_OTHER, wxSize(16, 16));

	Connect(wxEVT_CHAR, wxKeyEventHandler(CSHCDView::OnChar), NULL, this);
	Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(CSHCDView::OnKeyDown), NULL, this);

	Connect(wxID_ANY, wxEVT_SCROLLWIN_THUMBTRACK, wxScrollWinEventHandler(CSHCDView::OnScroll), NULL, this);
	Connect(wxID_ANY, wxEVT_SCROLLWIN_THUMBRELEASE, wxScrollWinEventHandler(CSHCDView::OnScroll), NULL, this);
	Connect(wxID_ANY, wxEVT_SCROLLWIN_LINEUP, wxScrollWinEventHandler(CSHCDView::OnScroll), NULL, this);
	Connect(wxID_ANY, wxEVT_SCROLLWIN_LINEDOWN, wxScrollWinEventHandler(CSHCDView::OnScroll), NULL, this);
	Connect(wxID_ANY, wxEVT_SCROLLWIN_PAGEUP, wxScrollWinEventHandler(CSHCDView::OnScroll), NULL, this);
	Connect(wxID_ANY, wxEVT_SCROLLWIN_PAGEDOWN, wxScrollWinEventHandler(CSHCDView::OnScroll), NULL, this);

	if (theJsonConfig->IsSelectedSCDItemToParent())
	{
		m_crSelectedBackground = wxColour(0, 220, 200);
		m_crSelectedText = wxColour(30, 30, 30);
	}

#ifndef __WXMSW__
	m_font = wxSystemSettings::GetFont(wxSYS_OEM_FIXED_FONT);
#endif

	m_pTxtCtrlForRename = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxBORDER_THEME);
	m_pTxtCtrlForRename->SetThemeEnabled(true);

	m_pTxtCtrlForRename->SetBackgroundColour(wxColour(220, 220, 220));
	m_pTxtCtrlForRename->SetBackgroundStyle(wxBG_STYLE_PAINT);
	m_pTxtCtrlForRename->SetFont(*_gFont);
	m_pTxtCtrlForRename->Show(false);

	m_pTxtCtrlForRename->Connect(wxEVT_CHAR_HOOK, wxKeyEventHandler(CSHCDView::OnCharHookFromTextCtrl), NULL, this);
	m_pTxtCtrlForRename->Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(CSHCDView::OnTextCtrlEnter), NULL, this);
	
	m_pMyTooltipView = new CMyTooltipView(this);
	m_pMyTooltipView->SetSize(60, 20);
	m_pMyTooltipView->Show(false);

}

CSHCDView::~CSHCDView()
{
	Disconnect(wxEVT_KEY_DOWN, wxKeyEventHandler(CSHCDView::OnKeyDown), NULL, this);
	Disconnect(wxEVT_CHAR, wxKeyEventHandler(CSHCDView::OnChar), NULL, this);

	Disconnect(wxEVT_SCROLLWIN_THUMBTRACK, wxScrollWinEventHandler(CSHCDView::OnScroll), NULL, this);
	Disconnect(wxEVT_SCROLLWIN_THUMBRELEASE, wxScrollWinEventHandler(CSHCDView::OnScroll), NULL, this);
	Disconnect(wxEVT_SCROLLWIN_LINEUP, wxScrollWinEventHandler(CSHCDView::OnScroll), NULL, this);
	Disconnect(wxEVT_SCROLLWIN_LINEDOWN, wxScrollWinEventHandler(CSHCDView::OnScroll), NULL, this);
	Disconnect(wxEVT_SCROLLWIN_PAGEUP, wxScrollWinEventHandler(CSHCDView::OnScroll), NULL, this);
	Disconnect(wxEVT_SCROLLWIN_PAGEDOWN, wxScrollWinEventHandler(CSHCDView::OnScroll), NULL, this);

	if (m_pDoubleBuffer)
		delete m_pDoubleBuffer;
	
	m_pDoubleBuffer = nullptr;
	
	m_pTxtCtrlForRename->Disconnect(wxEVT_CHAR_HOOK, wxKeyEventHandler(CSHCDView::OnCharHookFromTextCtrl), NULL, this);
	m_pTxtCtrlForRename->Disconnect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(CSHCDView::OnTextCtrlEnter), NULL, this);
}

void CSHCDView::OnPaint(wxPaintEvent& event)
{
	if (!m_pDoubleBuffer->IsOk())
		return;

	m_viewRect = GetClientRect();

	wxPaintDC dc(this);
	PrepareDC(dc);

	wxMemoryDC memDC(&dc);

	memDC.SelectObject(*m_pDoubleBuffer);
	memDC.Clear();

#ifdef __WXMSW__
	memDC.SetFont(*_gFont);
#else
	memDC.SetFont(m_font);
#endif

	wxBrush brush(m_crBackGround);
	memDC.SetBrush(brush);
	memDC.DrawRectangle(m_viewRect);

	Renderer(&memDC);

	dc.Blit(0, 0, m_pDoubleBuffer->GetWidth(), m_pDoubleBuffer->GetHeight(), &memDC, 0, 0);
	memDC.SelectObject(wxNullBitmap);
}

void CSHCDView::Renderer(wxDC* pDC)
{
	if (m_bDriveLoad)
		return;

	if (m_bSizeChanged)
		CalcItemCountInView(pDC);

	if (!m_bLoadOK)
		Initialize();

	DisplayItems(pDC);
	m_bSizeChanged = false;
}

void CSHCDView::OnErase(wxEraseEvent& event)
{
}

void CSHCDView::OnSize(wxSizeEvent& event)
{
	m_bSizeChanged = true;
	wxSize size = GetClientSize();
	if ((size.x == 0) || (size.y == 0))
		return;

	if (m_szChagned.x != size.x)
	{
		m_szChagned = size;
		if (m_pDoubleBuffer)
			delete m_pDoubleBuffer;

		m_pDoubleBuffer = new wxBitmap(m_szChagned.x, m_szChagned.y);
	}

	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CSHCDView::OnSetFocus(wxFocusEvent& event)
{
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CSHCDView::OnKillFocus(wxFocusEvent& event)
{
}

void CSHCDView::SetInit()
{
	Initialize();
}

void CSHCDView::Initialize()
{
	m_iStartIndex = 0;
	m_iStartLevel = 0;
	m_iEndLevel = m_iWidthItemCountCanDraw;

	m_Datas.clear();
	m_Datas = theSHCD->GetDatas();

	m_bLoadOK    = true;
	m_bDriveLoad = false;
	m_iTotalItemsCount = m_Datas.size();

	m_curIter = theSHCD->GetCur();
	m_pCurrNode = *m_curIter;
	
	m_iCurrentRowIndex = m_pCurrNode->_index;

	if (m_pCurrNode->_level < m_iEndLevel)
		m_iLevelPageDiff = 0;
	else
		m_iLevelPageDiff = m_pCurrNode->_level - m_iHeightItemCountCanDraw;

	int iCurrRow = m_pCurrNode->_row;
	if (iCurrRow > (m_iStartIndex + m_iHeightItemCountCanDraw))
	{
		m_iStartIndex = iCurrRow - 5;
		if (m_iStartIndex < 0)
			m_iStartIndex = 0;
	}

	m_iMaxLevel = theSHCD->GetMaxLevel();

	SetScrollbar(wxVERTICAL, 0, 1, m_iTotalItemsCount);
	SetScrollPos(wxVERTICAL, m_pCurrNode->_row);

	SetScrollbar(wxHORIZONTAL, 0, 1, m_iMaxLevel);
	SetScrollPos(wxHORIZONTAL, m_pCurrNode->_level);
}


void CSHCDView::CalcItemCountInView(wxDC* pDC)
{
	int iWidthSize = (DRAWNODE_HGAP * 2) + SCD_ICONWIDTH + MAX_WIDTH;
	int iHeightSize = (DRAWNODE_VGAP * 2) + SCD_ICONHEIGHT;

	m_szItem.SetWidth(iWidthSize);
	m_szItem.SetHeight(iHeightSize);

	m_iWidthItemCountCanDraw = m_viewRect.GetWidth() / iWidthSize;
	m_iHeightItemCountCanDraw = m_viewRect.GetHeight() / iHeightSize;
}

void CSHCDView::DisplayItems(wxDC* pDC)
{
	if (m_iTotalItemsCount == 0)
		return;

	int nDisplayItemCount = m_iStartIndex + m_iHeightItemCountCanDraw;
	if (nDisplayItemCount >= m_iTotalItemsCount)
		nDisplayItemCount = m_iTotalItemsCount;

	if (nDisplayItemCount <= 0)
		return;
	

	m_iLastRowIndex = nDisplayItemCount;
	int iIconLeft = DRAWNODE_HGAP;
	int iIconTop = DRAWNODE_VGAP;

	int iLeft = 0;
	int iTop = 0;
	int iRight = 0;
	int iBottom = 0;
	int iNameLeft = 0;
	int iNameTop = 0;
	int iNameRight = 0;
	int iNameBottom = 0;

	int iDispCount = 0;
	int iPrevRow = 0;

	int iIndex = m_iStartIndex;
	bool bCurrentSelected = false;
	wxIcon dispIco;
	for (iIndex ; iIndex < nDisplayItemCount; iIndex++)
	{
		wxVector<NODE* > rowData = m_Datas[iIndex];
		wxVector<NODE *>::iterator curRowIter = rowData.begin();

		if ((iIndex == m_iStartIndex))
			DisplayTopParentPath(pDC, rowData[0], m_iStartIndex);

		while (curRowIter != rowData.end())
		{
			NODE* pNode = (*curRowIter);
			
			int iRow = pNode->_row;
			int iLevel = pNode->_level;

			iLeft = ((iLevel - m_iLevelPageDiff) * m_szItem.GetWidth()) + MARGIN;
			iTop = (iRow - m_iStartIndex) * m_szItem.GetHeight() + MARGIN;
			iRight = m_szItem.GetWidth() + MARGIN;
			iBottom = m_szItem.GetHeight() - (MARGIN + 1);

			wxRect rcDisp(iLeft, iTop, iRight, iBottom);
			pNode->_rect = rcDisp;

			iIconLeft = rcDisp.GetLeft() + DRAWNODE_HGAP;
			iIconTop = rcDisp.GetTop() + DRAWNODE_VGAP + 1;

			iNameLeft = rcDisp.GetRight() - MAX_WIDTH;
			iNameTop = iTop + DRAWNODE_VGAP + 1;
			iNameRight = MAX_WIDTH;
			iNameBottom = iBottom;

			wxRect rcNameDisp(iNameLeft, iNameTop, iNameRight, iNameBottom);
			pNode->_rectName = rcNameDisp;

			wxString strName(pNode->_Name);
			wxString strDisp(wxT(""));

			if (pNode->_childNode.size() > 0)
				CalcDispStr(pDC, strName, strDisp, MAX_WIDTH - (SCD_ICONWIDTH + DRAWNODE_HGAP));// rcNameDisp, false);
			else
				strDisp = strName;

			pNode->_DispName = strDisp;

			bCurrentSelected = false;
			if (m_pCurrNode == pNode)
			{
				wxSize szSelected = pDC->GetTextExtent(strDisp);
				wxRect rcSelected(rcNameDisp);

				rcSelected.SetLeft(rcNameDisp.GetLeft() - (SCD_ICONWIDTH + 1 + DRAWNODE_HGAP * 2));
				rcSelected.SetTop(rcNameDisp.GetTop() - 4);
				rcSelected.SetWidth(szSelected.GetWidth() + (SCD_ICONWIDTH + 1 + DRAWNODE_HGAP * 4));
				rcSelected.SetBottom(rcNameDisp.GetBottom() + 1);

				if (theJsonConfig->IsSelectedSCDItemToParent())
				{
					wxRect rcSelectedParent;
					int iSelectedParentLeft = (iRow == 0 && iLevel == 0) ? 0 : MARGIN;
					int iSelectedParentRight = (iRow == 0 && iLevel == 0) ? 0 : rcNameDisp.GetLeft() - (SCD_ICONWIDTH + MARGIN * 3);
					rcSelectedParent.SetLeft(MARGIN);
					rcSelectedParent.SetTop(rcNameDisp.GetTop() - 4);
					rcSelectedParent.SetRight(iSelectedParentRight);
					rcSelectedParent.SetBottom(rcNameDisp.GetBottom() + 1);

					wxBrush bkBrushParent(m_crSelectedParentBackground);
					wxPen bkPenParent(wxColour(0, 0, 0));// m_crSelectedParentBackground);

					pDC->SetBrush(bkBrushParent);
					pDC->SetPen(bkPenParent);

					pDC->DrawRectangle(rcSelectedParent);
				}
		
				wxBrush bkBrush(m_crSelectedBackground);
				wxPen bkPen(m_crSelectedParentBackground);// m_crSelectedBackground);

				pDC->SetBrush(bkBrush);
				pDC->SetPen(bkPen);

				pDC->DrawRectangle(rcSelected);
				bCurrentSelected = true;

				SetFullPath();
				DisplayParentPath(pDC, pNode, pNode->_row);
			}

			pDC->SetTextForeground(m_crDefaultText);

			if (bCurrentSelected)
				pDC->SetTextForeground(m_crSelectedText);
			
			pDC->DrawLabel(strDisp, rcNameDisp);
			dispIco = (*curRowIter)->_parent == nullptr ? m_icoDrive : _gNormalFolderIco;

			pDC->DrawIcon(dispIco, wxPoint(iIconLeft, iIconTop));

			DrawColLineDown(pDC, curRowIter);
			++curRowIter;
		}

		DrawRowLine(pDC, rowData);
		DrawColLineUp(pDC, rowData);
	}

	m_bDriveLoad = false;
}

void CSHCDView::DisplayTopParentPath(wxDC* pDC, NODE* pNode, int iDispRowIndex)
{
	NODE* _pCurrNode = pNode;
	NODE* pParent = _pCurrNode->_parent;

	if (pParent != nullptr)
	{
		int iLevel = pParent->_level;
		int iRow = pParent->_row;

		if (iRow != iDispRowIndex)
			DrawParentPath(pDC, pParent, iDispRowIndex, wxColour(75, 75, 152));

		DisplayTopParentPath(pDC, pParent, iDispRowIndex);
	}
}

void CSHCDView::DisplayParentPath(wxDC* pDC, NODE* pNode, int iDispRowIndex)
{
	NODE* _pCurrNode = pNode;
	NODE* pParent = pNode->_parent;

	if (pParent != nullptr)
	{
		int iLevel = pParent->_level;
		int iRow = pParent->_row;

		if (theJsonConfig->IsSelectedSCDItemToParent())
			iRow = -1;

		if (iRow != iDispRowIndex)
			DrawParentPath(pDC, pParent, iDispRowIndex, wxColour(255, 51, 51));

		DisplayParentPath(pDC, pParent, iDispRowIndex);
	}
}

void CSHCDView::DrawParentPath(wxDC* pDC, NODE* pNode, int iDispRowIndex, const wxColour& color)
{
	int iLeft = 0;
	int iTop = 0;
	int iRight = 0;
	int iBottom = 0;
	int iNameLeft = 0;
	int iNameTop = 0;
	int iNameRight = 0;
	int iNameBottom = 0;

	int iIconLeft = DRAWNODE_HGAP;
	int iIconTop = DRAWNODE_VGAP;

	int iLevel = pNode->_level;

	iLeft = ((iLevel - m_iLevelPageDiff) * m_szItem.GetWidth()) + MARGIN;
	iTop = ((iDispRowIndex - m_iStartIndex) * m_szItem.GetHeight()) + MARGIN;
	iRight = m_szItem.GetWidth() + MARGIN;
	iBottom = m_szItem.GetHeight() - (MARGIN + 1);

	wxRect rcDisp(iLeft, iTop, iRight, iBottom);

	iIconLeft = rcDisp.GetLeft() + DRAWNODE_HGAP;
	iIconTop = rcDisp.GetTop() + DRAWNODE_VGAP + 1;

	iNameLeft = rcDisp.GetRight() - MAX_WIDTH;
	iNameTop = iTop + DRAWNODE_VGAP + 1;
	iNameRight = MAX_WIDTH;
	iNameBottom = iBottom;

	wxRect rcNameDisp(iNameLeft, iNameTop, iNameRight, iNameBottom);

	wxString strName(pNode->_Name);
	wxString strDisp(wxT(""));

	CalcDispStr(pDC, strName, strDisp, MAX_WIDTH - (SCD_ICONWIDTH + DRAWNODE_HGAP));// rcNameDisp, false);

	pDC->SetTextForeground(color);
	pDC->DrawLabel(strDisp, rcNameDisp);
	wxIcon ico = pNode->_parent == nullptr ? m_icoDrive : _gNormalFolderIco;
	pDC->DrawIcon(ico, wxPoint(iIconLeft, iIconTop));
}

void CSHCDView::DrawRowLine(wxDC* pDC, wxVector<NODE*>& vecDatas)
{
	int iCount = vecDatas.size();
	if (iCount == 0)
		return;

	NODE* pNode = vecDatas.at(0);
	wxVector<NODE *>::const_iterator iter = vecDatas.begin();
	
	wxPen bkPen(m_crLine, 1, wxPENSTYLE_SOLID);
	pDC->SetPen(bkPen);

	int x1, y1, x2, y2;
	while (iter != vecDatas.end())
	{
		if (pNode != (*iter))
		{
			wxRect rcPrev = pNode->_rectName;

			wxSize szSelected = pDC->GetTextExtent(pNode->_DispName);
			wxRect rcName(rcPrev);
			rcName.SetLeft(rcPrev.GetLeft() - DRAWNODE_HGAP);
			rcName.SetWidth(szSelected.GetWidth() + (DRAWNODE_HGAP * 2));
			
			wxRect rcNext = (*iter)->_rect;

			x1 = rcName.GetRight() + MARGIN + 1;
			y1 = rcName.GetTop() + (SCD_ICONHEIGHT / 2) - 1;

			x2 = rcNext.GetLeft() - MARGIN;
			y2 = y1;

			pDC->DrawLine(x1, y1, x2, y2);

			pNode = (*iter);
		}

		++iter;
	}
}

void CSHCDView::DrawColLineDown(wxDC* pDC, wxVector<NODE *>::iterator iterCurr)
{
	NODE* pNode = (*iterCurr);
	int iChildCount = pNode->_childNode.size();
	if (iChildCount < 2)
		return;

	wxVector<NODE *> vecChilds = pNode->_childNode;
	wxVector<NODE *>::const_iterator iterChild = vecChilds.begin();

	int iRow = 0;
	int iLevel = 0;
	int iLeft = 0;
	int iTop = 0;
	int iRight = 0;
	int iBottom = 0;

	int iRowTmp = 0;
	int x1, y1, x2, y2;

	NODE* pNowNode = vecChilds[0];
	while (iterChild != vecChilds.end())
	{
		NODE* pNextNode = (*iterChild);
		if (pNowNode != pNextNode)
		{
			iRow = pNowNode->_row;
			iLevel = (pNowNode->_level - m_iLevelPageDiff);

			iRowTmp = iRow - m_iStartIndex;

			iLeft = (iLevel * m_szItem.GetWidth()) + MARGIN;
			iTop = (iRowTmp * m_szItem.GetHeight()) + MARGIN;

			x1 = iLeft - (SCD_ICONWIDTH);
			y1 = iTop + (SCD_ICONHEIGHT / 2) + 2;

			iRow = pNextNode->_row;
			iLevel = pNextNode->_level - m_iLevelPageDiff;

			iRowTmp = iRow - m_iStartIndex;

			iLeft = (iLevel * m_szItem.GetWidth()) + MARGIN;
			iTop = (iRowTmp * m_szItem.GetHeight()) + MARGIN;

			x2 = x1;
			y2 = iTop + (SCD_ICONHEIGHT / 2) + 2;

			wxPen bkPen(m_crLine, 1, wxPENSTYLE_SOLID);
			pDC->SetPen(bkPen);

			pDC->DrawLine(x1, y1, x2, y2);

			if (m_iLastRowIndex > pNextNode->_row)
			{
				x1 = x2;
				y1 = y2;

				x2 = iLeft - MARGIN;
				y2 = y1;

				pDC->DrawLine(x1, y1, x2, y2);
			}
		}

		++iterChild;
	}
}

void CSHCDView::DrawColLineUp(wxDC* pDC, wxVector<NODE *>& vecDatas)
{
	if (m_iStartIndex == 0)
		return;

	if (vecDatas.size() == 0)
		return;

	NODE* pNode = vecDatas[0];
	NODE* pParent = nullptr;

	int x1, y1, x2, y2;
	int iRowIndex = 0;

	while (true)
	{
		pParent = pNode->_parent;
		iRowIndex = pParent->_row;

		if (iRowIndex < m_iStartIndex)
			break;

		pNode = pParent;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	wxRect rcDisp(pNode->_rect);

	x1 = rcDisp.GetLeft() - MARGIN;
	y1 = rcDisp.GetTop() + (SCD_ICONHEIGHT / 2) + 1;

	x2 = rcDisp.GetLeft() - SCD_ICONWIDTH;
	y2 = y1;
	
	wxPen bkPen(m_crLine, 1, wxPENSTYLE_SOLID);
	pDC->SetPen(bkPen);

	pDC->DrawLine(x1, y1, x2, y2);

	x1 = rcDisp.GetLeft() - SCD_ICONWIDTH;
	//	y1 = rcDisp.GetTop() + (SCD_ICONHEIGHT / 2) + 1;

	x2 = x1;
	y2 = 0;

	pDC->DrawLine(x1, y1, x2, y2);
	DrawColLineDownToRoot(pDC, vecDatas[0]);
}

void CSHCDView::DrawColLineDownToRoot(wxDC* pDC, NODE* pNode)
{
	NODE* _pCurrNode = pNode;
	NODE* pParent = _pCurrNode->_parent;
	NODE* pLastChild = theSHCD->FindLastChild(pParent);

	int iParentRowIndex = pParent->_row;
	int iLastChildRowIndex = pLastChild->_row;

	int x1, y1, x2, y2;
	
	if (iParentRowIndex < m_iStartIndex)
	{
		x1 = (((pParent->_level + 1 - (m_iLevelPageDiff)) * m_szItem.GetWidth()) + MARGIN) - SCD_ICONWIDTH;
		y1 = 0;

		if (m_iLastRowIndex <= iLastChildRowIndex)
		{
			x2 = x1;
			y2 = m_viewRect.GetBottom();

			wxPen bkPen(m_crLine, 1, wxPENSTYLE_SOLID);
			pDC->SetPen(bkPen);

			pDC->DrawLine(x1, y1, x2, y2);
		}
		
		if (pParent != theSHCD->GetRoot())
			DrawColLineDownToRoot(pDC, pParent);
	}
	
}

void CSHCDView::CalcDispStr(wxDC* pDC, const wxString& strSource, wxString& strOut, const int iCheckWidth)//const wxRect& rcName, bool bCheckRect)
{
	wxString strDisp(wxT(""));
	wxString strName(strSource);
	wxSize sizeEllipsis = pDC->GetTextExtent(wxT("..."));
	wxSize szNameSize = pDC->GetTextExtent(strName);

	int iLenIndex = 0;
	int iLen = strName.length();
	
	if (szNameSize.GetWidth() < iCheckWidth)
		strDisp = strName;
	else
	{
		for (int iIndex = 0; iIndex < iLen; iIndex++)
		{
			strDisp = strName.Left(iIndex + 1);
			wxSize sizeText = pDC->GetTextExtent(wxString(strDisp + wxT("...")));
			if ((sizeText.GetWidth()) >= iCheckWidth)
			{
				strDisp = strName.Left(iIndex);
				break;
			}
		}

		strDisp += wxT("...");
	}

	strOut = strDisp;
}

void CSHCDView::OnKeyDown(wxKeyEvent& event)
{
	bool bShiftDown = event.ShiftDown();
	if (theJsonConfig->IsShift_IME_KOR_MoveDrive())
	{
		if (bShiftDown)
			theCommonUtil->SetImeModeToEnglish(this);
	}

	int iKeyCode = event.GetKeyCode();
	ProcessKeyEvent(iKeyCode);

	event.Skip();
}

void CSHCDView::OnChar(wxKeyEvent& event)
{
	event.Skip();

	int iKeyCode = event.GetKeyCode();
	bool bShift = wxIsShiftDown();
	bool bCtrl = wxIsCtrlDown();

	wxString strKeyName(theCommonUtil->GetKeyName(event));

	if (bShift)
	{
		this->Freeze();
	
#ifdef __WXMSW__
		wxString strDriveName = strKeyName + wxT(":");
		if (theDriveInfo->IsExistDrive(strDriveName.MakeUpper() + SLASH))
		{
			m_bDriveLoad = true;
			wxString strSHCDFilePath = theCommonUtil->GetWorkDir() + SLASH + wxT("treedata") + SLASH + strDriveName.Left(1) + wxT(".SCD");
			DirectoryManagerDlg* pParent = (DirectoryManagerDlg *)m_pParent;

			pParent->SetSCDFileInfo(strSHCDFilePath, strDriveName);
			pParent->LoadSCDData(false);

			Initialize();
		}
		else
			wxMessageBox(strDriveName + wxT(" is not drive"), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);

		this->Thaw();
		theCommonUtil->RefreshWindow(this, m_viewRect);
#endif
	}
	
	if (bCtrl)
	{
		bool bPressFKey = false;
#ifdef __WXMSW__
		bPressFKey = (strKeyName.CmpNoCase(wxT("Ctrl-F")) == 0);
#else
		bPressFKey = (strKeyName.Cmp(wxT("Ctrl-F")) == 0);
#endif
		if (bPressFKey)
		{
			this->Freeze();
			DirectoryManagerDlg* pParent = (DirectoryManagerDlg *)m_pParent;
			pParent->FindDir();
			this->Thaw();
		}
	}
}

void CSHCDView::ProcessKeyEvent(const int nKeyCode)
{
	DirectoryManagerDlg* pParent = (DirectoryManagerDlg *)m_pParent;

	switch (nKeyCode)
	{
		case WXK_ESCAPE:
		{
			wxCloseEvent evt(wxEVT_CLOSE_WINDOW);
			wxPostEvent(pParent, evt);
		}
			break;

		case WXK_LEFT:
			CalcKeyLeft();
			break;

		case WXK_RIGHT:
			CalcKeyRight();
			break;

		case WXK_UP:
			CalcKeyUp();
			break;

		case WXK_DOWN:
			CalcKeyDown();
			break;

		case WXK_PAGEDOWN:
		{
			wxEventType evtType = wxEVT_SCROLLWIN_PAGEDOWN;
			ScrollVerticalProcess(evtType, false);
		}
			break;

		case WXK_PAGEUP:
		{
			wxEventType evtType = wxEVT_SCROLLWIN_PAGEUP;
			ScrollVerticalProcess(evtType, false);
		}
			break;

		case WXK_RETURN:
			MoveDir();
			break;

		case WXK_F2:
			RenameDir();
			break;

		case WXK_F5:
		{
			int iRet = wxMessageBox(theMsgManager->GetMessage(wxT("MSG_DLG_SCD_SEARCH_ENTIRE_DRV")), PROGRAM_FULL_NAME, wxICON_QUESTION | wxYES_NO);
			if (iRet == wxYES)
			{
				wxString strFullPath = theSHCD->GetFullPath(m_pCurrNode);
				wxString strSHCDFilePath = theCommonUtil->GetWorkDir() + SLASH + wxT("treedata") + SLASH + strFullPath.Left(1) + wxT(".SCD");

				pParent->SetSCDFileInfo(strSHCDFilePath, strFullPath);
				pParent->RefreshAll();
			}
		}
			break;

		case WXK_F6:
		{
			wxString strFullPath = theSHCD->GetFullPath(m_pCurrNode);
			if (theDriveInfo->IsExistDrive(strFullPath))
			{
				wxString strMsg = wxString::Format(theMsgManager->GetMessage(wxT("MSG_MCD_DRIVE_SELECT_AND_RELOAD")), strFullPath);
				wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxICON_QUESTION | wxOK);
			}
			else
				pParent->LoadPartFolder(strFullPath, m_pCurrNode);
		}
			break;
		case WXK_F7:
			AddDir();
			break;

		case WXK_DELETE:
			DeleteDir();
			break;

		default:
			break;
	}

	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CSHCDView::CalcKeyDown()
{
	if ((*m_curIter) == theSHCD->GetRoot())
	{
		m_curIter++;
		m_pCurrNode = *m_curIter;
	}
	else
	{
		FindChild(WXK_DOWN, m_pCurrNode);
	}

	MoveVerticalScroll();

}

void CSHCDView::CalcKeyUp()
{
	if (m_pCurrNode == theSHCD->GetRoot())
		return;

	FindChild(WXK_UP, m_pCurrNode);
	MoveVerticalScroll();
}

void CSHCDView::FindChild(int iKeyCode, NODE* pNode)
{
	int iCurrNodeLevel = pNode->_level;
	int iCurrNodeRow = pNode->_row;

	if (iKeyCode == WXK_UP) //Key up
	{
		// 표시하고 있는 row를 하나 줄임
		iCurrNodeRow--;
		if (iCurrNodeRow >= 0)                                                    
		{
			bool bFounded = false;
			while (iCurrNodeRow >= 0)
			{	//같은 depth의 데이터가 존재하는지 체크
				wxVector<NODE *> vecNodeDatas = m_Datas[iCurrNodeRow];
				wxVector<NODE *>::iterator fIter = vecNodeDatas.begin();

				for (fIter; fIter != vecNodeDatas.end(); ++fIter)
				{
					if ((*fIter)->_level == iCurrNodeLevel)
					{	//같은 depth의 데이터가 존재 할 경우
						CalcStartIndex(iKeyCode, *fIter);
						bFounded = true;
						break;
					}
				}

				if (bFounded)
					break;

				iCurrNodeRow--;
			}
			//같은 depth의 데이터를 찾지 못하였을 경우 현재위치에서 한단계 앞으로
			if (!bFounded)
			{
				m_curIter--;
				m_pCurrNode = *m_curIter;
			}
		}
		else
		{
			m_curIter--;
			m_pCurrNode = *m_curIter;
		}
	}
	else if (iKeyCode == WXK_DOWN)
	{
		iCurrNodeRow++;
		if (iCurrNodeRow < m_iTotalItemsCount)
		{
			bool bFounded = false;
			while (iCurrNodeRow < m_iTotalItemsCount)
			{
				wxVector<NODE *> vecNodeDatas = m_Datas[iCurrNodeRow];
				wxVector<NODE *>::iterator fIter = vecNodeDatas.begin();

				for (fIter; fIter != vecNodeDatas.end(); ++fIter)
				{
					if ((*fIter)->_level == iCurrNodeLevel)
					{
						CalcStartIndex(iKeyCode, *fIter);
						bFounded = true;
						break;
					}
				}

				if (bFounded)
					break;

				iCurrNodeRow++;
			}

			if (!bFounded)
			{
				m_curIter++;
				m_pCurrNode = *m_curIter;
			}
		}
		else
		{
			if (m_curIter != theSHCD->GetLast())
			{
				m_curIter++;
				m_pCurrNode = *m_curIter;
			}
		}
	}
}

void CSHCDView::CalcKeyLeft()
{
	if(*m_curIter == theSHCD->GetRoot())
	{
		m_PrevIter = nullptr;
		return;
	}

	m_pCurrNode = (*m_curIter)->_parent;
	m_curIter = theSHCD->GetBegin() + m_pCurrNode->_index;
	
	if (m_iStartIndex > m_pCurrNode->_row)
		m_iStartIndex = m_pCurrNode->_row;

	int iCurrDepth = m_pCurrNode->_level;
	if (iCurrDepth < m_iWidthItemCountCanDraw)
	{
		if (m_iLevelPageDiff > 0)
		{
			m_iLevelPageDiff--;
			m_iStartLevel--;
			m_iEndLevel--;
			
			if (m_iEndLevel < m_iWidthItemCountCanDraw)
			{
				m_iLevelPageDiff = 0;
				m_iStartLevel = 0;
				m_iEndLevel = m_iWidthItemCountCanDraw;
			}
		}
	}

	MoveVerticalScroll();
	MoveHorizontalScroll(m_pCurrNode->_level);
}

void CSHCDView::CalcKeyRight()
{
	wxVector<NODE* >::iterator lastIter = theSHCD->GetLast();
	if (lastIter == m_curIter)
		return;

	int iPrevDepth = m_pCurrNode->_level;
	int iCurrDepth = -1;

	m_curIter++;
	m_pCurrNode = *m_curIter;

	iCurrDepth = m_pCurrNode->_level;

	if (m_pCurrNode->_row > (m_iStartIndex + (m_iHeightItemCountCanDraw - 1)))
		m_iStartIndex = m_pCurrNode->_row - (m_iHeightItemCountCanDraw - 1);

	if (iCurrDepth > (m_iEndLevel - 1))
	{
		m_iLevelPageDiff++;
		m_iStartLevel++;
		m_iEndLevel++;
	}

	MoveVerticalScroll();
	MoveHorizontalScroll(m_pCurrNode->_level);
}

void CSHCDView::CalcStartIndex()
{
	m_iHeightPageCnt = m_iCurrentRowIndex / m_iHeightItemCountCanDraw;
	m_iStartIndex = m_iHeightPageCnt * m_iHeightItemCountCanDraw;
}

void CSHCDView::CalcStartIndex(int iKeyCode, NODE* pNode)
{
	int iIndex = pNode->_index;
	m_curIter = theSHCD->GetBegin() + iIndex;
	m_pCurrNode = *m_curIter;

	int iCurrRow = m_pCurrNode->_row;

	if (iKeyCode == WXK_UP)
	{
		if (iCurrRow < m_iStartIndex)
		{
			m_iStartIndex = iCurrRow;
			if (m_iStartIndex < 0)
				m_iStartIndex = 0;

			if (m_iStartIndex < m_iHeightItemCountCanDraw)
				m_iStartIndex = 0;
		}
	}
	else if (iKeyCode == WXK_DOWN)
	{
		if (iCurrRow > (m_iStartIndex + (m_iHeightItemCountCanDraw - 1)))
			m_iStartIndex = iCurrRow - (m_iHeightItemCountCanDraw - 1);
	}
}

void CSHCDView::MoveVerticalScroll()
{
	SetScrollPos(wxVERTICAL, m_pCurrNode->_row);
}

void CSHCDView::MoveHorizontalScroll(int iPos)
{
	SetScrollPos(wxHORIZONTAL, iPos);
}

void CSHCDView::SetFullPath()
{
	wxString strFullPath = theSHCD->GetFullPath(m_pCurrNode);

	DirectoryManagerDlg* pParent = (DirectoryManagerDlg *)m_pParent;
	pParent->SetNodePath(strFullPath);
}

void CSHCDView::MoveDir()
{
	this->Freeze();
	wxString strFullPath = theSHCD->GetFullPath(m_pCurrNode);
	bool bReadable = CLocalFileSystem::IsCanReadDir(strFullPath);
	this->Thaw();

	if (!bReadable)
		return;
	
	theSplitterManager->GetActiveTab()->GetActiveViewPanel()->ReloadPathOfView(strFullPath);
//	wxCommandEvent evt(wxEVT_DIRMANAGER_MOVE_DIR);

//	evt.SetString(strFullPath);
//	evt.SetInt(99);
//	wxPostEvent(_gActiveView, evt);

	DirectoryManagerDlg* pDlg = (DirectoryManagerDlg *)m_pParent;
	wxCloseEvent evtclose(wxEVT_CLOSE_WINDOW);
	wxPostEvent(pDlg, evtclose);
}

void CSHCDView::OnScroll(wxScrollWinEvent& event)
{
	wxEventType evtType = event.GetEventType();

	int iPos = event.GetPosition();
	int iOrientation = event.GetOrientation();
	if (iOrientation == wxVERTICAL)
	{
		ScrollVerticalProcess(evtType, false, iPos);
		MoveVerticalScroll();
	}
	else if (iOrientation == wxHORIZONTAL)
	{
		ScrollHorzProcess(evtType, iPos);
	}

//	RefreshWindow();
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CSHCDView::ScrollVerticalProcess(wxEventType evtType, bool bWheel, int iThumbTrackPos)
{
	if (evtType == wxEVT_SCROLLWIN_LINEDOWN)
	{
		if ((m_iStartIndex + m_iHeightItemCountCanDraw) < m_iTotalItemsCount)
		{
			if (bWheel)
				m_iStartIndex += 3;
			else
				m_iStartIndex++;

			if (m_pCurrNode->_row < m_iStartIndex)
				SetScrolledCurrentNode(m_iStartIndex);
		}
	}

	if (evtType == wxEVT_SCROLLWIN_LINEUP)
	{
		int iDiffNum = 0;
		if (bWheel)
		{
			m_iStartIndex -= 3;
			iDiffNum = 5;
		}
		else
		{
			m_iStartIndex--;
			iDiffNum = 2;
		}

		if (m_iStartIndex < 0)
			m_iStartIndex = 0;

		if (m_pCurrNode->_row > (m_iLastRowIndex - iDiffNum))
			SetScrolledCurrentNode(m_iStartIndex + (m_iHeightItemCountCanDraw - 1));
	}

	if (evtType == wxEVT_SCROLLWIN_THUMBTRACK)
	{
		if (m_iThumbTrackPos < iThumbTrackPos)
		{
			if ((iThumbTrackPos + m_iHeightItemCountCanDraw) < m_iTotalItemsCount)
			{
				m_iStartIndex = iThumbTrackPos;
				m_iThumbTrackPos = iThumbTrackPos;
			}
			else
				m_iStartIndex = m_iTotalItemsCount - m_iHeightItemCountCanDraw;

			SetScrolledCurrentNode(m_iStartIndex);

		}
		else
		{
			m_iThumbTrackPos = iThumbTrackPos;
			m_iStartIndex = iThumbTrackPos;
			if (m_iStartIndex < 0)
				m_iStartIndex = 0;

			if (m_pCurrNode->_row > (m_iLastRowIndex - 2))
				SetScrolledCurrentNode(m_iStartIndex + (m_iHeightItemCountCanDraw - 1));
		}
	}

	if (evtType == wxEVT_SCROLLWIN_PAGEUP)
	{
		m_iStartIndex -= m_iHeightItemCountCanDraw;
		if (m_iStartIndex < 0)
			m_iStartIndex = 0;

		if (m_pCurrNode->_row >= (m_iStartIndex + m_iHeightItemCountCanDraw))
			SetScrolledCurrentNode(m_iStartIndex + (m_iHeightItemCountCanDraw - 1));
		//	else
		//		SetScrolledCurrentNode(m_pCurrNode->GetRow() - 1);
	}

	if (evtType == wxEVT_SCROLLWIN_PAGEDOWN)
	{
		if ((m_iStartIndex + m_iHeightItemCountCanDraw) < m_iTotalItemsCount)
			m_iStartIndex += m_iHeightItemCountCanDraw;

		SetScrolledCurrentNode(m_iStartIndex);
	}
}

void CSHCDView::ScrollHorzProcess(wxEventType evtType, int iThumbTrackPos)
{
	if ((evtType == wxEVT_SCROLLWIN_LINEDOWN) || (evtType == wxEVT_SCROLLWIN_LINEUP))
	{
		if (evtType == wxEVT_SCROLLWIN_LINEDOWN)
		{
			if ((m_iWidthItemCountCanDraw + m_iLevelPageDiff + 1) < (m_iMaxLevel - 1))
			{
				m_iLevelPageDiff++;
				m_iStartLevel++;
				m_iEndLevel++;
			}
		}
		else if (evtType == wxEVT_SCROLLWIN_LINEUP)
		{
			if (m_iLevelPageDiff > 0)
			{
				m_iLevelPageDiff--;
				m_iStartLevel--;
				m_iEndLevel--;
			}
		}

		int iRestDepth = m_iMaxLevel - m_pCurrNode->_level;
		int iDiffDepth = (m_iLevelPageDiff + m_iEndLevel) % m_iWidthItemCountCanDraw;

		MoveHorizontalScroll(m_iWidthItemCountCanDraw + m_iLevelPageDiff + 1);
	}
	else if (evtType == wxEVT_SCROLLWIN_THUMBTRACK)
	{
		if (m_iLevelPageDiff < iThumbTrackPos)
		{
			if ((m_iLevelPageDiff + m_iEndLevel) < m_iMaxLevel)
			{
				m_iLevelPageDiff = iThumbTrackPos;
				m_iStartLevel++;
				m_iEndLevel++;
			}
		}
		else
		{
			if (m_iLevelPageDiff > 0)
			{
				m_iLevelPageDiff = iThumbTrackPos;
				m_iStartLevel--;
				m_iEndLevel--;
			}
		}

		MoveHorizontalScroll(m_pCurrNode->_level + m_iLevelPageDiff);
	}
}

void CSHCDView::SetScrolledCurrentNode(int iRowIndex)
{
	if (iRowIndex < 0)
		iRowIndex = 0;

	wxVector<NODE *> vecNodeDatas = m_Datas[iRowIndex];
	NODE* pNode = (*vecNodeDatas.begin());
	m_curIter = theSHCD->GetBegin() + pNode->_index;
	m_pCurrNode = (*m_curIter);
}

void CSHCDView::OnMouseWheel(wxMouseEvent& event)
{
	wxEventType evtType;
	int iWheelRotation = event.GetWheelRotation();
	if (iWheelRotation < 0)
	{
		evtType = wxEVT_SCROLLWIN_LINEDOWN;
		ScrollVerticalProcess(evtType, true);
	}
	else
	{
		evtType = wxEVT_SCROLLWIN_LINEUP;
		ScrollVerticalProcess(evtType, true);
	}

	MoveVerticalScroll();
	MoveHorizontalScroll(m_pCurrNode->_level);
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CSHCDView::OnMouseLBClick(wxMouseEvent& event)
{
	wxPoint pt = event.GetPosition();
	FindMouseClickItem(pt);

	event.Skip();
}

void CSHCDView::OnMouseLBDBClick(wxMouseEvent& event)
{
	wxPoint pt = event.GetPosition();
	if (FindMouseClickItem(pt))
		MoveDir();

	event.Skip();
}

bool CSHCDView::FindMouseClickItem(const wxPoint& pt)
{
	int iLeft, iTop, iRight, iBottom;
	int iFoundRow = -1;
	int iClickRow = -1;

	wxRect rcRow;
	bool bExistItem = false;
	for (int i = 0; i < m_iHeightItemCountCanDraw; i++)
	{
		iLeft = MARGIN;
		iTop = (i * m_szItem.GetHeight()) - (MARGIN * 2);
		iRight = m_viewRect.GetRight() - MARGIN;
		iBottom = (iTop + m_szItem.GetHeight()) + (MARGIN * 2);

		rcRow.SetLeft(iLeft);
		rcRow.SetTop(iTop);
		rcRow.SetRight(iRight);
		rcRow.SetBottom(iBottom);

		if (rcRow.Contains(pt))
		{
			iFoundRow = i;
			break;
		}
	}

	if (iFoundRow != -1)
	{
		iClickRow = m_iStartIndex + iFoundRow;

		wxVector<NODE* > rowData = m_Datas[iClickRow];
		wxVector<NODE *>::iterator curRowIter = rowData.begin();

		NODE* pNode;
		for (curRowIter; curRowIter != rowData.end(); ++curRowIter)
		{
			pNode = (*curRowIter);
			if (pNode->_rect.Contains(pt))
			{
				bExistItem = true;
				break;
			}
		}

		if (bExistItem)
		{
			m_curIter = curRowIter;
			m_pCurrNode = (*m_curIter);
			theCommonUtil->RefreshWindow(this, m_viewRect);
		}
	}

	return bExistItem;
}

void CSHCDView::OnRefreshAll(wxCommandEvent& event)
{
	Initialize();
	event.Skip();

	SetFocus();
}

NODE* CSHCDView::GetCurrentNode()
{
	return m_pCurrNode;
}

void CSHCDView::DeleteDir()
{
	Freeze();
	
	bool bGoTrash = true;
	if (wxIsShiftDown())
		bGoTrash = false;

	if (m_pCurrNode->_parent == nullptr)
	{
		wxString strMsg = wxT("The top-level directory is a drive and cannot be deleted.");
		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);

		Thaw();
		return;
	}

	wxString strSendDir(wxT(""));

	wxString strFullPath = theSHCD->GetFullPath(m_pCurrNode);
	wxString strMsg(wxT(""));

	if (!wxDirExists(strFullPath))
	{
		strMsg = strFullPath + wxT(" is not exist directory(or file) so that can't will be deleted.");
		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}

	strMsg = wxT("If you delete ") + strFullPath + wxT(", all subdirectorys and files in that path are deleted \n\nAre you sure you want to delete it anyway?");

	int iRet = wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxICON_QUESTION | wxYES_NO);
	if (iRet == wxNO)
	{
		Thaw();
		return;
	}

//	strSendDir = strFullPath;
	
	std::list<wxString> lst;
	lst.push_back(strFullPath);

	DlgDelete dlgDelete(_gMainFrame);
	dlgDelete.SetDeleteInfo(lst, bGoTrash);
	iRet = dlgDelete.ShowModal();
	dlgDelete.Destroy();
	if (iRet == wxID_CANCEL)
		return;
	
	theSHCD->DeleteDir(m_pCurrNode);

	Initialize();
//	strFullPath = theSHCD->GetFullPath(m_pCurrNode);

//	strSendDir += wxT("^");
//	strSendDir += strFullPath;

//	wxCommandEvent evt(wxEVT_DIRMANAGER_MOVE_DIR);
//	evt.SetString(strSendDir);
//	wxPostEvent(_gActiveView, evt);

	Thaw();
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CSHCDView::AddDir()
{
	Freeze();
	wxString strFullPath = theSHCD->GetFullPath(m_pCurrNode);

	DlgAddDir dlgDir(_gMainFrame);
	dlgDir.SetCurrentPath(strFullPath);
	int iRet = dlgDir.ShowModal();
	dlgDir.Destroy();
	if (iRet == wxID_CANCEL)
		return;

	DirectoryManagerDlg* pParent = (DirectoryManagerDlg *)m_pParent;
	if (theDriveInfo->IsExistDrive(strFullPath))
			pParent->LoadSCDData(false);
	else
		pParent->LoadPartFolder(strFullPath, m_pCurrNode);

	Thaw();
}

void CSHCDView::RenameDir()
{
	wxString strName = m_pCurrNode->_Name;
	DoRenameOn(strName);
}

void CSHCDView::OnTextCtrlEnter(wxCommandEvent& event)
{
	wxString strNewName = m_pTxtCtrlForRename->GetValue();
	wxString strOldName = m_pCurrNode->_Name;

	if (strNewName.Cmp(strOldName) == 0)
	{
		wxMessageBox(strNewName + wxT(" is the same dir exist\nInput different name"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}
	
	if(!theCommonUtil->IsCreatableDirectory(strNewName))
	{
		DoRenameOn(strNewName);
		return;
	}
		
	theSHCD->RenameDir(m_pCurrNode, strNewName);
	
	m_pTxtCtrlForRename->SetLabelText(wxT(""));
	m_pTxtCtrlForRename->Show(false);
	
	event.Skip();
}

void CSHCDView::DoRenameOn(const wxString& strName)
{
	wxClientDC dc(this);
	wxSize szName = dc.GetTextExtent(strName);
	
	int iPosX1 = m_pCurrNode->_rect.GetLeft();
	int iPosY1 = m_pCurrNode->_rect.GetTop() - 1;
	int iPosX2 = m_pCurrNode->_rect.GetWidth();
	int iPosY2 = m_pCurrNode->_rect.GetHeight() + 5;
	
	m_pTxtCtrlForRename->SetSize(wxRect(iPosX1, iPosY1, iPosX2, iPosY2));
	m_pTxtCtrlForRename->SetLabelText(strName);
	m_pTxtCtrlForRename->SetSelection(0, strName.Length());
	m_pTxtCtrlForRename->Show(true);
	m_pTxtCtrlForRename->SetFocus();
	
	wxString strDontUse(theMsgManager->GetMessage(wxT("MSG_INFO_RENAME_DONTUSE")));
	
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

void CSHCDView::OnCharHookFromTextCtrl(wxKeyEvent& event)
{
	int iKeyCode = event.GetKeyCode();
	if (iKeyCode == WXK_ESCAPE)
	{
		m_pTxtCtrlForRename->SetLabelText(wxT(""));
		m_pTxtCtrlForRename->Show(false);
		m_pMyTooltipView->Show(false);
	}
	else
		event.Skip();
}