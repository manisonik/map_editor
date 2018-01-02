#pragma once

template<class T> class CMSKCmd
{
private:
	typedef std::variant<
		void (T::*)(void),
		void (T::*)(UINT_PTR),
		void (T::*)(LONG_PTR),
		void (T::*)(UINT_PTR, LONG_PTR),
		void (T::*)(UINT_PTR, WORD, WORD),
		void (T::*)(WORD, WORD, LONG_PTR),
		void (T::*)(WORD, WORD, WORD, WORD),
		LRESULT(T::*)(void),
		LRESULT(T::*)(UINT_PTR),
		LRESULT(T::*)(LONG_PTR),
		LRESULT(T::*)(UINT_PTR, LONG_PTR),
		LRESULT(T::*)(UINT_PTR, WORD, WORD),
		LRESULT(T::*)(WORD, WORD, LONG_PTR),
		LRESULT(T::*)(WORD, WORD, WORD, WORD),
		void (T::*)(UINT, int, int),
		int (T::*)(LPCREATESTRUCT),
		bool (T::*)(HDC),
		bool (T::*)(int, NMHDR, LRESULT*),
		void (T::*)(int, LPDRAWITEMSTRUCT),
		void (T::*)(BOOL, LPARAM),
		void (T::*)(HRGN)
	> MSGCB;

public:
	CMSKCmd();
	~CMSKCmd();

	void Bind(MSGCB msgCb);
	void OnTest();
};

