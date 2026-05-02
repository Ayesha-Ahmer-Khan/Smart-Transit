#pragma once
// ============================================================
// FILE: CustomDate.h
// MODULE: 1 - Utility Layer
// OUTPUT-ID: M1-DT-001
// Expected Output: Date arithmetic, comparisons, formatted output
// OOP CONCEPTS: operator overloading, static methods, const methods
// ============================================================

#include <iostream>
#include "CustomString.h"

class CustomDate {
private:
    int m_day;
    int m_month;
    int m_year;

    bool isValidDate(int d, int m, int y) const;

public:
    // Constructors
    CustomDate();                          // Default: 1 Jan 2000
    CustomDate(int day, int month, int year); // Throws InvalidDateException
    CustomDate(const CustomDate& other);   // Copy constructor

    // Destructor (nothing on heap, but good practice)
    ~CustomDate() = default;

    // Assignment
    CustomDate& operator=(const CustomDate& other);

    // Arithmetic operators
    int         operator-(const CustomDate& other) const; // day difference
    CustomDate  operator+(int days)  const;  // add N days
    CustomDate& operator++();                // next day (prefix)

    // Comparison operators
    bool operator==(const CustomDate& other) const;
    bool operator!=(const CustomDate& other) const;
    bool operator< (const CustomDate& other) const;
    bool operator<=(const CustomDate& other) const;
    bool operator> (const CustomDate& other) const;
    bool operator>=(const CustomDate& other) const;

    // Stream operators
    friend std::ostream& operator<<(std::ostream& os, const CustomDate& d);
    friend std::istream& operator>>(std::istream& is, CustomDate& d);

    // Getters
    int getDay()   const { return m_day;   }
    int getMonth() const { return m_month; }
    int getYear()  const { return m_year;  }

    // Methods
    bool         isLeapYear()   const;
    CustomString getDayOfWeek() const;
    CustomDate   addMonths(int months) const;
    bool         isWeekend()    const;
    CustomString toString()     const;  // "DD/MM/YYYY"
    int          daysInMonth()  const;

    // [OOP: Static method - belongs to class, not any object]
    static CustomDate today();
    static CustomDate fromString(const CustomString& value);
};

// ============================================================
// FILE: CustomTime.h  (combined in same file for simplicity)
// OUTPUT-ID: M1-TM-001
// ============================================================

class CustomTime {
private:
    int m_hour;
    int m_minute;
    int m_second;

    bool isValidTime(int h, int m, int s) const;

public:
    CustomTime();
    CustomTime(int hour, int minute, int second);
    CustomTime(const CustomTime& other);
    ~CustomTime() = default;

    CustomTime& operator=(const CustomTime& other);
    int         operator-(const CustomTime& other) const; // seconds difference
    CustomTime  operator+(int seconds) const;
    CustomTime& operator++();  // add one minute

    bool operator==(const CustomTime& other) const;
    bool operator< (const CustomTime& other) const;
    bool operator> (const CustomTime& other) const;

    friend std::ostream& operator<<(std::ostream& os, const CustomTime& t);
    friend std::istream& operator>>(std::istream& is, CustomTime& t);

    int getHour()   const { return m_hour;   }
    int getMinute() const { return m_minute; }
    int getSecond() const { return m_second; }

    CustomString toString24h() const;  // "HH:MM:SS"
    CustomString toString12h() const;  // "HH:MM AM/PM"
    CustomTime   addMinutes(int mins) const;
    static CustomTime fromString(const CustomString& value);

    // [OOP: Static method]
    static CustomTime now();
};
