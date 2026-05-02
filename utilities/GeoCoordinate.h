#pragma once
// ============================================================
// FILE: GeoCoordinate.h
// MODULE: 1 - Utility Layer
// OUTPUT-ID: M1-GEO-001
// Expected Output: Distance calculations, coordinate display
// OOP CONCEPTS: operator overloading, const methods
// ============================================================

#include <iostream>
#include "CustomString.h"

class GeoCoordinate {
private:
    double m_latitude;   // Range: -90.0 to 90.0
    double m_longitude;  // Range: -180.0 to 180.0

public:
    // Constructors
    GeoCoordinate();                            // Default: (0, 0) = Null Island
    GeoCoordinate(double lat, double lon);      // Throws InvalidCoordinateException
    GeoCoordinate(const GeoCoordinate& other);  // Copy
    ~GeoCoordinate() = default;

    // Assignment
    GeoCoordinate& operator=(const GeoCoordinate& other);

    // Comparison (within 0.0001 degree epsilon)
    bool operator==(const GeoCoordinate& other) const;
    bool operator!=(const GeoCoordinate& other) const;

    // Subtraction -> returns distance in km (Haversine formula)
    double operator-(const GeoCoordinate& other) const;

    // Stream operators
    friend std::ostream& operator<<(std::ostream& os, const GeoCoordinate& g);
    friend std::istream& operator>>(std::istream& is, GeoCoordinate& g);

    // Getters
    double getLatitude()  const { return m_latitude;  }
    double getLongitude() const { return m_longitude; }

    // Methods
    double       distanceTo(const GeoCoordinate& other) const;
    double       bearingTo (const GeoCoordinate& other) const;
    CustomString toString() const;   // "lat,lon"
    bool         isValid()  const;
};
