#if !defined(THREAD_WORK_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Janastin Gerald $
   $Notice: (C) Copyright 2018 by Janastin Gerald. All Rights Reserved. $
   ======================================================================== */

#define THREAD_WORK_H

typedef void (*WorkerFuncPointer) (void*);

struct work_data
{
    void *Data;
    WorkerFuncPointer Worker;
    volatile int Completed;
};


#ifdef WIN32_PLATFORM
#include <windows.h>

DWORD WINAPI
Win32ThreadFunction(LPVOID Data)
{
    work_data* WorkData = (work_data*)Data;
    WorkData->Worker(WorkData->Data);
    WorkData->Completed = 1;
    return(0);
}


HANDLE
CreateWindowsWorkerThread(work_data* WorkData)
{
    HANDLE NewThread = CreateThread(0,
                                    0,
                                    Win32ThreadFunction,
                                    WorkData,
                                    0,
                                    0);
    Assert(NewThread);
    return(NewThread);
}

#else
#error "This Platform is not yet supported!!!"
#endif

//NOTE(gerald):Common code goes here!

void
AddWorkerThread(work_data* WorkData)
{
#ifdef WIN32_PLATFORM
    CreateWindowsWorkerThread(WorkData);
#endif    
}

int
GetProcessorCount(void)
{
    int Result = 0;

#ifdef WIN32_PLATFORM
    SYSTEM_INFO Info = {};
    GetSystemInfo(&Info);
    Result = Info.dwNumberOfProcessors;
#endif
    
    return(Result);
}

void
LockedAdd(volatile u32 *Target, u32 Value)
{
#ifdef WIN32_PLATFORM
    InterlockedExchangeAdd(Target, Value);
#endif
}

#endif
