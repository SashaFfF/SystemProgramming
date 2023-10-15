#pragma once

#define OnMenuClicked		1
#define OnExitClicked		2
#define OnButtonClicked		3
#define OnReadField			4
#define OnSaveFile			5
#define OnLoadFile			6
#define OnNewWindow			7

#define indexColorR			8
#define indexColorG			9
#define indexColorB			10
#define OnChangeTextColor		11
#define OnChangeBackgroundColor	12

#define OnChangeFont		13

#define TextBufferSize		256


char Buffer[TextBufferSize];
int countOfReadChars;

HWND hEditControl, hActiveWindow;
HHOOK hKeyboardHook;

HWND hStaticControl;
COLORREF textColor = RGB(0,0,0);
COLORREF backgroundColor = RGB(255,255,255);



PAINTSTRUCT ps;
HFONT font;

char path[200];

OPENFILENAMEA file;

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BdColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure);


HFONT hFont = CreateFont(18, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
	CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("Courier New"));

void AddMenu(HWND hWnd);
void AddWidgets(HWND hWnd);
void SaveData(LPCSTR path);
void LoadData(LPCSTR path);
void SetOpenFileParams(HWND hWnd);
