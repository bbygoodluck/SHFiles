#include "./ginc.h"
#include "CustomStatusBar.h"

static const char *numlockIndicators[] = { "OFF", "NUM" };
static const char *capslockIndicators[] = { "", "CAPS" };
static const char *ctrlIndicators[] = { "", "CTRL" };
static const char *shiftIndicators[] = { "", "SHIFT" };
static const char *altIndicators[] = { "", "ALT" };

wxBEGIN_EVENT_TABLE(CCustomStatusBar, wxStatusBar)
	EVT_SIZE(CCustomStatusBar::OnSize)
	EVT_TIMER(wxID_ANY, CCustomStatusBar::OnTimer)
	EVT_IDLE(CCustomStatusBar::OnIdle)
wxEND_EVENT_TABLE()

CCustomStatusBar::CCustomStatusBar(wxWindow *parent, long style)
	: wxStatusBar(parent, wxID_ANY, style, wxT("CCustomStatusBar"))
	, m_timer(this)
	, m_bSet(false)
{
	m_timer.Start(1000);
	UpdateClock();
}

CCustomStatusBar::~CCustomStatusBar()
{
	if (m_timer.IsRunning())
		m_timer.Stop();
}

void CCustomStatusBar::OnSize(wxSizeEvent& event)
{
	m_sz = event.GetSize();
	Setting();
}

void CCustomStatusBar::OnIdle(wxIdleEvent& event)
{
	SetStatusText(ctrlIndicators[wxGetKeyState(WXK_CONTROL)], Field_Ctrl);
	SetStatusText(shiftIndicators[wxGetKeyState(WXK_SHIFT)], Field_Shift);
	SetStatusText(altIndicators[wxGetKeyState(WXK_ALT)], Field_Alt);
	SetStatusText(numlockIndicators[wxGetKeyState(WXK_NUMLOCK)],Field_NumLockIndicator);
	SetStatusText(capslockIndicators[wxGetKeyState(WXK_CAPITAL)],Field_CapsLockIndicator);

	if (wxGetKeyState(WXK_CONTROL))
	{
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_BAR_NEW_TAB")), Field_F2);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_BAR_COPY")), Field_F3);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_BAR_CUT")), Field_F4);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_BAR_PASTE")), Field_F5);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_BAR_CONTEXT_MENU")), Field_F7);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_BAR_OPTION")), Field_F10);
		SetStatusText(wxT(" "), Field_F8);
	}
	else if (wxGetKeyState(WXK_ALT))
	{

		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_ALT_ANOTHER_COPY")), Field_F2);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_ALT_ANOTHER_MOVE")), Field_F3);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_ALT_ANOTHER_ONEVIEW")), Field_F4);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_ALT_ANOTHER_SPLITVERT")), Field_F5);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_ALT_ANOTHER_COMPREALEASE")), Field_F7);
		SetStatusText(wxT(" "), Field_F10);
		SetStatusText(wxT(" "), Field_F8);
	}
	else
	{
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_DEFAULT_RENAME")), Field_F2);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_DEFAULT_FILEEDIT")), Field_F3);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_DEFAULT_VIEWFAVORITE")), Field_F4);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_DEFAULT_REFRESH")), Field_F5);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_DEFAULT_MAKE_FOLDER")), Field_F7);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_DEFAULT_COMPRESS")), Field_F8);
		SetStatusText(theMsgManager->GetMessage(wxT("MSG_STATUS_DEFAULT_DIRMANAGER")), Field_F10);
	}
	
	event.Skip();
}

void CCustomStatusBar::OnTimer(wxTimerEvent& WXUNUSED(event))
{
	UpdateClock();
}

void CCustomStatusBar::Setting()
{
	wxClientDC dc(this);

	int iClientWidth = m_sz.GetWidth();
	int iFuncSize = (iClientWidth - 170) / 8;
	SetBackgroundColour(wxColour(220, 220, 220));

	wxSize sizeNumLock = dc.GetTextExtent(numlockIndicators[0]);
	sizeNumLock.IncTo(dc.GetTextExtent(numlockIndicators[1]));

	int widths[Field_Max];
	widths[Field_F2]                = iFuncSize;
	widths[Field_F3]                = iFuncSize;
	widths[Field_F4]                = iFuncSize;
	widths[Field_F5]                = iFuncSize;
	widths[Field_F7]                = iFuncSize;
	widths[Field_F8]                = iFuncSize;
	widths[Field_F10]               = iFuncSize;
	widths[Field_Ctrl]              = dc.GetTextExtent(ctrlIndicators[1]).x;
	widths[Field_Shift]             = dc.GetTextExtent(shiftIndicators[1]).x;
	widths[Field_Alt]               = dc.GetTextExtent(altIndicators[1]).x;
	widths[Field_NumLockIndicator]  = sizeNumLock.x;
	widths[Field_CapsLockIndicator] = dc.GetTextExtent(capslockIndicators[1]).x;
	widths[Field_Clock]             = iFuncSize;

	SetFieldsCount(Field_Max);
	SetStatusWidths(Field_Max, widths);

	m_bSet = true;
}

void CCustomStatusBar::UpdateClock()
{
	if (!m_bSet) return;

	SetStatusText(wxDateTime::Now().FormatTime(), Field_Clock);
}