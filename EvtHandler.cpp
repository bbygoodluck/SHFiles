#include "ginc.h"
#include "MainFrame.h"
#include "EvtHandler.h"

wxBEGIN_EVENT_TABLE(CEvtHandler, wxEvtHandler)
	EVT_MENU(wxID_ANY, CEvtHandler::OnMenuEvent)
	EVT_UPDATE_UI(wxID_ANY, CEvtHandler::OnMenuEventUpdate)
wxEND_EVENT_TABLE()

CEvtHandler::CEvtHandler(CMainFrame* _pMainFrame)
	: m_pMainFrame(_pMainFrame)
{
	
}

CEvtHandler::~CEvtHandler()
{
	
}

void CEvtHandler::OnMenuEvent(wxCommandEvent& event)
{
	int _iMenuID = event.GetId();
	
	if       (_iMenuID == XRCID("m_fMenu_Execute"))		             menu_file_execute();
	else if  (_iMenuID == XRCID("m_fMenu_Rename"))		             menu_file_rename();
	else if  (_iMenuID == XRCID("m_fMenu_NewTab"))		             menu_file_add_tab();
	else if  (_iMenuID == XRCID("m_fMenu_MakeFolder"))	             menu_file_add_dir();
	else if  (_iMenuID == XRCID("m_fMenu_FileEdit"))		         menu_file_edit();
	else if  (_iMenuID == XRCID("m_fMenu_Copy"))			         menu_file_CopyMove(_MENU_FILE_COPY);
	else if  (_iMenuID == XRCID("m_fMenu_Move"))			         menu_file_CopyMove(_MENU_FILE_MOVE);
	else if  (_iMenuID == XRCID("m_eMenu_Copy"))			         menu_file_CopyMove(_MENU_FILE_COPY_CLIPBOARD);
	else if  (_iMenuID == XRCID("m_eMenu_Cut"))			             menu_file_CopyMove(_MENU_FILE_MOVE_CLIPBOARD);
	else if  (_iMenuID == XRCID("m_eMenu_Paste"))		             menu_file_CopyMove(_MENU_FILE_PASTE_CLIPBOARD);
	else if  (_iMenuID == XRCID("m_fMenu_goTrash"))		             menu_file_trashOrDel();
	else if  (_iMenuID == XRCID("m_fMenu_goDel"))		             menu_file_trashOrDel();
	else if  (_iMenuID == XRCID("m_fMenu_Exit"))			         menu_file_programExit();
	else if  (_iMenuID == XRCID("m_eMenu_Release"))		             menu_edit_select_release(_MENU_EDIT_ALL_RELEASE);
	else if  (_iMenuID == XRCID("m_eMenu_SelAll"))		             menu_edit_select_release(_MENU_EDIT_ALL_SELECT);
	else if  (_iMenuID == XRCID("m_eMenu_ContextMenu"))		         menu_edit_ContextMenu();
	else if  (_iMenuID == XRCID("m_eMenu_Search"))	                 menu_edit_find();
	else if  (_iMenuID == XRCID("m_pMenu_Refresh"))                  menu_path_Refresh();
	else if  (_iMenuID == XRCID("m_pMenu_GotoDirDirectly"))          menu_path_GotoDirDirectly();
    else if  (_iMenuID == XRCID("m_menuComp_View"))                  menu_comp_compressView();
	else if  (_iMenuID == XRCID("m_menuComp_Progress"))              menu_comp_DoCompress(false);
	else if  (_iMenuID == XRCID("m_menuComp_CurrentDirCompRelease")) menu_comp_DoDeCompress(false);
	else if  (_iMenuID == XRCID("m_menuComp_MakeDirCompRelease"))    menu_comp_DoDeCompress(true);
	else if  (_iMenuID == XRCID("m_menuComp_SelDirCompRelease"))     menu_comp_SelDirCompRelease();
	else if  (_iMenuID == XRCID("m_pMenu_PathMng"))                  menu_path_DirManager();
	else if  (_iMenuID == XRCID("m_toolMenu_ENV"))	                 menu_tool_envsetting();
	else if  (_iMenuID == XRCID("m_toolMenu_CurSave"))	             menu_tool_cursave();
	else if  (_iMenuID == XRCID("m_viewMenu_FullScr"))	             menu_view_fullScreen();
	else if ((_iMenuID == XRCID("m_viewMenu_Window_0")) ||
		     (_iMenuID == XRCID("m_viewMenu_Window_1")) ||
		     (_iMenuID == XRCID("m_viewMenu_Window_2")))   	         menu_window_operation(_iMenuID, theJsonConfig->GetSplitStyle());
	else if ((_iMenuID == XRCID("m_viewMenu_Column_0")) ||
		     (_iMenuID == XRCID("m_viewMenu_Column_1")) ||
		     (_iMenuID == XRCID("m_viewMenu_Column_2")) ||
		     (_iMenuID == XRCID("m_viewMenu_Column_3")) ||
		     (_iMenuID == XRCID("m_viewMenu_Column_4")) ||
		     (_iMenuID == XRCID("m_viewMenu_Column_5")) ||
		     (_iMenuID == XRCID("m_viewMenu_Column_6")) ||
		     (_iMenuID == XRCID("m_viewMenu_Column_7")) ||
		     (_iMenuID == XRCID("m_viewMenu_Column_8")) ||
		     (_iMenuID == XRCID("m_viewMenu_Column_9")))             menu_column_operation(_iMenuID, theJsonConfig->GetColumnCount());
    else if ((_iMenuID == XRCID("m_viewMenu_DefaultFile")) ||
	     	 (_iMenuID == XRCID("m_viewMenu_HiddenFile"))  ||
		     (_iMenuID == XRCID("m_viewMenu_SystemFile"))  ||
		     (_iMenuID == XRCID("m_viewMenu_AllFile")))              menu_view_filelist(_iMenuID);
	else if  (_iMenuID == XRCID("m_favoriteMenu_Add"))               menu_favorite(_MENU_FAVORITE_ITEM_ADD);
	else if  (_iMenuID == XRCID("m_favoriteMenu_Manager"))           menu_favorite(_MENU_FAVORITE_MANAGER);
	else if  (_iMenuID == XRCID("m_helpMenu_thisProgram"))           menu_help_thispg();
}

void CEvtHandler::OnMenuEventUpdate(wxUpdateUIEvent& event)
{
	int _iupdateID = event.GetId();
	//윈도우창
	if ((_iupdateID == XRCID("m_viewMenu_Window_0")) ||	//단일창
		(_iupdateID == XRCID("m_viewMenu_Window_1")) ||	//수직창
		(_iupdateID == XRCID("m_viewMenu_Window_2")) ||	//수평창
		(_iupdateID == XRCID("m_viewMenu_Column_0")) ||	//컬럼자동
		(_iupdateID == XRCID("m_viewMenu_Column_1")) ||	//컬럼1
		(_iupdateID == XRCID("m_viewMenu_Column_2")) ||	//컬럼2
		(_iupdateID == XRCID("m_viewMenu_Column_3")) ||	//컬럼3
		(_iupdateID == XRCID("m_viewMenu_Column_4")) ||	//컬럼4
		(_iupdateID == XRCID("m_viewMenu_Column_5")) ||	//컬럼5
		(_iupdateID == XRCID("m_viewMenu_Column_6")) ||	//컬럼6
		(_iupdateID == XRCID("m_viewMenu_Column_7")) ||	//컬럼7
		(_iupdateID == XRCID("m_viewMenu_Column_8")) ||	//컬럼8
		(_iupdateID == XRCID("m_viewMenu_Column_9")))	//컬럼8
	{
		event.Check(false);
		//윈도우 스타일
		int iViewStyle = theJsonConfig->GetSplitStyle();
		wxString strMenuID = "m_viewMenu_Window_" + wxString::Format("%d", iViewStyle);
		if (_iupdateID == XRCID(strMenuID))
			event.Check(true);

		//컬럼
		int iColumnCnt = theJsonConfig->GetColumnCount();
		strMenuID = "m_viewMenu_Column_" + wxString::Format("%d", iColumnCnt);

		if (_iupdateID == XRCID(strMenuID))
			event.Check(true);
	}
	else if ((_iupdateID == XRCID("m_viewMenu_DefaultFile")) ||
		(_iupdateID == XRCID("m_viewMenu_HiddenFile")) ||
		(_iupdateID == XRCID("m_viewMenu_SystemFile")) ||
		(_iupdateID == XRCID("m_viewMenu_AllFile")))

	{
		event.Check(false);
		if (theJsonConfig->IsViewAllFile())
		{
			if (_iupdateID == XRCID("m_viewMenu_AllFile"))
				event.Check(true);
			else
				event.Check(false);
		}
		else
		{
			if (_iupdateID != XRCID("m_viewMenu_AllFile"))
			{
				event.Check(true);

				if (_iupdateID == XRCID("m_viewMenu_HiddenFile"))
				{
					if (!theJsonConfig->IsViewHiddenFile())
						event.Check(false);
				}

				if (_iupdateID == XRCID("m_viewMenu_SystemFile"))
				{
					if (!theJsonConfig->IsViewSystemFile())
						event.Check(false);
				}
			}
		}
	}
}

//파일메뉴
void CEvtHandler::menu_file_execute()
{
	wxMessageBox(theMsgManager->GetMessage(wxT("MSG_FILE_MENU_EXEC")), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
}
//새탭
void CEvtHandler::menu_file_add_tab()
{
	theSplitterManager->AddTabPage();
}

//새 폴더 생성
void CEvtHandler::menu_file_add_dir()
{
	theMenuOPHandler->ExecuteMenuOperation(_MENU_FILE_ADD_DIR);
}

//편집
void CEvtHandler::menu_file_edit()
{
	theMenuOPHandler->ExecuteMenuOperation(_MENU_FILE_EDIT);
}

//복사/이동
void CEvtHandler::menu_file_CopyMove(_MENU_EVENT_TYPE _menuType)
{
	theMenuOPHandler->ExecuteMenuOperation(_menuType);
}

//이름변경
void CEvtHandler::menu_file_rename()
{
	theMenuOPHandler->ExecuteMenuOperation(_MENU_FILE_RENAME);
}

// 휴지통으로/완전삭제
void CEvtHandler::menu_file_trashOrDel()
{
	_MENU_EVENT_TYPE mType = _MENU_FILE_DEL_TRASH;
	if (wxGetKeyState(WXK_SHIFT))
		mType = _MENU_FILE_DEL_COMP;
		
	theMenuOPHandler->ExecuteMenuOperation(mType);
}

//프로그램종료	
void CEvtHandler::menu_file_programExit()
{
	theMenuOPHandler->ExecuteMenuOperation(_MENU_FILE_PROGRAM_TERMINATE);
}

//편집메뉴
    // 전체선택/해제
void CEvtHandler::menu_edit_select_release(_MENU_EVENT_TYPE _menuType)
{
	theMenuOPHandler->ExecuteMenuOperation(_menuType);
}

//파일찾기
void CEvtHandler::menu_edit_find()
{
	theMenuOPHandler->ExecuteMenuOperation(_MENU_EDIT_FIND);
}
    // 컨텍스트 메뉴
void CEvtHandler::menu_edit_ContextMenu()
{
	theMenuOPHandler->ExecuteMenuOperation(_MENU_EDIT_CONTEXT_MENU);
}

void CEvtHandler::menu_path_Refresh()
{
	theMenuOPHandler->ExecuteMenuOperation(_MENU_PATH_REFRESH);
}

void CEvtHandler::menu_path_GotoDirDirectly()
{
	theMenuOPHandler->ExecuteMenuOperation(_MENU_PATH_GOTO_DIR_DIRECTLY);
}

void CEvtHandler::menu_path_DirManager()
{
	theMenuOPHandler->ExecuteMenuOperation(_MENU_PATH_DIR_MANAGER);
}

// 압축 보기
void CEvtHandler::menu_comp_compressView()
{
	wxMessageBox(wxT("Not yet!"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
}
// 압축하기
void CEvtHandler::menu_comp_DoCompress(bool bSetPassword)
{
	theMenuOPHandler->ExecuteMenuOperation(_MENU_COMPRESS);
}
// 압축해제
void CEvtHandler::menu_comp_DoDeCompress(bool isMakeFolder)
{
//	wxMessageBox(wxT("Not yet!"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
	_MENU_EVENT_TYPE menuType = !isMakeFolder ? _MENU_DECOMPRESS : _MENU_DECOMPRESS_MK_FOLDER; 
	theMenuOPHandler->ExecuteMenuOperation(menuType);
}

void CEvtHandler::menu_comp_SelDirCompRelease()
{
//	wxMessageBox(wxT("Not yet!"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
	theMenuOPHandler->ExecuteMenuOperation(_MENU_DECOMPRESS_SEL_DIR);
}

//보기메뉴
    //전체화면
void CEvtHandler::menu_view_fullScreen()
{
	theMenuOPHandler->ExecuteMenuOperation(_MENU_VIEW_FULLSCREEN);
}
    //윈도우분할
void CEvtHandler::menu_window_operation(int iMenuID, const int iCurrentWindowStyle)
{
	int iViewWindowSingle = XRCID("m_viewMenu_Window_0");
	int iViewWinStyle = (iMenuID - iViewWindowSingle);

	if (iViewWinStyle != iCurrentWindowStyle)
	{
		theJsonConfig->SetChangeGlobalVal(wxT("SplitWindowStyle"), iViewWinStyle);
		theSplitterManager->DoSplitter();
	}
}

	//컬럼변경
void CEvtHandler::menu_column_operation(int iMenuID, const int iCurrentDispColumn)
{
	int iAutoColumnID = XRCID("m_viewMenu_Column_0");
	int iDispColumn = (iMenuID - iAutoColumnID);

	if (iDispColumn != iCurrentDispColumn)
	{
		theJsonConfig->SetChangeGlobalVal(wxT("DispColumn"), iDispColumn);
		theMenuOPHandler->ExecuteMenuOperation(_MENU_VIEW_COLUMN_CHANGE);
	}
}
    //파일리스트 종료
void CEvtHandler::menu_view_filelist(int iMenuID)
{
	int iViewFileListFirst = XRCID("m_viewMenu_DefaultFile");
	int iViewFileList = iMenuID - iViewFileListFirst;

	theJsonConfig->SetViewFileKind(iViewFileList);
	theMenuOPHandler->ExecuteMenuOperation(_MENU_VIEW_FILELIST_CHANGE);
}

//즐겨찾기
void CEvtHandler::menu_favorite(_MENU_EVENT_TYPE menuType)
{
	theMenuOPHandler->ExecuteMenuOperation(menuType);
	
	LoadBookmark(true);
	SetBookmarkDropdown();
}

//도구
    // 환경설정
void CEvtHandler::menu_tool_envsetting()
{
	theMenuOPHandler->ExecuteMenuOperation(_MENU_TOOL_ENVIRONMENT);
}

	// 현재설정저장
void CEvtHandler::menu_tool_cursave()
{
	theMenuOPHandler->ExecuteMenuOperation(_MENU_TOOL_SAVECONFIG);
}

//도움말
	// 이프로그램은...
void CEvtHandler::menu_help_thispg()
{
	theMenuOPHandler->ExecuteMenuOperation(_MENU_HELP_THIS_PROGRAM);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ToolBar

void CEvtHandler::LoadBookmark(bool bFromFavoriteManager)
{
	if (theBookmark->GetBookmarkSize() > 0)
	{
		wxMenuBar* menuBar = m_pMainFrame->GetMenuBar();
		wxMenu *pFavoriteMenu = menuBar->GetMenu(menuBar->FindMenu(theMsgManager->GetMessage(wxT("MSG_TOOLBAR_FAVORITE"))));
		
		if (bFromFavoriteManager)
		{
			//기존 추가되었던 메뉴 삭제
			int iMenuCount = pFavoriteMenu->GetMenuItemCount();
			for (int i = 2; i < iMenuCount; i++)
			{
				wxMenuItem *pMenuItem = pFavoriteMenu->FindItemByPosition(2);
				pFavoriteMenu->Destroy(pMenuItem);
			}
		}

		pFavoriteMenu->AppendSeparator();
		theBookmark->CreateBookmarkMenu(pFavoriteMenu);
	}
}

void CEvtHandler::SetBookmarkDropdown()
{
	if (theBookmark->GetBookmarkSize() > 0)
	{
		wxToolBar* pToolBar = m_pMainFrame->GetToolBar();

		wxMenu* pFavoriteMenu = new wxMenu();
		theBookmark->CreateBookmarkMenu(pFavoriteMenu);

		pToolBar->SetDropdownMenu(TOOLBAR_ID_BOOKMARK, pFavoriteMenu);
	}
}

void CEvtHandler::RegisterToolbarEvent()
{
	this->Connect(TOOLBAR_ID_REFRESH, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEvtHandler::OnToolbarRefresh));
	this->Connect(TOOLBAR_ID_FILE_EDIT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEvtHandler::OnToolbarFileEdit));
	this->Connect(TOOLBAR_ID_FIND, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEvtHandler::OnToolbarFind));
	this->Connect(TOOLBAR_ID_DIR_MANAGER, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEvtHandler::OnToolbarDirManager));
	this->Connect(TOOLBAR_ID_BOOKMARK, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEvtHandler::OnToolbarBookmark));
	this->Connect(TOOLBAR_ID_ENV_SETTING, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEvtHandler::OnToolbarEnvSetting));
	this->Connect(TOOLBAR_ID_ENV_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEvtHandler::OnToolbarEnvSave));
	this->Connect(TOOLBAR_ID_THIS_PROGRAM, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(CEvtHandler::OnToolbarThisPG));
}

void CEvtHandler::OnToolbarRefresh(wxCommandEvent& event)
{
	menu_path_Refresh();
}

void CEvtHandler::OnToolbarFileEdit(wxCommandEvent& event)
{
	menu_file_edit();
}

void CEvtHandler::OnToolbarFind(wxCommandEvent& event)
{
	menu_edit_find();
}

void CEvtHandler::OnToolbarDirManager(wxCommandEvent& event)
{
	menu_path_DirManager();
}

void CEvtHandler::OnToolbarBookmark(wxCommandEvent& event)
{
	wxToolBar* pToolBar = m_pMainFrame->GetToolBar();
	wxSize szToolBar = pToolBar->GetSize();

	int iPos = pToolBar->GetToolPos(TOOLBAR_ID_BOOKMARK);
	wxPoint pt = pToolBar->GetPosition();
	pt.x = (pt.x + 60) * iPos;
	pt.y = szToolBar.GetHeight();

	wxMenu menuFavorite;
	theBookmark->CreateBookmarkMenu(&menuFavorite);
	
	pToolBar->PopupMenu(&menuFavorite, pt);
}

void CEvtHandler::OnToolbarEnvSetting(wxCommandEvent& event)
{
	menu_tool_envsetting();
}

void CEvtHandler::OnToolbarEnvSave(wxCommandEvent& event)
{
	menu_tool_cursave();
}

void CEvtHandler::OnToolbarFuncKeyView(wxCommandEvent& event)
{
	
}

void CEvtHandler::OnToolbarThisPG(wxCommandEvent& event)
{
	menu_help_thispg();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
void CEvtHandler::OnBookmarkSelected(wxCommandEvent& event)
{
	int iMenuID = event.GetId();
	wxString strBookmarkPath = theBookmark->GetBookmarkPath(iMenuID);
	
	theMenuOPHandler->SetFavoriteItem(strBookmarkPath);
	theMenuOPHandler->ExecuteMenuOperation(_MENU_FAVORITE_ITEM_SELECTED);
}