#pragma once

//bool getInput(char * c);
#include "MriTypeSupportImpl.h"
#include "Start.h"


//void convertUnityVeh(Mri::VehData * veh, UnityVehicle * uVeh);

UnityVehicle convertUnityVeh(Mri::VehData veh);

void publishSubjectCarLocationThread();

Mri::VehData getSubjectCarPosition();

void unityVehsMapThread();

//void vehsMapThread();

void OpenDDSThread(int argc, char * argv[]);





long generateV2xUniqueTimestamp(long v2x_timestamp);

void addV2xToMap(long extended_timestamp, Mri::V2XMessage v2x);

void transmitV2X(Mri::V2XMessage v2x, long destination_veh_id);

void publishVehDataMessage(Mri::VehData car);

void publishV2xMessage(Mri::V2XMessage v2x);

void ProcessDoNotPassWarningMessage(Mri::Aux2Strings dnpwAux);







