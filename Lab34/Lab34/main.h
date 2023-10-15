#pragma once

double sinResult = 0.0;
double cosResult = 0.0;
double tanResult = 0.0;
double ctgResult = 0.0;
HANDLE sinMutex, cosMutex, tanMutex, ctgMutex; // Мьютексы для синхронизации доступа к результатам

const WCHAR* fileName = L"result.txt";
const int fileSize = 1000;
HANDLE hFile = NULL;
HANDLE hMapping = NULL;
LPVOID pMappedData = NULL;
SIZE_T mappedDataSize = 0;