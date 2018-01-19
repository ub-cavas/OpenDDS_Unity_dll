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
extern std::map<long, Mri::V2XMessage> v2xs_map;
extern QueueTs<Mri::V2XMessage> v2x_queue_in;









//char *argv2[] = { "-DCPSConfigFile","rtps.ini" };
//-DCPSConfigFile rtps.ini

int main(int argc, char* argv[]) {

	std::map<long, Mri::V2XMessage> v2xs_map;
	std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now();	//sleep below
	threadOpenDDS_initialized = false;

	std::thread threadOpenDDS(OpenDDSThread, argc, argv);
	while (!threadOpenDDS_initialized)
	{
		//wait until threadOpenDDS is initialized;
	}
	
	std::thread threadVehsMap(vehsMapThread);

	srand(time(NULL));

	// main loop
	while (!finish_application)
	{
		//NS3 simulation thread
		//***********************************************************************************************
		//
		// do NS3 simulation, update transmition parameters of V2X message and resend V2x message
		//
		//***********************************************************************************************
		doSimulation();
		v2xsMapGarbageCollection(5000);

		t += std::chrono::milliseconds(50);	//each loop 50 ms
		std::this_thread::sleep_until(t);
	}
	//wait for a finish of all threads
	Sleep(500);
	threadOpenDDS.detach();
	threadVehsMap.detach();

	return 0;
}


void doSimulation() {

	Mri::V2XMessage v2x;
	long v2x_extended_timestamp = 0;


	//copy queue
	std::queue <Mri::V2XMessage>v2x_queue_copy;
	v2x_queue_in.swap(&v2x_queue_copy);

	while (!v2x_queue_copy.empty())
	{
		v2x = v2x_queue_copy.front();
		v2x_queue_copy.pop();

		v2x_extended_timestamp = generateV2xUniqueTimestamp(v2x.sender_timestamp);
		addV2xToMap(v2x_extended_timestamp, v2x);



		//this is a fake simulation func:
		transmitV2X(v2x, 1);


		//
		//************************************************************************************************
	}
	std::cout << "." ;
}



void v2xsMapGarbageCollection(long interval_ms) {
	//delete v2x messages older then interval, e.g. 5000 ms
	long timestamp_older = generateV2xUniqueTimestamp(GetTimestamp() - (interval_ms / 10));

	for (auto x = v2xs_map.begin(); x != v2xs_map.cend();) {

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