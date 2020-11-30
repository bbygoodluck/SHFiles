#include "../ginc.h"
#include "ImageMapBase.h"

CImageMapBase::CImageMapBase(CListView* pListView)
	: m_pListView(pListView)
{
	if (CreateThread(wxTHREAD_JOINABLE) != wxTHREAD_NO_ERROR)
	{
		wxMessageBox(wxT("Could not create the worker thread!"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return;
	}

	GetThread()->Run();
}

CImageMapBase::~CImageMapBase()
{
	Clear();
}

void CImageMapBase::Clear()
{
	m_iconMap.clear();
}

void CImageMapBase::TerminateRead()
{
	m_bReadStart = READ_STOP;
}

void CImageMapBase::AddIcon(const wxString& strFullPath, const wxString& strName, bool isDrive)
{
#ifdef __WXMSW__
	SHFILEINFO sfi;
	wxZeroMemory(sfi);

	DWORD dwNum = GetFileAttributes(strFullPath);

	SHGetFileInfo(strFullPath, dwNum, &sfi, sizeof(sfi), IMAGELIST_FLAG);// | SHGFI_ADDOVERLAYS);

	int iIconIndex = (sfi.iIcon & 0x00FFFFFF);
	int iOverlayIndex = isDrive ? 0 : (sfi.iIcon >> 24) - 1;

	ICON_MAP::const_iterator fIter = m_iconMap.find(strName);
	if (fIter == m_iconMap.end())
	{
		FILE_ICONINFO fInfo;
		fInfo.iIconIndex = iIconIndex;
		fInfo.iOverlayIndex = iOverlayIndex;

		ICON_MAP::value_type val(strName, fInfo);
		m_iconMap.insert(val);
	}

	DestroyIcon(sfi.hIcon);
#else
#endif
}

void CImageMapBase::Draw(wxDC* pDC, const wxString& strName, int x, int y, unsigned int flags, bool isDir, bool isDrive)
{
	ICON_MAP::const_iterator fIter = m_iconMap.find(strName);
#ifdef __WXMSW__
	int iIconIndex = isDir ? 1 : 0;
	int iOverlayIndex = -1;

	if (!isDrive)
	{
		if (fIter != m_iconMap.end())
		{
			FILE_ICONINFO fInfo = fIter->second;
			iIconIndex = fInfo.iIconIndex;
			iOverlayIndex = fInfo.iOverlayIndex;

			flags |= INDEXTOOVERLAYMASK(iOverlayIndex + 1);
		}
	}
	else
	{
		iIconIndex = theDriveInfo->GetDriveItem(strName)->GetIconIndex();
		iOverlayIndex = theDriveInfo->GetDriveItem(strName)->GetOverlayIconIndex();

		flags |= INDEXTOOVERLAYMASK(iOverlayIndex + 1);
	}
#else
#endif

	DoDraw(pDC, iIconIndex, x, y, flags);
}

void CImageMapBase::DoDraw(wxDC* pDC, int iIconIndex, int x, int y, unsigned int flags)
{
#ifdef __WXMSW__
	HDC hdc = wx_static_cast(HDC, pDC->GetHDC());
	ImageList_DrawEx(_gImageList
		, iIconIndex
		, hdc
		, x
		, y
		, 16
		, 16
		, RGB(0, 0, 0)
		, RGB(0, 0, 0)
		, flags);

	pDC->ReleaseHDC(hdc);
#else
#endif
}
