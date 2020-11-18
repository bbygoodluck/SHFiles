#ifndef __SKIP_KEY_MAP_H__
#define __SKIP_KEY_MAP_H__

class CSkipKeyMap final
{
private:
	CSkipKeyMap() {
		InitSkipKeyMap();
	};

public:
	~CSkipKeyMap() {
		m_SkipKeyMap.clear();
	};

public:
	static CSkipKeyMap* Get();
	bool IsExistSkipKey(int iKeyCode) 
	{
		std::unordered_map<int, int>::const_iterator fIter = m_SkipKeyMap.find(iKeyCode);
		if (fIter != m_SkipKeyMap.end())
			return true;

		return false;
	}

private:
	void InitSkipKeyMap();
	
private:
	static std::unique_ptr<CSkipKeyMap> m_pInstance;
	std::unordered_map<int, int> m_SkipKeyMap;
};

#endif