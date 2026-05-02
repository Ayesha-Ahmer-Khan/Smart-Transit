// ============================================================
// FILE: ExtendedDomainEntities.cpp
// Defines static members for: FuelLog, Incident, MaintenanceJob,
//                              ChargingStation, Trip
// ============================================================

#include "ExtendedDomainEntities.h"

int FuelLog::s_nextLogId            = 1;
int Incident::s_nextIncidentId      = 1;
int MaintenanceJob::s_nextJobId     = 1;
int ChargingStation::s_nextStationId = 1;
int Trip::s_nextTripId              = 1;
