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

#include "PlayerManager.h"
#include "AccountManager.h"

int main()
{
    GThreadManager->Launch([=] 
        {
            while (true)
            {
                cout << "PlayerThenAccount" << endl;
                GPlayerManager.PlayerThenAccount();
                this_thread::sleep_for(100ms);
            }
        });

    GThreadManager->Launch([=] 
        {
            while (true)
            {
                cout << "AccountThenPlayer" << endl;
                GAccountManager.AccountThenPlayer();
                this_thread::sleep_for(100ms);
            }
        });

    GThreadManager->Join();
}

