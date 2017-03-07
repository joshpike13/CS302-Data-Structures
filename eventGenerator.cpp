#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>

using namespace std;
#define ARR_SIZE 99999


void counting_sort(int arr[], int size);

int main()
{

	//Arrays to hold arrival time and transaction lengths
	int* arrivalTimes= new int[ARR_SIZE];
	int* transactionLengths= new int[ARR_SIZE];

	//Array to make sure arrivalTimes are unique
	int* duplicateTracker = new int[500001];

	//Zero out all entries in duplicate tracker
	for (int i = 0; i < 500001; i++)
		duplicateTracker[i] = 0;

	//Seed random nubmer generator
	srand(time(0));
	
	int val;
	bool repeat = true;

	//Generating random vals
	for (int i = 0; i < ARR_SIZE; i++)
	{
		while (repeat)
		{
			val = rand() % 500000;			//Generate random number between 0 and 500,000 inclusive
			
			if (duplicateTracker[val] != 1)		//If duplicate tracking array has a 0 in the same index as val, then val hasn't been generated before
			{
				repeat = false;			//Set repeat to false to exit loop after unique number is generated		
				duplicateTracker[val] = 1;	//Place a 1 into index val to denote that the number val has been generated before
			}
		}

		//Set current index of arrivalTimes to val after generating unique value
		arrivalTimes[i] = val;

		//Generate 99,999 random transaction lengths between 1 and 100 (inclusive)
		transactionLengths[i] = (rand() % 100) + 1;

		//Reset repeat
		repeat = true;
	}

	//Sort arrivalTimes array
	counting_sort(arrivalTimes, ARR_SIZE);

	//Data will be written to file "data.txt"
	ofstream data_file;
	data_file.open("data.txt");


	//this loop writes randomly generated data to the file
	//Format: ArrivalTime - 5 spaces - transaction time
	for (int i = 0; i < ARR_SIZE; i++)
	{
		data_file << arrivalTimes[i] << "     " << transactionLengths[i] << endl;	
	}
	
	//Delete arrays
	delete[] arrivalTimes;
	delete[] transactionLengths;
	delete[] duplicateTracker;

	return 0;	
}

void counting_sort(int arr[], int size)
{
	//Array to track frequency of each number - vals range from 0 to 500,000 (inclusive)
	int* count = new int[500001];

	//Sorted array to be returned 
	int* sorted_arr = new int[size];

	//Counting frequencies of each arr element
	for (int i = 0; i < size; i++)
	{
		count[arr[i]]++;
	}

	//Determining cumulative frequencies
	for (int i = 1; i < 500001; i++)
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


