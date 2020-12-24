#ifndef _MY_EVENT_DEFINE_H_INCLUDED
#define _MY_EVENT_DEFINE_H_INCLUDED

/********************************************************************************
// 설      명 : 사용자 정의 이벤트들
*********************************************************************************/
//외부 프로그램 실행
wxDECLARE_EVENT(wxEVT_EXEC_FILE_EDIT, wxCommandEvent);
//컨텍스트 메뉴 이벤트
wxDECLARE_EVENT(wxEVT_EXEC_CONTEXT_MENU, wxCommandEvent);
//이름변경
wxDECLARE_EVENT(wxEVT_DIR_FILE_RENAME, wxCommandEvent);
//전체선택
wxDECLARE_EVENT(wxEVT_ITEM_ALL_SELECT, wxCommandEvent);
//전체해제
wxDECLARE_EVENT(wxEVT_ITEM_ALL_RELEASE, wxCommandEvent);
//디렉토리 관리자 새로고침
wxDECLARE_EVENT(wxEVT_DIRMANAGER_REFRESH_ALL, wxCommandEvent);
#ifdef __WXMSW__
//드라이브 추가 삭제
wxDECLARE_EVENT(wxEVT_DRIVE_ADD_REMOVE, wxCommandEvent);
//디스크용량 갱신
wxDECLARE_EVENT(wxEVT_DISK_SPACE_UPDATE, wxCommandEvent);
//
wxDECLARE_EVENT(wxEVT_DECOMPRESS_DUP_CHECK, wxCommandEvent);
//FileSystemWatcher
wxDECLARE_EVENT(wxEVT_FILE_SYSTEM_WATCH, wxCommandEvent);
#endif
#endif
