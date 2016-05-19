#ifndef MYAPI
	#define MYAPI extern "C" __declspec(dllimport)
#endif

MYAPI int WordsCount(const wchar_t* text);