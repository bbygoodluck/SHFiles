#ifndef __CUSTOM_STATUS_BAR_H__
#define __CUSTOM_STATUS_BAR_H__

class CCustomStatusBar : public wxStatusBar
{
	enum _DEFAULT
	{
		Field_F2 = 0,
		Field_F3,
		Field_F4,
		Field_F5,
	//	Field_F6,
		Field_F7,
		Field_F8,
		Field_F10,
		Field_Ctrl,
		Field_Shift,
		Field_Alt,
		Field_NumLockIndicator,
		Field_CapsLockIndicator,
		Field_Clock,
		Field_Max
	};

public:
	CCustomStatusBar(wxWindow *parent, long style = wxSTB_DEFAULT_STYLE | wxSB_FLAT);
	virtual ~CCustomStatusBar();

	void UpdateClock();

private:
	void Setting();

private:
	wxTimer m_timer;
	wxSize m_sz;

	bool m_bSet;
private:
	void OnSize(wxSizeEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnTimer(wxTimerEvent& WXUNUSED(event));
	wxDECLARE_EVENT_TABLE();
};
#endif
