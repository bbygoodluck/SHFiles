#ifndef __SHFILES_H__
#define __SHFILES_H__

/*************************************************************************
program name    : SHFiles
Author          : Bae Byoung Yong
Date            : 2020.06 ~
**************************************************************************/

class CSHFiles : public wxApp
{
public:
	explicit CSHFiles();
	virtual ~CSHFiles();

	bool OnInit() override;
	int OnExit() override;

private:
	bool Initialize();
	bool LoadJsonConfigs();

private:
#ifdef NDEBUG
	wxSingleInstanceChecker* m_pSingleInstance;
#endif
	bool m_bAddFontRC;
	bool m_bFirstExec = true;

private:
	void OnActiveApp(wxActivateEvent& event);
	wxDECLARE_EVENT_TABLE();
};

#endif

