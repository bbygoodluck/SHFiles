#include "global.h"
#include "CommonUtil.h"

std::unique_ptr<CCommonUtil> CCommonUtil::m_pInstance = nullptr;

#ifdef __WXMSW__
int CCommonUtil::GetWindowsVersion()
{
	int nVersion = -1;
	OSVERSIONINFOEX osvi = { 0, };
	BOOL bVersion_ex_flag = 0;

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	bVersion_ex_flag = GetVersionEx((OSVERSIONINFO *)&osvi);
	if (!bVersion_ex_flag)
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO *)&osvi))
			return -1;
	}

	switch (osvi.dwPlatformId)
	{
		case VER_PLATFORM_WIN32_WINDOWS:		//Windows 9x
			{
				if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0) nVersion = WIN_OS_95;
				else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10) nVersion = WIN_OS_98;
				else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)	nVersion = WIN_OS_ME;
			}

			break;

		case VER_PLATFORM_WIN32_NT:				//Windows NT 기술 기반의 운영체제 버전
			{
				if (osvi.dwMajorVersion <= 4) nVersion = WIN_OS_NT;
				else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) nVersion = WIN_OS_2000;
				else if (bVersion_ex_flag)
				{
					if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) nVersion = WIN_OS_XP;
					else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2) nVersion = WIN_OS_2003;
					else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0) nVersion = WIN_OS_VISTA_OR_2008;
					else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1) nVersion = WIN_OS_7_OR_2008R2;
					else nVersion = WIN_OS_7_OVER;
				}
			}

			break;
		default:
			break;
	}

	return nVersion;
}
#endif

wxString CCommonUtil::GetSystemLang()
{
#ifdef __WXMSW__
	int icchLocalName = LOCALE_NAME_MAX_LENGTH;
	#if defined(_UNICODE) || defined(UNICODE)
		WCHAR lpBuffer[LOCALE_NAME_MAX_LENGTH];
	#else
		LPSTR lpBuffer[LOCALE_NAME_MAX_LENGTH];
	#endif

	wxZeroMemory(lpBuffer);
	GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SISO639LANGNAME, lpBuffer, icchLocalName);
	return wxString(lpBuffer);
#else
#endif
}

CCommonUtil* CCommonUtil::Get()
{
	if (m_pInstance.get() == NULL)
		m_pInstance.reset(new CCommonUtil());

	return m_pInstance.get();
}

void CCommonUtil::LoadImageList()
{
#ifdef __WXMSW__
	// IID_IImageList {46EB5926-582E-4017-9FDF-E8998DAA0950}
	static const GUID IID_IImageList = { 0x46EB5926, 0x582E, 0x4017, { 0x9F, 0xDF, 0xE8, 0x99, 0x8D, 0xAA, 0x9, 0x50 } };
	// IID_IImageList2 {192B9D83-50FC-457B-90A0-2B82A8B5DAE1}
	static const GUID IID_IImageList2 = { 0x192B9D83, 0x50FC, 0x457B, { 0x90, 0xA0, 0x2B, 0x82, 0xA8, 0xB5, 0xDA, 0xE1 } };

	_gImageList = nullptr;
	HRESULT res = SHGetImageList(SHIL_SMALL, IID_IImageList2, (void**)& _gImageList);
	if (FAILED(res))
		res = SHGetImageList(SHIL_SMALL, IID_IImageList, (void**)& _gImageList);
#else
	_gImageList = wxTheFileIconsTable->GetSmallImageList();
#endif
}

const wxString& CCommonUtil::GetWorkDir()
{
	if(strWorkingDirectory.IsEmpty())
		strWorkingDirectory = wxGetCwd();
		
	return strWorkingDirectory;
}

void CCommonUtil::RefreshWindow(wxWindow* pWindow, const wxRect& rect, bool bUpdate)
{
#ifdef __WXMSW__
	HWND hWnd = pWindow->GetHWND();
	if (rect.GetWidth() != 0)
	{
		RECT mswRect;
		const RECT* pRect;

		wxCopyRectToRECT(rect, mswRect);
		pRect = &mswRect;

		//	INT flags = RDW_INVALIDATE | RDW_NOCHILDREN | RDW_UPDATENOW | RDW_ERASE;
		//	::RedrawWindow(hWnd, pRect, NULL, flags);
		::InvalidateRect(hWnd, pRect, false);
	}
	else
		pWindow->Refresh(false);

	if (bUpdate) 
		pWindow->Update();
#else
	pWindow->RefreshRect(rect, false);
	pWindow->Update();
#endif
}

void CCommonUtil::GetSizeInfo(const double dblFileSize, wxString& strFileSize, wxString& strFileSizeType, wxColour& dispColor)
{
	double dblSize = 0.0;
	dispColor = theJsonConfig->GetDefaultColor();//wxColour(192, 192, 192);

	strFileSizeType.clear();

	if (dblFileSize < KILOBYTE)
		strFileSize = wxString::Format(wxT("%3.0f"), dblFileSize);

	if (dblFileSize >= KILOBYTE)
	{
		dblSize = dblFileSize / KILOBYTE;
		strFileSizeType = _T("KB");
	}

	if (dblFileSize >= MEGABYTE)
	{
		dblSize = dblFileSize / MEGABYTE;
		strFileSizeType = _T("MB");

		dispColor = *wxRED;
	}

	if (dblFileSize >= GIGABYTE)
	{
		dblSize = dblFileSize / GIGABYTE;
		strFileSizeType = _T("GB");

		dispColor = *wxGREEN;
	}

	if (dblFileSize >= TERABYTE)
	{
		dblSize = dblFileSize / TERABYTE;
		strFileSizeType = _T("TB");

		dispColor = *wxBLUE;
	}

	if (strFileSizeType.Cmp(wxT("")) != 0)
		strFileSize = wxString::Format(wxT("%3.2f"), dblSize);
}

wxString CCommonUtil::GetVolume(const wxString& strFullPath)
{
	wxFileName fn(strFullPath);
	return fn.GetVolume();
}

wxString CCommonUtil::GetExt(const wxString& strFileName)
{
	wxString strExt(wxT(""));
	wxString strName(strFileName);

	if (strName.find_last_of(wxT(".")) != wxString::npos)
		strExt = strName.substr(strName.find_last_of(wxT(".")) + 1);

	return strExt;
}

wxString CCommonUtil::GetKeyName(const wxKeyEvent& event)
{
	int iKeyCode = event.GetKeyCode();
	const char* virt = GetVirtualKeyCodeName(iKeyCode);

	if (virt)
		return virt;

	if (iKeyCode > 0 && iKeyCode < 32)
		return wxString::Format(wxT("Ctrl-%c"), (unsigned char)('A' + iKeyCode - 1));

	if (iKeyCode >= 32 && iKeyCode < 128)
		return wxString::Format(wxT("%c"), (unsigned char)iKeyCode);

#if wxUSE_UNICODE
	int uc = event.GetUnicodeKey();
	if (uc != WXK_NONE)
		return wxString::Format(wxT("%c"), uc);
#endif

	return wxT("UnKnown");
}

const char* CCommonUtil::GetVirtualKeyCodeName(int nKeyCode)
{
	switch (nKeyCode)
	{
#define WXK_(x) \
        case WXK_##x: return #x;

		WXK_(BACK)
			WXK_(TAB)
			WXK_(RETURN)
			WXK_(ESCAPE)
			WXK_(SPACE)
			WXK_(DELETE)
			WXK_(START)
			WXK_(LBUTTON)
			WXK_(RBUTTON)
			WXK_(CANCEL)
			WXK_(MBUTTON)
			WXK_(CLEAR)
			WXK_(SHIFT)
			WXK_(ALT)
			WXK_(CONTROL)
			WXK_(MENU)
			WXK_(PAUSE)
			WXK_(CAPITAL)
			WXK_(END)
			WXK_(HOME)
			WXK_(LEFT)
			WXK_(UP)
			WXK_(RIGHT)
			WXK_(DOWN)
			WXK_(SELECT)
			WXK_(PRINT)
			WXK_(EXECUTE)
			WXK_(SNAPSHOT)
			WXK_(INSERT)
			WXK_(HELP)
			WXK_(NUMPAD0)
			WXK_(NUMPAD1)
			WXK_(NUMPAD2)
			WXK_(NUMPAD3)
			WXK_(NUMPAD4)
			WXK_(NUMPAD5)
			WXK_(NUMPAD6)
			WXK_(NUMPAD7)
			WXK_(NUMPAD8)
			WXK_(NUMPAD9)
			WXK_(MULTIPLY)
			WXK_(ADD)
			WXK_(SEPARATOR)
			WXK_(SUBTRACT)
			WXK_(DECIMAL)
			WXK_(DIVIDE)
			WXK_(F1)
			WXK_(F2)
			WXK_(F3)
			WXK_(F4)
			WXK_(F5)
			WXK_(F6)
			WXK_(F7)
			WXK_(F8)
			WXK_(F9)
			WXK_(F10)
			WXK_(F11)
			WXK_(F12)
			WXK_(F13)
			WXK_(F14)
			WXK_(F15)
			WXK_(F16)
			WXK_(F17)
			WXK_(F18)
			WXK_(F19)
			WXK_(F20)
			WXK_(F21)
			WXK_(F22)
			WXK_(F23)
			WXK_(F24)
			WXK_(NUMLOCK)
			WXK_(SCROLL)
			WXK_(PAGEUP)
			WXK_(PAGEDOWN)
			WXK_(NUMPAD_SPACE)
			WXK_(NUMPAD_TAB)
			WXK_(NUMPAD_ENTER)
			WXK_(NUMPAD_F1)
			WXK_(NUMPAD_F2)
			WXK_(NUMPAD_F3)
			WXK_(NUMPAD_F4)
			WXK_(NUMPAD_HOME)
			WXK_(NUMPAD_LEFT)
			WXK_(NUMPAD_UP)
			WXK_(NUMPAD_RIGHT)
			WXK_(NUMPAD_DOWN)
			WXK_(NUMPAD_PAGEUP)
			WXK_(NUMPAD_PAGEDOWN)
			WXK_(NUMPAD_END)
			WXK_(NUMPAD_BEGIN)
			WXK_(NUMPAD_INSERT)
			WXK_(NUMPAD_DELETE)
			WXK_(NUMPAD_EQUAL)
			WXK_(NUMPAD_MULTIPLY)
			WXK_(NUMPAD_ADD)
			WXK_(NUMPAD_SEPARATOR)
			WXK_(NUMPAD_SUBTRACT)
			WXK_(NUMPAD_DECIMAL)
			WXK_(NUMPAD_DIVIDE)

			WXK_(WINDOWS_LEFT)
			WXK_(WINDOWS_RIGHT)
#if defined(__WXOSX__)
			WXK_(RAW_CONTROL)
#endif
#undef WXK_

		default:
			return NULL;
	}
}

#ifdef __WXMSW__
void CCommonUtil::SetImeModeToEnglish(wxWindow* pWindow)
{
	if (theJsonConfig->IsShift_IME_KOR_MoveDrive())
	{
		HIMC hIMC;
		DWORD dwConversion;
		DWORD dwSentence;

		hIMC = ::ImmGetContext(pWindow->GetHWND());
		ImmGetConversionStatus(hIMC, &dwConversion, &dwSentence);
		if (dwConversion & IME_CMODE_NATIVE)     // 한글 mode 
			ImmSetConversionStatus(hIMC, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);    // 한글모드아니게 설정

		ImmReleaseContext(pWindow->GetHWND(), hIMC);
	}
}

bool CCommonUtil::IsSameDrive(const wxString& strNewPath, const wxString strOldPath)
{
	wxString strNewDrive = strNewPath.Left(1);
	wxString strOldDrive = strOldPath.Left(1);
	//신 드라이브 == 기존 드라이브
	if(!Compare(strNewDrive, strOldDrive))
	{
		wxString strMsg(wxT(""));
		strMsg = strNewDrive + wxT(":") + SLASH;
		strMsg += wxT(" and ");
		strMsg += strOldDrive + wxT(":") + SLASH;
		strMsg += wxT(" is the same drive");
		
		wxMessageBox(strMsg, PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
		return true;
	}
	
	return false;
}
#endif

wxString CCommonUtil::GetParentPath(const wxString& strFullPath)
{
	wxString _strVolume;
	wxString _strPath;
	wxString _strName;
	wxString _strExt;

	wxFileName::SplitPath(strFullPath, &_strVolume, &_strPath, &_strName, &_strExt);
	
	return (_strPath.IsEmpty() ? _strVolume + wxT(":") + SLASH : _strVolume + wxT(":") + _strPath);
}

wxString CCommonUtil::GetPathName(const wxString& strFullPath)
{
	wxString _strVolume;
	wxString _strPath;
	wxString _strName;
	wxString _strExt;

	wxFileName::SplitPath(strFullPath, &_strVolume, &_strPath, &_strName, &_strExt);
	
	return (_strName.IsEmpty() ? strFullPath : _strName);
}

wxString CCommonUtil::GetFileName(const wxString& strFullPath, bool IsAppendExt)
{
	wxString _strpath(wxT(""));
	wxString _strext(wxT(""));
	wxString _strpathname(wxT(""));

	wxFileName::SplitPath(strFullPath, &_strpath, &_strpathname, &_strext);
	if (_strext.IsEmpty())
		return _strpathname;

	return _strpathname + (IsAppendExt ? wxT(".") + _strext : wxT(""));
}

int CCommonUtil::GetPathDepth(const wxString& strFullPath)
{
	wxStringTokenizer tokenizer(strFullPath, SLASH);
	return (int)tokenizer.CountTokens();
}

int CCommonUtil::Compare(const wxString& strOld, const wxString& strNew)
{
#ifdef __WXMSW__
	return strOld.CmpNoCase(strNew);
#else
	return strOld.Cmp(strNew);
#endif
}

void CCommonUtil::LaunchAndExec(const wxString& strExecProgram, const wxString& strPath)
{
#ifdef __WXMSW__
	wxString strExt = GetExt(strExecProgram);
	if(strExt.IsEmpty())
	{
		wxMessageBox(strExecProgram + wxT(" is not exist file extention info"), PROGRAM_FULL_NAME, wxICON_INFORMATION | wxOK);
		return;
	}	
	
	if(Compare(strExt.MakeLower(), wxT("exe")) == 0)
	{
		SHELLEXECUTEINFO execInfo;
		wxZeroMemory(execInfo);

		execInfo.cbSize = sizeof(execInfo);
		execInfo.lpFile = strExecProgram.t_str();
		execInfo.lpVerb = wxT("open");
		execInfo.fMask = SEE_MASK_FLAG_NO_UI;
		execInfo.lpDirectory = strPath.t_str();
		execInfo.nShow = SW_SHOWDEFAULT;

		ShellExecuteEx(&execInfo);
	}
	else
		wxLaunchDefaultApplication(strExecProgram);
#else
	wxExecute(strExecProgram, wxEXEC_ASYNC);
#endif
}

int CCommonUtil::GetIconIndex(const wxString& strPath)
{
	int iIconIndex = 0;
#ifdef __WXMSW__
	SHFILEINFO sfi;
	::ZeroMemory(&sfi, sizeof(sfi));

	DWORD dwNum = GetFileAttributes(strPath);

	SHGetFileInfo(strPath, dwNum, &sfi, sizeof(sfi), IMAGELIST_FLAG);// | SHGFI_ADDOVERLAYS);
	iIconIndex = (sfi.iIcon & 0x00FFFFFF);

	DestroyIcon(sfi.hIcon);
#endif

	return iIconIndex;
}

wxString CCommonUtil::SetComma(const wxString& strSize)
{
	wxString strTmp(strSize);
	wxString strReturn = wxT("");
	int nLen = strTmp.Len();

	if (nLen > 3)
	{
		int nRestItemCount = nLen % 3;
		int nCommaCount = (nLen / 3);

		if (nRestItemCount == 0)
			nCommaCount -= 1;

		if (nCommaCount == 0)
			strReturn = strTmp;
		else
		{
			if (nRestItemCount != 0)
				strReturn = strTmp.Left(nRestItemCount) + wxT(",");

			wxString strNext = strTmp.Mid(nRestItemCount, nLen);
			nLen = strNext.Len();

			for (int nCnt = 0; nCnt < nLen; nCnt = nCnt + 3)
			{
				strReturn += strNext.Mid(nCnt, 3);
				if ((nCnt + 3) < (nLen - 1))
					strReturn += wxT(",");
			}
		}
	}
	else
		strReturn = strTmp;

	return strReturn;
}

wxString CCommonUtil::GetEllipseString(wxDC* pDC, const wxString& strSource, const int iDispWidth)
{
	wxSize sizeEllipsis = pDC->GetTextExtent(wxT("..."));

	wxString strDisp(wxT(""));
	wxString strName(strSource);

	strDisp = strName;

	wxSize sizeText = pDC->GetTextExtent(strName);
	if (sizeText.GetWidth() + sizeEllipsis.GetWidth() > iDispWidth)
	{
		wxSize sizeText = pDC->GetTextExtent(strName);
		if (sizeText.GetWidth() + sizeEllipsis.GetWidth() > iDispWidth)
		{
			while ((sizeText.GetWidth() + sizeEllipsis.GetWidth()) > iDispWidth)
			{
				strDisp = strDisp.Left(strDisp.Len() - 1);
				sizeText = pDC->GetTextExtent(wxString(strDisp + wxT("...")));
			}

			strDisp += wxT("...");
		}
	}

	return strDisp;
}

wxVector<wxString> CCommonUtil::Split(const wxString& strSource, const wxString& strDelimiter)
{
	wxVector<wxString> tokens;
	//use wxStringTokenizer;

	wxStringTokenizer tokenizer(strSource, strDelimiter);
	while (tokenizer.HasMoreTokens())
	{
		wxString strToken = tokenizer.GetNextToken();
		tokens.push_back(strToken);
	}

	return tokens;
}

void CCommonUtil::ShowErrorMessage(const wxString& strItem, int iRetCode)
{
#ifdef __WXMSW__
	switch (iRetCode)
	{
		case 0x0020:
			wxMessageBox(strItem + theMsgManager->GetMessage(wxT("MSG_DLG_DELETE_USE_ANOTHER_PG")), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
			break;

		default:
			break;
	}
#else
#endif
}

wxString CCommonUtil::ChangeName(const wxString& strName)
{
	wxString sName, sNewName, sResult;
	char drive[MAX_PATH];
	char dir[MAX_PATH];
	char name[MAX_PATH];
	char ext[MAX_PATH];
	_splitpath(strName, drive, dir, name, ext);
	sName = name;

	int pos = sName.Find("_");
	if (pos == -1)
		sNewName = sName + wxT("_Copy") + wxString(ext);
	else
	{
		int pos1 = sName.Find(wxT("_Copy"));
		if (pos1 == -1)
		{
			sNewName = sName;
			sNewName.Replace(wxT("_Copy"), wxT(""));
			sNewName = sNewName + wxT("_Copy(01)") + wxString(ext);

		}
		else
		{
			wxString sCount;
			int pos2 = sName.Find(wxT(")"));
			if (pos2 == -1)
			{
				sNewName = sName;
				sNewName.Replace(wxT("_Copy"), wxT(""));
				sNewName = sNewName + wxT("_Copy(01)") + wxString(ext);
			}
			else
			{
				wxString strFileName = sName.Mid(0, pos2 - 3);// , sName.Len());
				sCount = sName.Mid(strFileName.Len() + 1, 2);
				int iCount = atoi(sCount);
				iCount++;
				sNewName = wxString::Format(wxT("%s%s%02d%s%s"), strFileName, wxT("("), iCount, wxT(")"), ext);
			}
		}
	}
	
	bool bExistFile = false;
	bool bExistDir = false;

	wxString strExist = wxString(drive) + wxString(dir) + sNewName;
	if (wxFileExists(strExist))
		bExistFile = true;

	if (wxDirExists(strExist))
		bExistDir = true;

	if (!bExistFile && !bExistDir)
		sResult = sNewName;
	else
		sResult = ChangeName(strExist);

	return sResult;
}

bool CCommonUtil::IsCreatableDirOrFileName(const wxString& strDir)
{
	wxString strDontUse(theMsgManager->GetMessage(wxT("MSG_INFO_RENAME_DONTUSE")));
	strDontUse.Replace(wxT(" "), wxT(""));
	
	wxString strRegEx(wxT("[") + strDontUse + wxT("]"));
	wxRegEx re(strRegEx);
	if(re.Matches(strDir))
	{
		wxMessageBox(wxT("The '") + strDir + wxT("' directory contains characters that cannot be used"), PROGRAM_FULL_NAME, wxICON_ERROR | wxOK);
		return false;
	}
	
	return true;
}