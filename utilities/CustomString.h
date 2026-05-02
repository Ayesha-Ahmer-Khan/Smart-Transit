#pragma once
// ============================================================
// FILE: CustomString.h
// MODULE: 1 - Utility Layer
// OUTPUT-ID: M1-STR-001
// Expected Output: When tested, will print string operations
//                  like concatenation, comparison, etc.
// OOP CONCEPTS SHOWN:
//   - Class design with private data
//   - Constructors (default, parameterized, copy, move)
//   - Destructor (memory cleanup)
//   - Operator overloading (=, +, +=, ==, !=, <, >, [], <<, >>)
//   - Friend functions
//   - const correctness
// ============================================================

#include <iostream>  // For std::ostream and std::istream
#include "../datastructs/DynamicArray.h"
// NOTE: std::string is NOT used inside this class (project rule)

class CustomString {
private:
    // [OOP: Encapsulation - data is private]
    char* m_data;    // Raw character array on heap
    int   m_length;  // Length of string (not counting '\0')

    // Private helper: copies a C-string into m_data
    void copyFrom(const char* src);

public:
    // -----------------------------------------------------------
    // CONSTRUCTORS
    // [OOP: Multiple constructors = constructor overloading]
    // -----------------------------------------------------------
    CustomString();                          // Default: empty string
    CustomString(const char* str);           // From C-string: CustomString s("hello")
    CustomString(const CustomString& other); // Copy constructor
    CustomString(CustomString&& other) noexcept; // Move constructor (C++11)

    // -----------------------------------------------------------
    // DESTRUCTOR
    // [OOP: Must free heap memory to avoid memory leaks]
    // -----------------------------------------------------------
    ~CustomString();

    // -----------------------------------------------------------
    // ASSIGNMENT OPERATORS
    // [OOP: Operator Overloading]
    // -----------------------------------------------------------
    CustomString& operator=(const CustomString& other);  // copy assign
    CustomString& operator=(CustomString&& other) noexcept; // move assign

    // -----------------------------------------------------------
    // ARITHMETIC / CONCATENATION OPERATORS
    // -----------------------------------------------------------
    CustomString  operator+(const CustomString& other) const; // "hello" + " world"
    CustomString& operator+=(const CustomString& other);      // s += " world"

    // -----------------------------------------------------------
    // COMPARISON OPERATORS
    // -----------------------------------------------------------
    bool operator==(const CustomString& other) const;
    bool operator!=(const CustomString& other) const;
    bool operator< (const CustomString& other) const; // alphabetic less
    bool operator> (const CustomString& other) const; // alphabetic greater

    // -----------------------------------------------------------
    // INDEX OPERATOR (bounds-checked)
    // -----------------------------------------------------------
    char& operator[](int index);              // Read-write access
    const char& operator[](int index) const; // Read-only access

    // -----------------------------------------------------------
    // STREAM OPERATORS (friend = can access private data)
    // [OOP: Friend function]
    // -----------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const CustomString& s);
    friend std::istream& operator>>(std::istream& is, CustomString& s);

    // -----------------------------------------------------------
    // UTILITY METHODS
    // -----------------------------------------------------------
    int  length()  const { return m_length; }
    bool isEmpty() const { return m_length == 0; }
    const char* c_str() const { return m_data; }

    CustomString toUpperCase()  const;
    CustomString toLowerCase()  const;
    CustomString trim()         const;
    CustomString substring(int start, int len) const;
    CustomString replace(const CustomString& oldText, const CustomString& newText) const;
    DynamicArray<CustomString> split(char delimiter) const;
    bool contains(const CustomString& sub) const;
    int  find(const CustomString& sub)     const; // -1 if not found
    bool startsWith(const CustomString& prefix) const;
    bool endsWith(const CustomString& suffix)   const;
    int  toInt()    const;
    double toDouble() const;
};
