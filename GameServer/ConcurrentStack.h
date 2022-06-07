#pragma once
#include <mutex>

template<typename T>
class LockStack
{
public:
    LockStack() {}
    LockStack(const LockStack&) = delete;
    LockStack& operator=(const LockStack&) = delete;

    void Push(T value)
    {
        lock_guard<mutex> lock(_mutex);
        _stack.push(std::move(value));
        _condVar.notify_one();
    }

    bool TryPop(T& value)
    {
        lock_guard<mutex> lock(_mutex);
        if (_stack.empty())
            return false;

        value = std::move(_stack.top());
        _stack.pop();
        return true;
    }

    void WaitPop(T& value)
    {
        unique_lock<mutex> lock(_mutex);
        _condVar.wait(lock, [this] { return _stack.empty() == false; });
        value = std::move(_stack.top());
        _stack.pop();
    }

private:
    stack<T> _stack;
    mutex _mutex;
    condition_variable _condVar;
};

template<typename T>
class LockFreeStack
{
    struct Node
    {
        Node(const T& value) : data(value)
        {

        }
        T data;
        Node* next;
    };

public:
    // 1) 새 노드를 만들고
    // 2) 새 노드의 next = head;
    // 3) head = 새 노드
    //[value]
    //[][][][][][]
    //[head]
    void Push(const T& value)
    {
        Node* node = new Node(value);
        node->next = _head;
        /*
        if (_head == node->next)
        {
            _head = node;
            return true;
        }
        else
        {
            node->next = _head;
            return false;
        }
        */

        while (_head.compare_exchange_weak(node->next, node) == false)
        { 
            //node->next = _head;
        } //계속 새치기 당하면 대기상태가 계속될수 있음 : live lock

        //이 사이에 새치기 당하면?
        //_head = node;
    }

    // 1) head 읽기
    // 2) head->next 읽기
    // 3) head = nead->next
    // 4) data 추출해서 반환
    // 5) 추출한 노드를 삭제
    //[][][][][][]
    //[head]
    bool TryPop(T& value)
    {
        ++_popCount;
        Node* oldHead = _head; // Head을 뺴오고 조작하는것이 문제...

        /*if (_head == oldHead)
        {
            _head = oldHead->next;
            return true;
        }
        else
        {
            oldHead = _head;
            return false;
        }*/

        while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false)
        {
            //oldHead = _head;
        }

        if (oldHead == nullptr)
            return false;

        value = oldHead->data;
        //잠시 삭제 보류
        //delete oldHead; // 삭제할 때 다른데에서 참조하고있다면? 크래쥐

        //GC가 있으면 사실 여기서 끝
        return true;
    }


private:
    //[][][][][][]
    //[head]
    atomic<Node> _head;
    atomic<uint32> _popCount = 0; // pop을 실행중인 쓰레드 개수
    atomic<Node*> _pendingList; // 삭제 되어야 할 노드들 (첫번째 노드)
};