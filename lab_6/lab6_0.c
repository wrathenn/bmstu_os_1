#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
 
#define W_SLEEP_MS 1000
#define R_SLEEP_MS 1500
 
#define WRITERS_CNT 3
#define READERS_CNT 5
 
#define ITERATIONS 4
 
HANDLE mutex, canRead, canWrite;
 
HANDLE writersThreads[WRITERS_CNT];
HANDLE readersThreads[READERS_CNT];
 
bool workingWriter = false;
 
LONG waitingWriters = 0;
LONG waitingReaders = 0;
LONG activeReaders = 0;
 
int value = 0;
 
void startWrite()
{
    InterlockedIncrement(&waitingWriters);
 
    if (workingWriter || activeReaders > 0)
        WaitForSingleObject(canWrite, INFINITE);
 
    InterlockedDecrement(&waitingWriters);
    workingWriter = true;
}
 
void stopWrite()
{
    workingWriter = false;
 
    if (waitingReaders > 0)
        SetEvent(canRead);
    else
        SetEvent(canWrite);
}
 
DWORD WINAPI writer(LPVOID lpParams)
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        startWrite();
 
        value++;
        printf("<<---Writer[ID = %d]: write value %d\n", lpParams, value);
 
        stopWrite();
        Sleep(W_SLEEP_MS);
    }
 
    return 0;
}
 
void startRead()
{
    WaitForSingleObject(mutex, INFINITE);
 
    InterlockedIncrement(&waitingReaders);
 
    if (workingWriter || waitingWriters > 0)
        WaitForSingleObject(canRead, INFINITE);
 
    InterlockedDecrement(&waitingReaders);
    InterlockedIncrement(&activeReaders);
    SetEvent(canRead);
 
    ReleaseMutex(mutex);
}
 
void stopRead()
{
    InterlockedDecrement(&activeReaders);
 
    if (activeReaders == 0)
        SetEvent(canWrite);
}
 
DWORD WINAPI reader(LPVOID lpParams)
{
    while (value < WRITERS_CNT * ITERATIONS)
    {
        startRead();
 
        printf("--->>Reader[ID = %d]: read value %d\n", lpParams, value);
 
        stopRead();
        Sleep(R_SLEEP_MS);
    }
 
    return 0;
}
 
int main()
{
    if ((mutex = CreateMutex(NULL, FALSE, NULL)) == NULL)
    {
        perror("Mutex error");
        exit(-1);
    }
 
    if ((canRead = CreateEvent(NULL, FALSE, TRUE, NULL)) == NULL)
    {
        perror("CanRead event error");
        exit(-1);
    }
 
    if ((canWrite = CreateEvent(NULL, FALSE, TRUE, NULL)) == NULL)
    {
        perror("CanWrite event error");
        exit(-1);
    }
 
    for (int i = 0; i < WRITERS_CNT; i++)
        if ((writersThreads[i] = CreateThread(NULL, 0, writer, i, 0, NULL)) == NULL)
        {
            perror("Writer thread creation error");
            exit(-1);
        }
 
    for (int i = 0; i < READERS_CNT; i++)
        if ((readersThreads[i] = CreateThread(NULL, 0, reader, i, 0, NULL)) == NULL)
        {
            perror("Reader thread creation error");
            exit(-1);
        }
 
    WaitForMultipleObjects(WRITERS_CNT, writersThreads, TRUE, INFINITE);
    WaitForMultipleObjects(READERS_CNT, readersThreads, TRUE, INFINITE);
 
    CloseHandle(mutex);
    CloseHandle(canRead);
    CloseHandle(canWrite);
 
    return 0;
}