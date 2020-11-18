#include "ginc.h"
#include "MainFrame.h"

wxIMPLEMENT_APP(CSHFiles);

wxBEGIN_EVENT_TABLE(CSHFiles, wxApp)
	EVT_ACTIVATE_APP(CSHFiles::OnActiveApp)
wxEND_EVENT_TABLE()

CSHFiles::CSHFiles()
#ifdef NDEBUG
	: m_pSingleInstance(nullptr)
#endif
{

}

CSHFiles::~CSHFiles()
{

}

bool CSHFiles::OnInit()
{
	//Read System Language
	_gSystemLang = theCommonUtil->GetSystemLang();
	wxLanguage enumLocale = _gSystemLang.CmpNoCase(wxT("ko")) == 0 ? wxLANGUAGE_KOREAN : wxLANGUAGE_ENGLISH;
	wxLocale((int)enumLocale);

	if (!LoadJsonConfigs())
		return false;

#ifdef NDEBUG
	m_pSingleInstance = new wxSingleInstanceChecker;
	if (m_pSingleInstance->IsAnotherRunning())
	{
		delete m_pSingleInstance;
		m_pSingleInstance = NULL;

		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_ALREADY_EXEC_PROCESS")), PROGRAM_FULL_NAME, wxOK | wxICON_INFORMATION);
		return false;
	}
#endif

	if (!wxApp::OnInit())
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_INITFAIL_WINDOW")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	if (!Initialize())
		return false;

	wxString strPGTitle = PROGRAM_FULL_NAME + ENV_OS_VER;

#ifndef NDEBUG
	strPGTitle += wxT(" - Debug Version");
#endif

	CMainFrame* pMainFrame = new CMainFrame(strPGTitle);
	pMainFrame->Show();

	this->SetTopWindow(pMainFrame);
	return true;
}

int CSHFiles::OnExit()
{
#ifdef NDEBUG
	if (m_pSingleInstance)
		delete m_pSingleInstance;

	m_pSingleInstance = NULL;
#endif

	_gVecIgnore.clear();
	_gCacheFileType.clear();

	return 0;
}

bool CSHFiles::Initialize()
{
	_gNormalFolderIco = wxArtProvider::GetIcon(wxART_FOLDER, wxART_OTHER, wxSize(16, 16));
	_gNormalFileIco   = wxArtProvider::GetIcon(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16));
	_gExternalPG      = wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_OTHER, wxSize(16, 16));

	//확장자 명령 설정 셋팅
	wxString strCurrentDir(theCommonUtil->GetWorkDir());

	//메뉴읽기
	wxXmlResource::Get()->InitAllHandlers();
	wxString strMenuXRC = wxT("menus_en.xrc");

	if (_gSystemLang.Cmp(wxT("ko")) == 0)
		strMenuXRC = wxT("menus_ko.xrc");
	else
		strMenuXRC = wxT("menus_en.xrc");

	wxString strXrcPath = strCurrentDir + SLASH + wxT("xrc") + SLASH + strMenuXRC;
	bool bXRCLoaded = wxXmlResource::Get()->Load(strXrcPath);

	if (!bXRCLoaded)
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_XRC_MENU_LOAD_FAILED")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	wxString strFontName(wxT(""));
	int nFontSize = 0;

	strFontName = theJsonConfig->GetFontName();
	nFontSize = theJsonConfig->GetFontSize();

#ifdef __WXMSW__
	//디렉토리 트리 탐색시 무시 항목리스트
	_gVecIgnore.push_back("");
	_gVecIgnore.push_back(".");
	_gVecIgnore.push_back("..");
	_gVecIgnore.push_back(".git");
	_gVecIgnore.push_back("winsxs");
	_gVecIgnore.push_back("WinSxS");

	//폰트 설정
	wxString strFixedsysFontPath(wxT(""));
	strFixedsysFontPath = strCurrentDir + SLASH + wxT("rc") + SLASH + wxT("hvgafix.fon");// wxT("FSEX300.ttf");

	//등록하고자 하는 폰트가 존재하는지 체크
	wxArrayString arrFontList = wxFontEnumerator::GetFacenames();
	bool bFontExist = false;
	for (auto item : arrFontList)
	{
	//	if (item.Cmp(wxT("Fixedsys Excelsior 3.01")) == 0)
		if (item.Cmp(wxT("Fixedsys")) == 0)
		{
			bFontExist = true;
			break;
		}
	}

	if (!bFontExist)
	{
		int iErr = AddFontResource(CONVSTR(strFixedsysFontPath));
		if (iErr != 0)
			m_bAddFontRC = true;
	}

	_gFont = new wxFont(nFontSize, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, strFontName);
#else
	_gFont = &wxSystemSettings::GetFont(wxSYS_OEM_FIXED_FONT);
#endif

	//이미지리스트
	theCommonUtil->LoadImageList();

	//디렉토리의 경우 유형은 "파일 폴더" 로 고정
	std::unordered_map<wxString, wxString>::const_iterator typeIter = _gCacheFileType.find(wxT("dir"));
	if (typeIter == _gCacheFileType.end())
	{
		std::unordered_map<wxString, wxString>::value_type val(wxT("dir"), theMsgManager->GetMessage(wxT("MSG_DIR_DESCRIPTION")));
		_gCacheFileType.insert(val);
	}

	return true;
}

bool CSHFiles::LoadJsonConfigs()
{
	if (!theJsonConfig->Load())
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_INITFAIL_WINDOW_SETTING")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	if (!theMsgManager->Load())
	{
		wxMessageBox(wxT("The Message config load failed"), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	if (!theBookmark->Load())
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_INITFAIL_WINDOW_BOOKMARK")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	if (!theExtInfo->Load())
	{
		wxMessageBox(theMsgManager->GetMessage(wxT("MSG_INITFAIL_WINDOW_EXTINFO")), PROGRAM_FULL_NAME, wxOK | wxICON_ERROR);
		return false;
	}

	return true;
}

void CSHFiles::OnActiveApp(wxActivateEvent& event)
{
	if(theSplitterManager && !_gAppActivated && !m_bFirstExec)
	{
		theSplitterManager->SetActivatePage();
		_gAppActivated = true;
	}

	if(m_bFirstExec)
		m_bFirstExec = false;
}
