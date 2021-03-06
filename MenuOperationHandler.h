#ifndef __MENU_OPERATION_HANDLER_H_INCLUDED__
#define __MENU_OPERATION_HANDLER_H_INCLUDED__

class CTabManager;
class CMenuOperationHandler// : public wxEvtHandler
{
private:
	explicit CMenuOperationHandler() {};

public:
	~CMenuOperationHandler() {};
	static CMenuOperationHandler *Get();

public:
	void ExecuteMenuOperation(_MENU_EVENT_TYPE _menuType, const wxString& strInfo = wxT(""));
	void SetFavoriteItem(const wxString& strItem);
	void ExecuteCompress(const std::vector<wxString>& vCompressDatas, const wxString& strCompressedFile, const wxString& strCompressType);

private:
	wxString GetTargetDirectory();
	// 복사/이동
	void DoCopyMove(_MENU_EVENT_TYPE _menuType);
	// 복사/이동
	void DoCopyMoveOperation(std::list<wxString>& lstDatas, const wxString& strPath, bool bMove);
	//이름변경
	void DoRename();
	//새 디렉토리 생성
	void DoMakeDirectory();
	//편집
	void DoFileEdit();
	//휴지통/삭제
	void DoDeleteTrash(_MENU_EVENT_TYPE _menuType);
	//CMD 실행
	void DoCMDExecute();
	//프로그램종료
	void DoProgramTerminate();
	//새로고침
	void DoRefreshDir();
	//전체선택/해제
	void DoAllSelectOrRelease(_MENU_EVENT_TYPE _menuType);
	//폴더직접이동
	void DoGotoDirDirectly();
	//디렉토리 관리기
	void DoDirectoryManager();
	//파일찾기
	void DoFileFind();
	//컨텍스트메뉴
	void DoDispContextMenu();
	//컬럼 변경
	void DoChageViewColumn();
	void SetViewColumnInTabs(CTabManager* pTabManager);
	//파일리스트
	void DoChangeFileList();
	//즐겨찾기메뉴 선택
	void DoFavoriteItemSelected();
	//전체화면
	void DoFullScreen();
	//환경설정
	void DoSettings();
	//환경설정저장
	void DoSaveConfig();
	//즐겨찾기
	void DoFavorite(_MENU_EVENT_TYPE _menuType);
	//이프로그램은
	void DoThisProgramIs();
	//압축하기
	void DoCompress();
	//압축해제
	void DoDecompress(_MENU_EVENT_TYPE _menuType);
#ifdef __WXMSW__
	//드라이브 생성 및 삭제
	void DoAddDriveAndRemove();
	//디스크용량 갱신
	void DoDiskspaceUpdate(const wxString& strDrive);
#endif
private:
	wxString m_strFavoriteItem = wxT("");
	wxString m_strCopyMoveSrcDir = wxT("");

private:
	static CMenuOperationHandler* m_pMenuOPHandlerInstance;

};
#endif
