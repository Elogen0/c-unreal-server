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

struct Session
{
    WSAOVERLAPPED overlapped = {};
    SOCKET socket = INVALID_SOCKET;
    char recvBuffer[1000] = {};
    int32 recvBytes = 0;
    int32 sendBytes = 0;
};

int main()
{
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    SOCKET listenSocket;
    
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    sockaddr_in InetAddr;
    ::memset(&InetAddr, 0, sizeof(InetAddr));
    InetAddr.sin_family = AF_INET;
    InetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    InetAddr.sin_port = htons(27015);

    if (::bind(listenSocket, (SOCKADDR*)&InetAddr, sizeof(InetAddr)) == SOCKET_ERROR)
        return 0;

    if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
        return 0;

    while (true)
    {
        SOCKADDR_IN clientAddr;
        int32 addrLen = sizeof(clientAddr);

        SOCKET clientSocket;
        while (true)
        {
            clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
            if (clientSocket != INVALID_SOCKET)
                break;

            if (::WSAGetLastError() == WSAEWOULDBLOCK)
                continue;

            return 0;
        }

        Session session = Session{ clientSocket };
        WSAEVENT wsaEvent = ::WSACreateEvent();
        session.overlapped.hEvent = wsaEvent;

        while (true)
        {
            WSABUF wsaBuf; 
            wsaBuf.buf = session.recvBuffer; //wsaBuf는 날려도 되지만 recvBuffers는 절대 건드리면 안됨
            wsaBuf.len = 1000;
            
            DWORD recvLen = 0;
            DWORD flags = 0;
            if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, nullptr))
            {
                if (::WSAGetLastError() == WSA_IO_PENDING)
                {
                    ::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
                    ::WSAGetOverlappedResult(session.socket, &session.overlapped, &recvLen, FALSE, &flags);
                }
                else
                {
                    //문제있는 상황
                    break;
                }
            }
        }

        ::closesocket(session.socket);
        ::WSACloseEvent(wsaEvent);
    }

    ::WSACleanup();
}

