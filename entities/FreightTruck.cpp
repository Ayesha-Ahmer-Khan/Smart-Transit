// ============================================================
// FILE: FreightTruck.cpp
// MODULE: 3 - Entity Layer
// ============================================================

#include "FreightTruck.h"
#include "../exceptions/TransitExceptions.h"
#include <cstdio>

// ============================================================
// FREIGHTTRUCK IMPLEMENTATION
// ============================================================

FreightTruck::FreightTruck()
    : Vehicle(),
      m_cargoWeightKg(0.0), m_maxPayloadKg(10000.0),
      m_cargoType("General"), m_requiresRefrigeration(false) {
    m_fuelType = FuelType::DIESEL;
}

FreightTruck::FreightTruck(const CustomString& regNo, const CustomString& make,
                           const CustomString& model, double maxPayloadKg,
                           const CustomString& cargoType, bool refrigerated,
                           double purchaseValue)
    : Vehicle(regNo, make, model, FuelType::DIESEL, 2, purchaseValue), // 2 crew members
      m_cargoWeightKg(0.0),
      m_maxPayloadKg(maxPayloadKg),
      m_cargoType(cargoType),
      m_requiresRefrigeration(refrigerated) {
}

FreightTruck::FreightTruck(const FreightTruck& other)
    : Vehicle(other),
      m_cargoWeightKg(other.m_cargoWeightKg),
      m_maxPayloadKg(other.m_maxPayloadKg),
      m_cargoType(other.m_cargoType),
      m_requiresRefrigeration(other.m_requiresRefrigeration) {
}

void FreightTruck::loadCargo(double weightKg) {
    if (m_cargoWeightKg + weightKg > m_maxPayloadKg) {
        throw VehicleOverCapacityException("FreightTruck::loadCargo");
    }
    m_cargoWeightKg += weightKg;
    markUpdated();
}

void FreightTruck::unloadCargo(double weightKg) {
    m_cargoWeightKg -= weightKg;
    if (m_cargoWeightKg < 0.0) m_cargoWeightKg = 0.0;
    markUpdated();
}

double FreightTruck::getLoadPercent() const {
    if (m_maxPayloadKg == 0.0) return 0.0;
    return (m_cargoWeightKg / m_maxPayloadKg) * 100.0;
}

// Heavier load = more fuel burned per km
double FreightTruck::getFuelCostPerKm() const {
    double baseRate = 25.0;   // Rs 25 per km empty
    // Add Rs 5 per ton of cargo
    double cargoTons = m_cargoWeightKg / 1000.0;
    return baseRate + (cargoTons * 5.0);
}

CustomString FreightTruck::getVehicleCategory() const {
    return CustomString("Freight Truck");
}

bool FreightTruck::performDailyCheck() const {
    bool loadOk   = (m_cargoWeightKg <= m_maxPayloadKg);
    bool statusOk = (m_status == AssetStatus::ACTIVE);
    return loadOk && statusOk;
}

// Heavy diesel trucks emit more CO2 per km
double FreightTruck::calculateEmissions(double km) const {
    double cargoTons = m_cargoWeightKg / 1000.0;
    return km * (1.5 + cargoTons * 0.1);  // More cargo = more emissions
}

CustomString FreightTruck::toString() const {
    char buf[300];
    sprintf(buf, "FreightTruck[%d] Reg:%s %s %s | Cargo:%s %.0f/%.0fkg (%.0f%%) | Refrig:%s",
            m_id, m_registrationNo.c_str(), m_make.c_str(), m_model.c_str(),
            m_cargoType.c_str(), m_cargoWeightKg, m_maxPayloadKg,
            getLoadPercent(), m_requiresRefrigeration ? "Yes" : "No");
    return CustomString(buf);
}

void FreightTruck::serialize(std::ofstream& out) const {
    Vehicle::serialize(out);
    out << m_cargoWeightKg    << "\n"
        << m_maxPayloadKg     << "\n"
        << m_cargoType.c_str()<< "\n"
        << m_requiresRefrigeration << "\n";
}

void FreightTruck::deserialize(std::ifstream& in) {
    Vehicle::deserialize(in);
    std::string cargoType;
    in >> m_cargoWeightKg >> m_maxPayloadKg >> cargoType >> m_requiresRefrigeration;
    m_cargoType = CustomString(cargoType.c_str());
}

// ============================================================
// ELECTRICSCOOTER IMPLEMENTATION
// ============================================================

ElectricScooter::ElectricScooter()
    : Vehicle(),
      m_batteryLevelPercent(100), m_dockingStationId(0) {
    m_fuelType = FuelType::ELECTRIC;
}

ElectricScooter::ElectricScooter(const CustomString& regNo, int dockingStationId,
                                  double purchaseValue)
    : Vehicle(regNo, CustomString("Generic"), CustomString("Scooter"),
              FuelType::ELECTRIC, 1, purchaseValue),  // 1 person capacity
      m_batteryLevelPercent(100),
      m_dockingStationId(dockingStationId) {
}

ElectricScooter::ElectricScooter(const ElectricScooter& other)
    : Vehicle(other),
      m_batteryLevelPercent(other.m_batteryLevelPercent),
      m_dockingStationId(other.m_dockingStationId) {
}

void ElectricScooter::charge(int percent) {
    m_batteryLevelPercent += percent;
    if (m_batteryLevelPercent > 100) m_batteryLevelPercent = 100;
    markUpdated();
}

void ElectricScooter::useBattery(int percent) {
    m_batteryLevelPercent -= percent;
    if (m_batteryLevelPercent < 0) m_batteryLevelPercent = 0;
    markUpdated();
}

// Very cheap: electric cost per km
double ElectricScooter::getFuelCostPerKm() const {
    return 2.0;  // Rs 2 per km (electricity)
}

CustomString ElectricScooter::getVehicleCategory() const {
    return CustomString("Electric Scooter");
}

// [OOP: performDailyCheck is specific to this vehicle type]
bool ElectricScooter::performDailyCheck() const {
    // Scooter needs at least 20% battery to be usable
    return (m_batteryLevelPercent >= 20) && (m_status == AssetStatus::ACTIVE);
}

// Electric = zero direct emissions
double ElectricScooter::calculateEmissions(double km) const {
    (void)km;
    return 0.0;
}

CustomString ElectricScooter::toString() const {
    char buf[250];
    sprintf(buf, "ElectricScooter[%d] Reg:%s Battery:%d%% Station:%d Docked:%s",
            m_id, m_registrationNo.c_str(),
            m_batteryLevelPercent, m_dockingStationId,
            isDocked() ? "Yes" : "No");
    return CustomString(buf);
}

void ElectricScooter::serialize(std::ofstream& out) const {
    Vehicle::serialize(out);
    out << m_batteryLevelPercent << "\n"
        << m_dockingStationId   << "\n";
}

void ElectricScooter::deserialize(std::ifstream& in) {
    Vehicle::deserialize(in);
    in >> m_batteryLevelPercent >> m_dockingStationId;
}
