#pragma once
class RefCountable
{
public:
    RefCountable() : _refCount(1) 
    {
    }
    virtual ~RefCountable() {}
    int32 GetRefCount() { return _refCount; }
    int32 AddRef() { return ++_refCount; }
    int32 ReleaseRef()
    {
        int32 refCount = --refCount;
        if (refCount == 0)
        {
            delete this;
        }
        return refCount;
    }

protected:
    int32 _refCount;
};

/*-----------
    ShardPtr
-------------*/

template<typename T>
class TSharedPtr
{
public:

private:
    inline void Set(T* ptr)
    {
        _ptr = ptr;
    }

private:
    T* _ptr = nullptr;
};