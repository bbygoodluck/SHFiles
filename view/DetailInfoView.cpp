#include "../ginc.h"
#include "DetailInfoView.h"

const int COUNT_GAP = 12;
const int POS_TOP	= 3;
const int DISKSPACE_START = 150;

wxBEGIN_EVENT_TABLE(CDetailInfoView, wxWindow)
	EVT_PAINT(CDetailInfoView::OnPaint)
	EVT_SIZE(CDetailInfoView::OnSize)
	EVT_ERASE_BACKGROUND(CDetailInfoView::OnErase)
	EVT_MOTION(CDetailInfoView::OnMouseMove)
wxEND_EVENT_TABLE()

CDetailInfoView::CDetailInfoView()
	: m_viewRect(0, 0, 0, 0)
	, m_strCurrentDir(wxT(""))
	, m_strItemInfo(wxT(""))
	, m_strSelectedItemInfo(wxT(""))
	, m_strCurrentDrive(wxT(""))
	, m_strDispDiskSpace(wxT(""))
	, m_strTotalSpace(wxT(""))
	, m_strFreeSpace(wxT(""))
	, m_strSelItemDisp(wxT(""))
	, m_dblFreeSpace(0.0)
	, m_dblTotalSpace(0.0)
	, m_pToolTip(nullptr)
	, m_pDoubleBuffer(nullptr)
	, m_bSizeChanged(false)
{
	wxToolTip::Enable(true);
	m_pToolTip = new wxToolTip(wxT(""));

	this->SetToolTip(m_pToolTip);

	SetBackgroundStyle(wxBG_STYLE_CUSTOM);

#ifdef __WXMSW__
	m_font.Create(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Segoe UI"));
#else
	m_font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
#endif
}

CDetailInfoView::CDetailInfoView(wxWindow* parent, const int nID, const wxPoint& pt, const wxSize& sz, long lStyle)
	: wxWindow(parent, nID, pt, sz, lStyle)
	, m_viewRect(0, 0, 0, 0)
	, m_strCurrentDir(wxT(""))
	, m_strItemInfo(wxT(""))
	, m_strSelectedItemInfo(wxT(""))
	, m_strCurrentDrive(wxT(""))
	, m_strDispDiskSpace(wxT(""))
	, m_strTotalSpace(wxT(""))
	, m_strFreeSpace(wxT(""))
	, m_strSelItemDisp(wxT(""))
	, m_dblFreeSpace(0.0)
	, m_dblTotalSpace(0.0)
	, m_pToolTip(nullptr)
	, m_pDoubleBuffer(nullptr)
	, m_bSizeChanged(false)
	, m_szChagned(0, 0)

{
	wxBitmap bmpInfo = wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, wxSize(16, 15));
	m_icoInfo.CopyFromBitmap(bmpInfo);
	wxToolTip::Enable(true);
	m_pToolTip = new wxToolTip(wxT(""));

	this->SetToolTip(m_pToolTip);

	SetBackgroundStyle(wxBG_STYLE_CUSTOM);

#ifdef __WXMSW__
	m_font.Create(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Segoe UI"));
#else
	m_font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
#endif
}

CDetailInfoView::~CDetailInfoView()
{
	if (m_pDoubleBuffer)
		delete m_pDoubleBuffer;
}

void CDetailInfoView::OnPaint(wxPaintEvent& event)
{
	if (!m_pDoubleBuffer->IsOk())
		return;

	m_viewRect = GetClientRect();

	wxPaintDC dc(this);
	PrepareDC(dc);

	wxMemoryDC memDC(&dc);

	memDC.SelectObject(*m_pDoubleBuffer);
	memDC.Clear();

	wxPen pen(*wxBLACK);
	wxBrush brush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	memDC.SetPen(pen);
	memDC.SetBrush(brush);
	memDC.SetFont(m_font);

	memDC.DrawRectangle(m_viewRect);
	Display(&memDC);

	dc.Blit(0, 0, m_pDoubleBuffer->GetWidth(), m_pDoubleBuffer->GetHeight(), &memDC, 0, 0);
	memDC.SelectObject(wxNullBitmap);
}

void CDetailInfoView::OnErase(wxEraseEvent& event)
{

}

void CDetailInfoView::Display(wxDC* pDC)
{
	int iDispWidth = 0;
	pDC->DrawIcon(m_icoInfo, 5, POS_TOP);
	iDispWidth += 40;

	if (m_dblFreeSpace != 0.0)
	{
		wxPen pen(wxColour(100, 100, 100));
		wxBrush brush(wxColour(192, 192, 192));

		wxRect rcDiskSpace(m_viewRect.GetLeft() + 25, POS_TOP, m_nDiskspaceWidth, m_viewRect.GetHeight() - (POS_TOP * 2));
		pDC->SetPen(pen);
		pDC->SetBrush(brush);
		pDC->DrawRectangle(rcDiskSpace);

		double dblTotalWidth = rcDiskSpace.GetWidth() * 1.0;
		double dblUsed = m_dblTotalSpace - m_dblFreeSpace;

		int iUsedWidth = (int)((dblUsed * dblTotalWidth) / m_dblTotalSpace);

		wxPen penUsed(wxColour(100, 100, 100));
		wxBrush brushUsed(wxColour(44, 184, 65));
		wxRect rcDiskSpaceUsed(m_viewRect.GetLeft() + 25, POS_TOP, iUsedWidth, m_viewRect.GetHeight() - ((POS_TOP * 2) + 1));

		pDC->SetPen(penUsed);
		pDC->SetBrush(brushUsed);
		pDC->DrawRectangle(rcDiskSpaceUsed);

		wxPen penLine(wxColour(255, 255, 255));
		pDC->SetPen(penLine);

		pDC->DrawLine(rcDiskSpace.GetLeft(), rcDiskSpace.GetBottom(), rcDiskSpace.GetRight() + 1, rcDiskSpace.GetBottom());
		pDC->DrawLine(rcDiskSpace.GetRight(), rcDiskSpace.GetTop(), rcDiskSpace.GetRight(), rcDiskSpace.GetBottom());

		pDC->SetTextForeground(wxColour(0, 0 ,0));
		pDC->DrawText(m_strDispDiskSpace, rcDiskSpace.GetRight() + 5, POS_TOP);

		iDispWidth += rcDiskSpace.GetWidth();
		wxSize szDiskSpace = pDC->GetTextExtent(m_strDispDiskSpace);

		iDispWidth += szDiskSpace.GetWidth();
	}

	if(!m_strItemInfo.IsEmpty())
	{
		wxRect rcFillRect(m_viewRect.GetLeft() + iDispWidth, POS_TOP, m_viewRect.GetWidth() - iDispWidth, m_viewRect.GetHeight());
		pDC->SetTextForeground(wxColour(0, 0, 0));
		wxString strDispDetail = theCommonUtil->GetEllipseString(pDC, m_strItemInfo, rcFillRect.GetWidth());

		pDC->DrawLabel(strDispDetail, rcFillRect, wxALIGN_LEFT);
	}
}

void CDetailInfoView::OnSize(wxSizeEvent& event)
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

void CDetailInfoView::OnMouseMove(wxMouseEvent& event)
{
	wxPoint ptMouse = event.GetPosition();
	if (m_DiskSpaceRect.Contains(ptMouse))
		m_pToolTip->SetReshow(100);
}

wxString CDetailInfoView::CalcDispStr(wxDC* pDC, const wxString& strSourceSave, const wxString& strSource, int iDispWidth)
{
	return theCommonUtil->GetEllipseString(pDC, strSource, iDispWidth);
}

void CDetailInfoView::SetSelectedItemInfo(const wxString& strSelectedItemInfo)
{
	m_strSelectedItemInfo = strSelectedItemInfo;
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CDetailInfoView::SetDetailInfo(const wxString& strDetailInfo)
{
	m_strItemInfo = strDetailInfo;
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CDetailInfoView::DisplayDiskSpaceInfo(const wxString& strDrive, bool bUpdate)
{
	bool _bUpdate = bUpdate;
	if(!bUpdate)
	{
		if(m_strCurrentDrive.IsEmpty() || theCommonUtil->Compare(m_strCurrentDrive, strDrive) != 0)
		{
			m_strCurrentDrive = strDrive;
			_bUpdate = true;
		}
	}

	if(!_bUpdate)
		return;

	if (wxIsReadable(m_strCurrentDrive))
	{
		int iDriveType = theDriveInfo->GetDriveType(m_strCurrentDrive);

		if ((iDriveType != wxFS_VOL_CDROM) &&
			(iDriveType != wxFS_VOL_DVDROM))
		{
			wxString strTotalSpace(wxT(""));
			wxString strFreeSpace(wxT(""));

			theDriveInfo->GetDiskSpace(m_strCurrentDrive, m_dblTotalSpace, m_dblFreeSpace);

			double dblFreePrecent = m_dblFreeSpace * 100.0 / m_dblTotalSpace;

			m_strTotalSpace = wxString::Format(wxT("%04.2f GB"), (m_dblTotalSpace / GIGABYTE));
			m_strFreeSpace = wxString::Format(wxT("%04.2f GB"), (m_dblFreeSpace / GIGABYTE));

			m_strDispDiskSpace = m_strCurrentDrive;
			m_strDispDiskSpace += wxT(" ");
			m_strDispDiskSpace += m_strFreeSpace;
			m_strDispDiskSpace += wxString::Format(wxT("(%.2f%) "), dblFreePrecent);
			m_strDispDiskSpace += theMsgManager->GetMessage(wxT("MSG_DISK_FREE"));
			m_strDispDiskSpace += wxT(" (");
			m_strDispDiskSpace += theMsgManager->GetMessage(wxT("MSG_DISK_TOTAL"));
			m_strDispDiskSpace += wxT(" ");
			m_strDispDiskSpace += m_strTotalSpace;
			m_strDispDiskSpace += wxT(")");
		}
		else
			m_strDispDiskSpace = wxT("");
	}

	theCommonUtil->RefreshWindow(this, m_viewRect);
}
