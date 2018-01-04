#include <thread>

#include <iostream> //cout
#include <queue>
#include <utility>      // std::pair, std::make_pair
#include <math.h>       // pow, sqrt
#include <stdlib.h>     /* srand, rand */

#include <dds/DCPS/Service_Participant.h>	//neccessary to start OpenDDSThreat without error
#include "OpenDDS.h"	//to start OpenDDSThread
#include "MriTypeSupportImpl.h"	//Mri data formats

#include "Sleep.h"	// Sleep()
#include "TimeSync.h"
#include "QueueTs.h"
#include "main.h"


extern bool finish_application;
extern bool threadOpenDDS_initialized;

extern std::map<long, Mri::VehData> vehs_map;

extern QueueTs<Mri::V2XMessage> v2x_queue;
std::map<long, Mri::V2XMessage> v2xs_map;

long timestamp_previous;			//used by generateV2xUniqueTimestamp
long sample_counter_per_timestamp;	//used by generateV2xUniqueTimestamp

//char *argv2[] = { "-DCPSConfigFile","rtps.ini" };
//-DCPSConfigFile rtps.ini

int main(int argc, char* argv[]) {

	std::map<long, Mri::V2XMessage> v2xs_map;
	
	timestamp_previous = 0;	//initial value
	
	threadOpenDDS_initialized = false;
	std::thread threadOpenDDS(OpenDDSThread, argc, argv);
	while (!threadOpenDDS_initialized)
	{
		//wait until threadOpenDDS is initialized;
	}
	
	
	std::thread threadVehsMap(vehsMapThread);


	srand(time(NULL));

	while (!finish_application)
	{
		//NS3 simulation thread
		doSimulation();
		v2xsMapGarbageCollection(5000);
		Sleep(100);
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
			//std::cout << "Erased v2x id:" << x->first << std::endl;
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

		/*std::cout << "V2X: senderId  = " << v2x.sender_id
			<< "     receiverId = " << v2x.recipient_id
			<< "sender_timestamp=" << v2x.sender_timestamp << std::endl;*/

		transmitV2X(v2x, 1);
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


void transmitV2X(Mri::V2XMessage v2x, long destination_veh_id) {

	const double V2X_RANGE = 150.0; //150 meters

	try
	{
		if (v2x.sender_id != destination_veh_id)
		{
			Mri::VehData veh_sender = vehs_map[v2x.sender_id];
			Mri::VehData veh_receiver = vehs_map[destination_veh_id];



			double distance = sqrt(pow((veh_receiver.position_x - veh_sender.position_x), 2.0) + pow((veh_receiver.position_y - veh_sender.position_y), 2.0));

			if (distance < V2X_RANGE)
			{
				//transmit
				long  delay = rand() % 50 + 15;	// delay 15 - 64 ms
												//update data in v2x message

				v2x.recipient_id = destination_veh_id;
				v2x.recipient_timestamp = v2x.sender_timestamp + (delay / 10);

				// resend v2x
				std::cout << "@@ RESEND V2X from veh_id:" << v2x.sender_id << " to veh_id:" << v2x.recipient_id << " timestamps: "
					<< v2x.sender_timestamp << " -> " << v2x.recipient_timestamp << std::endl;
			}
			else
			{
				std::cout << "-- NO TRANSMITION from veh_id:" << v2x.sender_id << " to veh_id:" << v2x.recipient_id << " timestamps: "
					<< v2x.sender_timestamp << " distance=" << distance << std::endl;
			}
		}
		


	}
	catch (const std::exception&)
	{
		//do nothing
		//transmit nothing
	}

}