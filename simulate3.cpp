/**@file PA05.cpp
 *@brief This program implements the bank simulation using 1 of 2 scenarios; either 1 Line with n tellers, or n lines with 1 teller per line
 * 
 *This program uses an Array based queue to manage the bank line, and uses a link based priority queue to manage the simulation event queue
 *
 *@author Josh Pike
 */

#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <string>
#include "ArrayQueue.h"
#include "PriorityQueue.h"

/** @struct Stats
 *  @brief This structure holds all of the data to be collected from the simulation to allow for easy passing between functions
 *  @var Stats::CPU_time
 *  Member CPU_time holds the amount of processor time it takes to run 1 simulation
 *  @var Stats::process_time 
 *  Member process_time holds the amount of virtual time processed in the simulation
 *  @var Stats::avg_wait
 *  Member avg_wait holds the average wait time for 1 simulation
 *  @var Stats::avg_line 
 *  Member avg_line holds the average length of the line during the simulation
 *  @var Stats::max_wait
 *  Member max_wait keeps track of the longest time any 1 customer had to wait during simulation
 *  @var Stats::max_length
 *  Member max_length keeps track of the longest the line becomes at any point during the simulation
 *  @var Stats::idle_time
 *  Member idle_time keeps track of the total idle time spent by tellers
 *  
 */
struct Stats {
	double CPU_time;
	int process_time;
	double avg_wait;
	int avg_length;
	int max_wait;
	int max_length;
	int idle_time;	

	void initialize()
	{
		CPU_time = 0;
		process_time = 0;
		avg_wait = 0;
		avg_length = 0;
		max_wait = 0;
		max_length = 0;
		idle_time = 0;	
	}
};


using namespace std;

#define MAX_ARRIVALS 99999



//Simulation Functions
/**@brief General simulator function that handles Stats data before calling either simulateA or simulateB
 *
 *@details Runs 10 simulations of either type A or B depending on what value user selects. Keeps track of average stats from all simulations, and writes those results to
 *an output file 
 *
 *@param n User selected int value that will determine how many tellers or queues to use
 *@param val Boolean value. True if user wants to run simulationA. False if user wants to run simulationB
 * 
 *@return void
 */
void sim(int n, bool val);

/**@brief Simulates bank when there is 1 Queue and n tellers 
 *
 *@details Simulates bank with specified number of tellers and 1 line, and calculates the desired statistics about the simulation
 *
 *@param n User selected int value that determines how many total tellers the bank will have
 *@param file String containing the name of the data file to be used to run the simulation
 *@param simData Pointer to a Stats struct, so data gathered from simulation can be displayed in sim() function
 * 
 *@return void
 */
void simulateA(int n, string file, Stats* simData);

/**@brief Simulates bank when there are n Queue and 1 teller per queue
 *
 *@details Simulates bank with specified number of queues and 1 teller for each queue, and calculates desired statistics about the simulation
 *
 *@param n User selected int value that determines how many total queues the bank will have
 *@param file String containing the name of the data file to be used to run the simulation
 *@param simData Pointer to a Stats struct, so data gathered from simulation can be displayed in sim() function
 * 
 *@return void
 */
void simulateB(int n, string file, Stats* simData);

//Simulation Helper Functions
/**@brief Processes an arrival event for simulateA function
 *
 *@details Determines if an arrival can be immediately processed (ie there is an open line and teller) and if not places arrival event into the line
 *
 *@param arr Pointer to the arrival event being processed
 *@param eventQueue Pointer to priority event queue in simulateA that is keeping track of all events
 *@param bankLine Pointer to array queue that is representing the line in the bank
 *@param tellerArr Pointer to array of boolean values that represent the availability of each teller
 *@param n User selected integer value that determine number of total tellers 
 *@return void
 */
void process_ArrivalA(Arrival* arr, PriorityQueue* eventQueue, ArrayQueue* bankLine, bool* tellerArr, int n);

/**@brief Processes a departure event for simulateA function
 *
 *@details Removes a departure event from the priority queue, and then creates a new departure event if bankLine is not empty. If bankline is empty then one teller is set to
 *available.
 *
 *@param dep Pointer to the departure event being processed
 *@param eventQueue Pointer to priority event queue in simulateA that is keeping track of all events
 *@param bankLine Pointer to array queue that is representing the line in the bank
 *@param tellerArr Pointer to array of boolean values that represent the availability of each teller
 *@param n User selected integer value that determine number of total tellers 
 *@return void
 */
void process_DepartureA(Departure* dep, PriorityQueue* eventQueue, ArrayQueue* bankLine, bool* tellerArr, int n);

/**@brief Processes an arrival event for simulateB function
 *
 *@details Finds the shortest line out of all the queues, and then determines if arrival can be immediately processd (if queue is empty). If not arrival is added to that
 *queue, and the index of that queue is stored in the arrival object
 *@param arr Pointer to the arrival event being processed
 *@param eventQueue Pointer to priority event queue in simulateA that is keeping track of all events
 *@param bankLines Pointer to array of ArrayQueue pointers, which represent each separate line at the bank
 *@param tellerArr Pointer to array of boolean values that represent the availability of each teller
 *@param n User selected integer value that determine number of total tellers 
 *@return void
 */
void process_ArrivalB(Arrival* arr, PriorityQueue* eventQueue, ArrayQueue** bankLines, bool* tellerArr, int n);

/**@brief Processes a departure event for simulateB function
 *
 *@details Removes a departure event from the priority queue, and then creates a new departure event if corresponding line is not empty. If line is empty, then the teller
 *corresponding to that line is set to true
 *
 *@param dep Pointer to the departure event being processed
 *@param eventQueue Pointer to priority event queue in simulateA that is keeping track of all events
 *@param bankLines Pointer to array of ArrayQueue pointers, which represent each separate line at the bank
 *@param tellerArr Pointer to array of boolean values that represent the availability of each teller
 *@param n User selected integer value that determine number of total tellers 
 *@return void
 */
void process_DepartureB(Departure* dep, PriorityQueue* eventQueue, ArrayQueue** bankLines, bool* tellerArr, int n);

/**@brief Determines if there is an available teller
 *
 *@details Loops through arr of boolean values to see if there is an available teller and returns true if there is one. Also sets index parameter equal to the index where
 *that teller is at so calling function knows what index to use
 *
 *@param arr Pointer to array of boolean values, which represents availability of tellers
 *@param n User selected n value which determines how many tellers there are
 *@param index reference to integer value so index of available teller can be returned also
 *@return bool Returns true if there is an available teller and false otherwise
 */
bool available_teller(bool* arr, int n, int& index);

/**@brief Sets a teller to available
 *@details finds a teller that is currently unavailable and sets its to true after simulation determines that a new teller should be available
 *
 *@parm arr Pointer to array of boolean values which represnet the availability of tellers
 *@param n User selected value which determines how many tellers there are
 *@return bool Returns true if there is an entry that is able to be set to true
 */
bool set_available(bool* arr, int n);

/**@brief Computes the average of 10 simulation runs
 *@details Sums all the data from each simulation, then calculates the average and stores in avg struct
 *
 *@param simData Pointer to array of stats structs, which hold the data for each individaul simulation run
 *@param avg reference to Stats struct which will be updated with average values of simData
 * 
 *@return void
 */
void compute_averages(Stats* simData, Stats& avg);

/**@brief Calculates the idle time for a teller
 *
 *@details determines if a teller has been available for an entire event and determines the time difference
 * 
 *@param tellerCurrent value of teller at current cycle
 *@param tellerPrevious value of teller at previous cycle
 *@param start reference variable to start of idle time
 *@param stop reference variable to when idle time stops
 *
 *@return int returns idle time for the teller, if there was any
 */
int calculate_idle(bool tellerCurrent, bool tellerPrevious, int currentTime, int& start, int& stop);

/**@brief Finds the shortest line out of all the total lines for the bank simulation
 * 
 *@details used for simulateB when there is more than one queue. Loops through all queues and determines which one is the shortest, and then returns its index
 * 
 *@param bankLines Pointer to array of ArrayQueue pointers
 *@param n number of queues in array
 *@return int returns the index of the shortest line in the array
 */
int shortest_line(ArrayQueue** bankLines, int n);

/**@brief Calculates the average length of all the lines after each event loop for simulateB
 *@details sums the lengths of all the lines and finds their average
 *
 *@param bankLines Pointer to array of ArrayQueue pointers
 *@param n number of queues in array
 *@return int returns the average length of all the queues
 */
int avg_line(ArrayQueue** bankLines, int n);

//Data Generating Functions

/**@brief Generates 99,999 random events and writes them into a file
 *@details uses random number generator to generate random arrival times and then uses counting sort to sort them. Then generates random transaction times for each event
 *
 *@param fileName string holding the name of the data file to written into
 */
void generate_events(string fileName);

/**@brief Sorts an array of integers by counting the frequency of each element in the array and using this information to place each value into the correct array index
 * 
 *@details This function implements the O(n+K) counting sort algorithm which sorts an array of integers without making any comparisons between the elements of the array.
 *First the function counts the occurrence of each number 0 - 999,999. This information is used to determine the cumulative frequency of each number 0-999,999, so each 
 *array index I indicates how many numbers less than or equal to I there are in the list of integers being sorted. This information is then used to place each integer in
 *the list into the correct index in the sorted array
 * 
 *@param arr[] Array of integers being sorted
 *@param size Size of array being sorted
 *@return void
 */ 
void counting_sort(int arr[], int size);



int main()
{
	cout << "Bank Simulation Options:" << endl;
	cout << "a - One Queue with n Tellers		b - n Queues with 1 Teller per Queue" << endl;
	cout << "Please enter the letter that corresponds with the desired option: " << endl;
	
	char c;
	int n;
	cin.clear();
	fflush(stdin); 
	cin >> c;
	cin.clear();
	fflush(stdin); 

	while ( (c != 'a') && (c != 'b') )
	{
		cout << "Invalid Input - Please enter either a or b: ";
		cin >> c;
		cin.clear();
		fflush(stdin);
	}

	switch (c)
	{
		case 'a':
			cout << "Running simulation with 1 Queue and n tellers... Please enter integer value for n: ";
			cin >> n;
			cin.clear();
			fflush(stdin);

			sim(n, true);
			break;

		case 'b':
			cout << "Running simulation with n Queues and 1 Teller per Queue... Please enter integer value for n: ";
			cin >> n;
			cin.clear();
			fflush(stdin);

			sim(n, false);
			break;
	}

	return 0;	
}

void sim(int n, bool val)
{
	Stats averages;
	Stats* simData = new Stats[10];
	averages.initialize();


	ofstream outputFile;
	outputFile.open("output.txt");

	string fileNames[] = {"data1.txt", "data2.txt", "data3.txt", "data4.txt", "data5.txt", "data6.txt", "data7.txt", "data8.txt", "data9.txt", "data10.txt"};
	cout << "Generating and sorting 10 different data files to test on simulation..." << endl << endl;

	//generates 10 data files 	
	for (int i = 0; i < 10; i++)					
	{
		generate_events(fileNames[i]);		
	}
	
	clock_t start;
	double simulation_time;

	if (val == true)	
	{	
		for (int i = 0; i < 10; i++)
		{
			cout << "Running simulation #" << i+1 << endl;
			string file = fileNames[i];
			simData[i].initialize();
			start = clock();
			simulateA(n, file, &simData[i]);
			simulation_time = (clock() - start) / (double) CLOCKS_PER_SEC;
			simData[i].CPU_time = simulation_time;
		}
	}
	else
	{
		for (int i = 0; i < 10; i++)
		{
			cout << "Running simulation #" << i+1 << endl;
			string file = fileNames[i];
			simData[i].initialize();
			start = clock();
			simulateB(n, file, &simData[i]);
			simulation_time = (clock() - start) / (double) CLOCKS_PER_SEC;
			simData[i].CPU_time = simulation_time;
		}
	}

	compute_averages(simData, averages);

	//Write stats from each simulation to output file
	for (int i = 0; i < 10; i++)
	{
		outputFile << "Simulation #" << i + 1 << endl;
		outputFile << "CPU Time = " << simData[i].CPU_time << "		Process Time = " << simData[i].process_time << endl;
		outputFile << "Average Waiting Time = " << simData[i].avg_wait << "	Max Waiting Time = " << simData[i].max_wait << endl;
		outputFile << "Average Line Length = " << simData[i].avg_length << "		Max Line Length = " << simData[i].max_length << endl;
		outputFile << "Total Teller Idle Time = " << simData[i].idle_time << endl << endl;;

	}

	//Write average stats to output file
	outputFile << "Averages of all 10 Simulations:" << endl;
	outputFile << "Average CPU Time = " << averages.CPU_time << "		Average Process Time = " << averages.process_time << endl;
	outputFile << "Average Waiting Time = " << averages.avg_wait << "	Max Waiting Time = " << averages.max_wait << endl;
	outputFile << "Average Line Length = " << averages.avg_length << "		Max Line Length = " << averages.max_length << endl;
	outputFile << "Average Total Teller Idle Time = " << averages.idle_time << endl;

	cout << "End simulation" << endl;

	delete[] simData;
}

void simulateA(int n, string file, Stats* simData)
{
	ArrayQueue bankLine(MAX_ARRIVALS);		//Bank Line implemented with array based queue
	PriorityQueue eventQueue;			//Event queue implemented with link based priority queue
	
	bool* tellerArray = new bool[n];		//Array of tellers initialized to true
	for (int i = 0; i < n; i++)
		tellerArray[i] = true;		

	//Open data file
	ifstream dataFile;
	dataFile.open(file.c_str());

	//Variables for keeping track of stats
	int processing_time;
	int wait;
	unsigned long cumulative_wait = 0;
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
	int idleArr[n];
	bool tA = true;
	bool tP;

	//Event Loop
	while ( !eventQueue.isEmpty() )
	{
		tP = tA;
		
		nextEvent = eventQueue.peekFront();
		if (nextEvent->getType() == true)						//getType() returns true if nextEvent points to an arrival
		{
			nextArrival = static_cast<Arrival*> (nextEvent);			//Cast base class pointer into arrival pointer
			currentTime = nextArrival->getArrivalTime();				//Update timer

			process_ArrivalA(nextArrival, &eventQueue, &bankLine, tellerArray, n);
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

			process_DepartureA(nextDeparture, &eventQueue, &bankLine, tellerArray, n);
		}

		
		line = bankLine.getCount();				//Get size of bankLine
		cumulative_line += line;				//Update cumulative total
		
		if ( line > max_line )					//Update max_line if current line is greater
			max_line = line;
		tA = tellerArray[n/2];
		idle_time = idle_time + calculate_idle(tA, tP, currentTime, idle_start, idle_stop);	//Keeps track of idle time for teller
	
	}
	
	simData->process_time = currentTime;				
	simData->avg_wait = (double) cumulative_wait / MAX_ARRIVALS;		
	simData->avg_length = cumulative_line / (MAX_ARRIVALS * 2);		
	simData->max_wait = max_wait;
	simData->max_length = max_line;
	simData->idle_time = idle_time;

	delete[] tellerArray;
}


void simulateB(int n, string file, Stats* simData)
{
	ArrayQueue** bankLines = new ArrayQueue*[n];

	for (int i = 0; i < n; i++)
	{
		bankLines[i] = new ArrayQueue(MAX_ARRIVALS);	//Creating each queue
	}		
	PriorityQueue eventQueue;			//Event queue implemented with link based priority queue
	

	bool* tellerArray = new bool[n];		//Array of tellers initialized to true
	for (int i = 0; i < n; i++)
		tellerArray[i] = true;		

	//Open data file
	ifstream dataFile;
	dataFile.open(file.c_str());

	//Variables for keeping track of stats
	int processing_time;
	int wait;
	unsigned long cumulative_wait = 0;
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

		
		nextEvent = eventQueue.peekFront();
		if (nextEvent->getType() == true)						//getType() returns true if nextEvent points to an arrival
		{
			nextArrival = static_cast<Arrival*> (nextEvent);			//Cast base class pointer into arrival pointer
			currentTime = nextArrival->getArrivalTime();				//Update timer

			process_ArrivalB(nextArrival, &eventQueue, bankLines, tellerArray, n);
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

			process_DepartureB(nextDeparture, &eventQueue, bankLines, tellerArray, n);
		}

		
		line = avg_line(bankLines,n);				//Get size of bankLine
		cumulative_line += line;				//Update cumulative total
		
		if ( line > max_line )					//Update max_line if current line is greater
			max_line = line;


	
	}
	
	simData->process_time = currentTime;				
	simData->avg_wait = (double) cumulative_wait / MAX_ARRIVALS;		
	simData->avg_length = cumulative_line / (MAX_ARRIVALS * 2);		
	simData->max_wait = max_wait;
	simData->max_length = max_line;


	delete[] tellerArray;

	for (int i = 0; i < n; i++)
	{
		delete bankLines[i];
	}	
	
	delete[] bankLines;	
	
}

void process_ArrivalA(Arrival* arr, PriorityQueue* eventQueue, ArrayQueue* bankLine, bool* tellerArr, int n)
{

	
	//Remove arrival event from the priority queue
	eventQueue->dequeue();

	int currentTime = arr->getArrivalTime();
	int transactionTime = arr->getTransactionLength();
	int departureTime;
	int index;
	
	//If bankLine is empty and there is an available teller then customer goes straight to that teller
	if(bankLine->isEmpty() && (available_teller(tellerArr, n, index) == true) )
	{
		departureTime = currentTime + transactionTime;
		Departure* newDeparture = new Departure(departureTime, arr);
		eventQueue->enqueue(newDeparture, departureTime);


		tellerArr[index] = false;
		newDeparture = NULL;
	}
	//Otherwise customer waits in line
	else
	{
		bankLine->enqueue(arr);
	}
}


void process_DepartureA(Departure* dep, PriorityQueue* eventQueue, ArrayQueue* bankLine, bool* tellerArr, int n)
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
		set_available(tellerArr, n);		//Sets one teller entry to true	
	}
}


void process_ArrivalB(Arrival* arr, PriorityQueue* eventQueue, ArrayQueue** bankLines, bool* tellerArr, int n)
{
	//Remove arrival event from the priority queue
	eventQueue->dequeue();

	int currentTime = arr->getArrivalTime();
	int transactionTime = arr->getTransactionLength();
	int departureTime;
	int index_of_shortest = shortest_line(bankLines, n);

	arr->setQueueIndex(index_of_shortest);			//Storing which queue this event goes into

	ArrayQueue* shortestLine = bankLines[index_of_shortest];
	
	//If bankLine is empty and there is an available teller then customer goes straight to that teller
	if(shortestLine->isEmpty() && (available_teller(tellerArr, n, index_of_shortest) == true) )
	{
		departureTime = currentTime + transactionTime;
		Departure* newDeparture = new Departure(departureTime, arr);
		eventQueue->enqueue(newDeparture, departureTime);


		tellerArr[index_of_shortest] = false;
		newDeparture = NULL;
	}
	//Otherwise customer waits in line
	else
	{
		shortestLine->enqueue(arr);
	}
}

void process_DepartureB(Departure* dep, PriorityQueue* eventQueue, ArrayQueue** bankLines, bool* tellerArr, int n)
{
	//Remove departure from priority queue
	eventQueue->dequeue();
	Arrival* link = dep->getLinkedArrival();			//Get address of arrival event corresponding to Departure 
	int currentTime = dep->getDepartureTime();
	int index_of_line;

	index_of_line = link->getQueueIndex();
	ArrayQueue* currentLine = bankLines[index_of_line];

	//If bank line is not empty 
	if ( !currentLine->isEmpty() )
	{
		Event* temp = currentLine->peekFront();
		Arrival* nextCustomer = static_cast<Arrival*> (temp);
		currentLine->dequeue();
		temp = NULL;

		int transactionTime = nextCustomer->getTransactionLength();
		int departureTime = currentTime + transactionTime;

		//Create new departure event and add it to eventQueue
		Departure* newDeparture = new Departure(departureTime, nextCustomer);
		eventQueue->enqueue(newDeparture, departureTime);

	}
	else
	{
		tellerArr[index_of_line] = true;
	}
}


bool available_teller(bool* arr, int n, int& index)
{	
	for (int i = 0; i < n; i++)
	{
		if (arr[i] == true)
		{
			index = i;
			return true;	
		}	
	}

	return false;
}

bool set_available(bool* arr, int n)
{
	//Finds first false entry and sets it to true
	for (int i = 0; i < n; i++)
	{
		if ( arr[i] == false )
		{
			arr[i] = true;
			return true;
		}
	}

	return false;
}

void compute_averages(Stats* simData, Stats& avg)
{
	for (int i = 0; i < 10; i++)
	{
		avg.CPU_time += simData[i].CPU_time;
		avg.process_time += simData[i].process_time;
		avg.avg_wait += simData[i].avg_wait;
		avg.avg_length += simData[i].avg_length;
		avg.max_length += simData[i].max_length;
		avg.max_wait += simData[i].max_wait;
		avg.idle_time += simData[i].idle_time;
	}

	avg.CPU_time /= 10;
	avg.process_time /= 10;
	avg.avg_wait /= 10;
	avg.avg_length /= 10;
	avg.max_length /= 10;
	avg.max_wait /= 10;
	avg.idle_time /= 10;	
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


int shortest_line(ArrayQueue** bankLines, int n)
{
	ArrayQueue* temp;
	int shortest = MAX_ARRIVALS;
	int length;
	int index;
	for (int i = 0; i < n; i++)
	{
		temp = bankLines[i];	
		length = temp->getCount();

		if (length < shortest)
		{
			shortest = length;
			index = i;
		}
	}

	return index;
}

int avg_line(ArrayQueue** bankLines, int n)
{
	ArrayQueue* temp;
	int length;
	int total = 0;
	for (int i = 0; i < n; i++)
	{
		temp = bankLines[i];
		length = temp->getCount();
		total = total + length;	
	}

	return (length / n);
}

void generate_events(string fileName)
{
	ofstream data_file;
	data_file.open(fileName.c_str());			//Open data file

	int* arrivalTimes= new int[MAX_ARRIVALS];			//Holds arrival times
	int* transactionLengths= new int[MAX_ARRIVALS];		//Holds transaction times


	srand(time(0));						//Seed random number generator
	for (int i = 0; i < MAX_ARRIVALS; i++)
	{
		arrivalTimes[i] = rand() % 100001;		//Generate random # from 0-100,000 inclusive
		transactionLengths[i] = (rand() % 100) + 1;	//Generate random # from 1-100 inclusive
	}

	counting_sort(arrivalTimes, MAX_ARRIVALS);			//Sort arrival times

	for (int i = 0; i < MAX_ARRIVALS; i++)	
		data_file << arrivalTimes[i] << "     " << transactionLengths[i] << endl;	

	delete[] arrivalTimes;					//delete arrays
	delete[] transactionLengths;
}

void counting_sort(int arr[], int size)
{
	int count[100001] = {0};				//Frequency tracker
	int* sorted_arr = new int[size];			//Data will be sorted into here

	for (int i = 0; i < size; i++)				//Count frequencies
	{
		count[arr[i]]++;
	}

	for (int i = 1; i < 100001; i++)			//Determine cumulative frequencies
	{
		count[i] = count[i] + count[i-1];
	}

	for (int i = size - 1; i > 0; i--)			//Place values into correct positions in sorted array
	{
		sorted_arr[count[arr[i]]] = arr[i];
		count[arr[i]]--;
	}

	copy(sorted_arr, sorted_arr + size, arr);		//copy sorted array into original array	
	
	delete[] sorted_arr;

}
