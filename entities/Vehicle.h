#pragma once
// ============================================================
// FILE: Vehicle.h
// MODULE: 3 - Entity Layer (LEVEL 3)
// OUTPUT-ID: M3-VEH-001
// Expected Output: Vehicle info, occupancy, fuel cost
// OOP CONCEPTS:
//   - Pure virtual methods (getFuelCostPerKm, etc.)
//   - Enum class for fuel type
//   - Operator overloading (- for distance between vehicles)
//   - Polymorphism: Vehicle* can point to CityBus, MetroTrain, etc.
// ============================================================

#include "TransitAsset.h"

// [OOP: Enum class - strongly typed constants]
enum class FuelType {
    DIESEL,
    CNG,
    ELECTRIC,
    HYBRID
};

// [OOP: Abstract class - has pure virtual methods]
// You CANNOT do: Vehicle v;  (compiler will refuse)
// But you CAN do: Vehicle* v = new CityBus(...)  (polymorphism)
class Vehicle : public TransitAsset {
protected:
    CustomString  m_registrationNo;    // "ABC-123"
    CustomString  m_make;              // "Toyota"
    CustomString  m_model;             // "Hiace"
    CustomString  m_colour;
    FuelType      m_fuelType;
    int           m_passengerCapacity; // Max passengers
    int           m_currentPassengers; // Current count
    double        m_odometer;          // Total km traveled
    GeoCoordinate m_currentPosition;
    int           m_assignedDriverId;
    int           m_currentRouteId;

public:
    Vehicle();
    Vehicle(const CustomString& regNo, const CustomString& make,
            const CustomString& model, FuelType fuelType,
            int capacity, double purchaseValue);
    Vehicle(const Vehicle& other);
    virtual ~Vehicle() = default;

    // -----------------------------------------------------------
    // PURE VIRTUAL METHODS
    // [OOP: Forces ALL concrete vehicle types to implement these]
    // -----------------------------------------------------------
    virtual double       getFuelCostPerKm()   const = 0;  // Each type has different cost
    virtual CustomString getVehicleCategory() const = 0;  // "Bus", "Train", etc.
    virtual bool         performDailyCheck()  const = 0;  // Returns pass/fail
    virtual double       calculateEmissions(double km) const = 0;

    // -----------------------------------------------------------
    // NON-VIRTUAL METHODS (same for all vehicles)
    // -----------------------------------------------------------
    void boardPassenger();    // Throws VehicleOverCapacityException if full
    void alightPassenger();   // Decrease passenger count
    int  getOccupancyPercent() const;
    bool isOverCapacity()      const;
    void updatePosition(const GeoCoordinate& newPos);
    CustomString getFuelTypeString() const;

    // Getters
    CustomString  getRegistrationNo()    const { return m_registrationNo;    }
    CustomString  getMake()              const { return m_make;              }
    CustomString  getModel()             const { return m_model;             }
    FuelType      getFuelType()          const { return m_fuelType;          }
    int           getCapacity()          const { return m_passengerCapacity; }
    int           getCurrentPassengers() const { return m_currentPassengers; }
    double        getOdometer()          const { return m_odometer;          }
    GeoCoordinate getCurrentPosition()   const { return m_currentPosition;   }
    int           getAssignedDriverId()  const { return m_assignedDriverId;  }

    // Setters
    void assignDriver(int driverId) { m_assignedDriverId = driverId; markUpdated(); }
    void assignRoute(int routeId)   { m_currentRouteId   = routeId;  markUpdated(); }
    void addOdometerKm(double km)   { m_odometer += km;              markUpdated(); }

    // Operators
    bool   operator<(const Vehicle& other) const; // by registration number
    double operator-(const Vehicle& other) const; // distance between vehicles

    // Override from TransitAsset
    CustomString toString()      const override;
    CustomString getEntityType() const override { return CustomString("Vehicle"); }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};
