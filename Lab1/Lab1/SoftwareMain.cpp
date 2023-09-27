#include <Windows.h>
#include <string>
#include "SoftwareDefinitions.h"

//точка входа в наше приложение
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {
	WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL,IDI_QUESTION), L"MainWndClass", SoftwareMainProcedure);

	if (!RegisterClassW(&SoftwareMainClass)) {
		return -1;
	}
	MSG SoftwareMainMessage = { 0 };
	CreateWindow(L"MainWndClass", L"Main window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 800, 500, NULL, NULL, NULL, NULL);
	
	while (GetMessage(&SoftwareMainMessage, NULL, NULL, NULL)) {
		TranslateMessage(&SoftwareMainMessage);
		DispatchMessage(&SoftwareMainMessage);
	}
	return 0;
}

WNDCLASS NewWindowClass(HBRUSH BdColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure) {
	WNDCLASS WindowClass = { 0 };

	WindowClass.hbrBackground = BdColor;
	WindowClass.hCursor = Cursor;
	WindowClass.hInstance = hInst;
	WindowClass.hIcon = Icon;
	WindowClass.lpszClassName = Name;
	WindowClass.lpfnWndProc = Procedure;

	return WindowClass;
}

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{
	case WM_COMMAND:
		switch (wp)
		{
		case OnButtonClicked:
			SetWindowTextA(hEditControl, "");
			break;
		case OnReadField:
			countOfReadChars = GetWindowTextA(hEditControl, Buffer, TextBufferSize);
			SetWindowTextA(hStaticControl, Buffer);
			SetWindowTextA(hStaticControl, std::to_string(countOfReadChars).c_str());
			break;
		case OnSaveFile:
			if (GetSaveFileNameA(&file)) {
				SaveData(path);
			};
			break;
		case OnLoadFile:
			if (GetOpenFileNameA(&file)) {
				LoadData(path);
			};
			break;
		case OnExitClicked:
			PostQuitMessage(0);
			break;
		default:
			break;
		}
		break;
	case WM_CREATE:
		AddMenu(hWnd);
		AddWidgets(hWnd);
		SetOpenFileParams(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:return DefWindowProc(hWnd, msg, wp, lp);
	}

}

void AddMenu(HWND hWnd) {
	HMENU RootMenu = CreateMenu();
	HMENU SubMenu = CreateMenu();

	AppendMenu(SubMenu, MF_STRING, OnSaveFile, L"Save");
	AppendMenu(SubMenu, MF_STRING, OnLoadFile, L"Load");
	//разделитель меню,используем флаг separator 
	AppendMenu(SubMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(SubMenu, MF_STRING, OnExitClicked, L"Exit");


	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"File");
	AppendMenu(RootMenu, MF_STRING, (UINT_PTR)SubMenu, L"Help");

	//hwnd- окно, на которое устанавливается меню
	SetMenu(hWnd, RootMenu);
}

void AddWidgets(HWND hWnd) {
	hStaticControl = CreateWindowA("static", "Hello:) ", WS_VISIBLE | WS_CHILD | ES_CENTER, 5, 5, 490, 20, hWnd, NULL, NULL, NULL);
	hEditControl = CreateWindowA("edit", "Edit control", ES_MULTILINE| WS_VSCROLL | WS_VISIBLE | WS_CHILD , 5, 30, 780, 200, hWnd, NULL, NULL, NULL);
	CreateWindowA("button", "Clear", WS_VISIBLE | WS_CHILD , 5, 240, 120, 30, hWnd, (HMENU)OnButtonClicked, NULL, NULL);
	CreateWindowA("button", "Read", WS_VISIBLE | WS_CHILD, 140, 240, 120, 30, hWnd, (HMENU)OnReadField, NULL, NULL);
}

void SaveData(LPCSTR path) {
	HANDLE file = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	int length = GetWindowTextLength(hEditControl)+1;
	char* data = new char[length];
	length = GetWindowTextA(hEditControl, data, length);
	DWORD bytesIterated; //кол-во считанных символов
	WriteFile(file, data, length, &bytesIterated, NULL);
	CloseHandle(file);
	delete[] data;
}

void LoadData(LPCSTR path) {
	HANDLE file = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD bytesIterated; //кол-во считанных символов

	ReadFile(file, Buffer, TextBufferSize, &bytesIterated, NULL);
	SetWindowTextA(hEditControl, Buffer);
	CloseHandle(file);
}

void SetOpenFileParams(HWND hWnd) {
	ZeroMemory(&file, sizeof(file));
	file.lStructSize = sizeof(file);
	file.hwndOwner = hWnd;
	file.lpstrFile = path;
	file.nMaxFile = sizeof(path);
	file.lpstrFilter = ".txt";
	file.lpstrFileTitle = NULL;
	file.nMaxFileTitle = 0;
	file.lpstrInitialDir = "D:\\5 sem\\SystemProgramming\\Lab1\\example.txt";
	file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}