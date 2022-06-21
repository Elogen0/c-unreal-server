#pragma once

/*------------------------------------------------
	NetAddress
주소를 저장해놓고 편하게 추출하기 위한 클래스
--------------------------------------------------*/
class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sockAddr);
	NetAddress(wstring ip, uint16 port);

	SOCKADDR_IN& GetSockAddr() { return _sockAddr; }
	wstring			GetIpAddress();
	uint16			GetPort() { return ::ntohs(_sockAddr.sin_port); }

public:
	/* ip주소를 가지고 IN_ADDR을 얻기*/
	static IN_ADDR	Ip2Address(const WCHAR* ip);

private:
	SOCKADDR_IN		_sockAddr = {};
};

