// ============================================================
// FILE: CustomDate.cpp
// MODULE: 1 - Utility Layer
// ============================================================

#include "CustomDate.h"
#include "../exceptions/TransitExceptions.h"
#include <ctime>    // For static today() and now()
#include <sstream>  // For building strings
#include <cstdio>   // sprintf

// -----------------------------------------------------------
// CUSTOMDATE IMPLEMENTATION
// -----------------------------------------------------------

bool CustomDate::isValidDate(int d, int m, int y) const {
    if (y < 1900 || y > 2100) return false;
    if (m < 1 || m > 12)      return false;
    if (d < 1)                 return false;

    int maxDays[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    // February gets 29 days in a leap year
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0))
        maxDays[2] = 29;

    return d <= maxDays[m];
}

// Default constructor: 1 January 2000
CustomDate::CustomDate() : m_day(1), m_month(1), m_year(2000) {}

// Parameterized constructor - throws exception for bad dates
CustomDate::CustomDate(int day, int month, int year) {
    if (!isValidDate(day, month, year)) {
        throw InvalidDateException("CustomDate::CustomDate");
    }
    m_day   = day;
    m_month = month;
    m_year  = year;
}

// Copy constructor
CustomDate::CustomDate(const CustomDate& other)
    : m_day(other.m_day), m_month(other.m_month), m_year(other.m_year) {}

// Copy assignment
CustomDate& CustomDate::operator=(const CustomDate& other) {
    if (this == &other) return *this;
    m_day   = other.m_day;
    m_month = other.m_month;
    m_year  = other.m_year;
    return *this;
}

// Subtract two dates -> returns number of days between them
// Simple calculation using total days since year 0
int CustomDate::operator-(const CustomDate& other) const {
    // Convert both dates to total days
    auto toDays = [](int d, int m, int y) -> long long {
        // Simplified calculation
        long long total = y * 365LL + d;
        int months[] = {0,0,31,59,90,120,151,181,212,243,273,304,334};
        total += months[m];
        total += (y - 1) / 4 - (y - 1) / 100 + (y - 1) / 400;
        return total;
    };
    long long d1 = toDays(m_day, m_month, m_year);
    long long d2 = toDays(other.m_day, other.m_month, other.m_year);
    return static_cast<int>(d1 - d2);
}

// Add N days to a date
CustomDate CustomDate::operator+(int days) const {
    int d = m_day;
    int m = m_month;
    int y = m_year;

    auto dim = [](int month, int year) {
        int maxDays[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
        bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (month == 2 && leap) return 29;
        return maxDays[month];
    };

    if (days >= 0) {
        while (days-- > 0) {
            ++d;
            if (d > dim(m, y)) {
                d = 1;
                ++m;
                if (m > 12) { m = 1; ++y; }
            }
        }
    } else {
        while (days++ < 0) {
            --d;
            if (d < 1) {
                --m;
                if (m < 1) { m = 12; --y; }
                d = dim(m, y);
            }
        }
    }

    return CustomDate(d, m, y);
}

// Prefix ++ -> go to next day
CustomDate& CustomDate::operator++() {
    *this = *this + 1;
    return *this;
}

bool CustomDate::operator==(const CustomDate& other) const {
    return m_day == other.m_day && m_month == other.m_month && m_year == other.m_year;
}

bool CustomDate::operator!=(const CustomDate& other) const {
    return !(*this == other);
}

bool CustomDate::operator<(const CustomDate& other) const {
    if (m_year  != other.m_year)  return m_year  < other.m_year;
    if (m_month != other.m_month) return m_month < other.m_month;
    return m_day < other.m_day;
}

bool CustomDate::operator<=(const CustomDate& other) const {
    return *this < other || *this == other;
}

bool CustomDate::operator>(const CustomDate& other) const {
    return !(*this <= other);
}

bool CustomDate::operator>=(const CustomDate& other) const {
    return !(*this < other);
}

std::ostream& operator<<(std::ostream& os, const CustomDate& d) {
    os << d.toString().c_str();
    return os;
}

bool CustomDate::isLeapYear() const {
    return (m_year % 4 == 0 && m_year % 100 != 0) || (m_year % 400 == 0);
}

bool CustomDate::isWeekend() const {
    // Use Zeller's algorithm to find day of week
    // 0=Sunday, 1=Monday, ..., 6=Saturday
    int d = m_day, m = m_month, y = m_year;
    if (m < 3) { m += 12; y--; }
    int k = y % 100, j = y / 100;
    int h = (d + (13*(m+1))/5 + k + k/4 + j/4 - 2*j) % 7;
    // h: 0=Sat, 1=Sun, 2=Mon, ..., 6=Fri
    return h == 0 || h == 1; // Saturday or Sunday
}

CustomString CustomDate::toString() const {
    char buf[20];
    sprintf(buf, "%02d/%02d/%04d", m_day, m_month, m_year);
    return CustomString(buf);
}

int CustomDate::daysInMonth() const {
    int maxDays[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m_month == 2 && isLeapYear()) return 29;
    return maxDays[m_month];
}

// Static: get today's date from system clock
CustomDate CustomDate::today() {
    time_t t = time(nullptr);
    tm* lt   = localtime(&t);
    return CustomDate(lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900);
}

// -----------------------------------------------------------
// CUSTOMTIME IMPLEMENTATION
// -----------------------------------------------------------

bool CustomTime::isValidTime(int h, int m, int s) const {
    return h >= 0 && h < 24 && m >= 0 && m < 60 && s >= 0 && s < 60;
}

CustomTime::CustomTime() : m_hour(0), m_minute(0), m_second(0) {}

CustomTime::CustomTime(int hour, int minute, int second) {
    if (!isValidTime(hour, minute, second)) {
        throw InvalidTimeException("CustomTime::CustomTime");
    }
    m_hour   = hour;
    m_minute = minute;
    m_second = second;
}

CustomTime::CustomTime(const CustomTime& other)
    : m_hour(other.m_hour), m_minute(other.m_minute), m_second(other.m_second) {}

CustomTime& CustomTime::operator=(const CustomTime& other) {
    m_hour   = other.m_hour;
    m_minute = other.m_minute;
    m_second = other.m_second;
    return *this;
}

// Difference in seconds
int CustomTime::operator-(const CustomTime& other) const {
    int s1 = m_hour * 3600 + m_minute * 60 + m_second;
    int s2 = other.m_hour * 3600 + other.m_minute * 60 + other.m_second;
    return s1 - s2;
}

// Add seconds
CustomTime CustomTime::operator+(int seconds) const {
    int total = m_hour * 3600 + m_minute * 60 + m_second + seconds;
    total     = total % 86400; // wrap around midnight
    if (total < 0) total += 86400;
    return CustomTime(total / 3600, (total % 3600) / 60, total % 60);
}

// Prefix ++ = add one minute
CustomTime& CustomTime::operator++() {
    *this = *this + 60;
    return *this;
}

bool CustomTime::operator==(const CustomTime& other) const {
    return m_hour == other.m_hour && m_minute == other.m_minute && m_second == other.m_second;
}

bool CustomTime::operator<(const CustomTime& other) const {
    if (m_hour   != other.m_hour)   return m_hour   < other.m_hour;
    if (m_minute != other.m_minute) return m_minute < other.m_minute;
    return m_second < other.m_second;
}

bool CustomTime::operator>(const CustomTime& other) const {
    return other < *this;
}

std::ostream& operator<<(std::ostream& os, const CustomTime& t) {
    os << t.toString24h().c_str();
    return os;
}

CustomString CustomTime::toString24h() const {
    char buf[12];
    sprintf(buf, "%02d:%02d:%02d", m_hour, m_minute, m_second);
    return CustomString(buf);
}

CustomString CustomTime::toString12h() const {
    char buf[15];
    int displayHour = m_hour % 12;
    if (displayHour == 0) displayHour = 12;
    const char* period = m_hour < 12 ? "AM" : "PM";
    sprintf(buf, "%02d:%02d %s", displayHour, m_minute, period);
    return CustomString(buf);
}

CustomTime CustomTime::addMinutes(int mins) const {
    return *this + (mins * 60);
}

CustomTime CustomTime::now() {
    time_t t = time(nullptr);
    tm* lt   = localtime(&t);
    return CustomTime(lt->tm_hour, lt->tm_min, lt->tm_sec);
}

std::istream& operator>>(std::istream& is, CustomDate& d) {
    char buffer[32];
    is >> buffer;
    d = CustomDate::fromString(CustomString(buffer));
    return is;
}

CustomString CustomDate::getDayOfWeek() const {
    int d = m_day, m = m_month, y = m_year;
    if (m < 3) { m += 12; y--; }
    int k = y % 100;
    int j = y / 100;
    int h = (d + (13 * (m + 1)) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
    const char* names[] = {"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    return CustomString(names[h]);
}

CustomDate CustomDate::addMonths(int months) const {
    int d = m_day;
    int m = m_month + months;
    int y = m_year;
    while (m > 12) { m -= 12; ++y; }
    while (m < 1) { m += 12; --y; }
    CustomDate temp(1, m, y);
    int maxDay = temp.daysInMonth();
    if (d > maxDay) d = maxDay;
    return CustomDate(d, m, y);
}

CustomDate CustomDate::fromString(const CustomString& value) {
    int d = 0, m = 0, y = 0;
    if (std::sscanf(value.c_str(), "%d/%d/%d", &d, &m, &y) != 3 &&
        std::sscanf(value.c_str(), "%d-%d-%d", &d, &m, &y) != 3) {
        throw InvalidDateException("CustomDate::fromString");
    }
    return CustomDate(d, m, y);
}

std::istream& operator>>(std::istream& is, CustomTime& t) {
    char buffer[32];
    is >> buffer;
    t = CustomTime::fromString(CustomString(buffer));
    return is;
}

CustomTime CustomTime::fromString(const CustomString& value) {
    int h = 0, m = 0, s = 0;
    if (std::sscanf(value.c_str(), "%d:%d:%d", &h, &m, &s) != 3) {
        throw InvalidTimeException("CustomTime::fromString");
    }
    return CustomTime(h, m, s);
}
