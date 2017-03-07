#include <iostream>
#include <fstream>
#include "ArrayQueue.h"
#include "PriorityQueue.h"


using namespace std;

#define MAX_ARRIVALS 99999

void processArrival(Arrival* arr, PriorityQueue* eventQueue, ArrayQueue* bankLine, bool& tellerAvailable);
void processDeparture(Departure* dep, PriorityQueue* eventQueue, ArrayQueue* bankLine, bool& tellerAvailable);

int main()
{
	ArrayQueue bankLine(MAX_ARRIVALS);
	PriorityQueue eventQueue;

	bool tellerAvailable = true;

	ifstream dataFile;
	dataFile.open("data.txt");

	int a, t;
	Arrival* temp;

	//Create and add arrival events to event queue
	for (int i = 0; i < MAX_ARRIVALS; i++)
	{
		dataFile >> a;			//Copy arrival time into a
		dataFile >> t;			//Copy transaction length into t

		temp = new Arrival(a,t);	//Create new arrival event

		eventQueue.enqueue(temp, a);	//Add to eventQueue, passing arrival time as priority
	}

	temp = NULL;

	//Pointers to manage events
	Event* newEvent;
	Arrival* newArrival;
	Departure* newDeparture;

	int currentTime;

	//Event Loop
	while ( !eventQueue.isEmpty() )
	{
		newEvent = eventQueue.peekFront();

		//If newEvent is an arrival then getType() will return true
		if (newEvent->getType() == true)
		{
			newArrival = static_cast<Arrival*> (newEvent);
			processArrival(newArrival, &eventQueue, &bankLine, tellerAvailable);
		}
		//Otherwise newEvent is a departure
		else
		{
			newDeparture = static_cast<Departure*> (newEvent);
			processDeparture(newDeparture, &eventQueue, &bankLine, tellerAvailable);
		}
	
	}

	
	return 0;
}

void processArrival(Arrival* arr, PriorityQueue* eventQueue, ArrayQueue* bankLine, bool& tellerAvailable)
{
	//Remove arrival event from the priority queue
	eventQueue->dequeue();

	int currentTime = arr->getArrivalTime();
	int transactionTime = arr->getTransactionLength();
	int departureTime;
	
	//If bankLine is empty and there is an available teller then customer goes straight to teller
	if(bankLine->isEmpty() && tellerAvailable)
	{
		departureTime = currentTime + transactionTime;
		Departure* newDeparture = new Departure(departureTime);
		eventQueue->enqueue(newDeparture, departureTime);

		tellerAvailable = false;
		newDeparture = NULL;
	}
	//Otherwise customer waits in line
	else
	{
		bankLine->enqueue(arr);
	}
}


void processDeparture(Departure* dep, PriorityQueue* eventQueue, ArrayQueue* bankLine, bool& tellerAvailable)
{
	//Remove departure from priority queue
	eventQueue->dequeue();
	
	
	//If bank line is not empty 
	if ( !bankLine->isEmpty() )
	{
		Event* temp = bankLine->peekFront();
		Arrival* nextCustomer = static_cast<Arrival*> (temp);
		bankLine->dequeue();
		temp = NULL;

		int currentTime = dep->getDepartureTime();
		int transactionTime = nextCustomer->getTransactionLength();
		int departureTime = currentTime + transactionTime;

		//Create new departure event and add it to eventQueue
		Departure* newDeparture = new Departure(departureTime);
		eventQueue->enqueue(newDeparture, departureTime);	
	}
	else
	{
		tellerAvailable = true;
	}
}