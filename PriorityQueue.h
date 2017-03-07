#include <iostream>
#include "Event.h"

using namespace std;

class PriorityQueue;

class Node{
	private:
		Node(Event*, int, Node*);
		Event* data;
		int priority;
		Node* next;
		friend class PriorityQueue;
};

class PriorityQueue{
	public:
		PriorityQueue(int = 0);
  		~PriorityQueue();
		bool enqueue(Event*, int);
		bool dequeue();
		Event* peekFront();
		bool isEmpty() const;
	private:
		Node* front;
};


Node :: Node(Event* newEntry, int p, Node* nd)
{
	data = newEntry;
	priority = p;
	next = nd;
}

PriorityQueue :: PriorityQueue(int size)
{
	front = NULL;
}


PriorityQueue :: ~PriorityQueue()
{
 	if ( !isEmpty() )
	{
		Node* current;
		Node* previous;
	
		previous = front;
		current = previous -> next;
		
		delete previous;
		while (current != NULL)
		{
			previous = current;
			current = previous -> next;
			delete previous;
		}
	
		front = NULL;
	}
}



bool PriorityQueue :: enqueue(Event* newEntry, int pri)
{
	Node* temp;
	Node* index;
	temp = new Node(newEntry, pri, NULL);
		
	//If queue is empty or if new node is higher priority than front, insert new node at front
	if ( (front == NULL) || (pri < front->priority) )
	{
		temp->next = front;
		front = temp;
	}
	else
	{
		index = front;
			
		//Find correct position to insert new node
		while (index->next != NULL && index->next->priority <= pri)
		{
			index = index->next;
		}

		//Update links to add new node to queue
		temp->next = index->next;
		index->next = temp;
	}

	temp = NULL;
	index = NULL;
	
	return true;	
}

bool PriorityQueue :: dequeue()
{
	bool result = false;
	if ( !isEmpty() )
	{
		Node* temp = front;
		front = front->next;

		//delete node
		delete temp;
		temp = NULL;

		result = true;
	}
	
	return result;

}

Event* PriorityQueue :: peekFront()
{
	return front->data;
}

bool PriorityQueue :: isEmpty() const
{
	if (front == NULL)
		return true;
	else
		return false;

}



