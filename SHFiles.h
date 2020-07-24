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

	virtual bool OnInit() wxOVERRIDE;
	virtual int OnExit() wxOVERRIDE;

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

