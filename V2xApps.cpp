#include "V2xApps.h"

#include <sstream>      // std::stringstream, std::stringbuf
#include <iostream> //cout

#include "MriTimeSync.h"


using std::cerr;
using std::cout;
using std::endl;
using std::string;

#define PI 3.14159265359
extern long THIS_APP_ID;




////convert BSM text to VehData struct
//Mri::VehData readVehDatafromString(std::string message) {
//
//
//		Mri::VehData _veh;
//		std::stringstream  lineStream(message);
//		std::string cell;
//
//		std::getline(lineStream, cell, ';');
//		_veh.orient_heading = std::atof(cell.c_str());
//	
//		std::getline(lineStream, cell, ';');
//		_veh.position_x = std::atof(cell.c_str());
//
//		std::getline(lineStream, cell, ';');
//		_veh.position_y = std::atof(cell.c_str());
//
//		std::getline(lineStream, cell, ';');
//		_veh.position_z = std::atof(cell.c_str());
//
//		std::getline(lineStream, cell, ';');
//		_veh.speed = std::atof(cell.c_str());
//
//		std::getline(lineStream, cell, ';');
//		_veh.vehicle_id = std::atol(cell.c_str());
//
//		return _veh;
//
//
//
//}



//convert BSM text to VehData struct
BSMCoreData readVehDatafromString(std::string message) {

	BSMCoreData _bsm;

	//Mri::VehData _veh;
	std::stringstream  lineStream(message);
	std::string cell;

	std::getline(lineStream, cell, ';');
	_bsm.orient_heading = std::atof(cell.c_str());

	std::getline(lineStream, cell, ';');
	_bsm.position_x = std::atof(cell.c_str());

	std::getline(lineStream, cell, ';');
	_bsm.position_y = std::atof(cell.c_str());

	std::getline(lineStream, cell, ';');
	_bsm.position_z = std::atof(cell.c_str());

	std::getline(lineStream, cell, ';');
	_bsm.speed = std::atof(cell.c_str());

	std::getline(lineStream, cell, ';');
	_bsm.vehicle_id = std::atol(cell.c_str());

	
	// 0 - unavailable, 1 - off, 2 - on;
	//_bsm.brake_boost = 0; //initial value
	cell = "0"; //initial value
	//try to read real value from bsm

	//if there is no brake_bost value in the bsm, cell remains unchanged ->   is 0
	std::getline(lineStream, cell, ';');
	_bsm.brake_boost = std::atol(cell.c_str());

	return _bsm;



}



//returns position of point after rotation
Point2D RotatePoint(Point2D point, double angle)
{
	Point2D _point;
	_point.x = point.x * cos(angle) - point.y * sin(angle);
	_point.y = point.y * cos(angle) + point.x * sin(angle);

	return _point;
};

// helper for PointInTriangle function
float sign(Point2D p1, Point2D p2, Point2D p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

//checks if point pt is inside a triangle v1,v2,v3
bool PointInTriangle(Point2D pt, Point2D v1, Point2D v2, Point2D v3)
{
	bool b1, b2, b3;

	b1 = sign(pt, v1, v2) < 0.0f;
	b2 = sign(pt, v2, v3) < 0.0f;
	b3 = sign(pt, v3, v1) < 0.0f;

	return ((b1 == b2) && (b2 == b3));
}





//checks if tested vehicle (t_) is on the opposite lane on the road
//if it does, return distance from human veh and the other('tested") veh
//if it doesn't return -1   which means no warning
float doNotPassWarning(double h_x, double h_y, double h_h, double t_x, double t_y, double t_h)
{
	// h_  human controlled veh
	// t_  tested veh

	//no warning -> distance = -1
	float distance = -1;


	//normalization
	h_h = fmod((h_h + (2 * PI)), (2 * PI));
	t_h = fmod((t_h + (2 * PI)), (2 * PI));


	//20181018
	//double alpha_right = h_h + (0.65 * PI);
	//alpha_right = fmod((alpha_right + (2 * PI)), (2 * PI));
	////double alpha_left = h_h - (0.65 * PI);
	//double alpha_left = h_h + (1.35 * PI);
	//alpha_left = fmod((alpha_left + (2 * PI)), (2 * PI));
	

	

	//if ((t_h < alpha_left && t_h > alpha_right) || ((alpha_left<(-PI / 2) && alpha_right>(PI / 2)) && (t_h < alpha_left || t_h > alpha_right)))



	if  ((abs(h_h-t_h) > (0.65 * PI)) && (abs(h_h - t_h) < (1.35 * PI)))
	{
		Point2D a_oryg = { 0,11 };
		Point2D b_oryg = { 260,18 };
		Point2D c_oryg = { 260,-3 };
		Point2D d_oryg = { 0,-3 };


		Point2D a = RotatePoint(a_oryg, h_h);
		Point2D b = RotatePoint(b_oryg, h_h);
		Point2D c = RotatePoint(c_oryg, h_h);
		Point2D d = RotatePoint(d_oryg, h_h);

		Point2D x_veh_point;


		a = { a.x + h_x, a.y + h_y };
		b = { b.x + h_x, b.y + h_y };
		c = { c.x + h_x, c.y + h_y };
		d = { d.x + h_x, d.y + h_y };

		x_veh_point = { t_x, t_y };

		if (PointInTriangle(x_veh_point, a, b, c) || PointInTriangle(x_veh_point, a, c, d))
		{
			//show warning and distance
			distance = sqrt(pow((t_x - h_x), 2.0) + pow((t_y - h_y), 2.0));

		}
	}


	return distance;

}




// output - float - time to collision in sec. If there is no collision value is -1
float intersectionWarning(double h_x, double h_y, double h_h, double h_speed, double t_x, double t_y, double t_h, double t_speed) {
	
	
	//____________________________________________________________
	//
	//	TEST ONLY
	//

	h_speed = 3;
	t_speed = 3;
	//____________________________________________________________
	
	
	
	
	
	float h_distance_intersection;
	float t_distance_intersection;
	float h_time_intersection;
	float t_time_intersection;

	float timeToCollision = -1; //initial value, -1 means no collision

	//line equation, y = mx + c
	//c = y - mx

	//std::cout << "Degree subject car : " << rad2Degree(h_h);
	float m1 = tan(h_h);
	float c1 = (h_y - m1 * h_x);

	float m2 = tan(t_h);
	float c2 = (t_y - m2 * t_x);

	float x_intercept = ((c2 - c1) / (m1 - m2));
	float y_intercept = (m1*x_intercept + c1);

	//slope of lines formed by intercept and the car.If same direction then point is in 
	//front else direction is opposite and point is at back
	float subjectCarIntercept = fmod((atan2((y_intercept - h_y), (x_intercept - h_x)) + 2 * PI), (2 * PI));

	float otherIntercept = fmod((atan2((y_intercept - t_y), (x_intercept - t_x)) + 2 * PI), (2 * PI));

	if (abs(h_h - t_h) <= 0.08) {
		std::cout << "Parallel cars";
		//outcome = "Parallel cars";
		timeToCollision = -1;
	}
	else {
		if (abs(subjectCarIntercept - h_h) <= 0.08 && abs(otherIntercept - t_h) <= 0.08) {
			h_distance_intersection = sqrt(pow((h_x - x_intercept), 2) + pow((h_y - y_intercept), 2));
			t_distance_intersection = sqrt(pow((t_x - x_intercept), 2) + pow((t_y - y_intercept), 2));

			h_time_intersection = (h_distance_intersection) / h_speed;
			t_time_intersection = (t_distance_intersection) / t_speed;

			if (abs(h_time_intersection - t_time_intersection) > 2) {
				//outcome = "They do not intersect.";
				timeToCollision = -1;
			}
			else
			{
				//std::cout << "Intersection at x : " << x_intercept << "  y : " << y_intercept;
				//outcome = "Intersection at x : " + (std::to_string)(x_intercept) + "  y : " + (std::to_string)(y_intercept);

				timeToCollision = t_time_intersection;

				if (timeToCollision > 5) {
					//we skip this warning
					timeToCollision = -1;
				}

			}
		}
		else {
			//std::cout << "They do not intersect.";
			//outcome = "They do not intersect.";
			timeToCollision = -1;
		}
	}
	return timeToCollision;
}

















//sendV2X(x.second.vehicle_id, GetTimestamp(), message_text);
void sendV2X(long sender_id, long sender_timestamp, string message) {

	Mri::V2XMessage v2x;

	v2x.sender_id = sender_id;
	v2x.sender_timestamp = sender_timestamp;
	v2x.message = message.c_str();
	v2x.recipient_id = -1;
	v2x.recipient_timestamp = -1;

	int success = writer_global_v2xmessage->write(v2x, DDS::HANDLE_NIL);
	if (success != DDS::RETCODE_OK) {
		//ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) ERROR: TimeSync send message write returned %d.\n"), success));
		throw std::string("ERROR: SendV2X message failed");
	}
	else
	{
		//cout << endl <<"     *****  SEND V2X message:" << v2x.message <<  " sender_id=" << v2x.sender_id << " sender_timestamp=" << v2x.sender_timestamp << endl;
	}

}




//converts data from VehData and brake Force veh to BSM text
string createBSMcoreData(Mri::VehData veh, float brakeForce) {


	const int BRAKE_BOOST_UNAVAILABLE = 0;
	const int BRAKE_BOOST_OFF = 1;
	const int BRAKE_BOOST_ON = 2;

	int brakeBoost = 0;
	if (brakeForce >= BRAKE_BOOST_THRESHOLD)
	{
		brakeBoost = BRAKE_BOOST_ON;
	}
	else
	{
		brakeBoost = BRAKE_BOOST_OFF;
	}

	std::stringstream tekst;
	tekst	<< veh.orient_heading << ";" << veh.position_x << ";" << veh.position_y 
			<< ";" << veh.position_z << ";" << veh.speed << ";" << veh.vehicle_id 
			<< ";" << brakeBoost;
	return tekst.str();


}




























//---------------------------------------------------------------
//NOT used now !!
//---------------------------------------------------------------
//sent message DNPW to other applications using OpenDDS 
void sendDNPWMessage(float distance_meters, long receiverAppId) {

	// DNPW - Do Not Pass Warning

	Mri::Aux2Strings auxMessage;
	//long distance_feets = distance_meters / 3.28084;

	std::string s =  std::to_string((long) distance_meters) ;

	auxMessage.receiverId = receiverAppId;
	auxMessage.senderId = THIS_APP_ID;
	auxMessage.tag = "dnpw";
	auxMessage.str1 = s.c_str();
	auxMessage.str2 = "";

	int success = writer_global_aux2strings->write(auxMessage, DDS::HANDLE_NIL);
	if (success != DDS::RETCODE_OK) {
		//ACE_ERROR((LM_ERROR, ACE_TEXT("(%P|%t) ERROR: TimeSync send message write returned %d.\n"), success));
		throw std::string("ERROR: sending DoNotPassWarning failed");
	}
	//sender.sendMessage(auxMessage);

	//std::cout << "Send timesync message at: " << GetTimestamp() << std::endl << std::endl;
	
}



