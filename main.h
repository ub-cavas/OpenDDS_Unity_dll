#pragma once

void v2xsMapGarbageCollection(long interval_ms);

void doSimulation();

long generateV2xUniqueTimestamp(long v2x_timestamp);

void addV2xToMap(long extended_timestamp, Mri::V2XMessage v2x);

void transmitV2X(Mri::V2XMessage v2x, long destination_veh_id);
