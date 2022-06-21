#pragma once

class SendBufferChunk;

/*----------------
	SendBuffer
-----------------*/

class SendBuffer
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize);
	~SendBuffer();

	BYTE*		Buffer() { return _buffer; }
	uint32		AllocSize() { return _allocSize; }
	uint32		WriteSize() { return _writeSize; }
	void		Close(uint32 writeSize);

private:
	/*chunk에서 메모리 공간*/
	BYTE* _buffer;
	uint32				_allocSize = 0;
	uint32				_writeSize = 0;
	//SendBuffer가 살아있는동안chunk가 사라지면 안되기때문에 ref를 잡고있어야함
	SendBufferChunkRef	_owner; 
};

/*--------------------
	SendBufferChunk
버퍼를 크게 만든다음에 잘라씀
TLS에 할당된 영역이기 때문에 멀티쓰레드 X
chunk를 잘라서 SendBuffer로 만듬
--------------------*/

class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	void				Reset();
	/*사용하려고 할당할 공간 예약*/
	SendBufferRef		Open(uint32 allocSize);
	/*SendBuffer로부터 실제 쓴 양을 받아 사용한 공간 확정*/
	void				Close(uint32 writeSize);

	bool				IsOpen() { return _open; }
	BYTE*				Buffer() { return &_buffer[_usedSize]; }
	uint32				FreeSize() { return static_cast<uint32>(_buffer.size()) - _usedSize; }

private:
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE>		_buffer = {};
	bool									_open = false;
	uint32									_usedSize = 0;
};

/*---------------------
	SendBufferManager
SendBuffer를 Pooling 하기위한 클래스
----------------------*/

class SendBufferManager
{
public:
	/*chunk에서 어느정도 메모리 예약*/
	SendBufferRef		Open(uint32 size);

private:
	SendBufferChunkRef	Pop();
	void				Push(SendBufferChunkRef buffer);

	static void			PushGlobal(SendBufferChunk* buffer);

private:
	USE_LOCK;
	Vector<SendBufferChunkRef> _sendBufferChunks;
};
