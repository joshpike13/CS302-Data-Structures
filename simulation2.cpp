#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <string>
#include "ArrayQueue.h"
#include "PriorityQueue.h"

//Global output file
ofstream outputFile;

using namespace std;

#define MAX_ARRIVALS 99999
#define ARR_SIZE 99999

void simulate(string fileName);
void processArrival(Arrival* arr, PriorityQueue* eventQueue, ArrayQueue* bankLine, bool& tellerAvailable);
void processDeparture(Departure* dep, PriorityQueue* eventQueue, ArrayQueue* bankLine, bool& tellerAvailable);
void generate_events(string fileName);
void counting_sort(int arr[], int size);
int calculate_idle(bool tellerCurrent, bool tellerPrevious, int currentTime, int& start, int& stop);
void average(int pTime, double avgW, int maxW, int avgL, int maxL, int idle);

int main()
{
	//Array to hold file names for 10 randomly generated sets of data
	string fileNames[] = {"data1.txt", "data2.txt", "data3.txt", "data4.txt", "data5.txt", "data6.txt", "data7.txt", "data8.txt", "data9.txt", "data10.txt"};

	clock_t start;
	double simulation_time;
	double cumulative_time = 0;
	double average_time;

	//Opening global output file
	outputFile.open("output.txt");

	//Generate 10 sets of random data
	for (int i = 0; i < 10; i++)
	{
		generate_events(fileNames[i]);
	}
	
	for (int i = 0; i < 10; i++)
	{
		outputFile << "			Simulation " << i + 1 << ":" << endl;
		//Run simulation and calculate simulation time
		start = clock();
		simulate(fileNames[i]);
		simulation_time = (clock() - start) / (double) CLOCKS_PER_SEC;

		//Add simulation time to cumulative total
		cumulative_time = cumulative_time + simulation_time;

		outputFile << "		Total CPU Time = " << simulation_time << endl;				
	}
	//Calculate average simulation CPU time
	average_time = cumulative_time / 10;
	
	
	
	return 0;	
}







void simulate(string fileName)
{
	ArrayQueue bankLine(MAX_ARRIVALS);		//Bank Line implemented with array based queue
	PriorityQueue eventQueue;			//Event queue implemented with link based priority queue

	//Variables for keeping track of stats
	int processing_time;
	int wait;
	int cumulative_wait = 0;
	double average_wait;
	int max_wait = 0;
	int line;
	int cumulative_line = 0;
	int max_line = 0;
	int average_line;
	int idle_start;
	int idle_stop;
	int idle_time;
	bool teller_previous;
	 
	

	bool tellerAvailable = true;			

	//Open data file
	ifstream dataFile;
	dataFile.open(fileName.c_str());


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
	Event* nextEvent;
	Arrival* nextArrival;
	Departure* nextDeparture;

	int currentTime;

	//Event Loop
	while ( !eventQueue.isEmpty() )
	{
		teller_previous = tellerAvailable;						//Keeps track of last state of tellerAvailable
			
		
		nextEvent = eventQueue.peekFront();
		if (nextEvent->getType() == true)						//getType() returns true if nextEvent points to an arrival
		{
			nextArrival = static_cast<Arrival*> (nextEvent);			//Cast base class pointer into arrival pointer
			currentTime = nextArrival->getArrivalTime();				//Update timer

			processArrival(nextArrival, &eventQueue, &bankLine, tellerAvailable);
		}
		else										//Otherwise newEvent is a departure
		{			
			nextDeparture = static_cast<Departure*> (nextEvent);			//Cast base class pointer into arrival departure 
			Arrival* link = nextDeparture->getLinkedArrival();			//Get address of arrival event corresponding to nextDeparture 
			currentTime = nextDeparture->getDepartureTime();			//Update timer
			
			
			wait = currentTime - (link->getTransactionLength()) - (link->getArrivalTime());			//wait time = d - t - a
			cumulative_wait += wait;									//Update cumulative wait time
	
			if ( wait > max_wait )										//Update max_wait if current wait is longer
				max_wait = wait;

			processDeparture(nextDeparture, &eventQueue, &bankLine, tellerAvailable);
		}

		
		line = bankLine->getCount();				//Get size of bankLine
		cumulative_line += line;				//Update cumulative total
		
		if ( line > max_line )					//Update max_line if current line is greater
			max_line = line;

		idle_time = idle_time + calculate_idle(tellerAvailable, teller_previous, currentTime, idle_start, idle_stop);	//Keeps track of idle time for teller
	
	}
	
	processing_time = currentTime;					//Processing time = time of final event in simulation
	avg_wait = (double) cumulative_weight / MAX_ARRIVALS;		//Calculate average wait time
	avg_line = cumulative_line / (MAX_ARRIVALS * 2);		//Calculate average length of line 

	//Write results to output file
	outputFile << "Total Teller Idle Time = " << idle_time << endl;
	outputFile << "Average Waiting Time = " << avg_wait << "	Max Waiting Time = " << max_wait << endl;
	outputFile << "Average Line Length = " << avg_line << "		Max Line Length = " << max_line << endl;
	outputFile << "Total Processing Time = " << processing_time;
	
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
		Departure* newDeparture = new Departure(departureTime, arr);
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
	
	int currentTime = dep->getDepartureTime();
	
	//If bank line is not empty 
	if ( !bankLine->isEmpty() )
	{
		Event* temp = bankLine->peekFront();
		Arrival* nextCustomer = static_cast<Arrival*> (temp);
		bankLine->dequeue();
		temp = NULL;

		int transactionTime = nextCustomer->getTransactionLength();
		int departureTime = currentTime + transactionTime;

		//Create new departure event and add it to eventQueue
		Departure* newDeparture = new Departure(departureTime, nextCustomer);
		eventQueue->enqueue(newDeparture, departureTime);	
	}
	else
	{
		tellerAvailable = true;
	}
}

void generate_events(string fileName)
{
	//Arrays to hold arrival time and transaction lengths
	int* arrivalTimes= new int[ARR_SIZE];
	int* transactionLengths= new int[ARR_SIZE];

	//Seed random nubmer generator
	srand(time(0));
	
	//Generating random vals
	for (int i = 0; i < ARR_SIZE; i++)
	{
		//Generate 99,999 random arrival times between 0 and 100,000 (inclusive)
		arrivalTimes[i] = rand() % 100001;

		//Generate 99,999 random transaction lengths between 1 and 100 (inclusive)
		transactionLengths[i] = (rand() % 100) + 1;
	}

	//Sort arrivalTimes array
	counting_sort(arrivalTimes, ARR_SIZE);

	//Data will be written to file "data.txt"
	ofstream data_file;
	data_file.open(fileName.c_str());


	//this loop writes randomly generated data to the file
	//Format: ArrivalTime - 5 spaces - transaction time
	for (int i = 0; i < ARR_SIZE; i++)
	{
		data_file << arrivalTimes[i] << "     " << transactionLengths[i] << endl;	
	}
	
	//Delete arrays
	delete[] arrivalTimes;
	delete[] transactionLengths;
}

void counting_sort(int arr[], int size)
{
	//Array to track frequency of each number - vals range from 0 to 100,000 (inclusive)
	int* count = new int[100001];

	//Sorted array to be returned 
	int* sorted_arr = new int[size];

	//Counting frequencies of each arr element
	for (int i = 0; i < size; i++)
	{
		count[arr[i]]++;
	}

	//Determining cumulative frequencies
	for (int i = 1; i < 100001; i++)
	{
		count[i] = count[i] + count[i-1];
	}

	//Placing values into the correct position based on the cumulative frequency array
	for (int i = size - 1; i > 0; i--)
	{
		sorted_arr[count[arr[i]]] = arr[i];
		count[arr[i]]--;
	}

	//Copying sorted array into original array 
	copy(sorted_arr, sorted_arr + size, arr);		
	
	delete[] sorted_arr;
	delete[] count;
}

int calculate_idle(bool tellerCurrent, bool tellerPrevious, int currentTime, int& start, int& stop)
{
	if ((tellerCurrent == true) && (tellerPrevious == false))	//Case 1: Teller is available and wasn't before -> start counting idle time
	{
		start = currentTime;
		return 0;
	}
	else if ((tellerCurrent == false) && (tellerPrevious == true))	//Case 2: Teller isn't available and was before -> stop counting 
	{
		stop = currentTime;			
		int val = stop - start;	//Calc elapsed time
		
		return val;	
	}
		
}

void average(int pTime, double avgW, int maxW, int avgL, int maxL, int idle)
{

	static int processing_time;
	static double avg_wait;
	static int max_wait;
	static int avg_line;
	static int max_line;
	static int idle_time;

	processing_time += pTime;
	avg_wait += avgW;
	max_wait += 
}
