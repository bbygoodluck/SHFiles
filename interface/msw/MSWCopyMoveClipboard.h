#ifndef __MSW_COPY_MOVE_CLIP_BOARD_H__
#define __MSW_COPY_MOVE_CLIP_BOARD_H__

#include "../ClipBoard.h"

class CMSWCopyMoveClipboard;
typedef CMSWCopyMoveClipboard CClipboard;

class CMSWCopyMoveClipboard : public CClipBoardBase
{
private:
	CMSWCopyMoveClipboard() {};

public:
	virtual ~CMSWCopyMoveClipboard() {
		Clear();
	};

public:
	virtual void AddData(const wxString& strData) wxOVERRIDE;
	virtual void AddData(const std::list<wxString>& lstDatas) wxOVERRIDE;
	virtual void CopyToClipboard() wxOVERRIDE;
	virtual void GetData(std::list<wxString>& lstDatas) wxOVERRIDE;
	void Clear();

public:
	static CMSWCopyMoveClipboard* Get();

private:
	static std::unique_ptr<CMSWCopyMoveClipboard> m_pInstance;
//	wxArrayString m_aDatas;
	std::list<wxString> m_aDatas;
};
#endif