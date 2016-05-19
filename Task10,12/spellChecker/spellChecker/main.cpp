#include <Windows.h>
#include <set>
#include <string>
#include <fstream>

void readDictionary(std::set<std::wstring>& dict) 
{
	dict.insert(L"first");
	dict.insert(L"second");
	dict.insert(L"third");
}

void leaveDictionaryWords(wchar_t* buffer, std::set<std::wstring>& dict) 
{
	unsigned int i;
	std::wstring word;
	std::wstring text;
	for (i = 0; buffer[i]; i++) {
		if( iswalpha(buffer[i]) || iswdigit(buffer[i]) ) {
			word += buffer[i];
		} else {
			if( word.size() > 0 ) {
				if (dict.count(word) > 0) {
					text += word;
				}
				word.clear();
			}
			text += buffer[i];
		}
	}
	if (word.size() > 0 && dict.count(word) > 0) {
		text += word;
	}
	std::fill_n(buffer, i * sizeof(wchar_t), 0);
	memcpy(buffer, text.c_str(), (text.size() + 1) * sizeof(wchar_t));
}

void proceedText(std::set<std::wstring>& dict) 
{
	wchar_t fileName[40];
	swprintf_s(fileName, L"Local\\Text%d", GetCurrentProcessId());
	HANDLE file = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, fileName);
	wchar_t* buffer = static_cast<wchar_t*>(MapViewOfFile(file, FILE_MAP_ALL_ACCESS, 0, 0, 0));
	leaveDictionaryWords(buffer, dict);
	CloseHandle(file);
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPWSTR commandLine, int cmdShow) 
{
	wchar_t eventName[40];
	swprintf_s(eventName, L"Global\\NewText%d", GetCurrentProcessId());
	HANDLE newTextEvent = CreateEvent(0, FALSE, FALSE, eventName);
	swprintf_s(eventName, L"Global\\TerminateProcesses", GetCurrentProcessId());
	HANDLE terminateProcessEvent = CreateEvent(0, TRUE, FALSE, eventName);
	HANDLE eventsInitialized = OpenEvent(EVENT_ALL_ACCESS, false, L"Global\\EventsInitialized");
	SetEvent(eventsInitialized);
	std::set<std::wstring> dictionary;
	readDictionary(dictionary); 
	bool terminated = false;
	HANDLE events[] = { newTextEvent, terminateProcessEvent };
	HANDLE textProceededEvent;
	while( !terminated ) {
		switch (WaitForMultipleObjects(2, events, false, INFINITE)) {
			case WAIT_OBJECT_0: 
				proceedText(dictionary);
				swprintf_s(eventName, L"Global\\TextProceeded%d", GetCurrentProcessId());
				textProceededEvent = OpenEvent(EVENT_ALL_ACCESS, false, eventName);
				SetEvent(textProceededEvent);
				CloseHandle(textProceededEvent);
				break;
			case WAIT_OBJECT_0 + 1:
				terminated = true;
				break;
		}
	}
	CloseHandle(newTextEvent);
	CloseHandle(terminateProcessEvent);
}