// ============================================================
// FILE: StaffMember.cpp
// MODULE: 3 - Entity Layer
// ============================================================

#include "StaffMember.h"
#include <cstdio>

// Define the static member
int StaffMember::s_totalStaff = 0;

// ============================================================
// STAFFMEMBER IMPLEMENTATION
// ============================================================

StaffMember::StaffMember()
    : Person(),
      m_employeeId("EMP-000"), m_department("General"),
      m_role(StaffRole::DRIVER),
      m_joiningDate(CustomDate::today()),
      m_baseSalary(30000.0),
      m_isOnDuty(false),
      m_licenseExpiry(CustomDate::today()) {
    s_totalStaff++;  // Increment shared counter
}

StaffMember::StaffMember(const CustomString& firstName, const CustomString& lastName,
                         const CustomString& cnic, const CustomDate& dob,
                         const CustomString& employeeId, StaffRole role, double baseSalary)
    : Person(firstName, lastName, cnic, dob),
      m_employeeId(employeeId),
      m_role(role),
      m_joiningDate(CustomDate::today()),
      m_baseSalary(baseSalary),
      m_isOnDuty(false),
      m_licenseExpiry(CustomDate::today()) {
    s_totalStaff++;
}

StaffMember::StaffMember(const StaffMember& other)
    : Person(other),
      m_employeeId(other.m_employeeId),
      m_department(other.m_department),
      m_role(other.m_role),
      m_joiningDate(other.m_joiningDate),
      m_baseSalary(other.m_baseSalary),
      m_isOnDuty(other.m_isOnDuty),
      m_licenseExpiry(other.m_licenseExpiry) {
    s_totalStaff++;
}

StaffMember::~StaffMember() {
    s_totalStaff--;  // Decrement when staff object is destroyed
}

int StaffMember::getYearsOfService() const {
    int days = CustomDate::today() - m_joiningDate;
    return days / 365;
}

double StaffMember::calculateMonthlySalary() const {
    return m_baseSalary;  // Base: subclasses may override this
}

bool StaffMember::isLicenseValid() const {
    return CustomDate::today() < m_licenseExpiry;
}

void StaffMember::clockIn()  { m_isOnDuty = true;  markUpdated(); }
void StaffMember::clockOut() { m_isOnDuty = false; markUpdated(); }

CustomString StaffMember::getRoleString() const {
    switch (m_role) {
        case StaffRole::DRIVER:             return CustomString("Driver");
        case StaffRole::CONDUCTOR:          return CustomString("Conductor");
        case StaffRole::DISPATCHER:         return CustomString("Dispatcher");
        case StaffRole::MAINTENANCE:        return CustomString("Maintenance");
        case StaffRole::TRAFFIC_CONTROLLER: return CustomString("Traffic Controller");
        case StaffRole::ADMIN:              return CustomString("Admin");
        default:                            return CustomString("Unknown");
    }
}

CustomString StaffMember::toString() const {
    char buf[300];
    sprintf(buf, "Staff[%d] %s | Role:%s | Emp:%s | Salary:%.0f | OnDuty:%s",
            m_id, getFullName().c_str(), getRoleString().c_str(),
            m_employeeId.c_str(), m_baseSalary, m_isOnDuty ? "YES" : "NO");
    return CustomString(buf);
}

void StaffMember::serialize(std::ofstream& out) const {
    Person::serialize(out);
    out << m_employeeId.c_str() << "\n";
    out << static_cast<int>(m_role) << "\n";
    out << m_baseSalary << "\n";
    out << m_isOnDuty << "\n";
}

void StaffMember::deserialize(std::ifstream& in) {
    Person::deserialize(in);
    std::string empId;
    int role;
    in >> empId >> role >> m_baseSalary >> m_isOnDuty;
    m_employeeId = CustomString(empId.c_str());
    m_role       = static_cast<StaffRole>(role);
}

// ============================================================
// DRIVER IMPLEMENTATION
// ============================================================

Driver::Driver()
    : StaffMember(),
      m_licenseClass("B"),
      m_assignedVehicleId(-1),
      m_hoursThisWeek(0.0),
      m_incidentCount(0) {
    m_role = StaffRole::DRIVER;
}

Driver::Driver(const CustomString& firstName, const CustomString& lastName,
               const CustomString& cnic, const CustomDate& dob,
               const CustomString& employeeId, const CustomString& licenseClass,
               double baseSalary)
    : StaffMember(firstName, lastName, cnic, dob, employeeId, StaffRole::DRIVER, baseSalary),
      m_licenseClass(licenseClass),
      m_assignedVehicleId(-1),
      m_hoursThisWeek(0.0),
      m_incidentCount(0) {
}

Driver::Driver(const Driver& other)
    : StaffMember(other),
      m_licenseClass(other.m_licenseClass),
      m_assignedVehicleId(other.m_assignedVehicleId),
      m_hoursThisWeek(other.m_hoursThisWeek),
      m_incidentCount(other.m_incidentCount) {
}

CustomString Driver::toString() const {
    char buf[300];
    sprintf(buf, "Driver[%d] %s | License:%s | Vehicle:%d | Hours:%.1f | Incidents:%d",
            m_id, getFullName().c_str(), m_licenseClass.c_str(),
            m_assignedVehicleId, m_hoursThisWeek, m_incidentCount);
    return CustomString(buf);
}

void Driver::serialize(std::ofstream& out) const {
    StaffMember::serialize(out);
    out << m_licenseClass.c_str() << "\n";
    out << m_assignedVehicleId << "\n";
    out << m_hoursThisWeek << "\n";
    out << m_incidentCount << "\n";
}

void Driver::deserialize(std::ifstream& in) {
    StaffMember::deserialize(in);
    std::string lic;
    in >> lic >> m_assignedVehicleId >> m_hoursThisWeek >> m_incidentCount;
    m_licenseClass = CustomString(lic.c_str());
}

// ============================================================
// CONDUCTOR IMPLEMENTATION
// ============================================================

Conductor::Conductor()
    : StaffMember(),
      m_assignedBusId(-1),
      m_ticketsSoldToday(0),
      m_cashCollectedToday(0.0) {
    m_role = StaffRole::CONDUCTOR;
}

Conductor::Conductor(const CustomString& firstName, const CustomString& lastName,
                     const CustomString& cnic, const CustomDate& dob,
                     const CustomString& employeeId, double baseSalary)
    : StaffMember(firstName, lastName, cnic, dob, employeeId, StaffRole::CONDUCTOR, baseSalary),
      m_assignedBusId(-1),
      m_ticketsSoldToday(0),
      m_cashCollectedToday(0.0) {
}

Conductor::Conductor(const Conductor& other)
    : StaffMember(other),
      m_assignedBusId(other.m_assignedBusId),
      m_ticketsSoldToday(other.m_ticketsSoldToday),
      m_cashCollectedToday(other.m_cashCollectedToday) {
}

CustomString Conductor::toString() const {
    char buf[300];
    sprintf(buf, "Conductor[%d] %s | Bus:%d | Tickets:%d | Cash:Rs%.2f",
            m_id, getFullName().c_str(),
            m_assignedBusId, m_ticketsSoldToday, m_cashCollectedToday);
    return CustomString(buf);
}

void Conductor::serialize(std::ofstream& out) const {
    StaffMember::serialize(out);
    out << m_assignedBusId << "\n";
}

void Conductor::deserialize(std::ifstream& in) {
    StaffMember::deserialize(in);
    in >> m_assignedBusId;
}
