#ifndef __SORTING_H__
#define __SORTING_H__

#include "../global/UnCopyable.h"
class CDirData;

class CSorting : private CUnCopyable
{
public:
	explicit CSorting();
	~CSorting();
public:
	static bool DirSortOfName(CDirData& lhs, CDirData& rhs);
	static bool DirSortOfSize(const CDirData& lhs, const CDirData& rhs);
	static bool DirSortOfTime(const CDirData& lhs, const CDirData& rhs);
	static bool DirSortOfType(const CDirData& lhs, const CDirData& rhs);


private:
	static int CmpDir(CDirData& lhs, CDirData& rhs);
	static int CmpName(const wxString& strLeft, const wxString& strRight);
};
#endif