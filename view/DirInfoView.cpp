#include "../ginc.h"
#include "DirInfoView.h"

wxBEGIN_EVENT_TABLE(CDirInfoView, wxWindow)
	EVT_PAINT(CDirInfoView::OnPaint)
	EVT_SIZE(CDirInfoView::OnSize)
	EVT_ERASE_BACKGROUND(CDirInfoView::OnErase)
wxEND_EVENT_TABLE()

CDirInfoView::CDirInfoView(wxWindow* parent, const int nID, const wxPoint& pt, const wxSize& sz, long lStyle)
	: wxWindow(parent, nID, pt, sz, lStyle)
	, m_strItemInfo(wxT(""))
	, m_strKeyInput(wxT(""))
{
#if defined(__WXMSW__)
	m_font.Create(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _T("Segoe UI"));
#else
	m_font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
#endif

}

CDirInfoView::~CDirInfoView()
{
	if (m_pDoubleBuffer)
		delete m_pDoubleBuffer;
		
	m_pDoubleBuffer = nullptr;
}

void CDirInfoView::OnPaint(wxPaintEvent& event)
{
	if (!m_pDoubleBuffer->IsOk())
		return;

	m_viewRect = GetClientRect();

	wxPaintDC dc(this);
	PrepareDC(dc);

	wxMemoryDC memDC(&dc);

	memDC.SelectObject(*m_pDoubleBuffer);
	memDC.Clear();

	wxPen pen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
	wxBrush brush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	memDC.SetPen(pen);
	memDC.SetBrush(brush);
	memDC.SetFont(m_font);

	memDC.DrawRectangle(m_viewRect);

	Renderer(&memDC);

	dc.Blit(0, 0, m_pDoubleBuffer->GetWidth(), m_pDoubleBuffer->GetHeight(), &memDC, 0, 0);
	memDC.SelectObject(wxNullBitmap);
}

void CDirInfoView::Renderer(wxDC* pDC)
{
	pDC->SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_SOLID));
	pDC->DrawLine(wxPoint(m_viewRect.GetLeft(), m_viewRect.GetTop()), wxPoint(m_viewRect.GetRight(), m_viewRect.GetTop()));
	pDC->DrawLine(wxPoint(m_viewRect.GetLeft(), m_viewRect.GetTop()), wxPoint(m_viewRect.GetLeft(), m_viewRect.GetBottom() + 1));
	pDC->DrawLine(wxPoint(m_viewRect.GetRight(), m_viewRect.GetTop()), wxPoint(m_viewRect.GetRight(), m_viewRect.GetBottom() + 1));

	if (!m_strItemInfo.IsEmpty())
	{
		pDC->SetTextForeground(wxColour(0, 64, 128));
		pDC->DrawText(m_strItemInfo, wxPoint(10, 3));
	}

	wxSize szKeyInput = pDC->GetTextExtent(m_strKeyInput);
	int iStartPos = m_viewRect.GetRight() - (szKeyInput.GetWidth() + 10);

	pDC->SetTextForeground(wxColour(50, 57, 255));
	pDC->DrawText(m_strKeyInput, wxPoint(iStartPos, 3));
}

void CDirInfoView::OnSize(wxSizeEvent& event)
{
	wxSize size = event.GetSize();
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

void CDirInfoView::OnErase(wxEraseEvent& event)
{
}

void CDirInfoView::SetDirInfo(const wxString& strInfo)
{
	m_strItemInfo = strInfo;
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CDirInfoView::SetKeyInputInfo(const wxString& strKeyInfo)
{
	m_strKeyInput = strKeyInfo;
	theCommonUtil->RefreshWindow(this, m_viewRect);
}