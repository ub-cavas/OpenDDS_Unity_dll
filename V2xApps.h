#pragma once

#include <string> 

#include "MriTypeSupportImpl.h"


struct BSMCoreData
{

	long vehicle_id;
	double position_x;
	double position_y;
	double position_z;
	double orient_heading;
	double speed;
	long brake_boost;	// 0 - unavailable, 1 - off, 2 - on;

};

struct Point2D
{
	double x;
	double y;
};


BSMCoreData readVehDatafromString(std::string message);

Point2D RotatePoint(Point2D point, double angle);

float sign(Point2D p1, Point2D p2, Point2D p3);

bool PointInTriangle(Point2D pt, Point2D v1, Point2D v2, Point2D v3);

float doNotPassWarning(double h_x, double h_y, double h_h, double t_x, double t_y, double t_h);

void sendV2X(long sender_id, long sender_timestamp, std::string message);

std::string createBSMcoreData(Mri::VehData veh, float brakeForce);

//void sendDNPWMessage(float distance_meters, long receiverAppId);

const float BRAKE_BOOST_THRESHOLD = 0.9f;	//if brake force is greater than threshold -> send brake_boost = 2 in BSM .  If not greater -> send 1
