
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>

#include <string> 
#include <thread>
#include <iostream> //cout
#include <ctime> //time format
#include <conio.h> //key press
#include <map>	//map - list of vehicles




#include "MriTypeSupportImpl.h"
#include "DataWriter_Aux2Strings.h"
#include "DataReader_Aux2Strings.h"
#include "DataReader_V2XMessage.h"
#include "DataWriter_V2XMessage.h"
#include "DataReader_VehData.h"

#include "QueueTs.h"
#include "TimeSync.h"
#include "OpenDDS.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;


Mri::Aux2StringsDataWriter_var  writer_global_aux2strings;
Mri::V2XMessageDataWriter_var  writer_global_v2xmessage;
long veh_id_to_remove;

QueueTs<Mri::VehData> vehdata_queue_in;
std::map<long, Mri::VehData> vehs_map;

//std::queue<Mri::V2XMessage> v2x_queue_in;
QueueTs<Mri::V2XMessage> v2x_queue_in;
std::map<long, Mri::V2XMessage> v2xs_map;


bool finish_application;
bool threadOpenDDS_initialized;

long timestamp_previous;			//used by generateV2xUniqueTimestamp
long sample_counter_per_timestamp;	//used by generateV2xUniqueTimestamp




void vehsMapThread() {
	

	Mri::VehData _veh;
	
	//std::map<long, Mri::VehData> vehs_map;
	std::map<long, Mri::VehData>::iterator it;
	
	veh_id_to_remove = -1; // initial value
	
	while (!finish_application)
	{
		//wait for something at the queue
		vehdata_queue_in.pop(_veh);
		{
			it = vehs_map.find(_veh.vehicle_id);
			if (it != vehs_map.end()) {
				//there is a car with id = _veh.vehicle_id


				//check if new data is older than in vehs_map
				if (_veh.timestamp >  it->second.timestamp)
				{
					//update data in vehs_map
					vehs_map[it->first] = _veh;
				}
				else
				{
					cout << "OpenDDS data is older than data in vehs_list. Veh_id=" << _veh.vehicle_id
						<< " timestamp_vehs_list=" << it->second.timestamp
						<< " timestamp_OpenDDS=" << _veh.timestamp << endl;
				}
			}
			else
			{
				//no car with id = _veh.vehicle_id
				vehs_map.emplace(_veh.vehicle_id, _veh);

			}

			//check if we have to remove a car
			if (veh_id_to_remove!=-1)
			{
				it = vehs_map.find(veh_id_to_remove);
				if (it != vehs_map.end()) 
				{
					vehs_map.erase(it);
					cout << endl << "####################################################################################" << endl;
					cout << "############  Removed from vehsMap vehicle_id =" << veh_id_to_remove << "     ############" << endl<<endl;
					veh_id_to_remove = -1; //reset this variable
				}
			}

		}
	}
}








void OpenDDSThread(int argc, char* argv[]){
	// start thread
	


	//char key = ' ';

	DDS::DomainId_t DomainID{ 246 };

	const char * topic_traffic_vehs_name = "Mri_TrafficVeh";
	const char * topic_subject_car_name = "Mri_SubjectCar";
	
	//initial value
	timestamp_previous = 0;	
	


	try
	{
		std::thread threadTimestamp(TimestampThread);
		//std::cout << "*****************   Timestamp: " << GetTimestamp() << std::endl << std::endl;

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
		DataReader_VehData reader_traffic_vehs(participant, subscriber, "Mri_TrafficVeh");
		DataReader_VehData reader_subject_car(participant, subscriber, "Mri_SubjectCar");

		// writer
		DataWriter_V2XMessage writer_v2xMessage(participant, publisher, "Mri_V2XfromNS3");
		writer_global_v2xmessage = writer_v2xMessage.msg_writer;

		//create reader to receive V2X message  Mri_V2XfromNS3  Mri_V2XtoNS3
		DataReader_V2XMessage reader_v2xmessage(participant, subscriber, "Mri_V2XtoNS3");
		

		



		long old_veh_timestamp = 0;
		std::map<long, Mri::VehData> vehs_map_copy;
		
		threadOpenDDS_initialized = true;

		//----------------------------------------------------------------------------------------------------------


		//while (key != 'q')
		while (true)
		{	
			//getInput(&key);

			Sleep(100);
			// if info about veh wasn't updated for 500 ms it means this vehicle dissapeared, so we have to delete info about this vehicle
			old_veh_timestamp = GetTimestamp() - 50;	//to find veh data not updated for 50 x 10ms = 500 ms
			if (vehs_map.size() > 0)
			{
				vehs_map_copy = vehs_map;

				for (auto& x : vehs_map_copy) {
					if (x.second.timestamp < old_veh_timestamp)
					{
						//select this veh_id to removing
						veh_id_to_remove = x.second.vehicle_id;

					}
					//std::cout << "timestamp=" << x.second.timestamp << " veh_id=" << x.second.vehicle_id << " x=" << x.second.position_x << " y=" << x.second.position_y << std::endl;
				}
				//std::cout << std::endl << std::endl;
			}
		}

		


		//-------------------------------------------------------------------------------------------------
		// Clean-up!
		participant->delete_contained_entities();
		dpf->delete_participant(participant);

		TheServiceParticipant->shutdown();
		
		threadTimestamp.detach();
		finish_application = true; 
		
	}
	catch (const CORBA::Exception& e) {
		e._tao_print_exception("Exception caught in OpenDDSThread:");
		
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