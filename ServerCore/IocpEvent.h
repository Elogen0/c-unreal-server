#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	//PreRecv, //0 Byte recv라는 고급기법에 활용
	Recv,
	Send
};

/*-------------------------------------------------------------------
	IocpEvent
이것을 상속한 클래스는 OVERLAPPED가 0번째 메모리에 세팅되서
IOCP사용시 OVERLAPPED로 형변환하여 넘겨줄수 있다.
넘어온 Overlaped type이 무엇인지 구분할 있도록 type정보와
자신을 생성한 owner의 정보를 가지고 있다.
주의) 가상함수를 사용하면 offset 0번에 가상함수 테이블이 세팅돼서
OVERLAPPED로 형변환 넘겨줄 수 없으니 사용하면 안된다.
----------------------------------------------------------------------*/
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void			Init();

public:
	EventType		eventType;
	IocpObjectRef	owner;
};

/*----------------
	ConnectEvent
-----------------*/
class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) { }
};

/*--------------------
	DisconnectEvent
----------------------*/
class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) { }
};

/*-------------------------------------------------
	AcceptEvent
Accept의 경우엔 추가로 SOCKET을 뱉어주기 때문에
Event를 던져 줄 때 같이 포함되야함. (session에 담아서 넘겨줌)
----------------------------------------------------*/
class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) { }

public:
	/*Listener에서 Dispatch실행시 어떤 세션을 넘겨줬는지 알아야 함*/
	SessionRef	session = nullptr;
};

/*----------------
	RecvEvent
-----------------*/

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) { }
};

/*----------------
	SendEvent
-----------------*/
class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) { }

	Vector<SendBufferRef> sendBuffers;
};