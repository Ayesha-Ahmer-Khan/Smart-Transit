#pragma once
// ============================================================
// FILE: Level5Vehicles.h
// MODULE: 3 - Entity Layer (LEVEL 5 - Most Specialised)
// OUTPUT-ID: M3-LV5-001
// Expected Output: Specialized vehicle/staff details
// OOP CONCEPTS:
//   - Multi-level inheritance (Level 5 extends Level 4)
//   - override on already-overridden methods
//   - Throwing domain-specific exceptions (CargoCriticalException)
// ============================================================

#include "CityBus.h"
#include "FreightTruck.h"
#include "StaffMember.h"
#include "../datastructs/LinkedList.h"
#include "../datastructs/DynamicArray.h"
#include "Interfaces.h"

// ============================================================
// LEVEL 5: ArticulatedBus  (extends CityBus)
// A double-segment bendy bus - longer, higher capacity
// Chain: Entity→TransitAsset→Vehicle→CityBus→ArticulatedBus
// ============================================================
class ArticulatedBus : public CityBus {
private:
    int    m_secondSegmentCapacity; // Extra passengers in rear segment
    double m_jointAngle;            // Current bend angle of the joint

public:
    ArticulatedBus();
    ArticulatedBus(const CustomString& regNo, const CustomString& busNumber,
                   int routeId, bool isAC, int secondSegCap, double purchaseValue);
    ArticulatedBus(const ArticulatedBus& other);
    ~ArticulatedBus() override = default;

    int    getSecondSegmentCapacity() const { return m_secondSegmentCapacity; }
    double getJointAngle()            const { return m_jointAngle;            }
    void   setJointAngle(double angle)      { m_jointAngle = angle;           }
    int    getTotalCapacity()         const {
        return m_passengerCapacity + m_secondSegmentCapacity;
    }

    // [OOP: Override again at Level 5 - higher drag factor makes it cost more]
    double       getFuelCostPerKm()   const override; // More drag = more fuel
    CustomString getVehicleCategory() const override;
    CustomString toString()           const override;
    CustomString getEntityType()      const override {
        return CustomString("ArticulatedBus");
    }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};

// ============================================================
// LEVEL 5: HighSpeedTrain  (extends MetroTrain)
// Chain: Entity→TransitAsset→Vehicle→MetroTrain→HighSpeedTrain
// ============================================================
class HighSpeedTrain : public MetroTrain {
private:
    double       m_maxOperatingSpeedKmh;
    double       m_tiltAngle;       // For cornering at high speed
    CustomString m_signalClass;     // "ETCS-2", "ATP", etc.

public:
    HighSpeedTrain();
    HighSpeedTrain(const CustomString& regNo, int lineId, int carriages,
                   double maxSpeed, const CustomString& signalClass, double purchaseValue);
    HighSpeedTrain(const HighSpeedTrain& other);
    ~HighSpeedTrain() override = default;

    double       getMaxOperatingSpeed() const { return m_maxOperatingSpeedKmh; }
    CustomString getSignalClass()       const { return m_signalClass;          }

    // [OOP: Override performDailyCheck with stricter rail-integrity checks]
    bool         performDailyCheck()  const override;
    CustomString getVehicleCategory() const override;
    CustomString toString()           const override;
    CustomString getEntityType()      const override {
        return CustomString("HighSpeedTrain");
    }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};

// ============================================================
// LEVEL 5: RefrigeratedTruck  (extends FreightTruck)
// Chain: Entity→TransitAsset→Vehicle→FreightTruck→RefrigeratedTruck
// ============================================================
class RefrigeratedTruck : public FreightTruck {
private:
    double m_targetTempCelsius;  // Desired temperature (e.g. -18 for frozen)
    double m_currentTempCelsius; // Actual temperature inside
    double m_tempToleranceDeg;   // How much deviation is allowed

public:
    RefrigeratedTruck();
    RefrigeratedTruck(const CustomString& regNo, const CustomString& make,
                      const CustomString& model, double maxPayloadKg,
                      double targetTemp, double purchaseValue);
    RefrigeratedTruck(const RefrigeratedTruck& other);
    ~RefrigeratedTruck() override = default;

    double getTargetTemp()  const { return m_targetTempCelsius;  }
    double getCurrentTemp() const { return m_currentTempCelsius; }

    void setCurrentTemp(double temp) {
        m_currentTempCelsius = temp;
        checkTempAlert();   // Immediately check after temperature update
    }

    // Throws CargoCriticalException if temperature is out of safe range
    void checkTempAlert() const;

    bool         performDailyCheck()  const override; // includes temp check
    CustomString getVehicleCategory() const override;
    CustomString toString()           const override;
    CustomString getEntityType()      const override {
        return CustomString("RefrigeratedTruck");
    }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};

// ============================================================
// LEVEL 5: SeniorDriver  (extends Driver)
// Chain: Entity→Person→StaffMember→Driver→SeniorDriver
// ============================================================
class SeniorDriver : public Driver {
private:
    int                          m_menteeCount;  // How many drivers they mentor
    DynamicArray<CustomString>   m_awardsList;   // Awards received over career

public:
    SeniorDriver();
    SeniorDriver(const CustomString& firstName, const CustomString& lastName,
                 const CustomString& cnic, const CustomDate& dob,
                 const CustomString& employeeId, const CustomString& licenseClass,
                 double baseSalary, int menteeCount);
    SeniorDriver(const SeniorDriver& other);
    ~SeniorDriver() override = default;

    int  getMenteeCount() const { return m_menteeCount; }
    void addAward(const CustomString& award) { m_awardsList.append(award); }
    int  getAwardCount()  const { return m_awardsList.size(); }

    // [OOP: Override calculateMonthlySalary to add seniority bonus]
    double       calculateMonthlySalary() const override;
    CustomString toString()               const override;
    CustomString getEntityType()          const override {
        return CustomString("SeniorDriver");
    }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};

// ============================================================
// LEVEL 4 STAFF: Dispatcher, MaintenanceTech, AdminUser
// (All extend StaffMember - Level 4 in staff branch)
// ============================================================

class Dispatcher : public StaffMember {
private:
    int          m_activeDeployments;
    CustomString m_zoneId;

public:
    Dispatcher();
    Dispatcher(const CustomString& firstName, const CustomString& lastName,
               const CustomString& cnic, const CustomDate& dob,
               const CustomString& employeeId, const CustomString& zoneId,
               double baseSalary);
    Dispatcher(const Dispatcher& other);
    ~Dispatcher() override = default;

    int          getActiveDeployments() const { return m_activeDeployments;  }
    CustomString getZoneId()            const { return m_zoneId;             }
    void         addDeployment()              { m_activeDeployments++;       }
    void         closeDeployment()            { if(m_activeDeployments>0) m_activeDeployments--; }

    CustomString toString()      const override;
    CustomString getEntityType() const override { return CustomString("Dispatcher"); }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};

class MaintenanceTech : public StaffMember {
private:
    CustomString m_specialisation;        // "Engine", "Electrical", "Bodywork"
    int          m_jobsCompletedThisMonth;

public:
    MaintenanceTech();
    MaintenanceTech(const CustomString& firstName, const CustomString& lastName,
                    const CustomString& cnic, const CustomDate& dob,
                    const CustomString& employeeId, const CustomString& specialisation,
                    double baseSalary);
    MaintenanceTech(const MaintenanceTech& other);
    ~MaintenanceTech() override = default;

    CustomString getSpecialisation()        const { return m_specialisation;        }
    int          getJobsCompletedThisMonth()const { return m_jobsCompletedThisMonth;}
    void         completeJob()                    { m_jobsCompletedThisMonth++;     }

    CustomString toString()      const override;
    CustomString getEntityType() const override { return CustomString("MaintenanceTech"); }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};

class AdminUser : public StaffMember, public INotifiable {
private:
    CustomString            m_username;
    CustomString            m_passwordHash;  // Stored hashed, not plaintext
    int                     m_accessLevel;   // 1=basic, 2=manager, 3=superadmin
    LinkedList<CustomString> m_activityLog;  // Record of all actions
    DynamicArray<CustomString> m_notificationLog; // INotifiable delivery log

public:
    AdminUser();
    AdminUser(const CustomString& firstName, const CustomString& lastName,
              const CustomString& cnic, const CustomDate& dob,
              const CustomString& employeeId, const CustomString& username,
              const CustomString& password, int accessLevel);
    AdminUser(const AdminUser& other);
    ~AdminUser() override = default;

    CustomString getUsername()    const { return m_username;    }
    int          getAccessLevel() const { return m_accessLevel; }
    bool         isAdmin()        const { return m_accessLevel >= 2; }

    // Simple hash: just marks as hashed (in real system: use bcrypt)
    bool verifyPassword(const CustomString& pwd) const;
    void logActivity(const CustomString& action);
    int  getActivityCount() const { return m_activityLog.size(); }

    // INotifiable implementation required by the specification.
    void sendNotification(const CustomString& msg, int priority) override;
    DynamicArray<CustomString> getDeliveryLog() const override;
    void clearLog() override;
    CustomString getChannelName() const override { return CustomString("AdminUser Notification"); }

    CustomString toString()      const override;
    CustomString getEntityType() const override { return CustomString("AdminUser"); }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};
