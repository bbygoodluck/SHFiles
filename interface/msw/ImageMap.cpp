#include "../../ginc.h"
#include "ImageMap.h"

CImageMap::CImageMap(CListView* pListView)
	: CImageMapBase(pListView)
{
	m_hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CImageMap::~CImageMap()
{
	CloseHandle(m_hEvent);
}

void CImageMap::SetThreadTerminate()
{
	Clear();
	TerminateRead();
	m_bThreadTerminate = true;

	SetEvent(m_hEvent);

	if (GetThread()->IsRunning())
		GetThread()->Wait();
}

void CImageMap::IconReadStart()
{
	Clear();
	SetEvent(m_hEvent);
}

wxThread::ExitCode CImageMap::Entry()
{
	while (1)
	{
		m_bReadStart = READ_STOP;
		WaitForSingleObject(m_hEvent, INFINITE);
		ResetEvent(m_hEvent);

		if (m_bThreadTerminate)
			break;

		m_bReadStart = READ_START;

		wxString strCurrPath(m_pListView->m_strCurrentPath);
		wxString strFullPath(wxT(""));
		wxString strName(wxT(""));
		bool bAddSlash = m_pListView->m_iPathDepth > 1 ? true : false;

		wxVector<CDirData>::iterator fIter = m_pListView->m_itemList.begin();
		//포지션 인덱스
		int nPosIndex = 0;
		//현재 페이지
		int iCurrentPage = 0;
		//이전페이지
		int iPrevPage = 0;
		//이미지 포지션
		int iTempColumn = 0;
		//포지션 사이즈
		int iPosTotalCount = m_pListView->m_posList.size();
		while(fIter != m_pListView->m_itemList.end())
		{
			if (m_bReadStart == READ_STOP)
				break;

			strName = fIter->GetName();
			strFullPath = bAddSlash ? strCurrPath + SLASH + strName : strCurrPath + strName;
			AddIcon(strFullPath, strName);

			fIter++;
			/*
			//컬럼계산
			//아이콘 영역 계산을 직접 수행하면
			//좌표 계산이 중복되어 아래 CPositionInfo(각 영역별 좌표) 클래스를 이용하여 처리하도록 수정
			iTempColumn = nPosIndex / m_pListView->m_nItemCountInColumn;
			//하나의 아이템 선택영역 계산
			int x1 = 1 + (iTempColumn * m_pListView->m_nDispColumnEndPoint);
			int iDispIndex = nPosIndex % m_pListView->m_nItemCountInColumn;
			int y1 = 1 + (m_pListView->m_iCharHeight * iDispIndex);
		//	int x2 = m_pListView->m_nDispColumnEndPoint;
			int y2 = m_pListView->m_iCharHeight;
			//아이콘 표시영역 계산
			int iIcon_x1 = x1 + m_pListView->ICON_SELECTED_WIDTH_GAP + m_pListView->GAP_WIDTH;
			int iIcon_y1 = y1 + 2;
			int iIcon_x2 = m_pListView->ICON_WIDTH;
			int iIcon_y2 = y2;
			//페이지계산
			iCurrentPage = m_pListView->m_nCurrentItemIndex / m_pListView->m_nDisplayItemInView;
			//현재페이지 > 이전페이지
			if(iCurrentPage > iPrevPage)
			{
				iPrevPage = iCurrentPage;
				//아이콘 표시 포지션 = 화면전체 아이템 수 - (한컬럼의 최대 아이콘 수 - 현재아이템의 컬럼포지션)
				//int iModPos = m_pListView->m_nCurrentItemIndex % m_pListView->m_nItemCountInColumn;
				//nPosIndex = m_pListView->m_nDisplayItemInView - (m_pListView->m_nItemCountInColumn - iModPos);
				//위 방법은 Refresh되지 않은항목들도 발생하므로
				//페이지가 변경될때는 표시 인덱스를 재계산
				nPosIndex = m_pListView->m_nCurrentItemIndex % m_pListView->m_nItemCountInColumn;//(m_pListView->m_nDispColumn / 2) * m_pListView->m_nItemCountInColumn;
			}
			else
			{	//아이콘 표시
				wxRect rcIcon(iIcon_x1, iIcon_y1, iIcon_x2, iIcon_y2);
				if(nPosIndex < m_pListView->m_nDisplayItemInView)
					theCommonUtil->RefreshWindow(m_pListView, rcIcon);

				nPosIndex++;
			}
			*/

			//CPositionInfo를 이용할 경우
			//페이지계산
			iCurrentPage = m_pListView->m_nCurrentItemIndex / m_pListView->m_nDisplayItemInView;
			if(iCurrentPage > iPrevPage)
			{	//페이지가 변경되었을 경우 아이콘 Refresh 재시작
				iPrevPage = iCurrentPage;
				nPosIndex = 0;
			}

			int iItemStartIndex = m_pListView->m_nStartIndex;
			wxString strItemName = m_pListView->m_itemList.at(iItemStartIndex).GetName();
			if(!IsExistIcon(strItemName))
				continue;

			if (iPosTotalCount > 0 && (nPosIndex < (iPosTotalCount - 1)))
			{	//Pos 인덱스 < 전체 포지션 - 1
				int iPosSize = m_pListView->m_posList.size();
				if((iPosSize >= 0) && (nPosIndex < iPosSize))
				{
					CPositionInfo posInfo = m_pListView->m_posList.at(nPosIndex);
					theCommonUtil->RefreshWindow(m_pListView, posInfo.m_iconRect);
				}

				nPosIndex++;
			}
		}

		Continue();
	}

	m_bThreadTerminate = false;
	return (wxThread::ExitCode)0;
}
