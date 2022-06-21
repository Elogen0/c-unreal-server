#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"

ThreadManager*      GThreadManager = nullptr;
Memory*             GMemory = nullptr;
DeadLockProfiler*   GDeadLockProfiler = nullptr;
SendBufferManager* GSendBufferManager;

class CoreGlobal
{
public:
    CoreGlobal()
    {
        GThreadManager = new ThreadManager();
        GMemory = new Memory();
        GSendBufferManager = new SendBufferManager();
        GDeadLockProfiler = new DeadLockProfiler();
        SocketUtils::Init();
    }

    ~CoreGlobal()
    {
        delete GThreadManager;
        delete GMemory;
        delete GSendBufferManager;
        delete GDeadLockProfiler;
        SocketUtils::Clear();
    }
} GCoreGlobal;
