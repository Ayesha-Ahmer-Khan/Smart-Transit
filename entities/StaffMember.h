#pragma once
// ============================================================
// FILE: StaffMember.h
// MODULE: 3 - Entity Layer (LEVEL 3)
// OUTPUT-ID: M3-STF-001
// Expected Output: Staff info, salary, duty status
// OOP CONCEPTS:
//   - Inherits from Person (Level 2)
//   - Static counter for total staff
//   - Enum class for staff roles
// ============================================================

#include "Person.h"

// [OOP: Enum class for type-safe role assignment]
enum class StaffRole {
    DRIVER,
    CONDUCTOR,
    DISPATCHER,
    MAINTENANCE,
    TRAFFIC_CONTROLLER,
    ADMIN
};

class StaffMember : public Person {
protected:
    CustomString m_employeeId;
    CustomString m_department;
    StaffRole    m_role;
    CustomDate   m_joiningDate;
    double       m_baseSalary;
    bool         m_isOnDuty;
    CustomDate   m_licenseExpiry;

    // [OOP: static member - counts ALL staff across all objects]
    static int s_totalStaff;

public:
    StaffMember();
    StaffMember(const CustomString& firstName, const CustomString& lastName,
                const CustomString& cnic, const CustomDate& dob,
                const CustomString& employeeId, StaffRole role, double baseSalary);
    StaffMember(const StaffMember& other);
    virtual ~StaffMember();  // Decrements counter

    // Getters
    CustomString getEmployeeId()  const { return m_employeeId; }
    CustomString getDepartment()  const { return m_department; }
    StaffRole    getRole()        const { return m_role;        }
    double       getBaseSalary()  const { return m_baseSalary; }
    bool         isOnDuty()       const { return m_isOnDuty;   }

    // [OOP: static method - call as StaffMember::getTotalStaff()]
    static int getTotalStaff() { return s_totalStaff; }

    // Methods
    int    getYearsOfService()          const;
    virtual double calculateMonthlySalary() const;   // Can be overridden
    bool   isLicenseValid()             const;
    void   clockIn();
    void   clockOut();
    CustomString getRoleString()        const;

    CustomString toString()      const override;
    CustomString getEntityType() const override { return CustomString("StaffMember"); }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};

// ============================================================
// FILE: Driver.h (Level 4 Staff - combined here)
// OUTPUT-ID: M3-DRV-001
// ============================================================

class Driver : public StaffMember {
protected:
    CustomString m_licenseClass;    // "A", "B", "LTV", "HTV"
    int          m_assignedVehicleId;
    double       m_hoursThisWeek;
    int          m_incidentCount;

public:
    Driver();
    Driver(const CustomString& firstName, const CustomString& lastName,
           const CustomString& cnic, const CustomDate& dob,
           const CustomString& employeeId, const CustomString& licenseClass,
           double baseSalary);
    Driver(const Driver& other);
    ~Driver() override = default;

    CustomString getLicenseClass()      const { return m_licenseClass;      }
    int          getAssignedVehicleId() const { return m_assignedVehicleId; }
    double       getHoursThisWeek()     const { return m_hoursThisWeek;     }
    int          getIncidentCount()     const { return m_incidentCount;     }

    void assignVehicle(int vehicleId) { m_assignedVehicleId = vehicleId; }
    void addDrivingHours(double hours){ m_hoursThisWeek += hours;        }
    void recordIncident()             { m_incidentCount++;                }

    CustomString toString()      const override;
    CustomString getEntityType() const override { return CustomString("Driver"); }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};

// ============================================================
// FILE: Conductor.h (Level 4 Staff - combined)
// OUTPUT-ID: M3-CON-001
// ============================================================

class Conductor : public StaffMember {
private:
    int    m_assignedBusId;
    int    m_ticketsSoldToday;
    double m_cashCollectedToday;

public:
    Conductor();
    Conductor(const CustomString& firstName, const CustomString& lastName,
              const CustomString& cnic, const CustomDate& dob,
              const CustomString& employeeId, double baseSalary);
    Conductor(const Conductor& other);
    ~Conductor() override = default;

    int    getTicketsSoldToday()    const { return m_ticketsSoldToday;    }
    double getCashCollectedToday()  const { return m_cashCollectedToday;  }

    void assignBus(int busId)           { m_assignedBusId = busId;      }
    void sellTicket(double fareAmount)  { m_ticketsSoldToday++;
                                          m_cashCollectedToday += fareAmount; }
    void resetDailyStats()              { m_ticketsSoldToday = 0;
                                          m_cashCollectedToday = 0.0;   }

    CustomString toString()      const override;
    CustomString getEntityType() const override { return CustomString("Conductor"); }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};
