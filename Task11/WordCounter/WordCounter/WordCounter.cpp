#define MYAPI extern "C" __declspec(dllexport)
#include "WordCounter.h"
#include <ctype.h>

int WordsCount(const wchar_t* text) 
{
	unsigned int i;
	unsigned int counter = 0;
	unsigned int wordLength = 0;
	for( i = 0; text[i]; i++ ) {
		if( iswalpha(text[i]) || iswdigit(text[i]) ) {
			wordLength++;
		} else {
			if( wordLength > 0 ) {
				counter++;
				wordLength = 0;
			}
		}
	}
	if( wordLength > 0 ) {
		counter++;
	}
	return counter;
}