#pragma once
// ============================================================
// FILE: TransitAsset.h
// MODULE: 3 - Entity Layer (LEVEL 2)
// OUTPUT-ID: M3-TA-001
// Expected Output: Asset details, depreciation value
// OOP CONCEPT: Inheritance (Level 2 from Entity)
//              Enum class for status
// ============================================================

#include "Entity.h"
#include "../utilities/GeoCoordinate.h"

// [OOP: Enum class - type-safe named constants]
enum class AssetStatus {
    ACTIVE,
    UNDER_MAINTENANCE,
    DECOMMISSIONED
};

class TransitAsset : public Entity {
protected:
    // [OOP: protected - visible to this class AND its subclasses]
    CustomString m_assetCode;      // e.g., "BUS-001"
    CustomDate   m_purchaseDate;
    double       m_purchaseValue;  // How much it cost when bought
    double       m_currentValue;   // Current value after depreciation
    AssetStatus  m_status;
    GeoCoordinate m_location;

public:
    // Constructors
    TransitAsset();
    TransitAsset(const CustomString& code, double purchaseValue, const CustomDate& purchaseDate);
    TransitAsset(const TransitAsset& other);

    virtual ~TransitAsset() = default;

    // Getters
    CustomString  getAssetCode()     const { return m_assetCode;     }
    double        getPurchaseValue() const { return m_purchaseValue; }
    double        getCurrentValue()  const { return m_currentValue;  }
    AssetStatus   getStatus()        const { return m_status;        }
    GeoCoordinate getLocation()      const { return m_location;      }

    // Setters (validate input)
    void setLocation(const GeoCoordinate& loc) { m_location = loc; }
    void setStatus(AssetStatus status)          { m_status   = status; markUpdated(); }

    // Methods
    void depreciate(int years);         // Reduce current value by 10% per year
    void scheduleMaintenance();         // Set status to UNDER_MAINTENANCE
    void decommission();                // Set status to DECOMMISSIONED
    int  getDaysInService()    const;   // Days since purchase
    CustomString getStatusString() const;

    // Operators
    bool operator<(const TransitAsset& other) const;  // by purchase value
    int  operator-(const TransitAsset& other) const;  // age difference in days

    // [OOP: override keyword - tells compiler this overrides a virtual method]
    CustomString toString()      const override;
    CustomString getEntityType() const override { return CustomString("TransitAsset"); }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};
