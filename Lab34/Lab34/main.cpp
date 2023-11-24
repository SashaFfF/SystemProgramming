#include <iostream>
#include <Windows.h>
#include <cmath>
#include <string>
#include "main.h"


DWORD WINAPI CalculateSin(LPVOID param) {
    double* input = (double*)param;

    WaitForSingleObject(mutex, INFINITE); // Захватываем мьютекс для синуса
    sinResult = sin(*input);
    std::cout << "(Поток для sin) sin(" << *input << ") = " << sinResult << std::endl;
    ReleaseMutex(mutex); // Освобождаем мьютекс
    return 0;
}

DWORD WINAPI CalculateCos(LPVOID param) {
    double* input = (double*)param;

    WaitForSingleObject(mutex, INFINITE); 
    cosResult = cos(*input);
    std::cout << "(Поток для cos) cos(" << *input << ") = " << cosResult << std::endl;
    ReleaseMutex(mutex); 
    return 0;
}

DWORD WINAPI CalculateTan(LPVOID param) {
    double* input = (double*)param;

    WaitForSingleObject(mutex, INFINITE);
    tanResult = tan(*input);
    std::cout << "(Поток для tg)  tg(" << *input << ") = " << tanResult << std::endl;
    ReleaseMutex(mutex); 
    return 0;
}

DWORD WINAPI CalculateCtg(LPVOID param) {
    double* input = (double*)param;

    WaitForSingleObject(mutex, INFINITE); 
    ctgResult = 1.0 / tan(*input);
    std::cout << "(Поток для ctg) ctg(" << *input << ") = " << ctgResult << std::endl;
    ReleaseMutex(mutex);
    return 0;
}

// Lab3 
void InitMappingFile() {
    hFile = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		std::cerr << "IniteMappingFile - CreateFile не работает, fname = "
			<< fileName << std::endl;
		return;
	}

	hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, fileSize, NULL);
	if (hMapping == NULL) {
		std::cerr << "IniteMappingFile - CreateFileMapping не работает, fname = "
			<< fileName << std::endl;
		CloseHandle(hFile);
		return;
	}

    pMappedData = MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, fileSize);
	if (pMappedData == NULL) {
		std::cerr << "fileMappingCreate - MapViewOfFile не работает, fname = "
			<< fileName << std::endl;
		CloseHandle(hMapping);
		CloseHandle(hFile);
		return;
	}
}

void UninitializeMappingFile() {
    if (pMappedData != NULL) {
        UnmapViewOfFile(pMappedData);
        pMappedData = NULL;
    }
    if (hMapping != NULL) {
        CloseHandle(hMapping);
        hMapping = NULL;
    }
    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }
}

void SaveData(std::string str, int size) {
    if (pMappedData != NULL) {
        size_t dataSize = size * sizeof(char);
        if (mappedDataSize + dataSize >= fileSize)
        {
            UninitializeMappingFile();
            InitMappingFile();
            mappedDataSize = 0;
        }
        memcpy((CHAR*)pMappedData + mappedDataSize, str.c_str(), dataSize);
        mappedDataSize += dataSize;
    }
}

int main() {
    double input_value = 0.5;

    mutex = CreateMutex(NULL, FALSE, NULL);
    if (mutex == NULL) {
        std::cerr << "Ошибка при создании мьютекса" << std::endl;
        return 1;
    }

    // отдельные потоки для каждой функции
    HANDLE sinThread = CreateThread(NULL, 0, CalculateSin, &input_value, 0, NULL);
    HANDLE cosThread = CreateThread(NULL, 0, CalculateCos, &input_value, 0, NULL);
    HANDLE tanThread = CreateThread(NULL, 0, CalculateTan, &input_value, 0, NULL);
    HANDLE ctgThread = CreateThread(NULL, 0, CalculateCtg, &input_value, 0, NULL);

    if (sinThread == NULL || cosThread == NULL || tanThread == NULL || ctgThread == NULL) {
        std::cerr << "Ошибка при создании потоков" << std::endl;
        return 1;
    }

    // ожидается завершения всех потоков
    WaitForSingleObject(sinThread, INFINITE);
    WaitForSingleObject(cosThread, INFINITE);
    WaitForSingleObject(tanThread, INFINITE);
    WaitForSingleObject(ctgThread, INFINITE);    

    // Закрываем дескрипторы мьтекса и потоков
    CloseHandle(mutex);
    CloseHandle(sinThread);
    CloseHandle(cosThread);
    CloseHandle(tanThread);
    CloseHandle(ctgThread);

    std::string result = "sin(" + std::to_string(input_value) + ") = " + std::to_string(sinResult) + "\n" +
                         "cos(" + std::to_string(input_value) + ") = " + std::to_string(cosResult) + "\n" +
                         "tg(" + std::to_string(input_value) + ") = " + std::to_string(tanResult) + "\n" + 
                         "ctg(" + std::to_string(input_value) + ") = " + std::to_string(ctgResult) + "\n";

    //std::cout << result;

    //Lab3
    InitMappingFile();
    SaveData(result, result.size());
    UninitializeMappingFile();

    return 0;
}
