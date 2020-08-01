#include "global.h"
#include "DriveInfo.h"

std::unique_ptr<CDriveInfo> CDriveInfo::m_pInstance(nullptr);

CDriveInfo* CDriveInfo::Get()
{
	if (m_pInstance.get() == nullptr)
	{
		m_pInstance.reset(new CDriveInfo());
		m_pInstance->Init();
	}

	return m_pInstance.get();
}

void CDriveInfo::Init()
{
	m_driveItems.clear();
	m_drive.clear();

	wxArrayString arrdrvList = wxFSVolume::GetVolumes();
	int nTotalDriveCount = arrdrvList.GetCount();

	for (int nCnt = 0; nCnt < nTotalDriveCount; nCnt++)
	{
		wxString strDrvName = arrdrvList.Item(nCnt);
		int nDotIndex = strDrvName.Find(':');
		if (nDotIndex == 1)
		{
			wxFSVolume fsv(strDrvName);

			if (fsv.IsOk())
			{
				wxFSVolumeKind fskind = fsv.GetKind();
			//	if (!theConfig->isViewFloppy())
				if (!theJsonConfig->IsViewFloppy())
				{
				//	if (fskind == wxFS_VOL_FLOPPY)
				//		continue;
				}

				wxString strDisplayName(wxT(""));
				wxString strDetailInfo(wxT(""));
				wxString strSpace(wxT(""));

				bool isNetDrive = (fskind == wxFS_VOL_NETWORK) ? true : false;
				strDetailInfo = GetMakeDisplayName(fsv.GetDisplayName(), isNetDrive);
				
				std::unique_ptr<CDriveItem> drvItem(new CDriveItem());

				drvItem->SetDriveName(strDrvName);
				drvItem->SetNetDriveFlag(isNetDrive);
				
				if (//(drvItem.m_iDriveType != wxFS_VOL_FLOPPY) &&
					(fskind != wxFS_VOL_CDROM) &&
					(fskind != wxFS_VOL_DVDROM))
				{
					wxString strTotalSpace(wxT(""));
					wxString strFreeSpace(wxT(""));

					GetDiskSpaceToString(strDrvName, strTotalSpace, strFreeSpace);
					strSpace = strFreeSpace + wxT(" / ") + strTotalSpace;
				}
				
				drvItem->SetDisplayName(strDetailInfo);
				drvItem->SetSpace(strSpace);
				drvItem->SetDriveType(fskind);
				drvItem->SetTypeName(m_strType);
				drvItem->SetDisplayEtc(m_strEtcDisplay);

				int iIconIndex = -1;
				int iOverlayIconIndex = -1;
				SetIconIndex(strDrvName, iIconIndex, iOverlayIconIndex);

				drvItem->SetIconIndex(iIconIndex);
				drvItem->SetOverlayIconIndex(iOverlayIconIndex);
				
				m_driveItems[strDrvName] = *drvItem.get();
				m_drive.push_back(std::move(drvItem));
			}
		}
	}

	m_nDriveCount = m_drive.size();
}

wxString CDriveInfo::GetMakeDisplayName(const wxString& strDisplayName, bool IsNet)
{
	wxString _strTmpDisp(wxT(""));
	wxString _strDrvieName(wxT(""));
	wxString _strDetail(wxT(""));

	int iDrvNameIndex = strDisplayName.find_first_of(wxT(":"));
	int iDetailIndex = strDisplayName.find_first_of(wxT("("));

	_strDrvieName = strDisplayName.Mid(iDrvNameIndex - 1, 1);
	_strDetail = strDisplayName.Left(iDetailIndex - 1);
	_strTmpDisp = wxT("[-");
	_strTmpDisp += _strDrvieName + wxT("-] ");
	if (IsNet)
		_strTmpDisp += wxT("NET ");

	_strTmpDisp += _strDetail;

	return _strTmpDisp;
}

void CDriveInfo::GetDiskSpace(const wxString& strVolume, double& dblTotalSpace, double& dblFreeSpace)
{
	wxDiskspaceSize_t total;
	wxDiskspaceSize_t free;

	wxGetDiskSpace(strVolume, &total, &free);

	dblTotalSpace = total.ToDouble();
	dblFreeSpace = free.ToDouble();
}

void CDriveInfo::GetDiskSpaceToString(const wxString& strVolume, wxString& strTotalSpace, wxString& strFreeSpace)
{
	double dblTotalSpace = 0.0;
	double dblFreeSpace = 0.0;

	if(wxIsReadable(strVolume))
		GetDiskSpace(strVolume, dblTotalSpace, dblFreeSpace);

	strTotalSpace = wxString::Format(wxT("%04.1f GB"), (dblTotalSpace / GIGABYTE));
	strFreeSpace = wxString::Format(wxT("%04.1f GB"), (dblFreeSpace / GIGABYTE));
}

void CDriveInfo::SetIconIndex(const wxString& strDriveName, int& iIconIndex, int& iOverlayIconIndex)
{
#ifdef __WXMSW__
	SHFILEINFO sfi;
	wxZeroMemory(sfi);

	DWORD dwNum = GetFileAttributes(strDriveName);
	SHGetFileInfo(strDriveName, dwNum, &sfi, sizeof(sfi), IMAGELIST_FLAG);// | SHGFI_ADDOVERLAYS);

	iIconIndex = (sfi.iIcon & 0x00FFFFFF);
	iOverlayIconIndex = (sfi.iIcon >> 24) - 1;

	DestroyIcon(sfi.hIcon);
#endif
}

void CDriveInfo::UpdateSpace(const wxString& strDriveName)
{
	if (strDriveName.CmpNoCase(wxT("")) == 0)
	{
		int iCount = GetDriveCount();
		for (int i = 0; i < iCount; i++)
		{
			CDriveItem* pItem = GetDriveItem(i);
			int iDriveType = pItem->GetDriveType();

			if (//(drvItem.m_iDriveType != wxFS_VOL_FLOPPY) &&
				(iDriveType != wxFS_VOL_CDROM) &&
				(iDriveType != wxFS_VOL_DVDROM))
			{
				wxString strTotalSpace(wxT(""));
				wxString strFreeSpace(wxT(""));
				wxString strSpace(wxT(""));

				GetDiskSpaceToString(pItem->GetDriveName(), strTotalSpace, strFreeSpace);
				strSpace = strFreeSpace + wxT(" / ") + strTotalSpace;

				pItem->SetSpace(strSpace);
			}
		}
	}
	else
	{
		if (!wxIsReadable(strDriveName))
			return;

		int iCount = GetDriveCount();
		for (int i = 0; i < iCount; i++)
		{
			CDriveItem* pItem = GetDriveItem(i);

			int iDriveType = pItem->GetDriveType();
			if (strDriveName.CmpNoCase(pItem->GetDriveName()) == 0)
			{
				if (//(drvItem.m_iDriveType != wxFS_VOL_FLOPPY) &&
					(iDriveType != wxFS_VOL_CDROM) &&
					(iDriveType != wxFS_VOL_DVDROM))
				{
					wxString strTotalSpace(wxT(""));
					wxString strFreeSpace(wxT(""));
					wxString strSpace(wxT(""));

					GetDiskSpaceToString(strDriveName, strTotalSpace, strFreeSpace);
					strSpace = strFreeSpace + wxT(" / ") + strTotalSpace;

					pItem->SetSpace(strSpace);
				}
			}
		}
	}
}

CDriveItem* CDriveInfo::GetDriveItem(const wxString& strDriveName)
{
	int iCount = GetDriveCount();
	for (int i = 0; i < iCount; i++)
	{
		CDriveItem* pItem = GetDriveItem(i);
		if (strDriveName.CmpNoCase(pItem->GetDriveName()) == 0)
		{
			return pItem;
		}
	}

	return nullptr;
}

const bool CDriveInfo::IsExistDrive(const wxString& strDriveName)
{
	std::unordered_map<wxString, CDriveItem>::const_iterator fIter = m_driveItems.find(strDriveName);
	if (fIter == m_driveItems.end())
		return false;

	return true;
}

wxString CDriveInfo::GetDriveName(int iIndex)
{
	CDriveItem* item = m_drive.at(iIndex).get();
	return item->GetDriveName();
}

wxString CDriveInfo::GetDisplayName(int iIndex) 
{
	CDriveItem* item = m_drive.at(iIndex).get();
	return item->GetDisplayName();
}

wxString CDriveInfo::GetDisplayName(const wxString& strDriveName) 
{
	wxString strDispName(wxT(""));

	int iCount = GetDriveCount();
	for (int i = 0; i < iCount; i++)
	{
		CDriveItem* pItem = GetDriveItem(i);
		if (strDriveName.CmpNoCase(pItem->GetDriveName()) == 0)
		{
			strDispName = pItem->GetDisplayName();
			break;
		}
	}

	return strDispName;
}

wxString CDriveInfo::GetSpace(const wxString& strDriveName)
{
	CDriveItem* pItem = GetDriveItem(strDriveName);
	if (pItem == nullptr)
		return wxT("");

	return pItem->GetSpace();
}

wxString CDriveInfo::GetEtcInfo(const wxString& strDriveName)
{
	CDriveItem* pItem = GetDriveItem(strDriveName);
	if (pItem == nullptr)
		return wxT("");

	return pItem->GetDisplayEtc();
}

int CDriveInfo::GetDriveType(const wxString& strDriveName) 
{
	CDriveItem* pItem = GetDriveItem(strDriveName);
	if (pItem == nullptr)
		return -1;

	return pItem->GetDriveType();
}