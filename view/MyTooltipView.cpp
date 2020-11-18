#include "../ginc.h"
#include "MyToolTipView.h"

wxBEGIN_EVENT_TABLE(CMyTooltipView, wxWindow)
	EVT_PAINT(CMyTooltipView::OnPaint)
	EVT_ERASE_BACKGROUND(CMyTooltipView::OnErase)
	EVT_SIZE(CMyTooltipView::OnSize)
wxEND_EVENT_TABLE()

CMyTooltipView::CMyTooltipView(wxWindow* pParent)
	: wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, CHILD_WINDOW_STYLE)
{
	
}

CMyTooltipView::~CMyTooltipView()
{
	
}

void CMyTooltipView::OnPaint(wxPaintEvent& event)
{
	if (!m_pDoubleBuffer->IsOk())
		return;

	m_viewRect = GetClientRect();

	wxPaintDC dc(this);
	PrepareDC(dc);

	wxPen pen(wxColour(0, 0, 0));
	wxBrush brush(wxColour(252, 242, 156));
	
	wxMemoryDC memDC(&dc);
	memDC.SelectObjectAsSource(*m_pDoubleBuffer);
	
	memDC.SetPen(pen);
	memDC.SetBrush(brush);
	memDC.DrawRoundedRectangle(m_viewRect, -0.15);

	Renderer(&memDC);

	dc.Blit(0, 0, m_pDoubleBuffer->GetWidth(), m_pDoubleBuffer->GetHeight(), &memDC, 0, 0);
	
	memDC.SetFont(wxNullFont);
	memDC.SetPen(wxNullPen);
	memDC.SetBrush(wxNullBrush);
	memDC.SelectObjectAsSource(wxNullBitmap);
}

void CMyTooltipView::Renderer(wxDC* pDC)
{
	pDC->SetTextBackground(wxColour(0, 0, 0));
	pDC->DrawLabel(m_strTooltip, m_szChagned, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL);
}

void CMyTooltipView::OnErase(wxEraseEvent& event)
{
	
}

void CMyTooltipView::OnSize(wxSizeEvent& event)
{
	wxSize size = event.GetSize();//GetClientSize();
	if ((size.x == 0) || (size.y == 0))
		return;
	
	m_szChagned = size;
	if(m_pDoubleBuffer)
		delete m_pDoubleBuffer;

	m_pDoubleBuffer = new wxBitmap(m_szChagned.x, m_szChagned.y);
	theCommonUtil->RefreshWindow(this, m_viewRect);
}

void CMyTooltipView::SetTooltipText(const wxString& strTooltip)
{
	m_strTooltip = strTooltip;
//	theCommonUtil->RefreshWindow(this, m_viewRect);
}