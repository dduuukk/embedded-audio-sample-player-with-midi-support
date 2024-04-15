#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <array>

template <typename T, std::size_t S>
class circleBuffer
{
public:
	circleBuffer();

	void bufferReset();

	bool checkEmpty();

	bool checkFull();

	void enqueueData(T data);

	T dequeueData();

private:
	std::array<T, S> buffer;
	uint16_t head;
	uint16_t tail;
	uint16_t size;
	bool full;
};

template <typename T, std::size_t S> circleBuffer<T, S>::circleBuffer()
{
	bufferReset();
}

template <typename T, std::size_t S> void circleBuffer<T, S>::bufferReset()
{
	head = 0;
	tail = 0;
	full = false;
}

template <typename T, std::size_t S> bool circleBuffer<T, S>::checkEmpty()
{
	return !full && (head == tail);
}

template <typename T, std::size_t S> bool circleBuffer<T, S>::checkFull()
{
	return full;
}

template <typename T, std::size_t S> void circleBuffer<T, S>::enqueueData(T data)
{
	if (checkFull())
	{
		tail++;

		if (tail == S)
			tail = 0;
	}

	buffer[head] = data;

	head++;

	if (head == S)
		head = 0;

	if (head == tail)
		full = true;
}

template <typename T, std::size_t S> T circleBuffer<T, S>::dequeueData()
{
	T dataOut = buffer[tail];

	tail++;

	if (tail == S)
		tail = 0;

	full = false;

	return dataOut;
}

#endif