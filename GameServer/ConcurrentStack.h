#pragma once
#include <mutex>

//정확히는 lockfree방식은 아님
//template<typename T>
//class LockStack
//{
//public:
//    LockStack() {}
//    LockStack(const LockStack&) = delete;
//    LockStack& operator=(const LockStack&) = delete;
//
//    void Push(T value)
//    {
//        lock_guard<mutex> lock(_mutex);
//        _stack.push(std::move(value));
//        _condVar.notify_one();
//    }
//
//    bool TryPop(T& value)
//    {
//        lock_guard<mutex> lock(_mutex);
//        if (_stack.empty())
//            return false;
//
//        value = std::move(_stack.top());
//        _stack.pop();
//        return true;
//    }
//
//    void WaitPop(T& value)
//    {
//        unique_lock<mutex> lock(_mutex);
//        _condVar.wait(lock, [this] { return _stack.empty() == false; });
//        value = std::move(_stack.top());
//        _stack.pop();
//    }
//
//private:
//    stack<T> _stack;
//    mutex _mutex;
//    condition_variable _condVar;
//};
//
//template<typename T>
//class LockFreeStack
//{
//    struct Node
//    {
//        Node(const T& value) : data(make_shared<T>(value)), next(nullptr)
//        {
//
//        }
//        shared_ptr<T> data;
//        shared_ptr<Node> next;
//    };
//
//public:
//    void Push(const T& value)
//    {
//        shared_ptr<Node> node = make_shared<Node>(value);
//        node->next = std::atomic_load(&_head);
//
//        while (std::atomic_compare_exchange_weak(&_head, &node->next, node) == false)
//        {
//        }
//    }
//
//    shared_ptr<T> TryPop()
//    {
//        //shared pointer의 동작이 원자적으로 일어나지 않기때문에
//        shared_ptr<Node> oldHead = std::atomic_load(&_head);
//        while (old && std::atomic_compare_exchange_weak(&_head, &oldHead, oldHead->next) == false)
//        {
//
//        }
//
//        if (oldHead == nullptr)
//        {
//            return shared_ptr<T>();
//        }
//        return oldHead->data;
//    }
//
//private:
//    shared_ptr<Node> _head;
//};


template<typename T>
class LockFreeStack
{
    struct Node;

    //포인터와 포인터 참조횟수
    struct CountedNodePtr
    {
        int32 externalCount = 0; // 16
        Node* ptr = nullptr; // 64
        //나중에 ptr의 주소에 Count를 합쳐서 한번에 하도록 수정
    };
    struct Node
    {
        Node(const T& value) : data(make_shared<T>(value)), next(nullptr)
        {

        }
        shared_ptr<T> data;
        atomic<int32> internalCount = 0;
        CountedNodePtr next;
    };

public:
    void Push(const T& value)
    {
        CountedNodePtr node;
        node.ptr = new Node(value);
        node.externalCount = 1;
        node.ptr->next = _head;
        while (_head.compare_exchange_weak(node.ptr->next, node) == false)
        {

        }
    }

    shared_ptr<T> TryPop()
    {
        CountedNodePtr oldHead = _head;
        while (true)
        {
            //참조권 획득 (externalCount를 현 시점 기준 +1한 애가 이김)
            IncreaseHeadCount(oldHead);
            //최소한 externalCount >= 2 일테니 삭테X(안전하게 접근할 수 있는)
            Node* ptr = oldHead.ptr;

            //데이터 없음
            if (ptr == nullptr)
                return shared_ptr<T>();

            //소유권(꺼내서 쓸자) 획득(참조권 여러명 획득 할 수 있음)(ptrnext로 head를 바꿔치기 한 애가 이김
            if (_head.compare_exchange_strong(oldHead, ptr->next))
            {
                shared_ptr<T> res;
                res.swap(ptr->data);
                //external : 1 -> 2(나+1) -> 4(나+1 남+2) 먼저 들어오는애가 사용하겠다는 수
                //internal : 1 -> 0인 애가 delete 뒷수습을 위해

                // 삭제 ~ 나말고 또 누가있는가?
                const int32 countIncrease = oldHead.externalCount - 2;

                if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
                    delete ptr;

                return res;
            }
            else if (ptr->internalCount.fetch_sub(1) == 1)
            {
                //참조권은 얻었으나, 소유권은 실패 -> 뒷수습은 내가 한다.
                delete ptr;
            }
        }
    }

private:
    void IncreaseHeadCount(CountedNodePtr& oldCounter)
    {
        while (true)
        {
            CountedNodePtr newCounter = oldCounter;
            newCounter.externalCount++; //번호가 늘어나는 방향으로만

            if (_head.compare_exchange_strong(oldCounter, newCounter))
            {
                oldCounter.externalCount = newCounter.externalCount;
                break;
            }
        }
    }

private:
    shared_ptr<CountedNodePtr> _head;
};