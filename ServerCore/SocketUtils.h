#pragma once
#include "NetAddress.h"

/*----------------
	SocketUtils
각종 Socket 세팅에 대한 작동을 간편하게 하기위한
WrapperClass
-----------------*/

class SocketUtils
{
public:
	/*함수 포인터 역할. Socket 재사용을 위해 Network관련 Ex함수들의 주소값을
	BindWindowsFunction함수에서 받아옴*/
	static LPFN_CONNECTEX		ConnectEx;
	static LPFN_DISCONNECTEX	DisconnectEx;
	static LPFN_ACCEPTEX		AcceptEx;

public:
	static void		Init();
	static void		Clear();

	/*런타임에 쓸 함수포인터를 얻어오는 함수*/
	static bool		BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);
	/*소켓 만들기*/
	static SOCKET	CreateSocket();

	/*SET SOCKET OPTION*/
	static bool		SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	static bool		SetReuseAddress(SOCKET socket, bool flag);
	static bool		SetRecvBufferSize(SOCKET socket, int32 size);
	static bool		SetSendBufferSize(SOCKET socket, int32 size);
	static bool		SetTcpNoDelay(SOCKET socket, bool flag);
	static bool		SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);

	/*Wrapper 함수*/
	static bool		Bind(SOCKET socket, NetAddress netAddr);
	static bool		BindAnyAddress(SOCKET socket, uint16 port);
	static bool		Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static void		Close(SOCKET& socket);
};

/*SET SOCKETOPTION WRAPPER*/
template<typename T>
static inline bool SetSockOpt(SOCKET socket, int32 level, int32 optName, T optVal)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}