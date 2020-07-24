#include "global.h"
#include "MyEventDef.h"
//외부 프로그램 실행
wxDEFINE_EVENT(wxEVT_EXEC_FILE_EDIT, wxCommandEvent);
//컨텍스트 메뉴 이벤트
wxDEFINE_EVENT(wxEVT_EXEC_CONTEXT_MENU, wxCommandEvent);
//이름변경
wxDEFINE_EVENT(wxEVT_DIR_FILE_RENAME, wxCommandEvent);
//전체선택
wxDEFINE_EVENT(wxEVT_ITEM_ALL_SELECT, wxCommandEvent);
//전체해제
wxDEFINE_EVENT(wxEVT_ITEM_ALL_RELEASE, wxCommandEvent);
//디렉토리 관리자 새로고침
wxDEFINE_EVENT(wxEVT_DIRMANAGER_REFRESH_ALL, wxCommandEvent);

