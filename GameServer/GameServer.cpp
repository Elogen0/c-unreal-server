// GameServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>

#include <mutex>
#include <future>
#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"

LockQueue<int32> q;
LockStack<int32> s;

void Push()
{
    while (true)
    {
        int32 value = rand() % 100;
        q.Push(value);

        this_thread::sleep_for(10ms);
    }
}

void Pop()
{
    while (true)
    {
        int32 data = 0;
        if (q.TryPop(data))
            cout << data << endl;
    }
}
int main()
{
}

