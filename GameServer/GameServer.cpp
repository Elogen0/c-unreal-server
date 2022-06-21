// GameServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <future>
#include "ThreadManager.h"

#include "Service.h"
#include "Listener.h"

class GameSession : public Session
{

};

int main()
{
    ServerServiceRef service = MakeShared<ServerService>(
        NetAddress(L"127.0.0.1", 7777),
        MakeShared<IocpCore>(),
        MakeShared<GameSession>,
        100);

    ASSERT_CRASH(service->Start());
    for (int32 i = 0; i < 5; ++i)
    {
        GThreadManager->Launch([=]()
            {
                while (true)
                {
                    service->GetIocpCore()->Dispatch();
                }
            });
    }
    GThreadManager->Join();
}

