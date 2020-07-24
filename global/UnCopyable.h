#ifndef __UNCOPYABLE_H__
#define __UNCOPYABLE_H__

class CUnCopyable
{
protected:
	CUnCopyable()  { }
	~CUnCopyable() { }

private:
	CUnCopyable(const CUnCopyable&);
	CUnCopyable& operator=(const CUnCopyable&);
};

#endif