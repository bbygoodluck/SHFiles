#ifndef __EVENT_HANDLER_INCLUDED__
#define __EVENT_HANDLER_INCLUDED__

class CMainFrame;
class CEvtHandler : public wxEvtHandler
{
public:
	explicit CEvtHandler(CMainFrame* _pMainFrame);
	~CEvtHandler();

	void LoadBookmark(bool bFromFavoriteManager);
	void SetBookmarkDropdown();
	void RegisterToolbarEvent();
	void OnBookmarkSelected(wxCommandEvent& event);

private:
	CMainFrame* m_pMainFrame;

private:
	//파일메뉴
	    // 실행
	void menu_file_execute();
		// 새탭
	void menu_file_add_tab();
	    // 새 폴더 생성
	void menu_file_add_dir();
	    // 편집
	void menu_file_edit();
	    // 복사/이동
	void menu_file_CopyMove(_MENU_EVENT_TYPE _menuType);
	    // 이름변경
	void menu_file_rename();
	    //휴지통/완전삭제
	void menu_file_trashOrDel();
		//CMD 실행
	void menu_file_ExecCMD();
	    // 프로그램종료
	void menu_file_programExit();
	//편집메뉴
	    // 전체선택/해제
	void menu_edit_select_release(_MENU_EVENT_TYPE _menuType);
	    //파일찾기
	void menu_edit_find();
		//컨텍스트 메뉴
	void menu_edit_ContextMenu();

	//경로
	    //새로고침
	void menu_path_Refresh();
	void menu_path_GotoDirDirectly();
	    //디렉토리 관리기
	void menu_path_DirManager();

	//압축메뉴
		// 압축 보기
	void menu_comp_compressView();
		// 압축하기
	void menu_comp_DoCompress(bool bSetPassword);
		// 압축해제
	void menu_comp_DoDeCompress(bool isMakeFolder);
	void menu_comp_SelDirCompRelease();

	//보기메뉴
	    //전체화면
	void menu_view_fullScreen();
	    //윈도우 분할
	void menu_window_operation(int iMenuID, const int iCurrentWindowStyle);
	    //컬럼
	void menu_column_operation(int iMenuID, const int iCurrentDispColumn);
	    //파일리스트 종료
	void menu_view_filelist(int iMenuID);
	//즐겨찾기
	void menu_favorite(_MENU_EVENT_TYPE menuType);

	//도구
		// 환경설정
	void menu_tool_envsetting();
	    // 현재설정저장
	void menu_tool_cursave();
	//도움말
	    // 이프로그램은...
	void menu_help_thispg();
private:
	void OnMenuEvent(wxCommandEvent& event);
	void OnMenuEventUpdate(wxUpdateUIEvent& event);
	//툴바
	void OnToolbarRefresh(wxCommandEvent& event);
	void OnToolbarFileEdit(wxCommandEvent& event);
	void OnToolbarFind(wxCommandEvent& event);
	void OnToolbarDirManager(wxCommandEvent& event);
	void OnToolbarBookmark(wxCommandEvent& event);
	void OnToolbarEnvSetting(wxCommandEvent& event);
	void OnToolbarEnvSave(wxCommandEvent& event);
	void OnToolbarFuncKeyView(wxCommandEvent& event);
	void OnToolbarThisPG(wxCommandEvent& event);
	wxDECLARE_EVENT_TABLE();

};
#endif
