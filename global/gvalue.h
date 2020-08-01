#ifndef __GLOBAL_VALUE_H__
#define __GLOBAL_VALUE_H__

class CTabManager;

#ifdef __WXMSW__
	typedef HIMAGELIST SYSTEM_IMAGE_LIST;
#else
	typedef wxImageList* SYSTEM_IMAGE_LIST;
#endif

// 메인윈도우
extern wxFrame* _gMainFrame;

// 이미지리스트
extern SYSTEM_IMAGE_LIST _gImageList;

// 전역폰트
extern wxFont* _gFont;

// Default 폴더 이미지
extern wxIcon _gNormalFolderIco;

// Default 파일 이미지
extern wxIcon _gNormalFileIco;

// 외부편집프로그램 메뉴 이미지
extern wxBitmap _gExternalPG;

// 디렉토리 탐색시 제외 항목
extern wxVector<wxString> _gVecIgnore;

// System language
extern wxString _gSystemLang;

// 파일유형 캐쉬
extern std::unordered_map<wxString, wxString> _gCacheFileType;

// Global TabManager
extern CTabManager* _gTabManager;

//App Activate Flag
extern bool _gAppActivated;

//외부편집프로그램 목록
extern std::unordered_map<int, EXTERNAL_EDIT_PROGRAM> _gExternalPGList;

#endif
