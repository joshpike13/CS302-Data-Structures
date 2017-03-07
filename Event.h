#ifndef EVENT_H
#define EVENT_H

using namespace std;

class Event {
	
	public:
		Event(bool type)
		{
			arrival = type;
		}

		virtual ~Event() {}
	
		bool getType()
		{
			return arrival;
		}

	protected:
		bool arrival;
		
};

class Arrival : public Event {
	
	public:
		Arrival(int a, int t) : Event(true)
		{
			arrivalTime = a;
			transactionLength = t;
		}

		int getArrivalTime()
		{
			return arrivalTime;
		}

		int getTransactionLength()
		{
			return transactionLength;
		}
		
		void setQueueIndex(int index)
		{
			queueIndex = index;
		}

		int getQueueIndex()
		{
			return queueIndex;
		}
	
	private:
		int arrivalTime;
		int transactionLength;
		int queueIndex;			//Only used when simulating with multiple lines
};

class Departure : public Event {
	
	public:
		Departure(int d, Arrival* a) : Event(false)
		{
			departureTime = d;
			linkedArrival = a;
		}

		int getDepartureTime()
		{
			return departureTime;
		}

		Arrival* getLinkedArrival()
		{
			return linkedArrival;
		}
	
	private:
		int departureTime;
		Arrival* linkedArrival;
};


#endif



