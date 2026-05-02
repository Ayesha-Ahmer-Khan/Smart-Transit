// ============================================================
// FILE: Level5Vehicles.cpp
// MODULE: 3 - Entity Layer (Level 5 + extra Staff)
// ============================================================

#include "Level5Vehicles.h"
#include "../exceptions/TransitExceptions.h"
#include <cstdio>
#include <cmath>  // fabs

// ============================================================
// ARTICULATEDBUS
// ============================================================

ArticulatedBus::ArticulatedBus()
    : CityBus(), m_secondSegmentCapacity(30), m_jointAngle(0.0) {}

ArticulatedBus::ArticulatedBus(const CustomString& regNo, const CustomString& busNumber,
                                int routeId, bool isAC, int secondSegCap, double purchaseValue)
    : CityBus(regNo, busNumber, routeId, isAC, purchaseValue),
      m_secondSegmentCapacity(secondSegCap),
      m_jointAngle(0.0) {
    // Articulated bus has more capacity (front + rear segment)
    m_passengerCapacity += secondSegCap;
}

ArticulatedBus::ArticulatedBus(const ArticulatedBus& other)
    : CityBus(other),
      m_secondSegmentCapacity(other.m_secondSegmentCapacity),
      m_jointAngle(other.m_jointAngle) {
}

// Articulated bus uses 20% more fuel due to extra weight and drag
double ArticulatedBus::getFuelCostPerKm() const {
    return CityBus::getFuelCostPerKm() * 1.20; // 20% drag factor
}

CustomString ArticulatedBus::getVehicleCategory() const {
    return CustomString("Articulated Bus");
}

CustomString ArticulatedBus::toString() const {
    char buf[350];
    sprintf(buf,
            "ArticulatedBus[%d] Reg:%s BusNo:%s TotalCapacity:%d JointAngle:%.1fdeg",
            m_id, m_registrationNo.c_str(), m_busNumber.c_str(),
            getTotalCapacity(), m_jointAngle);
    return CustomString(buf);
}

void ArticulatedBus::serialize(std::ofstream& out) const {
    CityBus::serialize(out);
    out << m_secondSegmentCapacity << "\n" << m_jointAngle << "\n";
}

void ArticulatedBus::deserialize(std::ifstream& in) {
    CityBus::deserialize(in);
    in >> m_secondSegmentCapacity >> m_jointAngle;
}

// ============================================================
// HIGHSPEEDTRAIN
// ============================================================

HighSpeedTrain::HighSpeedTrain()
    : MetroTrain(), m_maxOperatingSpeedKmh(250.0),
      m_tiltAngle(0.0), m_signalClass("ETCS-2") {}

HighSpeedTrain::HighSpeedTrain(const CustomString& regNo, int lineId, int carriages,
                                double maxSpeed, const CustomString& signalClass,
                                double purchaseValue)
    : MetroTrain(regNo, lineId, carriages, maxSpeed, false, purchaseValue),
      m_maxOperatingSpeedKmh(maxSpeed),
      m_tiltAngle(0.0),
      m_signalClass(signalClass) {
}

HighSpeedTrain::HighSpeedTrain(const HighSpeedTrain& other)
    : MetroTrain(other),
      m_maxOperatingSpeedKmh(other.m_maxOperatingSpeedKmh),
      m_tiltAngle(other.m_tiltAngle),
      m_signalClass(other.m_signalClass) {
}

// Stricter daily check: also checks rail signal system
bool HighSpeedTrain::performDailyCheck() const {
    bool parentCheck  = MetroTrain::performDailyCheck();  // Do parent checks too
    bool signalOk     = !m_signalClass.isEmpty();          // Signal class must be set
    bool speedCapOk   = m_maxOperatingSpeedKmh > 0;
    return parentCheck && signalOk && speedCapOk;
}

CustomString HighSpeedTrain::getVehicleCategory() const {
    return CustomString("High Speed Train");
}

CustomString HighSpeedTrain::toString() const {
    char buf[300];
    sprintf(buf,
            "HighSpeedTrain[%d] Reg:%s Line:%d MaxSpeed:%.0fkm/h Signal:%s Carriages:%d",
            m_id, m_registrationNo.c_str(), m_lineId,
            m_maxOperatingSpeedKmh, m_signalClass.c_str(), m_carriageCount);
    return CustomString(buf);
}

void HighSpeedTrain::serialize(std::ofstream& out) const {
    MetroTrain::serialize(out);
    out << m_maxOperatingSpeedKmh << "\n"
        << m_tiltAngle << "\n"
        << m_signalClass.c_str() << "\n";
}

void HighSpeedTrain::deserialize(std::ifstream& in) {
    MetroTrain::deserialize(in);
    std::string sig;
    in >> m_maxOperatingSpeedKmh >> m_tiltAngle >> sig;
    m_signalClass = CustomString(sig.c_str());
}

// ============================================================
// REFRIGERATEDTRUCK
// ============================================================

RefrigeratedTruck::RefrigeratedTruck()
    : FreightTruck(), m_targetTempCelsius(-18.0),
      m_currentTempCelsius(-18.0), m_tempToleranceDeg(2.0) {
}

RefrigeratedTruck::RefrigeratedTruck(const CustomString& regNo, const CustomString& make,
                                      const CustomString& model, double maxPayloadKg,
                                      double targetTemp, double purchaseValue)
    : FreightTruck(regNo, make, model, maxPayloadKg,
                   CustomString("Refrigerated Goods"), true, purchaseValue),
      m_targetTempCelsius(targetTemp),
      m_currentTempCelsius(targetTemp),  // Start at correct temp
      m_tempToleranceDeg(2.0) {
}

RefrigeratedTruck::RefrigeratedTruck(const RefrigeratedTruck& other)
    : FreightTruck(other),
      m_targetTempCelsius(other.m_targetTempCelsius),
      m_currentTempCelsius(other.m_currentTempCelsius),
      m_tempToleranceDeg(other.m_tempToleranceDeg) {
}

// [OOP: This is the critical safety method]
// Throws CargoCriticalException if temperature is out of range
void RefrigeratedTruck::checkTempAlert() const {
    double diff = fabs(m_currentTempCelsius - m_targetTempCelsius);
    if (diff > m_tempToleranceDeg) {
        throw CargoCriticalException("RefrigeratedTruck::checkTempAlert");
    }
}

bool RefrigeratedTruck::performDailyCheck() const {
    bool parentCheck = FreightTruck::performDailyCheck();
    bool tempOk      = true;
    try {
        checkTempAlert();  // Will throw if out of range
    } catch (const CargoCriticalException&) {
        tempOk = false;    // Catch it here and report check failed
    }
    return parentCheck && tempOk;
}

CustomString RefrigeratedTruck::getVehicleCategory() const {
    return CustomString("Refrigerated Truck");
}

CustomString RefrigeratedTruck::toString() const {
    char buf[350];
    sprintf(buf,
            "RefrigeratedTruck[%d] Reg:%s %s | Temp:%.1f/%.1f C | Payload:%.0f/%.0fkg",
            m_id, m_registrationNo.c_str(), m_make.c_str(),
            m_currentTempCelsius, m_targetTempCelsius,
            m_cargoWeightKg, m_maxPayloadKg);
    return CustomString(buf);
}

void RefrigeratedTruck::serialize(std::ofstream& out) const {
    FreightTruck::serialize(out);
    out << m_targetTempCelsius  << "\n"
        << m_currentTempCelsius << "\n"
        << m_tempToleranceDeg   << "\n";
}

void RefrigeratedTruck::deserialize(std::ifstream& in) {
    FreightTruck::deserialize(in);
    in >> m_targetTempCelsius >> m_currentTempCelsius >> m_tempToleranceDeg;
}

// ============================================================
// SENIORDRIVER
// ============================================================

SeniorDriver::SeniorDriver()
    : Driver(), m_menteeCount(0) {}

SeniorDriver::SeniorDriver(const CustomString& firstName, const CustomString& lastName,
                            const CustomString& cnic, const CustomDate& dob,
                            const CustomString& employeeId, const CustomString& licenseClass,
                            double baseSalary, int menteeCount)
    : Driver(firstName, lastName, cnic, dob, employeeId, licenseClass, baseSalary),
      m_menteeCount(menteeCount) {
}

SeniorDriver::SeniorDriver(const SeniorDriver& other)
    : Driver(other),
      m_menteeCount(other.m_menteeCount),
      m_awardsList(other.m_awardsList) {
}

// Senior drivers get 15% seniority bonus + Rs 1000 per mentee
double SeniorDriver::calculateMonthlySalary() const {
    double base    = Driver::calculateMonthlySalary(); // Use parent salary
    double bonus   = base * 0.15;                      // 15% seniority bonus
    double mentee  = m_menteeCount * 1000.0;           // Rs 1000 per mentee
    return base + bonus + mentee;
}

CustomString SeniorDriver::toString() const {
    char buf[300];
    sprintf(buf,
            "SeniorDriver[%d] %s | License:%s | Mentees:%d | Awards:%d | Salary:Rs%.0f",
            m_id, getFullName().c_str(), m_licenseClass.c_str(),
            m_menteeCount, m_awardsList.size(), calculateMonthlySalary());
    return CustomString(buf);
}

void SeniorDriver::serialize(std::ofstream& out) const {
    Driver::serialize(out);
    out << m_menteeCount << "\n" << m_awardsList.size() << "\n";
    for (int i = 0; i < m_awardsList.size(); i++) {
        out << m_awardsList[i].c_str() << "\n";
    }
}

void SeniorDriver::deserialize(std::ifstream& in) {
    Driver::deserialize(in);
    int awardCount;
    in >> m_menteeCount >> awardCount;
    for (int i = 0; i < awardCount; i++) {
        std::string award;
        in >> award;
        m_awardsList.append(CustomString(award.c_str()));
    }
}

// ============================================================
// DISPATCHER
// ============================================================

Dispatcher::Dispatcher()
    : StaffMember(), m_activeDeployments(0), m_zoneId("Z-0") {
    m_role = StaffRole::DISPATCHER;
}

Dispatcher::Dispatcher(const CustomString& firstName, const CustomString& lastName,
                        const CustomString& cnic, const CustomDate& dob,
                        const CustomString& employeeId, const CustomString& zoneId,
                        double baseSalary)
    : StaffMember(firstName, lastName, cnic, dob, employeeId, StaffRole::DISPATCHER, baseSalary),
      m_activeDeployments(0),
      m_zoneId(zoneId) {
}

Dispatcher::Dispatcher(const Dispatcher& other)
    : StaffMember(other),
      m_activeDeployments(other.m_activeDeployments),
      m_zoneId(other.m_zoneId) {
}

CustomString Dispatcher::toString() const {
    char buf[250];
    sprintf(buf, "Dispatcher[%d] %s | Zone:%s | ActiveDeployments:%d",
            m_id, getFullName().c_str(), m_zoneId.c_str(), m_activeDeployments);
    return CustomString(buf);
}

void Dispatcher::serialize(std::ofstream& out) const {
    StaffMember::serialize(out);
    out << m_activeDeployments << "\n" << m_zoneId.c_str() << "\n";
}

void Dispatcher::deserialize(std::ifstream& in) {
    StaffMember::deserialize(in);
    std::string zone;
    in >> m_activeDeployments >> zone;
    m_zoneId = CustomString(zone.c_str());
}

// ============================================================
// MAINTENANCETECH
// ============================================================

MaintenanceTech::MaintenanceTech()
    : StaffMember(), m_specialisation("General"), m_jobsCompletedThisMonth(0) {
    m_role = StaffRole::MAINTENANCE;
}

MaintenanceTech::MaintenanceTech(const CustomString& firstName, const CustomString& lastName,
                                  const CustomString& cnic, const CustomDate& dob,
                                  const CustomString& employeeId,
                                  const CustomString& specialisation, double baseSalary)
    : StaffMember(firstName, lastName, cnic, dob, employeeId, StaffRole::MAINTENANCE, baseSalary),
      m_specialisation(specialisation),
      m_jobsCompletedThisMonth(0) {
}

MaintenanceTech::MaintenanceTech(const MaintenanceTech& other)
    : StaffMember(other),
      m_specialisation(other.m_specialisation),
      m_jobsCompletedThisMonth(other.m_jobsCompletedThisMonth) {
}

CustomString MaintenanceTech::toString() const {
    char buf[250];
    sprintf(buf, "MaintenanceTech[%d] %s | Specialisation:%s | JobsThisMonth:%d",
            m_id, getFullName().c_str(),
            m_specialisation.c_str(), m_jobsCompletedThisMonth);
    return CustomString(buf);
}

void MaintenanceTech::serialize(std::ofstream& out) const {
    StaffMember::serialize(out);
    out << m_specialisation.c_str() << "\n" << m_jobsCompletedThisMonth << "\n";
}

void MaintenanceTech::deserialize(std::ifstream& in) {
    StaffMember::deserialize(in);
    std::string spec;
    in >> spec >> m_jobsCompletedThisMonth;
    m_specialisation = CustomString(spec.c_str());
}

// ============================================================
// ADMINUSER
// ============================================================

AdminUser::AdminUser()
    : StaffMember(), m_username("admin"), m_passwordHash("admin_hash"),
      m_accessLevel(1) {
    m_role = StaffRole::ADMIN;
}

AdminUser::AdminUser(const CustomString& firstName, const CustomString& lastName,
                     const CustomString& cnic, const CustomDate& dob,
                     const CustomString& employeeId, const CustomString& username,
                     const CustomString& password, int accessLevel)
    : StaffMember(firstName, lastName, cnic, dob, employeeId, StaffRole::ADMIN, 60000.0),
      m_username(username),
      m_passwordHash(password + CustomString("_hash")), // Simple "hash"
      m_accessLevel(accessLevel) {
}

AdminUser::AdminUser(const AdminUser& other)
    : StaffMember(other),
      m_username(other.m_username),
      m_passwordHash(other.m_passwordHash),
      m_accessLevel(other.m_accessLevel),
      m_notificationLog(other.m_notificationLog) {
}

bool AdminUser::verifyPassword(const CustomString& pwd) const {
    CustomString hashed = pwd + CustomString("_hash");
    return hashed == m_passwordHash;
}

void AdminUser::logActivity(const CustomString& action) {
    // Prepend timestamp to action
    CustomString entry = CustomDate::today().toString()
                       + CustomString(" | ")
                       + action;
    m_activityLog.append(entry);
}

CustomString AdminUser::toString() const {
    char buf[250];
    sprintf(buf, "AdminUser[%d] %s | Username:%s | AccessLevel:%d | LogEntries:%d",
            m_id, getFullName().c_str(),
            m_username.c_str(), m_accessLevel, m_activityLog.size());
    return CustomString(buf);
}

void AdminUser::serialize(std::ofstream& out) const {
    StaffMember::serialize(out);
    out << m_username.c_str() << "\n"
        << m_passwordHash.c_str() << "\n"
        << m_accessLevel << "\n";
}

void AdminUser::deserialize(std::ifstream& in) {
    StaffMember::deserialize(in);
    std::string uname, phash;
    in >> uname >> phash >> m_accessLevel;
    m_username     = CustomString(uname.c_str());
    m_passwordHash = CustomString(phash.c_str());
}

void AdminUser::sendNotification(const CustomString& msg, int priority) {
    char buf[300];
    std::snprintf(buf, sizeof(buf), "[ADMIN:%s] P%d: %s",
                  m_username.c_str(), priority, msg.c_str());
    m_notificationLog.append(CustomString(buf));
}

DynamicArray<CustomString> AdminUser::getDeliveryLog() const {
    return m_notificationLog;
}

void AdminUser::clearLog() {
    m_notificationLog.clear();
}
