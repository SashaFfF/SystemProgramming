#include <iostream>
#include <Windows.h>
#include <cmath>
#include <string>
#include "main.h"

//////////////////////// Lab4 ///////////////////////////

DWORD WINAPI CalculateSin(LPVOID param) {
    double* input = (double*)param;

    WaitForSingleObject(sinMutex, INFINITE); // Захватываем мьютекс для синуса
    sinResult = sin(*input);
    ReleaseMutex(sinMutex); // Освобождаем мьютекс
    return 0;
}

DWORD WINAPI CalculateCos(LPVOID param) {
    double* input = (double*)param;

    WaitForSingleObject(cosMutex, INFINITE); 
    cosResult = cos(*input);
    ReleaseMutex(cosMutex); 
    return 0;
}

DWORD WINAPI CalculateTan(LPVOID param) {
    double* input = (double*)param;

    WaitForSingleObject(tanMutex, INFINITE);
    tanResult = tan(*input);
    ReleaseMutex(tanMutex); 
    return 0;
}

DWORD WINAPI CalculateCtg(LPVOID param) {
    double* input = (double*)param;

    WaitForSingleObject(ctgMutex, INFINITE); 
    ctgResult = 1.0 / tan(*input);
    ReleaseMutex(ctgMutex);
    return 0;
}

//////////////////////// Lab3 ///////////////////////////
void InitMappingFile() {
    hFile = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		std::cerr << "IniteMappingFile - CreateFile failed, fname = "
			<< fileName << std::endl;
		return;
	}

	hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, fileSize, NULL);
	if (hMapping == NULL) {
		std::cerr << "IniteMappingFile - CreateFileMapping failed, fname = "
			<< fileName << std::endl;
		CloseHandle(hFile);
		return;
	}

    pMappedData = MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, fileSize);
	if (pMappedData == NULL) {
		std::cerr << "fileMappingCreate - MapViewOfFile failed, fname = "
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

    // Создаем мьютексы для синхронизации доступа к результатам
    sinMutex = CreateMutex(NULL, FALSE, NULL);
    cosMutex = CreateMutex(NULL, FALSE, NULL);
    tanMutex = CreateMutex(NULL, FALSE, NULL);
    ctgMutex = CreateMutex(NULL, FALSE, NULL);

    if (sinMutex == NULL || cosMutex == NULL || tanMutex == NULL || ctgMutex == NULL) {
        std::cerr << "Ошибка при создании мьютексов!" << std::endl;
        return 1;
    }

    // Создаем отдельные потоки для каждой функции
    HANDLE sinThread = CreateThread(NULL, 0, CalculateSin, &input_value, 0, NULL);
    HANDLE cosThread = CreateThread(NULL, 0, CalculateCos, &input_value, 0, NULL);
    HANDLE tanThread = CreateThread(NULL, 0, CalculateTan, &input_value, 0, NULL);
    HANDLE ctgThread = CreateThread(NULL, 0, CalculateCtg, &input_value, 0, NULL);

    if (sinThread == NULL || cosThread == NULL || tanThread == NULL || ctgThread == NULL) {
        std::cerr << "Ошибка при создании потоков!" << std::endl;
        return 1;
    }

    // Ожидаем завершения всех потоков
    WaitForSingleObject(sinThread, INFINITE);
    WaitForSingleObject(cosThread, INFINITE);
    WaitForSingleObject(tanThread, INFINITE);
    WaitForSingleObject(ctgThread, INFINITE);

    // Закрываем и удаляем мьютексы
    CloseHandle(sinMutex);
    CloseHandle(cosMutex);
    CloseHandle(tanMutex);
    CloseHandle(ctgMutex);

    // Закрываем дескрипторы потоков
    CloseHandle(sinThread);
    CloseHandle(cosThread);
    CloseHandle(tanThread);
    CloseHandle(ctgThread);

    std::string result = "sin(" + std::to_string(input_value) + ") = " + std::to_string(sinResult) + "\n" +
                         "cos(" + std::to_string(input_value) + ") = " + std::to_string(cosResult) + "\n" +
                         "tg(" + std::to_string(input_value) + ") = " + std::to_string(tanResult) + "\n" + 
                         "ctg(" + std::to_string(input_value) + ") = " + std::to_string(ctgResult) + "\n";

    std::cout << result;

    InitMappingFile();
    SaveData(result, result.size());
    UninitializeMappingFile();

    return 0;
}
