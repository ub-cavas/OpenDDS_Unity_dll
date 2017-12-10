#include <thread>

#include <iostream> //cout
#include <queue>

#include <dds/DCPS/Service_Participant.h>	//neccessary to start OpenDDSThreat without error
#include "OpenDDS.h"	//to start OpenDDSThread
#include "MriTypeSupportImpl.h"	//Mri data formats

#include "Sleep.h"	// Sleep()
#include "TimeSync.h"	//Sleep() 
#include "QueueTs.h"
#include "main.h"


extern bool finish_application;
extern bool threadOpenDDS_initialized;

extern QueueTs<Mri::V2XMessage> v2x_queue;
std::map<long, Mri::V2XMessage> v2xs_map;

long timestamp_previous;			//used by generateV2xUniqueTimestamp
long sample_counter_per_timestamp;	//used by generateV2xUniqueTimestamp

//char *argv2[] = { "-DCPSConfigFile","rtps.ini" };
//-DCPSConfigFile rtps.ini

int main(int argc, char* argv[]) {

	std::map<long, Mri::V2XMessage> v2xs_map;
	std::map<long, Mri::V2XMessage>::iterator it_v2x;
	timestamp_previous = 0;	//initial value
	
	threadOpenDDS_initialized = false;
	std::thread threadOpenDDS(OpenDDSThread, argc, argv);
	while (!threadOpenDDS_initialized)
	{
		//wait until threadOpenDDS is initialized;
	}
	
	
	std::thread threadVehsMap(vehsMapThread);

	while (!finish_application)
	{
		//NS3 simulation thread
		doSimulation();
		v2xsMapGarbageCollection(2000);
		Sleep(500);
	}
	//wait for a finish of all threads
	Sleep(500);
	threadOpenDDS.detach();
	threadVehsMap.detach();

	return 0;
}


void v2xsMapGarbageCollection(long interval_ms) {
	//delete v2x messages older then interval, e.g. 1000 ms
	long timestamp_older = generateV2xUniqueTimestamp(GetTimestamp() - (interval_ms / 10));

	for (auto x = v2xs_map.begin(); x !=v2xs_map.cend();){

		if (x->first < timestamp_older)
		{
			x = v2xs_map.erase(x);
			std::cout << "Erased v2x id:" << x->first << std::endl;
		}
		else
		{
			++x;
		}
	}
		

}

void doSimulation() {

	Mri::V2XMessage v2x;
	long v2x_extended_timestamp = 0;


	//copy queue
	std::queue <Mri::V2XMessage>v2x_queue_copy;
	v2x_queue.swap(&v2x_queue_copy);

	while (!v2x_queue_copy.empty())
	{
		v2x = v2x_queue_copy.front();
		v2x_queue_copy.pop();

		v2x_extended_timestamp = generateV2xUniqueTimestamp(v2x.sender_timestamp);
		addV2xToMap(v2x_extended_timestamp, v2x);

		std::cout << "V2X: senderId  = " << v2x.sender_id
			<< "     receiverId = " << v2x.recipient_id
			<< "sender_timestamp=" << v2x.sender_timestamp << std::endl;
	}
	std::cout << std::endl << std::endl;
}


long generateV2xUniqueTimestamp(long v2x_timestamp) {

	long timestamp_now = GetTimestamp();
	if (timestamp_now > timestamp_previous)
	{
		sample_counter_per_timestamp = 0;
		timestamp_previous = timestamp_now;
	}
	else
	{
		sample_counter_per_timestamp++;
	}
	return ((v2x_timestamp * 1000) + sample_counter_per_timestamp);
}

void addV2xToMap(long extended_timestamp, Mri::V2XMessage v2x) {

	std::map<long, Mri::V2XMessage>::iterator it_v2x;

	it_v2x = v2xs_map.find(extended_timestamp);

	if (it_v2x != v2xs_map.end())
	{
		std::cerr << "Error - duplicated elements in v2xs_map." << std::endl;
	}
	else
	{
		v2xs_map.emplace(extended_timestamp, v2x);
	}
}