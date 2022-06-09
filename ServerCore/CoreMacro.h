#pragma once

#define OUT

/*----------------------
        Lock
------------------------*/
//##은 숫자가 들어감
#define USE_MANY_LOCKS(count)   Lock _locks[count];
#define USE_LOCK                USE_MANY_LOCKS(1)
#define READ_LOCK_IDX(idx)      ReadLockGuard readLockGuard_##idx(_locks[idx]);
#define READ_LOCK               READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)     WriteLockGuard readLockGuard_##idx(_locks[idx]);
#define WRITE_LOCK              WRITE_LOCK_IDX(0)
/*----------------------
        Crash
------------------------*/
#define CRASH(cause)                        \
{                                           \
    uint32* crash = nullptr;                \
    __analysis_assume(crash != nullptr);    \
    *crash = 0xDEADBEEF;                    \
}

#define ASSERT_CRASH(expr)                  \
{                                           \
    if (!(expr))                            \
    {                                       \
        CRASH("ASSERT_CRASH");              \
        __analysis_assume(expr);            \
    }                                       \
}