#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <conio.h>
#include <tchar.h>
#include <string>
#include <iostream> 
#include <fstream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <tchar.h>
#define LINE_SIZE 1024

#define BUF_SIZE 256
TCHAR szName[] = TEXT("Local\\nnnmeow");
HANDLE hMapFile;
char* pBuf;
int* arranswer = new int[9];
std::string fileName = "data.txt";
LPCWSTR fileNameLP = TEXT("data.txt");


const TCHAR szExchangeNumber[] = _T("ChatLine9847589345673246758");
HANDLE hMapFileExchange = NULL;
char* pBufExchange = NULL;
const int CHAT_SIZE = 1024;
std::string strBuf;
WNDCLASS windowClass;
BOOL bRet = false;
std::wstring name(L"Steve Nash");
const wchar_t* message = name.c_str();
UINT msgReg;


int FileMappingSave(std::string str) {

    //создает объект сопоставления файлов с hMapFile, именем объекта
    HANDLE mappingHandle = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,   // read/write access
        FALSE,                 // do not inherit the name
        szName);               // name of mapping object

    //возвращает указатель на представление файла 
    pBuf = (char*)MapViewOfFile(
        mappingHandle,   // handle to map object
        FILE_MAP_ALL_ACCESS, // read/write permission
        0, //dwFileOffsetHigh
        0, //dwFileOffsetLow
        0);

    if (pBuf == NULL)
    {
        _tprintf(TEXT("Could not map view of file (%d).\n"),
            GetLastError());

        CloseHandle(hMapFile);

        return 1;
    }

    //для записи строки в представление, к которому могут обращаться другие процессы.
    std::copy(str.begin(), str.end(), pBuf);

    UnmapViewOfFile(pBuf);
    CloseHandle(mappingHandle);
    CloseHandle(hMapFile);
    DeleteObject(pBuf);

    return 0;
}

int FileMappingRead() {
    bool flag = false;//существует ли файл
   
    if (GetFileAttributes (fileNameLP) == -1) {
        flag = true;
    }
    //создает дескриптор файла
    hMapFile = CreateFile(
        _T("data.txt"), ///тут поменять
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hMapFile == NULL)
    {
        _tprintf(TEXT("Could not create file mapping object (%d).\n"),
            GetLastError());
        return 1;
    }

    //создает объект сопоставления файлов с hMapFile, именем объекта
    hMapFile = CreateFileMapping(
        hMapFile,                       // дескриптор файла
        NULL,               // защита 
        PAGE_READWRITE,                    // атрибуты защиты
        0,            // старшее слово размера
        BUF_SIZE,             // младшее слово размера
        szName                     // имя объекта
    );

    if (hMapFile == NULL)
    {
        _tprintf(TEXT("Could not open file mapping object (%d).\n"),
            GetLastError());
        return 1;
    }

    //получение указателя на представление файла
    pBuf = (char*)MapViewOfFile(
        hMapFile, // handle to map object
        FILE_MAP_ALL_ACCESS,  // read/write permission
        0,
        0,
        BUF_SIZE);

    if ((LPTSTR)pBuf == NULL)
    {
        _tprintf(TEXT("Could not map view of file (%d).\n"),
            GetLastError());

        //CloseHandle(hMapFile);

        return 1;
    }
    if (!flag) {
        //MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);

        std::string str(pBuf);
        //for (int j = 0; j < BUF_SIZE; j++)
        //    str += pBuf[j];
        std::string sep = "\r\n";
        //std::cout << str << std::endl;

        int i = 0;
        for (size_t p = 0, q = 0; p != str.npos && i < 8; p = q) {
            arranswer[i] = stoi(str.substr(p + (p != 0), (q = str.find(sep, p + 1)) - p - (p != 0)));
            i++;
        }

        if (i != 8)
            return 1;
        //MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);
    }
    else
        return 1;
    return 0;
}

int FileFopenSave(std::string str) {
    FILE* file;
    int b = fopen_s(&file, fileName.c_str(), "wb");//проверить все способы создается ли файл если его не существует
    if (b != 0) {
        printf("Cannot open file. ");
        return 1;
    }
    fwrite(str.c_str(), sizeof(char), str.size(), file);
    fclose(file);
    return 0;
}

int FileFopenRead() {
    FILE* file;

    if (fopen_s(&file, fileName.c_str(), "r") == 0) {
        int num1;
        int numbers[10];

        for (int i = 0; i < 9; i++) {
            fscanf_s(file, "%d", &num1);
            arranswer[i] = num1;
        }
        fclose(file);
        return 0;
    }
    return 1;
}

void FileFstreamSave(std::string str) {
    std::ofstream file(fileName.c_str());
    file << str;
    file.close();
}

int FileFstreamRead(std::string str) {
    std::ifstream fileCheck(fileName.c_str());

    if (!fileCheck.is_open()) {
        return 1;
    }
    fileCheck.close();

    std::ifstream file(fileName.c_str());
    for (int i = 0; i < 10; i++) {
        file >> str;
        arranswer[i] = std::stoi(str);
    }
    file.close();
}

int FileWinAPISave(std::string str) {
    DWORD bytesWrite;
    HANDLE fileHandle = CreateFile(fileNameLP, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (fileHandle == INVALID_HANDLE_VALUE) {
        std::cout << "open file error" << std::endl;
        return 1;
    }

    WriteFile(fileHandle, str.c_str(), str.size(), &bytesWrite, NULL);
    std::cout << GetLastError() << std::endl;


    CloseHandle(fileHandle);
    return 0;
}

int FileWinAPIRead() {
    bool flag = false;//существует ли файл
    if (GetFileAttributes(fileNameLP) == INVALID_FILE_ATTRIBUTES) {
        flag = true;
    }

    char buffer[256];
    int number;
    DWORD bytesRead;

    HANDLE fileHandle = CreateFile(fileNameLP, GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (!flag) {

        ReadFile(fileHandle, buffer, sizeof(buffer), &bytesRead, NULL);

        std::string str(buffer);

        std::string sep = "\r\n";
        //std::cout << str << std::endl;

        int i = 0;
        for (size_t p = 0, q = 0; p != str.npos && i < 8; p = q) {
            arranswer[i] = stoi(str.substr(p + (p != 0), (q = str.find(sep, p + 1)) - p - (p != 0)));
            i++;
        }

        if (i != 8)
            return 1;

        CloseHandle(fileHandle);
    }
    else {
        CloseHandle(fileHandle);
        return 1;
    }
    
    return 0;
}

int SaveFile(std::string str, int typeofsave) {

    switch (typeofsave)
    { 
    case 0: //• При помощи отображения файлов на память (CreateFileMapping/MapViewOfFile)
        if (FileMappingSave(str) == 1)
            std::cout << "save error (" << typeofsave << ")" << std::endl;
        break;
    case 1: //При помощи файловых переменных (fopen / fread / fwrite / fclose).
        if (FileFopenSave(str) == 1)
            std::cout << "save error (" << typeofsave << ")" << std::endl;
        break;
    case 2: // При помощи потоков ввода-вывода (fstream).
        FileFopenSave(str);
        break;
    case 3: // При помощи файловых функций WinAPI/NativeAPI.
        if (FileWinAPISave(str) == 1)
            std::cout << "save error (" << typeofsave << ")" << std::endl;
        break;
    default:
        return 0;
    }
    return 0;
}

int* ReadFile(int typeofsave) {

    switch (typeofsave)
    {
    case 0: //• При помощи отображения файлов на память (CreateFileMapping/MapViewOfFile)
        if (FileMappingRead() == 0)
            return arranswer;
        else
            return NULL;
        break;
    case 1: //При помощи файловых переменных (fopen / fread / fwrite / fclose).
        if (FileFopenRead() == 0)
            return arranswer;
        else
            return NULL;
        break;
    case 2: // При помощи потоков ввода-вывода (fstream).
        if (FileFopenRead() == 0)
            return arranswer;
        else
            return NULL;
        break;
    case 3: // При помощи файловых функций WinAPI/NativeAPI.
        if (FileWinAPIRead() == 0)
            return arranswer;
        else
            return NULL;
        break;
    default:
        return 0;
    }
    return 0;
}

void BroadcastMessage()
{
    // Все подключенные окна получат это сообщение
    SendMessage(HWND_BROADCAST, msgReg, 0, (LPARAM)message);
}


int RewiteBuf(std::string str) {
    //puts(pBufExchange);
    std::cout << str << std::endl;
    std::copy(str.begin(), str.end(), pBufExchange);
    BroadcastMessage();
    return ' ';
}

int Init(UINT msgReg1) {
     hMapFileExchange = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        CHAT_SIZE,
        szExchangeNumber
    );

    if (hMapFileExchange == NULL) {
        printf("hMapFileExchange error: %d\n", GetLastError());
        return 1;
    }

    pBufExchange = (char*)MapViewOfFile(
        hMapFileExchange,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        CHAT_SIZE
    );

    if (pBufExchange == NULL) {
        printf("pBufExchange error: %d\n", GetLastError());
        return 1;
    }

    return 0;
}

int CloseBuf() {
    UnmapViewOfFile(pBufExchange);
    CloseHandle(hMapFileExchange);
    UnregisterClass(windowClass.lpszClassName, GetModuleHandle(NULL));
    return 0;
}