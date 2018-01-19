#pragma once

//bool getInput(char * c);
#include "MriTypeSupportImpl.h"


void vehsMapThread();

void OpenDDSThread(int argc, char * argv[]);





long generateV2xUniqueTimestamp(long v2x_timestamp);

void addV2xToMap(long extended_timestamp, Mri::V2XMessage v2x);

void transmitV2X(Mri::V2XMessage v2x, long destination_veh_id);

void publishV2xMessage(Mri::V2XMessage v2x);







