#pragma once
// ============================================================
// FILE: ExtendedDomainEntities.h
// MODULE: 3 - Supporting Domain Entities (Extended Set)
// OUTPUT-ID: M3-EXT-001
// Expected Output: Fuel logs, incidents, maintenance jobs, stations
// OOP CONCEPTS: operator* for FuelLog total cost, enum class severity
// ============================================================

#include "../utilities/CustomString.h"
#include "../utilities/CustomDate.h"
#include "../utilities/CustomTime.h"
#include "../utilities/GeoCoordinate.h"
#include "../datastructs/DynamicArray.h"
#include <iostream>
#include <cstdio>

// ============================================================
// FUELLOG: Records a single fuel fill-up for a vehicle
// ============================================================
class FuelLog {
private:
    int          m_logId;
    int          m_vehicleId;
    double       m_litresFilled;
    double       m_costPerLitre;
    double       m_totalCost;
    double       m_odometerAtFill;   // Odometer reading when filled
    CustomDate   m_fillDate;

    static int s_nextLogId;

public:
    FuelLog()
        : m_logId(s_nextLogId++), m_vehicleId(-1),
          m_litresFilled(0.0), m_costPerLitre(0.0),
          m_totalCost(0.0), m_odometerAtFill(0.0),
          m_fillDate(CustomDate::today()) {}

    FuelLog(int vehicleId, double litres, double costPerLitre, double odometer)
        : m_logId(s_nextLogId++), m_vehicleId(vehicleId),
          m_litresFilled(litres), m_costPerLitre(costPerLitre),
          m_totalCost(litres * costPerLitre),    // Auto-calculate total
          m_odometerAtFill(odometer),
          m_fillDate(CustomDate::today()) {}

    // Getters
    int    getLogId()          const { return m_logId;          }
    int    getVehicleId()      const { return m_vehicleId;      }
    double getLitresFilled()   const { return m_litresFilled;   }
    double getCostPerLitre()   const { return m_costPerLitre;   }
    double getTotalCost()      const { return m_totalCost;      }
    double getOdometerAtFill() const { return m_odometerAtFill; }

    // [OOP: operator* computes total cost: fuelLog * multiplier]
    double operator*(double multiplier) const {
        return m_totalCost * multiplier;
    }

    bool operator==(const FuelLog& other) const { return m_logId == other.m_logId; }

    friend std::ostream& operator<<(std::ostream& os, const FuelLog& f) {
        char buf[250];
        std::snprintf(buf, sizeof(buf),
                "FuelLog[%d] Vehicle:%d Date:%s Litres:%.1f @ Rs%.2f/L = Rs%.2f Odometer:%.0fkm",
                f.m_logId, f.m_vehicleId, f.m_fillDate.toString().c_str(),
                f.m_litresFilled, f.m_costPerLitre, f.m_totalCost, f.m_odometerAtFill);
        os << buf;
        return os;
    }
};


// ============================================================
// INCIDENT SEVERITY
// ============================================================
enum class IncidentSeverity {
    LOW,      // Minor issue, no immediate action needed
    MEDIUM,   // Needs attention within 24 hours
    HIGH,     // Urgent, requires immediate response
    CRITICAL  // Emergency, all units respond
};

// ============================================================
// INCIDENT: Any unplanned event during operations
// ============================================================
class Incident {
private:
    int              m_incidentId;
    int              m_vehicleId;
    int              m_driverId;
    CustomString     m_description;
    GeoCoordinate    m_location;
    CustomDate       m_date;
    CustomTime       m_time;
    IncidentSeverity m_severity;
    bool             m_isResolved;

    static int s_nextIncidentId;

public:
    Incident()
        : m_incidentId(s_nextIncidentId++), m_vehicleId(-1), m_driverId(-1),
          m_description("Unknown incident"), m_severity(IncidentSeverity::LOW),
          m_isResolved(false) {}

    Incident(int vehicleId, int driverId, const CustomString& desc,
             const GeoCoordinate& loc, IncidentSeverity severity)
        : m_incidentId(s_nextIncidentId++),
          m_vehicleId(vehicleId), m_driverId(driverId),
          m_description(desc), m_location(loc),
          m_date(CustomDate::today()), m_time(CustomTime::now()),
          m_severity(severity), m_isResolved(false) {}

    // Getters
    int              getIncidentId() const { return m_incidentId; }
    int              getVehicleId()  const { return m_vehicleId;  }
    IncidentSeverity getSeverity()   const { return m_severity;   }
    bool             isResolved()    const { return m_isResolved;  }
    CustomString     getDescription()const { return m_description; }

    void resolve() { m_isResolved = true; }

    CustomString getSeverityString() const {
        switch (m_severity) {
            case IncidentSeverity::LOW:      return CustomString("LOW");
            case IncidentSeverity::MEDIUM:   return CustomString("MEDIUM");
            case IncidentSeverity::HIGH:     return CustomString("HIGH");
            case IncidentSeverity::CRITICAL: return CustomString("CRITICAL");
            default:                         return CustomString("UNKNOWN");
        }
    }

    bool operator==(const Incident& other) const {
        return m_incidentId == other.m_incidentId;
    }

    friend std::ostream& operator<<(std::ostream& os, const Incident& inc) {
        char buf[300];
        std::snprintf(buf, sizeof(buf),
                "Incident[%d] Vehicle:%d Driver:%d Severity:%s Resolved:%s\n  Desc: %s",
                inc.m_incidentId, inc.m_vehicleId, inc.m_driverId,
                inc.getSeverityString().c_str(),
                inc.m_isResolved ? "YES" : "NO",
                inc.m_description.c_str());
        os << buf;
        return os;
    }
};


// ============================================================
// MAINTENANCE JOB STATUS
// ============================================================
enum class JobStatus {
    SCHEDULED,
    IN_PROGRESS,
    COMPLETED,
    CANCELLED
};

// ============================================================
// MAINTENANCEJOB: A repair or service task for a vehicle
// ============================================================
class MaintenanceJob {
private:
    int                      m_jobId;
    int                      m_vehicleId;
    int                      m_technicianId;
    CustomString             m_description;
    CustomDate               m_scheduledDate;
    CustomDate               m_completedDate;
    DynamicArray<CustomString> m_parts;      // Parts needed for this job
    double                   m_cost;
    JobStatus                m_status;

    static int s_nextJobId;

public:
    MaintenanceJob()
        : m_jobId(s_nextJobId++), m_vehicleId(-1), m_technicianId(-1),
          m_description("General maintenance"), m_scheduledDate(CustomDate::today()),
          m_completedDate(CustomDate::today()), m_cost(0.0), m_status(JobStatus::SCHEDULED) {}

    MaintenanceJob(int vehicleId, int techId, const CustomString& desc,
                   const CustomDate& scheduledDate)
        : m_jobId(s_nextJobId++), m_vehicleId(vehicleId), m_technicianId(techId),
          m_description(desc), m_scheduledDate(scheduledDate),
          m_completedDate(CustomDate::today()), m_cost(0.0), m_status(JobStatus::SCHEDULED) {}

    // Getters
    int          getJobId()       const { return m_jobId;       }
    int          getVehicleId()   const { return m_vehicleId;   }
    JobStatus    getStatus()      const { return m_status;      }
    double       getCost()        const { return m_cost;        }
    CustomString getDescription() const { return m_description; }

    void addPart(const CustomString& part) { m_parts.append(part); }
    void setCost(double cost) { m_cost = cost; }

    void startJob() {
        m_status = JobStatus::IN_PROGRESS;
    }

    void completeJob(double finalCost) {
        m_cost          = finalCost;
        m_status        = JobStatus::COMPLETED;
        m_completedDate = CustomDate::today();
    }

    CustomString getStatusString() const {
        switch (m_status) {
            case JobStatus::SCHEDULED:   return CustomString("SCHEDULED");
            case JobStatus::IN_PROGRESS: return CustomString("IN PROGRESS");
            case JobStatus::COMPLETED:   return CustomString("COMPLETED");
            case JobStatus::CANCELLED:   return CustomString("CANCELLED");
            default:                     return CustomString("UNKNOWN");
        }
    }

    bool operator==(const MaintenanceJob& other) const {
        return m_jobId == other.m_jobId;
    }

    friend std::ostream& operator<<(std::ostream& os, const MaintenanceJob& j) {
        char buf[300];
        std::snprintf(buf, sizeof(buf),
                "MaintenanceJob[%d] Vehicle:%d Tech:%d Status:%s Cost:Rs%.2f\n  Desc: %s",
                j.m_jobId, j.m_vehicleId, j.m_technicianId,
                j.getStatusString().c_str(), j.m_cost, j.m_description.c_str());
        os << buf;
        return os;
    }
};


// ============================================================
// CHARGINGSTATION: For electric vehicles
// ============================================================
class ChargingStation {
private:
    int                    m_stationId;
    GeoCoordinate          m_location;
    int                    m_totalSlots;
    int                    m_availableSlots;
    double                 m_powerOutputKW;      // How fast it charges
    DynamicArray<int>      m_currentVehicleIds;  // Which vehicles are charging

    static int s_nextStationId;

public:
    ChargingStation()
        : m_stationId(s_nextStationId++), m_totalSlots(4),
          m_availableSlots(4), m_powerOutputKW(50.0) {}

    ChargingStation(const GeoCoordinate& loc, int slots, double powerKW)
        : m_stationId(s_nextStationId++), m_location(loc),
          m_totalSlots(slots), m_availableSlots(slots), m_powerOutputKW(powerKW) {}

    // Getters
    int           getStationId()       const { return m_stationId;       }
    int           getTotalSlots()      const { return m_totalSlots;      }
    int           getAvailableSlots()  const { return m_availableSlots;  }
    double        getPowerOutputKW()   const { return m_powerOutputKW;   }
    GeoCoordinate getLocation()        const { return m_location;        }
    bool          hasAvailableSlot()   const { return m_availableSlots > 0; }

    // Connect a vehicle to charge
    bool connectVehicle(int vehicleId) {
        if (m_availableSlots <= 0) return false;
        m_currentVehicleIds.append(vehicleId);
        m_availableSlots--;
        return true;
    }

    // Disconnect a vehicle when done
    void disconnectVehicle(int vehicleId) {
        m_currentVehicleIds.remove(vehicleId);
        m_availableSlots++;
        if (m_availableSlots > m_totalSlots) m_availableSlots = m_totalSlots;
    }

    // Estimate charge time for a vehicle at given battery level
    double estimateChargeTimeHours(int currentBatteryPercent, double batteryCapacityKWh) const {
        double neededKWh = batteryCapacityKWh * (100 - currentBatteryPercent) / 100.0;
        return neededKWh / m_powerOutputKW;
    }

    bool operator==(const ChargingStation& other) const {
        return m_stationId == other.m_stationId;
    }

    friend std::ostream& operator<<(std::ostream& os, const ChargingStation& cs) {
        char buf[250];
        std::snprintf(buf, sizeof(buf),
                "ChargingStation[%d] Location:%s Slots:%d/%d Power:%.0fkW",
                cs.m_stationId, cs.m_location.toString().c_str(),
                cs.m_availableSlots, cs.m_totalSlots, cs.m_powerOutputKW);
        os << buf;
        return os;
    }
};


// ============================================================
// TRIP: A scheduled journey of a vehicle on a route
// ============================================================
enum class TripStatus {
    SCHEDULED,
    IN_PROGRESS,
    COMPLETED,
    CANCELLED
};

class Trip {
private:
    int          m_tripId;
    int          m_vehicleId;
    int          m_driverId;
    int          m_routeId;
    CustomTime   m_scheduledStartTime;
    CustomTime   m_actualStartTime;
    CustomTime   m_endTime;
    int          m_passengersBoardedTotal;
    TripStatus   m_status;
    int          m_delayMinutes;

    static int s_nextTripId;

public:
    Trip()
        : m_tripId(s_nextTripId++), m_vehicleId(-1), m_driverId(-1), m_routeId(-1),
          m_passengersBoardedTotal(0), m_status(TripStatus::SCHEDULED), m_delayMinutes(0) {}

    Trip(int vehicleId, int driverId, int routeId, const CustomTime& scheduledStart)
        : m_tripId(s_nextTripId++), m_vehicleId(vehicleId), m_driverId(driverId),
          m_routeId(routeId), m_scheduledStartTime(scheduledStart),
          m_actualStartTime(scheduledStart),
          m_passengersBoardedTotal(0), m_status(TripStatus::SCHEDULED), m_delayMinutes(0) {}

    // Getters
    int        getTripId()              const { return m_tripId;                   }
    int        getVehicleId()           const { return m_vehicleId;                }
    int        getDriverId()            const { return m_driverId;                 }
    int        getRouteId()             const { return m_routeId;                  }
    TripStatus getStatus()              const { return m_status;                   }
    int        getDelayMinutes()        const { return m_delayMinutes;             }
    int        getPassengersBoardedTotal() const { return m_passengersBoardedTotal; }

    // Trip lifecycle
    void startTrip() {
        if (m_status != TripStatus::SCHEDULED) return;
        m_actualStartTime = CustomTime::now();
        m_status          = TripStatus::IN_PROGRESS;
        int scheduledSecs = m_scheduledStartTime.getHour() * 3600 +
                            m_scheduledStartTime.getMinute() * 60;
        int actualSecs    = m_actualStartTime.getHour() * 3600 +
                            m_actualStartTime.getMinute() * 60;
        m_delayMinutes = (actualSecs - scheduledSecs) / 60;
        if (m_delayMinutes < 0) m_delayMinutes = 0;
    }

    void endTrip() {
        m_endTime = CustomTime::now();
        m_status  = TripStatus::COMPLETED;
    }

    void cancelTrip() { m_status = TripStatus::CANCELLED; }

    void boardPassenger() { m_passengersBoardedTotal++; }

    CustomString getStatusString() const {
        switch (m_status) {
            case TripStatus::SCHEDULED:   return CustomString("SCHEDULED");
            case TripStatus::IN_PROGRESS: return CustomString("IN PROGRESS");
            case TripStatus::COMPLETED:   return CustomString("COMPLETED");
            case TripStatus::CANCELLED:   return CustomString("CANCELLED");
            default:                      return CustomString("UNKNOWN");
        }
    }

    bool operator==(const Trip& other) const { return m_tripId == other.m_tripId; }

    friend std::ostream& operator<<(std::ostream& os, const Trip& t) {
        char buf[300];
        std::snprintf(buf, sizeof(buf),
                "Trip[%d] Vehicle:%d Driver:%d Route:%d Status:%s Passengers:%d Delay:%dmin",
                t.m_tripId, t.m_vehicleId, t.m_driverId, t.m_routeId,
                t.getStatusString().c_str(), t.m_passengersBoardedTotal, t.m_delayMinutes);
        os << buf;
        return os;
    }
};

