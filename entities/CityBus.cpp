// ============================================================
// FILE: CityBus.cpp
// MODULE: 3 - Entity Layer (Level 4 Concrete Vehicles)
// ============================================================

#include "CityBus.h"
#include <cstdio>

// ============================================================
// CITYBUS IMPLEMENTATION
// ============================================================

CityBus::CityBus()
    : Vehicle(),                              // Call Vehicle() constructor
      m_busNumber("BUS-000"),
      m_routeId(-1),
      m_isAirConditioned(false),
      m_nextStopId(-1) {
}

CityBus::CityBus(const CustomString& regNo, const CustomString& busNumber,
                 int routeId, bool isAC, double purchaseValue)
    : Vehicle(regNo, CustomString("CityBus"), CustomString("Standard"),
              FuelType::DIESEL, 50, purchaseValue),   // 50 passenger capacity
      m_busNumber(busNumber),
      m_routeId(routeId),
      m_isAirConditioned(isAC),
      m_nextStopId(-1) {
}

CityBus::CityBus(const CityBus& other)
    : Vehicle(other),                         // Copy Vehicle part
      m_busNumber(other.m_busNumber),
      m_routeId(other.m_routeId),
      m_isAirConditioned(other.m_isAirConditioned),
      m_nextStopId(other.m_nextStopId) {
}

// Diesel costs more than CNG; AC buses consume more fuel
double CityBus::getFuelCostPerKm() const {
    double baseCost = (m_fuelType == FuelType::DIESEL) ? 18.0 : 12.0; // Rs per km
    if (m_isAirConditioned) baseCost *= 1.15; // 15% more fuel for AC
    return baseCost;
}

CustomString CityBus::getVehicleCategory() const {
    return CustomString("City Bus");
}

// Daily check: always returns true in simplified version
// In real version: check tyres, brakes, lights
bool CityBus::performDailyCheck() const {
    // [OOP: this pointer - refers to current object]
    bool fuelOk    = (this->m_odometer < 500000); // under 500k km
    bool statusOk  = (this->m_status == AssetStatus::ACTIVE);
    return fuelOk && statusOk;
}

double CityBus::calculateEmissions(double km) const {
    // Diesel buses emit ~0.8 kg CO2 per km
    return km * 0.8;
}

CustomString CityBus::toString() const {
    char buf[300];
    sprintf(buf, "CityBus[%d] Reg:%s BusNo:%s Route:%d AC:%s Passengers:%d/%d",
            m_id, m_registrationNo.c_str(), m_busNumber.c_str(),
            m_routeId, m_isAirConditioned ? "Yes" : "No",
            m_currentPassengers, m_passengerCapacity);
    return CustomString(buf);
}

void CityBus::serialize(std::ofstream& out) const {
    Vehicle::serialize(out);  // Serialize parent data first
    out << m_busNumber.c_str() << "\n";
    out << m_routeId << "\n";
    out << m_isAirConditioned << "\n";
}

void CityBus::deserialize(std::ifstream& in) {
    Vehicle::deserialize(in);
    std::string busNum;
    in >> busNum >> m_routeId >> m_isAirConditioned;
    m_busNumber = CustomString(busNum.c_str());
}

// ============================================================
// METROTRAIN IMPLEMENTATION
// ============================================================

MetroTrain::MetroTrain()
    : Vehicle(),
      m_lineId(-1), m_carriageCount(4),
      m_maxSpeed(80.0), m_isUnderground(false) {
}

MetroTrain::MetroTrain(const CustomString& regNo, int lineId, int carriages,
                       double maxSpeed, bool underground, double purchaseValue)
    : Vehicle(regNo, CustomString("MetroTrain"), CustomString("Urban"),
              FuelType::ELECTRIC, carriages * 200, purchaseValue), // 200 per carriage
      m_lineId(lineId),
      m_carriageCount(carriages),
      m_maxSpeed(maxSpeed),
      m_isUnderground(underground) {
}

MetroTrain::MetroTrain(const MetroTrain& other)
    : Vehicle(other),
      m_lineId(other.m_lineId),
      m_carriageCount(other.m_carriageCount),
      m_maxSpeed(other.m_maxSpeed),
      m_isUnderground(other.m_isUnderground) {
}

double MetroTrain::getFuelCostPerKm() const {
    // Electric: very cheap per km compared to diesel
    return 5.0 * m_carriageCount;  // Rs 5 per km per carriage
}

CustomString MetroTrain::getVehicleCategory() const {
    return CustomString("Metro Train");
}

bool MetroTrain::performDailyCheck() const {
    return (m_status == AssetStatus::ACTIVE) && (m_carriageCount > 0);
}

// Electric train: ZERO direct emissions
double MetroTrain::calculateEmissions(double km) const {
    (void)km; // suppress unused parameter warning
    return 0.0;
}

CustomString MetroTrain::toString() const {
    char buf[300];
    sprintf(buf, "MetroTrain[%d] Reg:%s Line:%d Carriages:%d MaxSpeed:%.0fkm/h Underground:%s",
            m_id, m_registrationNo.c_str(),
            m_lineId, m_carriageCount, m_maxSpeed,
            m_isUnderground ? "Yes" : "No");
    return CustomString(buf);
}

void MetroTrain::serialize(std::ofstream& out) const {
    Vehicle::serialize(out);
    out << m_lineId << "\n" << m_carriageCount << "\n"
        << m_maxSpeed << "\n" << m_isUnderground << "\n";
}

void MetroTrain::deserialize(std::ifstream& in) {
    Vehicle::deserialize(in);
    in >> m_lineId >> m_carriageCount >> m_maxSpeed >> m_isUnderground;
}

// ============================================================
// RIDEHAILCAR IMPLEMENTATION
// ============================================================

RideHailCar::RideHailCar()
    : Vehicle(),
      m_driverRating(4.0), m_tripCount(0), m_surgeMultiplier(1.0) {
}

RideHailCar::RideHailCar(const CustomString& regNo, const CustomString& make,
                          const CustomString& model, double purchaseValue)
    : Vehicle(regNo, make, model, FuelType::CNG, 4, purchaseValue), // 4 passengers
      m_driverRating(4.0), m_tripCount(0), m_surgeMultiplier(1.0) {
}

RideHailCar::RideHailCar(const RideHailCar& other)
    : Vehicle(other),
      m_driverRating(other.m_driverRating),
      m_tripCount(other.m_tripCount),
      m_surgeMultiplier(other.m_surgeMultiplier) {
}

// Cost per km with surge multiplier applied
double RideHailCar::getFuelCostPerKm() const {
    return 15.0 * m_surgeMultiplier;  // Rs 15 base, multiplied by surge
}

CustomString RideHailCar::getVehicleCategory() const {
    return CustomString("Ride Hail Car");
}

bool RideHailCar::performDailyCheck() const {
    return (m_status == AssetStatus::ACTIVE) && (m_driverRating >= 3.0);
}

double RideHailCar::calculateEmissions(double km) const {
    return km * 0.15;  // CNG emits less than diesel
}

CustomString RideHailCar::toString() const {
    char buf[300];
    sprintf(buf, "RideHailCar[%d] Reg:%s %s %s Rating:%.1f Surge:%.1fx Trips:%d",
            m_id, m_registrationNo.c_str(), m_make.c_str(), m_model.c_str(),
            m_driverRating, m_surgeMultiplier, m_tripCount);
    return CustomString(buf);
}

void RideHailCar::serialize(std::ofstream& out) const {
    Vehicle::serialize(out);
    out << m_driverRating << "\n" << m_tripCount << "\n" << m_surgeMultiplier << "\n";
}

void RideHailCar::deserialize(std::ifstream& in) {
    Vehicle::deserialize(in);
    in >> m_driverRating >> m_tripCount >> m_surgeMultiplier;
}
