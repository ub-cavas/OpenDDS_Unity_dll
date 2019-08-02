#pragma once


#define PI 3.14159265359


////used to control threads' access to unityVehsMapSet
//#define ACCESS_unityVehsMapThread 1		//access for unityVehsMapThread
//#define ACCESS_getVehsArray 2
//#define ACCESS_request_getVehsArray 3 //request to get access for unityVehsMapThread
//#define ACCESS_none 4
 



struct UnityVehicle
{
	long  timestamp;				//tick every 10 ms*, all applications have to be synchronized ; Mri
	long  vehicle_id;
	long  vehicle_type;				/* vehicle type number from VISSIM */

	long  color;					/* RGB */
	float position_x;				/* in m */
	float position_y;				/* in m */
	float position_z;				/* in m */
	float orient_heading;			/* in radians */
	float orient_pitch;			/* in radians */
	float orient_roll;				//in radians ; Mri
	float speed;					/* in m/s */
	long leading_vehicle_id;		/* relevant vehicle in front */
	long trailing_vehicle_id;		/* next vehicle back on the same lane */
	long link_id;

	float link_coordinate;			/* in m */
	long  lane_index;				/* 0 = rightmost */
	long  turning_indicator;		/* 1 = left, 0 = none, -1 = right */

	bool operator < (const UnityVehicle& pt) const
	{
		return timestamp < pt.timestamp;
	}
};

#define _RD_dll __declspec (dllexport)

extern "C" _RD_dll bool start_opendds();

extern "C" _RD_dll void stop_opendds();


extern "C" _RD_dll void getVehsArray(int * Num_Vehicles, UnityVehicle ** VehicleData);

extern "C" _RD_dll void updateSubjectCarLocation( float pos_x, float pos_y, float pos_z, float heading, float pitch, float roll,float speed);

extern "C" _RD_dll void updateBrakeStatus(float brake); // get brake value (between 0 and 1) from Unity. Later we need to convert this float value to: BRAKE_BOOST_UNAVAILABLE = 0;BRAKE_BOOST_OFF = 1; BRAKE_BOOST_ON = 2;

extern "C" _RD_dll float GetDnpwDistance();

extern "C" _RD_dll float GetEeblDistance();

extern "C" _RD_dll float GetIwTime();




//UnityVehicle interpolateVehPosition(std::set<UnityVehicle>* _set, long x_timestamp);

UnityVehicle interpolateVehPosition(std::set<UnityVehicle>* _set, long x_timestamp, long x_elapsed_microseconds);

UnityVehicle lerpRD(UnityVehicle * u_prev, UnityVehicle * u_next, long x_timestamp, long elapsedMicrosecond);




//UnityVehicle lerpRD(UnityVehicle * u_prev, UnityVehicle * u_next, long x_timestamp);

