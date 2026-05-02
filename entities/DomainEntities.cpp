// ============================================================
// FILE: DomainEntities.cpp
// Defines static member variables for: Stop, Route, Ticket, Passenger
// These CANNOT be inline (requires GCC 7+)
// Putting them here in a .cpp file works with ALL GCC versions
// ============================================================

#include "DomainEntities.h"

// [OOP: Static members must be defined ONCE in exactly ONE .cpp file]
// The declaration is in the header (static int s_nextStopId;)
// The DEFINITION (with initial value) goes here

int Stop::s_nextStopId        = 1;
int Route::s_nextRouteId      = 1;
int Ticket::s_nextTicketId    = 1;
int Passenger::s_nextPassengerId = 1;
