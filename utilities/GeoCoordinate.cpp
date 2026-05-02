// ============================================================
// FILE: GeoCoordinate.cpp
// MODULE: 1 - Utility Layer
// ============================================================

#include "GeoCoordinate.h"
#include "../exceptions/TransitExceptions.h"
#include <cmath>   // sin, cos, sqrt, atan2, M_PI
#include <cstdio>  // sprintf

// Pi constant
static const double PI = 3.14159265358979323846;

// Convert degrees to radians
static double toRad(double deg) { return deg * PI / 180.0; }

// -----------------------------------------------------------
// CONSTRUCTORS
// -----------------------------------------------------------

GeoCoordinate::GeoCoordinate() : m_latitude(0.0), m_longitude(0.0) {}

GeoCoordinate::GeoCoordinate(double lat, double lon) {
    if (lat < -90.0 || lat > 90.0 || lon < -180.0 || lon > 180.0) {
        throw InvalidCoordinateException("GeoCoordinate::GeoCoordinate");
    }
    m_latitude  = lat;
    m_longitude = lon;
}

GeoCoordinate::GeoCoordinate(const GeoCoordinate& other)
    : m_latitude(other.m_latitude), m_longitude(other.m_longitude) {}

GeoCoordinate& GeoCoordinate::operator=(const GeoCoordinate& other) {
    m_latitude  = other.m_latitude;
    m_longitude = other.m_longitude;
    return *this;
}

// -----------------------------------------------------------
// COMPARISON OPERATORS
// -----------------------------------------------------------
bool GeoCoordinate::operator==(const GeoCoordinate& other) const {
    // Within 0.0001 degree (about 11 meters)
    double epsilon = 0.0001;
    return std::abs(m_latitude  - other.m_latitude)  < epsilon &&
           std::abs(m_longitude - other.m_longitude) < epsilon;
}

bool GeoCoordinate::operator!=(const GeoCoordinate& other) const {
    return !(*this == other);
}

// Subtraction operator -> returns distance in km
double GeoCoordinate::operator-(const GeoCoordinate& other) const {
    return distanceTo(other);
}

std::ostream& operator<<(std::ostream& os, const GeoCoordinate& g) {
    os << g.toString().c_str();
    return os;
}

// -----------------------------------------------------------
// HAVERSINE FORMULA
// Calculates the shortest distance between two points on a sphere
// This is the real formula used in GPS systems!
// -----------------------------------------------------------
double GeoCoordinate::distanceTo(const GeoCoordinate& other) const {
    const double R = 6371.0;  // Earth's radius in km

    double lat1 = toRad(m_latitude);
    double lat2 = toRad(other.m_latitude);
    double dLat = toRad(other.m_latitude  - m_latitude);
    double dLon = toRad(other.m_longitude - m_longitude);

    // Haversine formula
    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(lat1) * cos(lat2) *
               sin(dLon / 2) * sin(dLon / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return R * c;  // Distance in km
}

// Calculates compass bearing from this point to another
double GeoCoordinate::bearingTo(const GeoCoordinate& other) const {
    double dLon = toRad(other.m_longitude - m_longitude);
    double lat1 = toRad(m_latitude);
    double lat2 = toRad(other.m_latitude);

    double y = sin(dLon) * cos(lat2);
    double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
    double bearing = atan2(y, x) * 180.0 / PI;

    return fmod(bearing + 360.0, 360.0);  // Normalize to 0-360
}

CustomString GeoCoordinate::toString() const {
    char buf[40];
    sprintf(buf, "%.4f, %.4f", m_latitude, m_longitude);
    return CustomString(buf);
}

bool GeoCoordinate::isValid() const {
    return m_latitude  >= -90.0  && m_latitude  <= 90.0 &&
           m_longitude >= -180.0 && m_longitude <= 180.0;
}

std::istream& operator>>(std::istream& is, GeoCoordinate& g) {
    double lat = 0.0;
    double lon = 0.0;
    char comma = 0;
    is >> lat;
    if (is.peek() == ',') is >> comma;
    is >> lon;
    g = GeoCoordinate(lat, lon);
    return is;
}
