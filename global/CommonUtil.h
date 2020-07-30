#ifndef __COMMON_UTIL_H__
#define __COMMON_UTIL_H__


#include <memory>
class CCommonUtil
{
private:
	explicit CCommonUtil() {};

public:
	~CCommonUtil() {};

#ifdef __WXMSW__
	static int GetWindowsVersion();
#endif

	static CCommonUtil* Get();
	//platform 언어가져오기
	wxString GetSystemLang();

	//실행디렉토리 가져오기
	const wxString& GetWorkDir();
	//색상정보가져오기
	wxColour GetColor(unsigned long &ulColor)
	{
		unsigned long r = GetRedColor(ulColor);
		unsigned long g = GetGreenColor(ulColor);
		unsigned long b = GetBlueColor(ulColor);

		return wxColour(r, g, b);
	}

	//RGB colour 값 구하기
	const unsigned long GetRedColor(unsigned long ulColor)   { return ((ulColor & 0x00FF0000) >> 16); }
	const unsigned long GetGreenColor(unsigned long ulColor) { return ((ulColor & 0x0000FF00) >> 8); }
	const unsigned long GetBlueColor(unsigned long ulColor)  { return (ulColor & 0x000000FF); }

	//이미지리스트 로드
	void LoadImageList();

	//화면 다시그리기
	void RefreshWindow(wxWindow* pWindow, const wxRect& rect = wxRect(0, 0, 0, 0), bool bUpdate = false);
	//파일사이즈 정보가져오기
	void GetSizeInfo(const double dblFileSize, wxString& strFileSize, wxString& strFileSizeType, wxColour& dispColor);
	//Volume
	wxString GetVolume(const wxString& strFullPath);
	//확장자 가져오기
	wxString GetExt(const wxString& strFileName);
	//키명 가져오기
	wxString GetKeyName(const wxKeyEvent& event);
	const char* GetVirtualKeyCodeName(int nKeyCode);
	//IME 설정
#ifdef __WXMSW__
	void SetImeModeToEnglish(wxWindow* pWindow);
	bool IsSameDrive(const wxString& strNewPath, const wxString strOldPath);
#endif
	//Get Parent Directory
	wxString GetParentPath(const wxString& strFullPath);
	/*
		GetPathName()
	*/
	wxString GetPathName(const wxString& strFullPath);
	int GetPathDepth(const wxString& strFullPath);
	int Compare(const wxString& strOld, const wxString& strNew);
	//프로그램 실행
	void LaunchAndExec(const wxString& strExecProgram, const wxString& strPath);
	//아이콘 인덱스
	int GetIconIndex(const wxString& strPath);
	//1000자리 콤마
	wxString SetComma(const wxString& strSize);
	//문자열 ...표시
	wxString GetEllipseString(wxDC* pDC, const wxString& strSource, const int iDispWidth);
	//자르기
	wxVector<wxString> Split(const wxString& strSource, const wxString& strDelimiter);
	void ShowErrorMessage(const wxString& strItem, int iRetCode);
	//파일명 변경
	wxString ChangeName(const wxString& strName);
	wxString GetFileName(const wxString& strFullPath);
	bool IsCreatableDirOrFileName(const wxString& strDir);
	
private:
	static std::unique_ptr<CCommonUtil> m_pInstance;
	wxString strWorkingDirectory = wxT("");

};

#endif
