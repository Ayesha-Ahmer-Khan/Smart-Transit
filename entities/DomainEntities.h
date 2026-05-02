#pragma once
// ============================================================
// FILE: DomainEntities.h
// MODULE: 3 - Supporting Domain Entities
// OUTPUT-ID: M3-DOM-001
// Expected Output: Stop/Route/Ticket/Passenger info
// OOP CONCEPTS: Composition, enum class, operator overloading
// ============================================================

#include "../utilities/CustomString.h"
#include "../utilities/CustomDate.h"
#include "../utilities/CustomTime.h"
#include "../utilities/GeoCoordinate.h"
#include "../datastructs/DynamicArray.h"
#include "../datastructs/LinkedList.h"
#include <iostream>

// ============================================================
// STOP: A bus/metro stop in the city
// ============================================================
class Stop {
private:
    int          m_stopId;
    CustomString m_stopName;
    GeoCoordinate m_position;
    bool         m_hasShelter;

    static int s_nextStopId;

public:
    Stop() : m_stopId(s_nextStopId++), m_stopName("Unknown"),
             m_hasShelter(false) {}

    Stop(const CustomString& name, double lat, double lon, bool shelter = false)
        : m_stopId(s_nextStopId++), m_stopName(name),
          m_position(lat, lon), m_hasShelter(shelter) {}

    Stop(const Stop& other)
        : m_stopId(other.m_stopId), m_stopName(other.m_stopName),
          m_position(other.m_position), m_hasShelter(other.m_hasShelter) {}
    Stop& operator=(const Stop& other) {
        if (this == &other) return *this;
        m_stopId    = other.m_stopId;
        m_stopName  = other.m_stopName;
        m_position  = other.m_position;
        m_hasShelter= other.m_hasShelter;
        return *this;
    }

    // Getters
    int           getStopId()   const { return m_stopId;   }
    CustomString  getStopName() const { return m_stopName; }
    GeoCoordinate getPosition() const { return m_position; }
    bool          hasShelter()  const { return m_hasShelter; }

    // Operators
    bool operator==(const Stop& other) const { return m_stopId == other.m_stopId; }
    bool operator!=(const Stop& other) const { return !(*this == other);          }
    bool operator< (const Stop& other) const { return m_stopId < other.m_stopId;  }

    friend std::ostream& operator<<(std::ostream& os, const Stop& s) {
        char buf[200];
        std::snprintf(buf, sizeof(buf), "Stop[%d] %s @ %s Shelter:%s",
                 s.m_stopId, s.m_stopName.c_str(),
                 s.m_position.toString().c_str(),
                 s.m_hasShelter ? "Yes" : "No");
        os << buf;
        return os;
    }
};

// Static member definition

// ============================================================
// ROUTE: A named transit route with a sequence of stops
// ============================================================
class Route {
private:
    int          m_routeId;
    CustomString m_routeName;
    LinkedList<Stop> m_stops;      // Ordered list of stops
    double       m_totalDistanceKm;
    int          m_frequencyMins;  // How often buses run
    bool         m_isActive;

    static int s_nextRouteId;

public:
    Route() : m_routeId(s_nextRouteId++), m_routeName("Unnamed"),
              m_totalDistanceKm(0.0), m_frequencyMins(30), m_isActive(true) {}

    Route(const CustomString& name, int frequencyMins)
        : m_routeId(s_nextRouteId++), m_routeName(name),
          m_totalDistanceKm(0.0), m_frequencyMins(frequencyMins), m_isActive(true) {}

    // Add a stop to the end of this route
    void addStop(const Stop& stop) {
        if (m_stops.size() > 0) {
            // Add distance from last stop to this new stop
            Stop& lastStop = m_stops[m_stops.size() - 1];
            m_totalDistanceKm += lastStop.getPosition().distanceTo(stop.getPosition());
        }
        m_stops.append(stop);
    }

    // Remove a stop by ID
    void removeStop(int stopId) {
        for (int i = 0; i < m_stops.size(); i++) {
            if (m_stops[i].getStopId() == stopId) {
                m_stops.remove(m_stops[i]);
                return;
            }
        }
    }

    // Getters
    int          getRouteId()          const { return m_routeId;           }
    CustomString getRouteName()        const { return m_routeName;         }
    int          getTotalStops()       const { return m_stops.size();      }
    double       getTotalDistanceKm()  const { return m_totalDistanceKm;   }
    int          getFrequencyMins()    const { return m_frequencyMins;     }
    bool         isActive()            const { return m_isActive;          }

    void activate()   { m_isActive = true;  }
    void deactivate() { m_isActive = false; }

    // Get stop at position i
    Stop& getStop(int index) { return m_stops[index]; }

    bool operator==(const Route& other) const { return m_routeId == other.m_routeId; }

    friend std::ostream& operator<<(std::ostream& os, const Route& r) {
        char buf[200];
        std::snprintf(buf, sizeof(buf), "Route[%d] '%s' Stops:%d Dist:%.1fkm Freq:%dmin Active:%s",
                 r.m_routeId, r.m_routeName.c_str(), r.m_stops.size(),
                 r.m_totalDistanceKm, r.m_frequencyMins, r.m_isActive ? "Yes" : "No");
        os << buf;
        return os;
    }
};


// ============================================================
// TICKET TYPE
// ============================================================
enum class TicketType {
    SINGLE_JOURNEY,
    DAY_PASS,
    MONTHLY_PASS,
    STUDENT_CONCESSION
};

// ============================================================
// TICKET: A single ticket issued to a passenger
// ============================================================
class Ticket {
private:
    int          m_ticketId;
    int          m_passengerId;
    int          m_tripId;
    double       m_fareAmount;
    CustomDate   m_issueDate;
    CustomTime   m_issueTime;
    TicketType   m_type;
    bool         m_isValidated;  // Has it been used?

    static int s_nextTicketId;

public:
    Ticket()
        : m_ticketId(s_nextTicketId++), m_passengerId(-1), m_tripId(-1),
          m_fareAmount(0.0), m_issueDate(CustomDate::today()),
          m_issueTime(CustomTime::now()), m_type(TicketType::SINGLE_JOURNEY),
          m_isValidated(false) {}

    Ticket(int passengerId, int tripId, double fare, TicketType type)
        : m_ticketId(s_nextTicketId++),
          m_passengerId(passengerId), m_tripId(tripId),
          m_fareAmount(fare),
          m_issueDate(CustomDate::today()), m_issueTime(CustomTime::now()),
          m_type(type), m_isValidated(false) {}

    // Getters
    int        getTicketId()     const { return m_ticketId;     }
    int        getPassengerId()  const { return m_passengerId;  }
    int        getTripId()       const { return m_tripId;       }
    double     getFareAmount()   const { return m_fareAmount;   }
    TicketType getType()         const { return m_type;         }
    bool       isValidated()     const { return m_isValidated;  }

    // Validate the ticket (mark as used)
    void validate() { m_isValidated = true; }

    CustomString getTypeString() const {
        switch (m_type) {
            case TicketType::SINGLE_JOURNEY:     return CustomString("Single Journey");
            case TicketType::DAY_PASS:           return CustomString("Day Pass");
            case TicketType::MONTHLY_PASS:       return CustomString("Monthly Pass");
            case TicketType::STUDENT_CONCESSION: return CustomString("Student");
            default:                             return CustomString("Unknown");
        }
    }

    bool operator==(const Ticket& other) const { return m_ticketId == other.m_ticketId; }

    friend std::ostream& operator<<(std::ostream& os, const Ticket& t) {
        char buf[300];
        std::snprintf(buf, sizeof(buf),
                 "Ticket[%d] Passenger:%d Trip:%d Fare:Rs%.2f Type:%s Validated:%s",
                 t.m_ticketId, t.m_passengerId, t.m_tripId,
                 t.m_fareAmount, t.getTypeString().c_str(),
                 t.m_isValidated ? "YES" : "NO");
        os << buf;
        return os;
    }
};


// ============================================================
// PASSENGER: A person who travels using the system
// ============================================================
class Passenger {
private:
    int          m_passengerId;
    CustomString m_name;
    CustomString m_contactNumber;
    int          m_loyaltyPoints;
    LinkedList<int> m_travelHistory;  // List of trip IDs

    static int s_nextPassengerId;

public:
    Passenger()
        : m_passengerId(s_nextPassengerId++),
          m_name("Guest"), m_loyaltyPoints(0) {}

    Passenger(const CustomString& name, const CustomString& contact)
        : m_passengerId(s_nextPassengerId++),
          m_name(name), m_contactNumber(contact), m_loyaltyPoints(0) {}

    // Getters
    int          getPassengerId()   const { return m_passengerId;   }
    CustomString getName()          const { return m_name;          }
    int          getLoyaltyPoints() const { return m_loyaltyPoints; }

    // Loyalty system
    void addLoyaltyPoints(int pts)   { m_loyaltyPoints += pts;  }
    bool redeemPoints(int pts) {
        if (pts > m_loyaltyPoints) return false;  // Not enough points
        m_loyaltyPoints -= pts;
        return true;
    }

    void recordTrip(int tripId) { m_travelHistory.append(tripId); }
    int  getTripCount() const   { return m_travelHistory.size();  }

    bool operator==(const Passenger& other) const {
        return m_passengerId == other.m_passengerId;
    }

    friend std::ostream& operator<<(std::ostream& os, const Passenger& p) {
        char buf[200];
        std::snprintf(buf, sizeof(buf), "Passenger[%d] %s Trips:%d Points:%d",
                 p.m_passengerId, p.m_name.c_str(),
                 p.m_travelHistory.size(), p.m_loyaltyPoints);
        os << buf;
        return os;
    }
};

