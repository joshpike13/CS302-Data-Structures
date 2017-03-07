#include <iostream>
#include "Event.h"

using namespace std;

class ArrayQueue {

	public:
		ArrayQueue(int size);
		~ArrayQueue();
		bool enqueue(Event* newEntry);
		bool dequeue();
		bool isEmpty() const;
		bool isFull() const;
		int getCount();
		Event* peekFront();

	private:
		int max;
		int front;
		int rear;
		int count;
		Event** data;	
};


ArrayQueue :: ArrayQueue(int size)
{
	max = size;
	front = 0; 
	rear = max - 1;
	count = 0;
	data = new Event*[max];
}


ArrayQueue :: ~ArrayQueue()
{
	delete[] data;
}


bool ArrayQueue :: enqueue(Event* newEntry)
{
	bool result = false;
	if ( !isFull() )	
	{
		rear = (rear + 1) % max;
		data[rear] = newEntry;
		count++;
		result = true;
	}
	
	return result;
}

bool ArrayQueue :: dequeue()
{
	bool result = false;
	if ( !isEmpty() )
	{
		front = (front + 1) % max;
		count--;
		result = true;
	}
	
	return result;		
}

bool ArrayQueue :: isEmpty() const
{
	return (count == 0);
}


bool ArrayQueue :: isFull() const
{
	return (count == max);
}

int ArrayQueue :: getCount()
{
	return count;
}

Event* ArrayQueue :: peekFront()
{
	if( !isEmpty() )
	{
		return data[front];
	}
	else
	{
		return NULL;
	}
}
