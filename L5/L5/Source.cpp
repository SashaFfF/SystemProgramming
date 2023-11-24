#include <windows.h>
#include <string>
#include <wininet.h>
#include "Wininet.h"
#include <Netlistmgr.h>



// Глобальные переменные
HWND hwnd;
HKEY hRegKey;
COLORREF backgroundColor;
LANGID langId;

// Определение констант для мониторинга изменений в реестре
#define REG_KEY_PATH L"SOFTWARE\\MyBrightnessApp"
#define WM_REGISTRY_CHANGE (WM_USER + 1)

void Update();

int get_lang() {
    langId = LOWORD(GetKeyboardLayout(0));
    COLORREF backgroundColor;

    if (langId == 0x0409) {  // en
        return 1;
    }
    else if (langId == 0x0419) {  // ru
        return 2;
    }
    else {
        return 3;
    }
}

int GetCurrentKeyboardLayout()
{
    HKL keyboardLayout = GetKeyboardLayout(0);
    LCID localeId = LOWORD(keyboardLayout);
    LANGID langId = MAKELANGID(PRIMARYLANGID(localeId), SUBLANGID(localeId));

    if (langId == LANG_RUSSIAN)
    {
        return 1;
    }
    else if (langId == LANG_ENGLISH)
    {
        return 2;
    }
    else if (langId == MAKELANGID(LANG_BELARUSIAN, SUBLANG_DEFAULT))
    {
        return 3;
    }
    else
    {
        return 0;
    }
}

// Функция для записи цвета в реестр
void WriteColorToRegistry(int brightnessLevel, COLORREF color) {
    HKEY hSubKey;
    WCHAR subkey[64];
    swprintf_s(subkey, L"Level%d", brightnessLevel);

    if (RegCreateKeyEx(hRegKey, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSubKey, NULL) == ERROR_SUCCESS) {
        RegSetValueEx(hSubKey, L"Color", 0, REG_DWORD, (LPBYTE)&color, sizeof(COLORREF));
        RegCloseKey(hSubKey);
    }
}

// Функция для создания ключей с цветами в реестре
void CreateColorKeysInRegistry() {
    for (int i = 0; i < 10; i++) {
        COLORREF color = RGB(i * 25, i * 25, i * 25);
        WriteColorToRegistry(i, color);
    }
}

//KEY_NOTIFY - Это флаг, указывающий, что мы хотим открыть ключ для мониторинга изменений в реестре
void OpenRegistryKeyForMonitoring() {
    LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REG_KEY_PATH, 0, KEY_NOTIFY, &hRegKey);
    if (result != ERROR_SUCCESS) {
        MessageBox(NULL, L"Не удалось открыть ключ Реестра", L"Ошибка", MB_ICONERROR);
    }
}

COLORREF ReadColorFromRegistry(int brightnessLevel) {
    HKEY hSubKey;
    WCHAR subkey_name[64];
    swprintf_s(subkey_name, L"Level%d", brightnessLevel);

    //KEY_QUERY_VALUE чтение значений 
    if (RegOpenKeyEx(hRegKey, subkey_name, 0, KEY_QUERY_VALUE, &hSubKey) == ERROR_SUCCESS) {
        DWORD valueType;
        COLORREF color = RGB(255, 255, 255);
        DWORD dataSize = sizeof(COLORREF);

        if (RegQueryValueEx(hSubKey, L"Color", NULL, &valueType, (LPBYTE)&color, &dataSize) == ERROR_SUCCESS) {
            RegCloseKey(hSubKey);
            return color;
        }

        RegCloseKey(hSubKey);
    }

    return RGB(255, 255, 255);
}

void UpdateWindowColor(COLORREF color) {
    backgroundColor = color;

    // Установка цвета фона окна  GCLP_HBRBACKGROUND - Это флаг, который указывает на свойство класса окна, отвечающее за цвет фона.
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG)CreateSolidBrush(backgroundColor));
    RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
}

// Функция для мониторинга изменений в ключе Реестра
void MonitorRegistryChanges() {
    RegNotifyChangeKeyValue(hRegKey, TRUE, REG_NOTIFY_CHANGE_LAST_SET, hwnd, TRUE);
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE: {
        SetTimer(hwnd, 1, 1000, NULL);
        Update();
        break;
    }
    case WM_TIMER:
        Update();
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void Update() {
    int i = get_lang();
    UpdateWindowColor(ReadColorFromRegistry(i));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"BrightnessColorApp";
    RegisterClass(&wc);

    hwnd = CreateWindow(L"BrightnessColorApp", L"Монитор изменения цвета", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 600, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, L"Не удалось создать окно", L"Ошибка", MB_ICONERROR);
        return 1;
    }
    ShowWindow(hwnd, nCmdShow);

    OpenRegistryKeyForMonitoring();


    Update();


    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    RegCloseKey(hRegKey);

    return 0;
}