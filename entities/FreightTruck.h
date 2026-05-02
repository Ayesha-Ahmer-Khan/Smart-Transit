#pragma once
// ============================================================
// FILE: FreightTruck.h
// MODULE: 3 - Entity Layer (LEVEL 4 Concrete Vehicle)
// OUTPUT-ID: M3-FRT-001
// Expected Output: Truck info with cargo weight, payload, type
// OOP: Inherits Vehicle, overrides all pure virtuals
// ============================================================

#include "Vehicle.h"

class FreightTruck : public Vehicle {
protected:
    double       m_cargoWeightKg;        // Current cargo weight
    double       m_maxPayloadKg;         // Maximum allowed payload
    CustomString m_cargoType;            // "Electronics", "Food", etc.
    bool         m_requiresRefrigeration;

public:
    FreightTruck();
    FreightTruck(const CustomString& regNo, const CustomString& make,
                 const CustomString& model, double maxPayloadKg,
                 const CustomString& cargoType, bool refrigerated,
                 double purchaseValue);
    FreightTruck(const FreightTruck& other);
    ~FreightTruck() override = default;

    // Getters
    double       getCargoWeightKg()       const { return m_cargoWeightKg;         }
    double       getMaxPayloadKg()        const { return m_maxPayloadKg;           }
    CustomString getCargoType()           const { return m_cargoType;              }
    bool         requiresRefrigeration()  const { return m_requiresRefrigeration;  }

    // Load / unload cargo
    void loadCargo(double weightKg);    // throws VehicleOverCapacityException if over
    void unloadCargo(double weightKg);
    double getLoadPercent() const;

    // [OOP: override all pure virtuals from Vehicle]
    double       getFuelCostPerKm()   const override; // scales with cargo weight
    CustomString getVehicleCategory() const override;
    bool         performDailyCheck()  const override;
    double       calculateEmissions(double km) const override;

    CustomString toString()      const override;
    CustomString getEntityType() const override { return CustomString("FreightTruck"); }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};

// ============================================================
// FILE: ElectricScooter.h  (combined for organisation)
// OUTPUT-ID: M3-ESC-001
// Expected Output: Scooter info with battery level, docking
// ============================================================

class ElectricScooter : public Vehicle {
private:
    int m_batteryLevelPercent;  // 0-100
    int m_dockingStationId;     // Which station it's docked at (-1 = in use)

public:
    ElectricScooter();
    ElectricScooter(const CustomString& regNo, int dockingStationId, double purchaseValue);
    ElectricScooter(const ElectricScooter& other);
    ~ElectricScooter() override = default;

    // Getters
    int  getBatteryLevel()     const { return m_batteryLevelPercent; }
    int  getDockingStationId() const { return m_dockingStationId;    }
    bool isLowBattery()        const { return m_batteryLevelPercent < 20; }
    bool isDocked()            const { return m_dockingStationId >= 0;   }

    // Methods
    void charge(int percent);    // Increase battery
    void useBattery(int percent); // Decrease battery (per trip)
    void dock(int stationId)   { m_dockingStationId = stationId; }
    void undock()              { m_dockingStationId = -1;        }

    // [OOP: override all pure virtuals]
    double       getFuelCostPerKm()   const override; // Electricity cost (cheap)
    CustomString getVehicleCategory() const override;
    bool         performDailyCheck()  const override; // Checks battery health
    double       calculateEmissions(double km) const override; // Returns 0.0 (electric)

    CustomString toString()      const override;
    CustomString getEntityType() const override { return CustomString("ElectricScooter"); }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};
