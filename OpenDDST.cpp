
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>

#include <string> 
#include <thread>
#include <iostream> //cout
#include <ctime> //time format
#include <conio.h> //key press
#include <map>	//map - list of vehicles
#include <atomic> //access
#include <mutex>



#include "MriTypeSupportImpl.h"
#include "DataWriter_Aux2Strings.h"
#include "DataReader_Aux2Strings.h"
#include "DataReader_V2XMessage.h"
#include "DataWriter_V2XMessage.h"
#include "DataReader_VehData.h"
#include "DataWriter_VehData.h"


#include "ParticipantClass.h"
#include "SubscriberClass.h"
#include "PublisherClass.h"

#include "QueueTs.h"
#include "TimeSync.h"
#include "OpenDDS.h"
#include "Start.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;


Mri::Aux2StringsDataWriter_var  writer_global_aux2strings;
Mri::V2XMessageDataWriter_var  writer_global_v2xmessage;
Mri::VehDataDataWriter_var writer_global_vehdata;
std::atomic<long> veh_id_to_remove;

QueueTs<Mri::VehData> vehdata_queue_in;
Mri::VehData subjectCar;

std::map<long, Mri::VehData> vehs_map;
std::map<long, UnityVehicle> unityVehsMap;

std::map<long, std::set<UnityVehicle>> unityVehsMapSets;
std::atomic<int> access_unityVehMapSets;

std::mutex mutexMap;





//std::queue<Mri::V2XMessage> v2x_queue_in;
QueueTs<Mri::V2XMessage> v2x_queue_in;
std::map<long, Mri::V2XMessage> v2xs_map;


bool finish_application;
bool threadOpenDDS_initialized;

long timestamp_previous;			//used by generateV2xUniqueTimestamp
long sample_counter_per_timestamp;	//used by generateV2xUniqueTimestamp


std::atomic<float> dnpw_closestVehicleMessage_distance;
std::atomic<long> dnpw_closestVehicleMessage_timestamp;


std::string log_rd_VehsMapThread;
std::string log_rd_Opendds;



UnityVehicle  convertUnityVeh(Mri::VehData veh) {
	//no copy strings

	UnityVehicle uVeh;

	uVeh.color = veh.color;
	uVeh.lane_index = veh.lane_index;
	uVeh.leading_vehicle_id = veh.leading_vehicle_id;
	uVeh.link_coordinate = veh.link_coordinate;
	uVeh.link_id = veh.link_id;
	uVeh.orient_heading = veh.orient_heading;
	uVeh.orient_pitch = veh.orient_pitch;
	uVeh.orient_roll = veh.orient_roll;
	uVeh.position_x = veh.position_x;
	uVeh.position_y = veh.position_y;
	uVeh.position_z = veh.position_z;
	uVeh.speed = veh.speed;
	uVeh.timestamp = veh.timestamp;
	uVeh.trailing_vehicle_id = veh.trailing_vehicle_id;
	uVeh.turning_indicator = veh.turning_indicator;
	uVeh.vehicle_id = veh.vehicle_id;
	uVeh.vehicle_type = veh.vehicle_type;

	return uVeh;

}



void convertUnityVehOld(Mri::VehData* veh, UnityVehicle* uVeh) {
	//no copy strings

	uVeh->color = veh->color;
	uVeh->lane_index = veh->lane_index;
	uVeh->leading_vehicle_id = veh->leading_vehicle_id;
	uVeh->link_coordinate = veh->link_coordinate;
	uVeh->link_id = veh->link_id;
	uVeh->orient_heading = veh->orient_heading;
	uVeh->orient_pitch = veh->orient_pitch;
	uVeh->orient_roll = veh->orient_roll;
	uVeh->position_x = veh->position_x;
	uVeh->position_y = veh->position_y;
	uVeh->position_z = veh->position_z;
	uVeh->speed = veh->speed;
	uVeh->timestamp = veh->timestamp;
	uVeh->trailing_vehicle_id = veh->trailing_vehicle_id;
	uVeh->turning_indicator = veh->turning_indicator;
	uVeh->vehicle_id = veh->vehicle_id;
	uVeh->vehicle_type = veh->vehicle_type;

}



void publishSubjectCarLocationThread() {

	Mri::VehData _veh;


	std::chrono::time_point<std::chrono::system_clock> t = std::chrono::system_clock::now();	//sleep below

	while (!finish_application) {

		_veh = getSubjectCarPosition();
		_veh.timestamp = GetTimestamp();

		publishVehDataMessage(_veh);

		t += std::chrono::milliseconds(50);	//each loop 50 ms
		std::this_thread::sleep_until(t);
	}

	cout << endl << "-------------------" << endl << "-  publishSubjectCarThread Stopped " << endl << "-------------------" << endl << endl;
}


Mri::VehData getSubjectCarPosition() {
	return subjectCar;
}


void unityVehsMapThread() {


	Mri::VehData _veh;

//	UnityVehicle _uVeh;

	


	//std::map<long, Mri::VehData> vehs_map;
	//std::map<long, UnityVehicle>::iterator it;
	

	try
	{
		std::map<long, std::set<UnityVehicle>>::iterator it;
		veh_id_to_remove = -1; // initial value

		while (!finish_application)
		{
			//wait for something at the queue
			vehdata_queue_in.pop(_veh);
			{
				//log_rd_VehsMapThread = "Start after queue pop,";

				//while (access_unityVehMapSets != ACCESS_none) {             // wait while getVehsArray accessing VehMapSets
				//	
				//	if (access_unityVehMapSets == ACCESS_request_getVehsArray)
				//	{
				//		//it has a priority, change access
				//		access_unityVehMapSets = ACCESS_getVehsArray;
				//	}
				//	
				//	std::this_thread::yield();
				//	//log_rd_VehsMapThread += " ddddd,";
				//}

				//access_unityVehMapSets = ACCESS_unityVehsMapThread;

				//log_rd_VehsMapThread += " GOT Access,";
				
				
				addVehToMap(_veh);



				//
				//it = unityVehsMapSets.find(_veh.vehicle_id);


				//log_rd_VehsMapThread += " find,";



				//if (it != unityVehsMapSets.end()) {
				//	//there is a car with id = _veh.vehicle_id

				//	_uVeh = convertUnityVeh(_veh);
				//	//convertUnityVeh(&_veh, &_uVeh);

				//	//add to set
				//	it->second.emplace(_uVeh);
				//	//cout << "Timestamp=" << GetTimestamp() << " veh timestamp=" << _veh.timestamp << endl;
				//}
				//else
				//{
				//	//no car with id = _veh.vehicle_id
				//	//add new set to the map

				//	//convert from _veh to _uVeh
				//	_uVeh = convertUnityVeh(_veh);
				//	//convertUnityVeh(&_veh, &_uVeh);

				//	std::set<UnityVehicle> _set;
				//	_set.emplace(_uVeh);
				//	unityVehsMapSets.emplace(_veh.vehicle_id, _set);
				//	//cout << "NEW VEH Timestamp=" << GetTimestamp() << " veh timestamp=" << _veh.timestamp << endl;

				//	cout << "++++ ++++ NEW VEH = " << _veh.vehicle_id << " ++++ ++++" << endl << endl;

				//}

				//check if we have to remove a car















				// TEST


				if (veh_id_to_remove != -1)
				{


					if (removeVehFromMap()==0)
					{
						//success
						cout << "############  Removed from vehsMap vehicle_id =" << veh_id_to_remove << "     ############" << endl << endl;
					}
				}





















				//check if other thread want to access unityVehsMapSets
				
					//if (access_unityVehMapSets == ACCESS_request_getVehsArray)
					//{	
					//	//it has a priority, change access
					//	access_unityVehMapSets = ACCESS_getVehsArray;
					//}
					//else
					//{
					//	access_unityVehMapSets = ACCESS_none;
					//}


			}
		}
	}
	catch (const std::exception&)
	{
		cout << "ERROR" << endl;
		access_unityVehMapSets = ACCESS_none;
	}

	
}





int removeVehFromMap() {

	std::lock_guard<std::mutex> guard(mutexMap);

	std::map<long, std::set<UnityVehicle>>::iterator it;
	it = unityVehsMapSets.find(veh_id_to_remove);
	
	veh_id_to_remove = -1; //reset this global variable
	
	if (it != unityVehsMapSets.end())
	{
		unityVehsMapSets.erase(it);
		return 0;
	}
	else
	{
		// can't find vehicle
		return -1;
	}
}



void addVehToMap(Mri::VehData _veh)
{

	std::lock_guard<std::mutex> guard(mutexMap);

	UnityVehicle _uVeh;

	std::map<long, std::set<UnityVehicle>>::iterator it;
	it = unityVehsMapSets.find(_veh.vehicle_id);

	//convert from veh to Unity Veh
	_uVeh = convertUnityVeh(_veh);
	//convertUnityVeh(&_veh, &_uVeh);

	if (it != unityVehsMapSets.end()) {
		//there is a car with id = _veh.vehicle_id

		//add to set
		it->second.emplace(_uVeh);
		//cout << "Timestamp=" << GetTimestamp() << " veh timestamp=" << _veh.timestamp << endl;
	}
	else
	{
		//no car with id = _veh.vehicle_id
		//add new set to the map

		std::set<UnityVehicle> _set;
		_set.emplace(_uVeh);
		unityVehsMapSets.emplace(_veh.vehicle_id, _set);

		cout << "++++ ++++ NEW VEH = " << _veh.vehicle_id << " ++++ ++++" << endl << endl;

	}
}






//..........................................................................................................


void OpenDDSThread(int argc, char* argv[]){
	// start_opendds thread
	


	//char key = ' ';

	DDS::DomainId_t DomainID{ 246 };

	const char * topic_traffic_vehs_name = "Mri_TrafficVeh";
	const char * topic_subject_car_name = "Mri_SubjectCar";
	
	//initial value
	timestamp_previous = 0;	

	access_unityVehMapSets = ACCESS_none;
	

	//________________________________________________________
	//initialization
	subjectCar.vehicle_id = 1;
	subjectCar.position_x = 0;
	subjectCar.position_y = 0;
	subjectCar.position_z = 0;
	subjectCar.orient_heading = 0;
	subjectCar.orient_pitch = 0;
	subjectCar.orient_roll = 0;
	subjectCar.speed = 0;
	subjectCar.timestamp = 0;
	
	std::cout << "*******^^^^^^^^*  Starting openddsThread *********" << std::endl;

	try
	{




		//it helps a lot !!!!!
		ACE::init();
		
		/*It was a following error :

		ACE_INET_Addr::ACE_INET_Addr : localhost : WSA Startup not initialized
			notification pipe open failed : WSA Startup not initialized
			ACE_INET_Addr::ACE_INET_Addr : localhost : WSA Startup not initialized
			notification pipe open failed : WSA Startup not initialized
			ACE_Select_Reactor_T::open failed inside ACE_Select_Reactor_T::CTOR : WSA Startup
			not initialized*/


		// Initialize DomainParticipantFactory
 		DDS::DomainParticipantFactory_var dpf =
			TheParticipantFactoryWithArgs(argc, argv);

		// Create DomainParticipant
		DDS::DomainParticipant_var participant =
			dpf->create_participant(DomainID,
				PARTICIPANT_QOS_DEFAULT,
				0,
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		if (CORBA::is_nil(participant.in())) {
			cerr << "Participant: Failed to create participant..." << endl;
		}
		else {
			cout << "Participant: participant created successfully" << endl;
		}

		// Create Subscriber
		DDS::Subscriber_var subscriber = participant->create_subscriber(
			SUBSCRIBER_QOS_DEFAULT,
			DDS::SubscriberListener::_nil(),
			OpenDDS::DCPS::DEFAULT_STATUS_MASK);

		if (CORBA::is_nil(subscriber.in())) {
			cerr << "create_subscriber failed." << endl;
		}
		else {
			cout << "SubscriberClass: subscriber created correctly" << endl;
		}

		// Create Publisher
		DDS::Publisher_var publisher = participant->create_publisher(
			PUBLISHER_QOS_DEFAULT,
			DDS::PublisherListener::_nil(),
			::OpenDDS::DCPS::DEFAULT_STATUS_MASK
		);
		if (CORBA::is_nil(publisher.in())) {
			cerr << "create_publisher failed." << endl;
		}
		else {
			cout << "PublisherClass: publisher created correctly" << endl;
		}


		
		
			// run timestamp synchronization
		TimeSynchronization(participant, subscriber, publisher);
		//Timestamps  synchronized !

		// topics:	Mri_TrafficVeh Mri_SubjectCar
		//WARNING: both readers uses the same "on_data_available" code in DataReaderListenerImpl_VehData
		//we can do this, because formats of the messages are the same

		cout << "Create reader_traffic..." << endl;
		DataReader_VehData reader_traffic_vehs(participant, subscriber, "Mri_TrafficVeh");

		//subjec Car writer
		cout << "Create writer_subjectCar..." << endl;
		DataWriter_VehData writer_vehdata_message(participant, publisher, "Mri_SubjectCar");
		writer_global_vehdata = writer_vehdata_message.msg_writer;


		//DataReader_VehData reader_subject_car(participant, subscriber, "Mri_SubjectCar");

		// 
		
		// for reading dnpw v2x messages
		dnpw_closestVehicleMessage_distance = 99999;	//initial value.  99999 means a vehicle is out of range
		dnpw_closestVehicleMessage_timestamp = 0;
		DataReader_Aux2Strings reader(participant, subscriber, "Mri_Control");
		



		//long old_veh_timestamp = 0;

		//std::map<long, UnityVehicle> unity_vehs_map_copy;

		threadOpenDDS_initialized = true;





		//----------------------------------------------------------------------------------------------------------


		//cout << "It's time for loop..." << endl;

		//while (key != 'q')
		while (!finish_application)
		{	
			//getInput(&key);

			Sleep(100);
			//log_rd_Opendds = "After sleep, ";	
			


			if (dnpw_closestVehicleMessage_distance != 99999)
			{
				long oldDistanceTimestamp = GetTimestamp() - 50;  //500 ms
				if (dnpw_closestVehicleMessage_timestamp < oldDistanceTimestamp)
				{
					//old value, let's reset it
					dnpw_closestVehicleMessage_distance = 99999;
					dnpw_closestVehicleMessage_timestamp = 0;
				}

			}


			garbageCollectionMap();


			//// if info about veh wasn't updated for 500 ms it means this vehicle dissapeared, so we have to delete info about this vehicle
			//old_veh_timestamp = GetTimestamp() - 50;	//to find veh data not updated for 50 x 10ms = 500 ms
			//
			//std::map<long, std::set<UnityVehicle>> unityVehsMapSetsCopy2;
			//try
			//	{

			//	while (access_unityVehMapSets != ACCESS_none) {
			//		std::this_thread::yield();
			//	}

			//
			//	unityVehsMapSetsCopy2 = unityVehsMapSets;											
			//	if (!unityVehsMapSetsCopy2.empty()) {
			//			for (auto x : unityVehsMapSetsCopy2)
			//			{
			//				std::set<UnityVehicle>::iterator it;		
			//				it = x.second.end();
			//				--it;
			//				if (it->timestamp < old_veh_timestamp)
			//				{
			//					veh_id_to_remove = x.first;
			//					std::cout << "&&&&&&&&&&&&&  REMOVE id=" << veh_id_to_remove <<std::endl << std::endl;
			//					break;
			//				}
			//			}
			//	}
			//}

			//catch (const std::exception&)
			//{
			//	cout << "ERROR main loop " << endl;
			//}
	}

		


		//-------------------------------------------------------------------------------------------------
		// Clean-up!

		//threadTimestamp.detach();

		participant->delete_contained_entities();
		dpf->delete_participant(participant);

		//TheServiceParticipant->shutdown();
		
		
		finish_application = true; 

		cout << endl << "$$$$$$$$$$$$$$$$$$$$$$" << endl << "$        End of OpenDDS thread" << endl << "$$$$$$$$$$$$$$$$$$$$$$" << endl << endl;
		
	}
	catch (const CORBA::Exception& e) {
		e._tao_print_exception("Exception caught in OpenDDSThread:");
		
	}
}




void garbageCollectionMap()
{

	std::lock_guard<std::mutex> guard(mutexMap);

	// if info about veh wasn't updated for 500 ms it means this vehicle dissapeared, so we have to delete info about this vehicle
	long old_veh_timestamp = GetTimestamp() - 50;	//to find veh data not updated for 50 x 10ms = 500 ms

	std::map<long, std::set<UnityVehicle>> unityVehsMapSetsCopy2;
	try
	{

	/*	while (access_unityVehMapSets != ACCESS_none) {
			std::this_thread::yield();
		}*/


		unityVehsMapSetsCopy2 = unityVehsMapSets;
		if (!unityVehsMapSetsCopy2.empty()) {
			for (auto x : unityVehsMapSetsCopy2)
			{
				std::set<UnityVehicle>::iterator it;
				it = x.second.end();
				--it;
				if (it->timestamp < old_veh_timestamp)
				{
					veh_id_to_remove = x.first;
					std::cout << "&&&&&&&&&&&&&  REMOVE id=" << veh_id_to_remove << std::endl << std::endl;
					break;
				}
			}
		}
	}

	catch (const std::exception&)
	{
		cout << "ERROR main loop " << endl;
	}

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

	const double V2X_RANGE = 300.0; //150 meters

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
				

				publishV2xMessage(v2x);

			}
			else
			{
				v2x.recipient_id = destination_veh_id;
				std::cout << endl << "-- NO TRANSMITION from veh_id:" << v2x.sender_id << " to veh_id:" << v2x.recipient_id << " timestamps: "
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

void publishVehDataMessage(Mri::VehData car) {
	int success = writer_global_vehdata->write(car, DDS::HANDLE_NIL);
	if (success == DDS::RETCODE_OK)
	{
		//std::cout << "Send SubjectCar position x=" << car.position_x << ", y=" << car.position_y << ", z=" << car.position_z << ", timestamp=" << car.timestamp << std::endl;
	}
	else
	{
		throw std::string("ERROR: DataWriter VehData::sendMessage write");
		//ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) ERROR: Publisher::sendMessage write returned %d.\n"), success));
	}
}


void publishV2xMessage(Mri::V2XMessage v2x) {
	int success =  writer_global_v2xmessage->write(v2x, DDS::HANDLE_NIL);
	if (success == DDS::RETCODE_OK)
	{
		
		std::cout << endl << "@@ RESEND V2X from veh_id:" << v2x.sender_id << " to veh_id:" << v2x.recipient_id << " timestamps: "
			<< v2x.sender_timestamp << " -> " << v2x.recipient_timestamp << std::endl;
	}
	else
	{
		throw std::string("ERROR: DataWriter V2XMessage::sendMessage write");
		//ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) ERROR: Publisher::sendMessage write returned %d.\n"), success));
	}
}


void ProcessDoNotPassWarningMessage(Mri::Aux2Strings dnpwAux)
{
	float aux_distance = atof(dnpwAux.str1);
	if (aux_distance < dnpw_closestVehicleMessage_distance)
	{
		//this vehicle is closer, we have to update
		dnpw_closestVehicleMessage_distance = aux_distance;
		dnpw_closestVehicleMessage_timestamp = GetTimestamp();

		cout << "Warning distance=" << dnpw_closestVehicleMessage_distance << " at=" << dnpw_closestVehicleMessage_timestamp << endl;
	}
}













//void vehsMapThread() {
//	
//
//	Mri::VehData _veh;
//	
//	//std::map<long, Mri::VehData> vehs_map;
//	std::map<long, Mri::VehData>::iterator it;
//	
//	veh_id_to_remove = -1; // initial value
//	
//
//	std::cout << "***************   vehsMapThread " << std::endl;
//
//	while (!finish_application)
//	{
//		//wait for something at the queue
//		vehdata_queue_in.pop(_veh);
//		{
//			it = vehs_map.find(_veh.vehicle_id);
//			if (it != vehs_map.end()) {
//				//there is a car with id = _veh.vehicle_id
//
//
//				//check if new data is older than in vehs_map
//				if (_veh.timestamp >  it->second.timestamp)
//				{
//					//update data in vehs_map
//					vehs_map[it->first] = _veh;
//				}
//				else
//				{
//					cout << "OpenDDS data is older than data in vehs_list. Veh_id=" << _veh.vehicle_id
//						<< " timestamp_vehs_list=" << it->second.timestamp
//						<< " timestamp_OpenDDS=" << _veh.timestamp << endl;
//				}
//			}
//			else
//			{
//				//no car with id = _veh.vehicle_id
//				vehs_map.emplace(_veh.vehicle_id, _veh);
//
//			}
//
//			//check if we have to remove a car
//			if (veh_id_to_remove!=-1)
//			{
//				it = vehs_map.find(veh_id_to_remove);
//				if (it != vehs_map.end()) 
//				{
//					vehs_map.erase(it);
//					cout << endl << "####################################################################################" << endl;
//					cout << "############  Removed from vehsMap vehicle_id =" << veh_id_to_remove << "     ############" << endl<<endl;
//					veh_id_to_remove = -1; //reset this variable
//				}
//			}
//
//		}
//	}
//}




//
//bool getInput(char *c)
//{
//	if (_kbhit())
//	{
//		*c = _getch();
//		return true; // Key Was Hit
//	}
//	return false; // No keys were pressed
//}