#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <wx/wx.h>
#include <wx/app.h>
#include <wx/snglinst.h>
#include <wx/display.h>
#include <wx/artprov.h>
#include <wx/aui/aui.h>
#include <wx/busyinfo.h>
#include <wx/xrc/xmlres.h>
#include <wx/fontenum.h>
#include <wx/splitter.h>
#include <wx/aui/auibook.h>
#include <wx/tokenzr.h>
#include <wx/intl.h>
#include <wx/language.h>
#include <wx/volume.h>
#include <wx/event.h>
#include <wx/dialog.h>
#include <wx/clipbrd.h>
#include <wx/regex.h>

#include <memory>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

using namespace std;

#ifdef __WXMSW__
	#include <wx/msw/private.h>
	#include <windows.h>
	#include <shellapi.h>
	#include <shlobj.h>
	#include <CommCtrl.h>
	#include <WinUser.h>
	#include <imm.h>
#endif
// 열거형
#include "genum.h"

// 각종 define
#include "defs.h"

// extern 전역변수
#include "gvalue.h"

// 버전정보
#include "Version.h"

// 메세지 관리
#include "MsgManager.h"

//공통함수 모음
#include "CommonUtil.h"

// 확장자 정보
#include "Extinfo.h"

// 환경설정
#include "JsonConfig.h"

// 북마크(즐겨찾기)
#include "JsonBookmark.h"
// 쓰레드 Lock
#include "Lock.h"

//SplitterManager
#include "../SplitterManager.h"

//메뉴처리및 각종 이벤처 처리 담당Handler
#include "../MenuOperationHandler.h"

//이벤트 정의
#include "MyEventDef.h"

// 트리정보
#include "../interface/SHCD.h"
#include "../interface/SkipKeyMap.h"
//공용함수
#define theCommonUtil	    (CCommonUtil::Get())

//환경설정
#define theJsonConfig       (CJsonConfig::Get())

//Message정의
#define theMsgManager	    (CMsgManager::Get())

//확장자 정의
#define theExtInfo          (CExtInfo::Get())

//분할윈도우 관리
#define theSplitterManager  (CSplitterManager::Get())

//메뉴처리및 각종 이벤처 처리 담당Handler
#define theMenuOPHandler    (CMenuOperationHandler::Get())

//드라이브정보(Windows only)
#ifdef __WXMSW__
	#include "DriveInfo.h"
	#include "../interface/msw/MSWCopyMoveClipboard.h"
	#define theDriveInfo     (CDriveInfo::Get())
	#define theClipboard     (CClipboard::Get())
#endif

// 북마크(즐겨찾기)
#define theBookmark          (CJsonBookmark::Get())

//디렉토리 트리관리
#define theSHCD              (CDirTree::Get())
//Skip 키
#define theSkipKeyMap    (CSkipKeyMap::Get())
#endif
