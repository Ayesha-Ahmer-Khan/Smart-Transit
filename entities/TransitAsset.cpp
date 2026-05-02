// ============================================================
// FILE: TransitAsset.cpp
// MODULE: 3 - Entity Layer
// ============================================================

#include "TransitAsset.h"
#include <cstdio>  // sprintf

// [OOP: Constructor calls PARENT constructor in initializer list]
TransitAsset::TransitAsset()
    : Entity(),                          // Call Entity() first
      m_assetCode("UNKNOWN"),
      m_purchaseDate(CustomDate::today()),
      m_purchaseValue(0.0),
      m_currentValue(0.0),
      m_status(AssetStatus::ACTIVE) {
}

TransitAsset::TransitAsset(const CustomString& code, double purchaseValue, const CustomDate& purchaseDate)
    : Entity(),                          // Parent constructor first!
      m_assetCode(code),
      m_purchaseDate(purchaseDate),
      m_purchaseValue(purchaseValue),
      m_currentValue(purchaseValue),     // Starts at full value
      m_status(AssetStatus::ACTIVE) {
}

TransitAsset::TransitAsset(const TransitAsset& other)
    : Entity(other),                     // Copy parent data too
      m_assetCode(other.m_assetCode),
      m_purchaseDate(other.m_purchaseDate),
      m_purchaseValue(other.m_purchaseValue),
      m_currentValue(other.m_currentValue),
      m_status(other.m_status),
      m_location(other.m_location) {
}

// Reduce value by 10% per year of depreciation
void TransitAsset::depreciate(int years) {
    for (int i = 0; i < years; i++) {
        m_currentValue *= 0.90;  // 10% depreciation per year
    }
    markUpdated();
}

void TransitAsset::scheduleMaintenance() {
    m_status = AssetStatus::UNDER_MAINTENANCE;
    markUpdated();
}

void TransitAsset::decommission() {
    m_status = AssetStatus::DECOMMISSIONED;
    markUpdated();
}

int TransitAsset::getDaysInService() const {
    CustomDate today = CustomDate::today();
    return today - m_purchaseDate;
}

CustomString TransitAsset::getStatusString() const {
    switch (m_status) {
        case AssetStatus::ACTIVE:             return CustomString("ACTIVE");
        case AssetStatus::UNDER_MAINTENANCE:  return CustomString("UNDER MAINTENANCE");
        case AssetStatus::DECOMMISSIONED:     return CustomString("DECOMMISSIONED");
        default:                              return CustomString("UNKNOWN");
    }
}

bool TransitAsset::operator<(const TransitAsset& other) const {
    return m_purchaseValue < other.m_purchaseValue;
}

int TransitAsset::operator-(const TransitAsset& other) const {
    return m_purchaseDate - other.m_purchaseDate;
}

CustomString TransitAsset::toString() const {
    char buf[256];
    sprintf(buf, "Asset[%d] Code:%s Status:%s Value:%.2f",
            m_id, m_assetCode.c_str(), getStatusString().c_str(), m_currentValue);
    return CustomString(buf);
}

void TransitAsset::serialize(std::ofstream& out) const {
    out << m_id << "\n";
    out << m_assetCode.c_str() << "\n";
    out << m_purchaseValue << "\n";
    out << m_currentValue  << "\n";
    out << static_cast<int>(m_status) << "\n";
}

void TransitAsset::deserialize(std::ifstream& in) {
    std::string code;
    in >> m_id >> code >> m_purchaseValue >> m_currentValue;
    int status;
    in >> status;
    m_assetCode = CustomString(code.c_str());
    m_status    = static_cast<AssetStatus>(status);
}
