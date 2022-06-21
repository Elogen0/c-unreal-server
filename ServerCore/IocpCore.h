#pragma once

/*------------------------
    IocpObject
IOCP에 등록 할 오브젝트는 이 클래스를 상속하여 사용
IocpCore로부터 IocpEvent를 통해 실행된다.
--------------------------*/
class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
    virtual HANDLE  GetHandle() abstract;
    /* IocpCore Dispatch에서 실행시켜 준다. */
    virtual void    Dispatch(class IocpEvent* iocpEvent, int32 numObBytes = 0) abstract;
};

/*------------------------
        IocpCore
--------------------------*/

class IocpCore
{
public:
    IocpCore();
    ~IocpCore();

    HANDLE      GetHandle() { return _iocpHandle; }

    /*iocp 관찰 요청*/
    bool        Register(IocpObjectRef iocpObject);
    /*I/O 완료될 경우 실행*/
    bool        Dispatch(uint32 timeoutMs = INFINITE);
private:
    HANDLE      _iocpHandle;
};

