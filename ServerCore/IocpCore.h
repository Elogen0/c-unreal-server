#pragma once

/*------------------------
        IocpObject
--------------------------*/
class IocpCore
{
public:
    virtual HANDLE Gethandle() abstract;
    virtual void Dispatch(class iocpEvent* iocpEvent, int32 numObBytes = 0) abstract;
};

/*------------------------
        IocpObject
--------------------------*/

class IocpCore
{
public:
    IocpCore();
    ~IocpCore();

    HANDLE GetHandle() {return _iocp}

    bool Register(IocpObjectRef iocpObject);
    bool Dispatch(uint32 timeoutMs = INFINITE);
private:
    HANDLE _iocpHandle;
};

