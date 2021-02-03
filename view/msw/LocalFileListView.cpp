#include "../../ginc.h"
#include "../../engine/msw/LocalWatcherMonitorHandler.h"
#include "LocalFileListView.h"

wxBEGIN_EVENT_TABLE(CLocalFileListView, CListView)
	EVT_PAINT(CLocalFileListView::OnPaint)
	EVT_ERASE_BACKGROUND(CLocalFileListView::OnErase)
	EVT_MY_CUSTOM_COMMAND(wxEVT_DRIVE_ADD_REMOVE, wxID_ANY, CLocalFileListView::OnDriveAddOrRemove)
	EVT_MY_CUSTOM_COMMAND(wxEVT_DISK_SPACE_UPDATE, wxID_ANY, CLocalFileListView::OnUpdateDriveSpace)
wxEND_EVENT_TABLE()

CLocalFileListView::CLocalFileListView(wxWindow* parent, const int nID, const wxSize& sz)
	: CListView(parent, nID, sz)
	, m_pWatcherHandler(nullptr)
{
	m_pWatcherHandler = std::make_unique<CLocalWatcherMonitorHandler>(this);
}

CLocalFileListView::~CLocalFileListView()
{

}

void CLocalFileListView::OnPaint(wxPaintEvent& event)
{
	if (!m_pDoubleBuffer->IsOk() || m_pDoubleBuffer == nullptr)
		return;

	m_viewRect = GetClientRect();

	wxPaintDC dc(this);
	PrepareDC(dc);

	wxPen pen(theJsonConfig->GetBackgroundColor());
	wxBrush brush(theJsonConfig->GetBackgroundColor());

	wxMemoryDC memDC(&dc);
	memDC.SelectObject(*m_pDoubleBuffer);
//	memDC.SelectObjectAsSource(*m_pDoubleBuffer);
	//ttf 폰트를 사용할 경우 일부 폴더의 Refresh 속도가 느려짐 Why?
	memDC.SetFont(*_gFont);
	memDC.SetPen(pen);
	memDC.SetBrush(brush);

	memDC.DrawRectangle(m_viewRect);
	Renderer(&memDC);

	dc.Blit(0, 0, m_pDoubleBuffer->GetWidth(), m_pDoubleBuffer->GetHeight(), &memDC, 0, 0);

	memDC.SetFont(wxNullFont);
	memDC.SetPen(wxNullPen);
	memDC.SetBrush(wxNullBrush);
	memDC.SelectObjectAsSource(wxNullBitmap);
}

void CLocalFileListView::OnErase(wxEraseEvent& event)
{

}

void CLocalFileListView::Renderer(wxDC* pDC)
{
	// 컬럼계산
	CalcColumn(pDC);
	// 컬럼그리기
	DrawColumn(pDC);
	//항목 표시
	DisplayItems(pDC);

	m_bSizeOrColumnChanged = false;
	m_bDirLoaded   = false;
}

void CLocalFileListView::AddDrive()
{
	wxString strDetailInfo(wxT(""));
	//드라이브 정보 적재
	int iDriveCount = theDriveInfo->GetDriveCount();
	wxString strName = wxT("");

	for (int iIndex = 0; iIndex < iDriveCount; iIndex++)
	{
		CDirData dirItem;
		CDriveItem* drvItem = theDriveInfo->GetDriveItem(iIndex);

		wxString strDriveName(drvItem->GetDriveName());
		int iDriveType = drvItem->GetDriveType();

		strName = drvItem->GetDisplayName();

		if ((iDriveType != wxFS_VOL_CDROM) &&
			(iDriveType != wxFS_VOL_DVDROM))
			strName += wxT(" - ") + drvItem->GetSpace();

		dirItem.SetName(strName);
		dirItem.SetDriveName(drvItem->GetDriveName());
		dirItem.SetType(CDirData::item_type::drive);

		m_itemList.push_back(dirItem);
	}
}

void CLocalFileListView::LoadDir(const wxString& strPath)
{
	wxBusyCursor wait;
	if(!ReadIconThreadStop())
	{
		wxMessageBox(wxT("Check Reading Icon Thread"), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return;
	}

	m_pViewPanel->TransferInfomation(TRANSFER_LISTVIEW_TO_PATHVIEW, strPath);

	Initialize();

	m_strCurrentPath = strPath;
	m_pWatcherHandler->AddWatchDir(strPath);
	//Get Path Depth
	m_iPathDepth = theCommonUtil->GetPathDepth(strPath);
	if (!ReadDirectory())
		return;

	AddDrive();
	DoSortStart();

	m_nTotalItems = m_itemList.size();
	if (m_nCurrentItemIndex > m_nTotalItems)
		m_nCurrentItemIndex = 0;

	m_bDirLoaded = true;

	m_strVolume = theCommonUtil->GetVolume(wxString(m_strCurrentPath));
	m_strVolume += wxT(":") + SLASH;

	if(m_iHistoryIndex != 0)
	{
		m_nCurrentItemIndex = m_iHistoryIndex;
		m_nStartIndex = m_iHistoryStartIndex;
	}

	m_iHistoryIndex = 0;
	m_iHistoryStartIndex = 0;

	m_pViewPanel->TransferInfomation(TRANSFER_LISTVIEW_DISK_SIZE_TO_DETAILVIEW_SET, m_strVolume);
}

bool CLocalFileListView::ReadDirectory()
{
	CLocalFileSystem localFileSys;
	if (!localFileSys.BeginFindFiles(m_strCurrentPath, false))
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_SEARCH_DIR_INIT_ERR")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	wxString strDesc(wxT(""));
	wxString strName(wxT(""));
	wxString strExt(wxT(""));
	bool isDir = false;
	unsigned long lattr = 0;
	wxLongLong llSize(0);
	wxDateTime dt(0.0);

	while (localFileSys.GetNextFile(strName, &isDir, &lattr, &llSize, &dt))
	{
		if (!theJsonConfig->IsViewAllFile())
		{
			if (!theJsonConfig->IsViewHiddenFile())
			{
				if (lattr & ATTR_HIDDEN)
					continue;
			}
			/*
			else if(theJsonConfig->IsViewHiddenFile() && !theJsonConfig->IsViewSystemFile())
			{
				if (lattr & ATTR_SYSTEM)
					continue;
			}
			else if(!theJsonConfig->IsViewHiddenFile() && theJsonConfig->IsViewSystemFile())
			{
				if (lattr & ATTR_HIDDEN)
				{
					if(!(lattr & ATTR_SYSTEM))
						continue;
				}
			}
			*/
		}

		CDirData dirItem;
		dirItem.SetName(strName);

		if (isDir)
		{
			dirItem.SetType(CDirData::item_type::dir);
			m_iDirCount++;
			strDesc = theMsgManager->GetMessage(wxT("MSG_DIR_DESCRIPTION"));
		}
		else
		{
			dirItem.SetType(CDirData::item_type::file);

			m_iFileCount++;
			strExt = theCommonUtil->GetExt(strName);
			strDesc = theExtInfo->GetExtInfo(strExt, m_strCurrentPath + (m_iPathDepth != 1 ? SLASH + strName : strName));

			m_dblFileSizeInDir += llSize.ToDouble();
		}

		dirItem.SetAttribute(lattr);
		dirItem.SetSize(llSize);
		dirItem.SetDateTime(dt);
		dirItem.SetPath(m_strCurrentPath);
		dirItem.SetIconIndex(0);
		dirItem.SetMatch(false);
		dirItem.SetExt(strExt);
		dirItem.SetTypeName(strDesc);

		m_itemList.push_back(dirItem);

		m_strMaxName = FindMaxData(strName, m_strMaxName);
		m_strMaxTypeName = FindMaxData(strDesc, m_strMaxTypeName);

		strName = wxT("");
		strDesc = wxT("");
		strExt = wxT("");

		isDir = false;
		lattr = 0;
		llSize = 0;
		dt = 0;
	}

	m_pViewPanel->TransferInfomation(TRANSFER_LISTVIEW_DIRINFO_TO_DIRINFOVIEW);
	return true;
}

void CLocalFileListView::DoCreate(const wxString& strName)
{
#if !defined(NDEBUG)
	#ifdef _UNICODE
		std::wcout << wxT("DoCreate : ") << strName << std::endl;
	#else
		std::cout << "DoCreate : " << strName << std::endl;
	#endif
#endif // NDEBUG

	m_bCreatedWatch = true;
	wxString strFullPathName = MakeFullPathName(strName);
	bool isDir = false;
	unsigned long lattr = 0;
	wxLongLong llSize(0);
	wxDateTime dt(0.0);

	bool bGetAttribute = CLocalFileSystem::GetAttributeInfo(strFullPathName, isDir, lattr, &llSize, &dt);
	bool IsOpen = false;
	//읽기권한 체크
	if(!isDir)
		IsOpen = CLocalFileSystem::IsFileReadablePrivileges(strFullPathName);

	if(!bGetAttribute && !IsOpen)
		return;

	wxString strExt(wxT(""));
	wxString strDesc(wxT(""));

	bool bExist = false;
	wxVector<CDirData>::iterator iter = GetItemExist(strName, bExist);

	if (bExist)
		return;

	if (!theJsonConfig->IsViewAllFile())
	{
		if (!theJsonConfig->IsViewHiddenFile() && !theJsonConfig->IsViewSystemFile())
		{
			if ( (lattr & ATTR_HIDDEN) || (lattr & ATTR_SYSTEM) )
				return;
		}
	}

	if (theJsonConfig->IsViewHiddenFile())
	{
		if((lattr & ATTR_SYSTEM) && !theJsonConfig->IsViewSystemFile())
			return;
	}

	m_pImageMap->AddIcon(strFullPathName, strName);

	CDirData dirItem;
	dirItem.SetName(strName);

	strDesc = wxT("");
	strExt = wxT("");

	if (isDir)
	{
		m_iDirCount++;
		dirItem.SetType(CDirData::item_type::dir);
		strDesc = theMsgManager->GetMessage(wxT("MSG_DIR_DESCRIPTION"));
	}
	else
	{
		m_iFileCount++;
		dirItem.SetType(CDirData::item_type::file);

		strExt = theCommonUtil->GetExt(strName);
		strDesc = theExtInfo->GetExtInfo(strExt, strFullPathName);

		if(m_bClipboardMove)
		{
			m_dblFileSizeInDir += llSize.ToDouble();
			m_bClipboardMove = false;
		}
	}

	dirItem.SetAttribute(lattr);
	dirItem.SetSize(llSize);

	if(!bGetAttribute)
		dt = wxDateTime::Now();

	dirItem.SetDateTime(dt);
	dirItem.SetPath(m_strCurrentPath);
	dirItem.SetExt(strExt);
	dirItem.SetTypeName(strDesc);

	m_itemList.push_back(dirItem);

	m_pViewPanel->TransferInfomation(TRANSFER_LISTVIEW_DIRINFO_TO_DIRINFOVIEW);

	m_bSizeOrColumnChanged = true;
	m_bIsDisplayDetailInfo = false;

	m_strMaxName = FindMaxData(strName, m_strMaxName);
	m_strMaxTypeName = FindMaxData(strDesc, m_strMaxTypeName);

	m_nTotalItems = wx_static_cast(int, m_itemList.size());
	DoSortStart();

	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CLocalFileListView::DoModify(const wxString& strName)
{
#if !defined(NDEBUG)
	#ifdef _UNICODE
		std::wcout << wxT("DoModify : ") << strName << std::endl;
	#else
		std::cout << "DoModify : " << strName << std::endl;
	#endif
#endif // NDEBUG

	bool isDir = false;
	unsigned long lattr = 0;
	wxLongLong llSize(0);
	wxDateTime dt(0.0);

	wxString strFullPathName = MakeFullPathName(strName);
	bool bGetAttribute = CLocalFileSystem::GetAttributeInfo(strFullPathName, isDir, lattr, &llSize, &dt);
	bool IsOpen = false;
	//읽기권한 체크
	if(!isDir)
		IsOpen = CLocalFileSystem::IsFileReadablePrivileges(strFullPathName);

	if(!bGetAttribute && !IsOpen)
		return;

	bool bExist = false;
	wxVector<CDirData>::iterator iter = GetItemExist(strName, bExist);

	if (!bExist)
		return;

	//속성이 정상적으로 Load된 경우만 처리함
	if(!m_bCreatedWatch)
	{
		// 수정전의 사이즈를 뺀다.
		if(iter->IsFile())
			m_dblFileSizeInDir -= iter->GetSize().ToDouble();
	}

	//수정후의 사이즈를 다시 더한다.
	if(!isDir)
		m_dblFileSizeInDir += llSize.ToDouble();

	iter->SetAttribute(lattr);
	iter->SetSize(llSize);

	if(!bGetAttribute)
		dt = wxDateTime::Now();

	iter->SetDateTime(dt);

	m_bIsDisplayDetailInfo = false;

	m_pViewPanel->TransferInfomation(TRANSFER_LISTVIEW_DIRINFO_TO_DIRINFOVIEW);
	m_pViewPanel->TransferInfomation(TRANSFER_LISTVIEW_DISK_SIZE_TO_DETAILVIEW_UPDATE, m_strVolume);
	theMenuOPHandler->ExecuteMenuOperation(_MENU_DISK_SPACE_UPDATE, m_strVolume);

	theCommonUtil->RefreshWindow(this, m_viewRect);
	m_bCreatedWatch = false;
}

void CLocalFileListView::DoDelete(const wxString& strName)
{
#if !defined(NDEBUG)
	#ifdef _UNICODE
		std::wcout << wxT("DoDelete : ") << strName << std::endl;
	#else
		std::cout << "DoDelete : " << strName << std::endl;
	#endif
#endif // NDEBUG

	bool bExist = false;
	wxVector<CDirData>::iterator iter = GetItemExist(strName, bExist);

	if (!bExist)
		return;

	//삭제가 실패하여 실제로 데이터가 남아 있을경우는 항목제거 처리를 하지 않음
	wxString strFullPathName = MakeFullPathName(strName);
	bool bRealExist = iter->IsDir() ? wxDirExists(strFullPathName) : wxFileExists(strFullPathName);
	if(bRealExist)
		return;

	wxString strDesc = iter->GetTypeName();

	m_strMaxName = FindMaxData(strName, m_strMaxName);
	m_strMaxTypeName = FindMaxData(strDesc, m_strMaxTypeName);

	if (iter->IsDir())
		m_iDirCount--;
	else
	{
		m_dblFileSizeInDir -= iter->GetSize().ToDouble();
		m_iFileCount--;
	}

	m_itemList.erase(iter);

	m_nCurrentItemIndex--;

	m_nTotalItems = wx_static_cast(int, m_itemList.size());
	if ((m_nTotalItems <= m_nCurrentItemIndex) || (m_nCurrentItemIndex < 0))
	{
		m_nStartIndex = 0;
		m_nCurrentItemIndex = 0;
	}

	m_bSizeOrColumnChanged = true;
	m_bIsDisplayDetailInfo = false;

	m_pViewPanel->TransferInfomation(TRANSFER_LISTVIEW_DIRINFO_TO_DIRINFOVIEW);
	m_pViewPanel->TransferInfomation(TRANSFER_LISTVIEW_DISK_SIZE_TO_DETAILVIEW_UPDATE, m_strVolume);
	theMenuOPHandler->ExecuteMenuOperation(_MENU_DISK_SPACE_UPDATE, m_strVolume);

	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CLocalFileListView::DoRename(const wxString& strOldName, const wxString& strNewName)
{
#if !defined(NDEBUG)
	#ifdef _UNICODE
		std::wcout << wxT("DoRename : ") << strOldName << wxT(" to ") << strNewName << std::endl;
	#else
		std::cout << wxT("DoRename : ") << strOldName << wxT(" to ") << strNewName << std::endl;
	#endif
#endif // NDEBUG

	bool isDir = false;
	unsigned long lattr = 0;
	wxLongLong llSize(0);
	wxDateTime dt(0.0);
	wxDateTime dtNow(0.0);

	wxString strFullPathName = MakeFullPathName(strNewName);
	bool bGetAttribute = CLocalFileSystem::GetAttributeInfo(strFullPathName, isDir, lattr, &llSize, &dt);
	bool IsOpen = false;
	//읽기권한 체크
	if(!isDir)
		IsOpen = CLocalFileSystem::IsFileReadablePrivileges(strFullPathName);

	if(!bGetAttribute && !IsOpen)
		return;

	//이름변경의 경우 기존 데이터를 삭제하고 새로운 아이템을 다시 등록하는것으로 처리함
	bool bOldExist = false;
	bool bNewExist = false;
	wxVector<CDirData>::iterator iterOld = GetItemExist(strOldName, bOldExist);
	wxVector<CDirData>::iterator iterNew = GetItemExist(strNewName, bNewExist);

	if(bNewExist)
	{
		if(iterNew->IsFile())
			m_dblFileSizeInDir -= iterNew->GetSize().ToDouble();

		m_itemList.erase(iterNew);
	}

	if(!bOldExist)
	{
		if(!IsOpen)
			return;
	}
	else
	{
		bool bOldIsFile = iterOld->IsFile();

		if(bOldIsFile)
			m_dblFileSizeInDir -= iterOld->GetSize().ToDouble();

		m_itemList.erase(iterOld);
	}

	m_pImageMap->AddIcon(strFullPathName, strNewName);

	CDirData dirItem;
	dirItem.SetName(strNewName);
	wxString strDesc(wxT(""));

	if(!isDir)
	{
		wxString strExt = theCommonUtil->GetExt(strNewName);

		strDesc = theExtInfo->GetExtInfo(strExt, strFullPathName);

		dirItem.SetType(CDirData::item_type::file);
		dirItem.SetExt(strExt);

		m_dblFileSizeInDir += llSize.ToDouble();
	}
	else
	{
		dirItem.SetType(CDirData::item_type::dir);
		strDesc = theMsgManager->GetMessage(wxT("MSG_DIR_DESCRIPTION"));
	}

	dirItem.SetAttribute(lattr);
	dirItem.SetSize(llSize);
	dirItem.SetDateTime(dt);
	dirItem.SetPath(m_strCurrentPath);

	dirItem.SetTypeName(strDesc);

	m_itemList.push_back(dirItem);

	m_strMaxName = FindMaxData(strNewName, m_strMaxName);
	m_strMaxTypeName = FindMaxData(strDesc, m_strMaxTypeName);

	m_pViewPanel->TransferInfomation(TRANSFER_LISTVIEW_DIRINFO_TO_DIRINFOVIEW);
	theMenuOPHandler->ExecuteMenuOperation(_MENU_DISK_SPACE_UPDATE, m_strVolume);

	m_nTotalItems = wx_static_cast(int, m_itemList.size());
	DoSortStart();

	m_bSizeOrColumnChanged = true;
	m_bIsDisplayDetailInfo = false;

	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CLocalFileListView::ExecuteExternalProgramForEdit(int iIndex)
{
	EXTERNAL_EDIT_PROGRAM extInfo = _gExternalPGList.at(iIndex);

	wxString strFilePG = extInfo._strPGPath;
	wxString strArgs = extInfo._strPGArgs;
	if (strFilePG == wxT(""))
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_FILE_MENU_EDIT_NOT_SET")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}

	if (!wxFileExists(strFilePG))
	{
		wxString strMsg = wxString::Format(theMsgManager->GetMessage(wxT("MSG_FILE_MENU_EDIT_NOT_EXIST_PROGRAM")), strFilePG);
		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}

	std::unordered_map<int, SELITEM_INFO>::iterator iTer = m_hashSelectedItem.begin();
	CDirData data;
	if (iTer == m_hashSelectedItem.end())
	{
		data = m_itemList.at(m_nCurrentItemIndex);
		wxString strFullPath = data.GetFullPath();
		wxExecute(strFilePG + wxT(" ") + strArgs + wxT(" ") + wxT("\"") + data.GetFullPath() + wxT("\""));
	}
	else
	{
		while(iTer != m_hashSelectedItem.end())
		{
			SELITEM_INFO _Info = iTer->second;
			data = m_itemList.at(_Info.m_iSelIndex);
			wxExecute(strFilePG + wxT(" ") + strArgs + wxT(" ") + wxT("\"") + data.GetFullPath() + wxT("\""));

			iTer++;
		}

		DoSelectedItemsClear();
		theCommonUtil->RefreshWindow(this, m_viewRect);
	}
}

void CLocalFileListView::DoRenameFromMenu(const wxString& strOldPathName, const wxString& strNewPathName)
{
	wxRename(strOldPathName, strNewPathName);
}

void CLocalFileListView::DoUpdateModificationTimeOfDir()
{
	bool isDir = false;
	unsigned long lattr = 0;
	wxLongLong llSize(0);
	wxDateTime dt(0.0);

	CDirData* pFirst = (CDirData *)&m_itemList.at(0);
	if (CLocalFileSystem::GetAttributeInfo(m_strCurrentPath, isDir, lattr, &llSize, &dt))
	{
		if(m_posList.size() > 0)
		{
			CPositionInfo pos = m_posList.at(0);
			pFirst->SetDateTime(dt);

			if(m_nCurrentItemIndex < m_nDisplayItemInView)
				theCommonUtil->RefreshWindow(this, pos.m_mainRect);
		}
	}
}

void CLocalFileListView::OnDriveAddOrRemove(wxCommandEvent& event)
{
	wxVector<CDirData>::iterator iter = m_itemList.end() - 1;
	for (iter; iter != m_itemList.begin(); --iter)
	{
		if (!iter->IsDrive())
			break;

		m_itemList.pop_back();
	}

	AddDrive();
	m_nTotalItems = m_itemList.size();

	ReCalcPage();
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CLocalFileListView::OnUpdateDriveSpace(wxCommandEvent& event)
{
	wxString strDiskName = event.GetString();
	wxString strDriveName(wxT(""));
	wxVector<CDirData>::iterator iter = m_itemList.end() - 1;

	for (iter; iter != m_itemList.begin(); --iter)
	{
		CDirData* pData = (CDirData *)&(*iter);
		strDriveName = pData->GetDriveName();

		if (strDiskName.CmpNoCase(strDriveName) == 0)
		{
			CDriveItem* drvItem;
			wxString strName;
			int iDriveType;

			drvItem = theDriveInfo->GetDriveItem(strDriveName);
			strName = drvItem->GetDisplayName();
			iDriveType = drvItem->GetDriveType();

			if ((iDriveType != wxFS_VOL_CDROM) &&
				(iDriveType != wxFS_VOL_DVDROM))
			{
				wxString strSpace = drvItem->GetSpace();
				strName += wxT(" - ") + strSpace;

				pData->SetName(strName);
			}

			break;
		}
	}
}
