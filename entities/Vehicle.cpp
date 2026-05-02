// ============================================================
// FILE: Vehicle.cpp
// MODULE: 3 - Entity Layer
// ============================================================

#include "Vehicle.h"
#include "../exceptions/TransitExceptions.h"
#include <cstdio>

Vehicle::Vehicle()
    : TransitAsset(),
      m_registrationNo("UNREG"),
      m_make("Unknown"), m_model("Unknown"), m_colour("White"),
      m_fuelType(FuelType::DIESEL),
      m_passengerCapacity(0), m_currentPassengers(0),
      m_odometer(0.0),
      m_assignedDriverId(-1), m_currentRouteId(-1) {
}

Vehicle::Vehicle(const CustomString& regNo, const CustomString& make,
                 const CustomString& model, FuelType fuelType,
                 int capacity, double purchaseValue)
    : TransitAsset(regNo, purchaseValue, CustomDate::today()),
      m_registrationNo(regNo),
      m_make(make), m_model(model), m_colour("White"),
      m_fuelType(fuelType),
      m_passengerCapacity(capacity), m_currentPassengers(0),
      m_odometer(0.0),
      m_assignedDriverId(-1), m_currentRouteId(-1) {
}

Vehicle::Vehicle(const Vehicle& other)
    : TransitAsset(other),
      m_registrationNo(other.m_registrationNo),
      m_make(other.m_make), m_model(other.m_model),
      m_colour(other.m_colour),
      m_fuelType(other.m_fuelType),
      m_passengerCapacity(other.m_passengerCapacity),
      m_currentPassengers(other.m_currentPassengers),
      m_odometer(other.m_odometer),
      m_currentPosition(other.m_currentPosition),
      m_assignedDriverId(other.m_assignedDriverId),
      m_currentRouteId(other.m_currentRouteId) {
}

// Board one passenger (throws if full)
void Vehicle::boardPassenger() {
    if (m_currentPassengers >= m_passengerCapacity) {
        throw VehicleOverCapacityException("Vehicle::boardPassenger");
    }
    m_currentPassengers++;
}

// One passenger gets off
void Vehicle::alightPassenger() {
    if (m_currentPassengers > 0) m_currentPassengers--;
}

int Vehicle::getOccupancyPercent() const {
    if (m_passengerCapacity == 0) return 0;
    return (m_currentPassengers * 100) / m_passengerCapacity;
}

bool Vehicle::isOverCapacity() const {
    return m_currentPassengers > m_passengerCapacity;
}

void Vehicle::updatePosition(const GeoCoordinate& newPos) {
    m_currentPosition = newPos;
}

CustomString Vehicle::getFuelTypeString() const {
    switch (m_fuelType) {
        case FuelType::DIESEL:   return CustomString("Diesel");
        case FuelType::CNG:      return CustomString("CNG");
        case FuelType::ELECTRIC: return CustomString("Electric");
        case FuelType::HYBRID:   return CustomString("Hybrid");
        default:                 return CustomString("Unknown");
    }
}

bool Vehicle::operator<(const Vehicle& other) const {
    return m_registrationNo < other.m_registrationNo;
}

// Returns km distance between two vehicles using their GPS
double Vehicle::operator-(const Vehicle& other) const {
    return m_currentPosition.distanceTo(other.m_currentPosition);
}

CustomString Vehicle::toString() const {
    char buf[300];
    sprintf(buf, "Vehicle[%d] %s %s %s | Reg:%s | Fuel:%s | Passengers:%d/%d | Odom:%.1fkm",
            m_id, m_make.c_str(), m_model.c_str(),
            getVehicleCategory().c_str(),   // [OOP: Polymorphic call!]
            m_registrationNo.c_str(),
            getFuelTypeString().c_str(),
            m_currentPassengers, m_passengerCapacity,
            m_odometer);
    return CustomString(buf);
}

void Vehicle::serialize(std::ofstream& out) const {
    out << m_id << "\n";
    out << m_registrationNo.c_str() << "\n";
    out << m_make.c_str() << "\n";
    out << m_model.c_str() << "\n";
    out << static_cast<int>(m_fuelType) << "\n";
    out << m_passengerCapacity << "\n";
    out << m_odometer << "\n";
    out << m_purchaseValue << "\n";
}

void Vehicle::deserialize(std::ifstream& in) {
    std::string reg, make, model;
    int fuelType, capacity;
    double odometer, purchaseValue;
    in >> m_id >> reg >> make >> model >> fuelType >> capacity >> odometer >> purchaseValue;
    m_registrationNo    = CustomString(reg.c_str());
    m_make              = CustomString(make.c_str());
    m_model             = CustomString(model.c_str());
    m_fuelType          = static_cast<FuelType>(fuelType);
    m_passengerCapacity = capacity;
    m_odometer          = odometer;
    m_purchaseValue     = purchaseValue;
}
