#pragma once

#define OnMenuClicked		1
#define OnExitClicked		2
#define OnButtonClicked		3
#define OnReadField			4
#define OnSaveFile			5
#define OnLoadFile			6
#define OnNewWindow			7
#define TextBufferSize		256


char Buffer[TextBufferSize];
int countOfReadChars;

HWND hEditControl;
HWND hStaticControl;

char path[200];

OPENFILENAMEA file;

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BdColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure);

void AddMenu(HWND hWnd);
void AddWidgets(HWND hWnd);
void SaveData(LPCSTR path);
void LoadData(LPCSTR path);
void SetOpenFileParams(HWND hWnd);
