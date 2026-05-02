#pragma once
// ============================================================
// FILE: CityBus.h
// MODULE: 3 - Entity Layer (LEVEL 4 - Concrete Vehicle)
// OUTPUT-ID: M3-BUS-001
// Expected Output: Bus details with route and AC info
// OOP CONCEPTS:
//   - Inherits from Vehicle (Level 3)
//   - override keyword on all pure virtual methods
//   - Adds its own specific fields
// ============================================================

#include "Vehicle.h"

// [OOP: Level 4 - Inherits from Vehicle which inherits from TransitAsset
//       which inherits from Entity]
// Chain: Entity -> TransitAsset -> Vehicle -> CityBus
class CityBus : public Vehicle {
protected:
    // protected so Level5 can access
public:
    CustomString m_busNumber;       // "Bus-17A"
    int          m_routeId;
    bool         m_isAirConditioned;
    int          m_nextStopId;

public:
    CityBus();
    CityBus(const CustomString& regNo, const CustomString& busNumber,
            int routeId, bool isAC, double purchaseValue);
    CityBus(const CityBus& other);
    ~CityBus() override = default;

    // Getters
    CustomString getBusNumber()      const { return m_busNumber;       }
    bool         isAirConditioned()  const { return m_isAirConditioned; }
    int          getRouteId()        const { return m_routeId;          }

    // -----------------------------------------------------------
    // OVERRIDE PURE VIRTUAL METHODS (from Vehicle)
    // [OOP: override keyword confirms we are implementing the abstract methods]
    // -----------------------------------------------------------
    double       getFuelCostPerKm()   const override;  // Diesel/CNG rate
    CustomString getVehicleCategory() const override;  // Returns "City Bus"
    bool         performDailyCheck()  const override;  // Tyre + brake check
    double       calculateEmissions(double km) const override;

    // Override from Entity chain
    CustomString toString()      const override;
    CustomString getEntityType() const override { return CustomString("CityBus"); }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};

// ============================================================
// FILE: MetroTrain.h (combined for organization)
// OUTPUT-ID: M3-MTR-001
// ============================================================

class MetroTrain : public Vehicle {
protected:
    int    m_lineId;
    int    m_carriageCount;
    double m_maxSpeed;
    bool   m_isUnderground;

public:
    MetroTrain();
    MetroTrain(const CustomString& regNo, int lineId, int carriages,
               double maxSpeed, bool underground, double purchaseValue);
    MetroTrain(const MetroTrain& other);
    ~MetroTrain() override = default;

    int    getLineId()        const { return m_lineId;        }
    int    getCarriageCount() const { return m_carriageCount; }
    double getMaxSpeed()      const { return m_maxSpeed;      }
    bool   isUnderground()    const { return m_isUnderground; }

    // [OOP: override - implements all pure virtual methods from Vehicle]
    double       getFuelCostPerKm()   const override;  // Electric rate
    CustomString getVehicleCategory() const override;
    bool         performDailyCheck()  const override;
    double       calculateEmissions(double km) const override; // Returns 0 (electric)

    CustomString toString()      const override;
    CustomString getEntityType() const override { return CustomString("MetroTrain"); }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};

// ============================================================
// FILE: RideHailCar.h (combined for organization)
// OUTPUT-ID: M3-RHC-001
// ============================================================

class RideHailCar : public Vehicle {
private:
    double m_driverRating;    // 1.0 - 5.0
    int    m_tripCount;
    double m_surgeMultiplier; // 1.0 = normal, 1.5 = surge

public:
    RideHailCar();
    RideHailCar(const CustomString& regNo, const CustomString& make,
                const CustomString& model, double purchaseValue);
    RideHailCar(const RideHailCar& other);
    ~RideHailCar() override = default;

    double getDriverRating()    const { return m_driverRating;    }
    double getSurgeMultiplier() const { return m_surgeMultiplier; }
    int    getTripCount()       const { return m_tripCount;       }

    void setSurgeMultiplier(double surge) { m_surgeMultiplier = surge; }
    void incrementTripCount()             { m_tripCount++;             }

    double       getFuelCostPerKm()   const override;
    CustomString getVehicleCategory() const override;
    bool         performDailyCheck()  const override;
    double       calculateEmissions(double km) const override;

    CustomString toString()      const override;
    CustomString getEntityType() const override { return CustomString("RideHailCar"); }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};
