#pragma once
// ============================================================
// FILE: TransitExceptions.h
// MODULE: 6 - Exception Handling Layer
// OUTPUT-ID: M6-EX-001
// Expected Output: This file is a header. No direct output.
//                  When an exception is thrown and caught,
//                  you will see its message printed.
// OOP CONCEPT: Inheritance - 3-level exception hierarchy
// ============================================================

#include <exception>   // std::exception (base of everything)
#include <string>      // Only allowed here for what() return type

// ===========================================================
// LEVEL 1: Base Exception (Abstract)
// All our exceptions inherit from this
// ===========================================================
class TransitException : public std::exception {
protected:
    std::string m_message;   // The error message
    std::string m_source;    // Which function threw this
    int         m_code;      // Error code number

public:
    // [OOP CONCEPT: Constructor with parameters]
    TransitException(const std::string& msg, const std::string& src, int code)
        : m_message(msg), m_source(src), m_code(code) {}

    // [OOP CONCEPT: virtual destructor - MUST have in base class]
    virtual ~TransitException() = default;

    // Override from std::exception
    const char* what() const noexcept override {
        return m_message.c_str();
    }

    // [OOP CONCEPT: Pure Virtual Method - makes this class abstract]
    virtual std::string getCategory() const = 0;

    // Getters
    std::string getMessage() const { return m_message; }
    std::string getSource()  const { return m_source;  }
    int         getCode()    const { return m_code;    }
};

// ===========================================================
// LEVEL 2: Category Exceptions (8 categories)
// These inherit from TransitException
// ===========================================================

class VehicleException : public TransitException {
public:
    VehicleException(const std::string& msg, const std::string& src, int code)
        : TransitException(msg, src, code) {}
    std::string getCategory() const override { return "VEHICLE"; }
};

class RouteException : public TransitException {
public:
    RouteException(const std::string& msg, const std::string& src, int code)
        : TransitException(msg, src, code) {}
    std::string getCategory() const override { return "ROUTE"; }
};

class TripException : public TransitException {
public:
    TripException(const std::string& msg, const std::string& src, int code)
        : TransitException(msg, src, code) {}
    std::string getCategory() const override { return "TRIP"; }
};

class PaymentException : public TransitException {
public:
    PaymentException(const std::string& msg, const std::string& src, int code)
        : TransitException(msg, src, code) {}
    std::string getCategory() const override { return "PAYMENT"; }
};

class StaffException : public TransitException {
public:
    StaffException(const std::string& msg, const std::string& src, int code)
        : TransitException(msg, src, code) {}
    std::string getCategory() const override { return "STAFF"; }
};

class TicketException : public TransitException {
public:
    TicketException(const std::string& msg, const std::string& src, int code)
        : TransitException(msg, src, code) {}
    std::string getCategory() const override { return "TICKET"; }
};

class FileException : public TransitException {
public:
    FileException(const std::string& msg, const std::string& src, int code)
        : TransitException(msg, src, code) {}
    std::string getCategory() const override { return "FILE"; }
};

class ValidationException : public TransitException {
public:
    ValidationException(const std::string& msg, const std::string& src, int code)
        : TransitException(msg, src, code) {}
    std::string getCategory() const override { return "VALIDATION"; }
};

class ContainerException : public TransitException {
public:
    ContainerException(const std::string& msg, const std::string& src, int code)
        : TransitException(msg, src, code) {}
    std::string getCategory() const override { return "CONTAINER"; }
};

// ===========================================================
// LEVEL 3: Specific Exceptions (20 specific classes)
// ===========================================================

// --- Vehicle Specifics ---
class VehicleNotFoundException : public VehicleException {
public:
    VehicleNotFoundException(const std::string& src = "Unknown")
        : VehicleException("Vehicle not found in the system.", src, 1001) {}
};

class VehicleOverCapacityException : public VehicleException {
public:
    VehicleOverCapacityException(const std::string& src = "Unknown")
        : VehicleException("Vehicle has exceeded passenger capacity.", src, 1002) {}
};

class VehicleBreakdownException : public VehicleException {
public:
    VehicleBreakdownException(const std::string& src = "Unknown")
        : VehicleException("Vehicle has broken down.", src, 1003) {}
};

class CargoCriticalException : public VehicleException {
public:
    CargoCriticalException(const std::string& src = "Unknown")
        : VehicleException("Cargo temperature is out of safe range!", src, 1004) {}
};

// --- Route Specifics ---
class RouteNotFoundException : public RouteException {
public:
    RouteNotFoundException(const std::string& src = "Unknown")
        : RouteException("Route not found.", src, 2001) {}
};

class StopNotFoundException : public RouteException {
public:
    StopNotFoundException(const std::string& src = "Unknown")
        : RouteException("Stop not found on this route.", src, 2002) {}
};

class RouteConflictException : public RouteException {
public:
    RouteConflictException(const std::string& src = "Unknown")
        : RouteException("Route conflict detected.", src, 2003) {}
};

// --- Trip Specifics ---
class TripAlreadyActiveException : public TripException {
public:
    TripAlreadyActiveException(const std::string& src = "Unknown")
        : TripException("A trip is already active for this vehicle.", src, 3001) {}
};

class TripCancelledException : public TripException {
public:
    TripCancelledException(const std::string& src = "Unknown")
        : TripException("This trip has been cancelled.", src, 3002) {}
};

class NoDriverAssignedException : public TripException {
public:
    NoDriverAssignedException(const std::string& src = "Unknown")
        : TripException("No driver has been assigned to this trip.", src, 3003) {}
};

// --- Payment Specifics ---
class PaymentDeclinedException : public PaymentException {
public:
    PaymentDeclinedException(const std::string& src = "Unknown")
        : PaymentException("Payment was declined.", src, 4001) {}
};

class InsufficientFundsException : public PaymentException {
public:
    InsufficientFundsException(const std::string& src = "Unknown")
        : PaymentException("Insufficient funds for this transaction.", src, 4002) {}
};

class InvalidCardException : public PaymentException {
public:
    InvalidCardException(const std::string& src = "Unknown")
        : PaymentException("Invalid card details provided.", src, 4003) {}
};

// --- Staff Specifics ---
class DriverNotFoundException : public StaffException {
public:
    DriverNotFoundException(const std::string& src = "Unknown")
        : StaffException("Driver not found.", src, 5001) {}
};

class LicenseExpiredException : public StaffException {
public:
    LicenseExpiredException(const std::string& src = "Unknown")
        : StaffException("Driver license has expired.", src, 5002) {}
};

class UnauthorisedAccessException : public StaffException {
public:
    UnauthorisedAccessException(const std::string& src = "Unknown")
        : StaffException("Unauthorised access attempt detected.", src, 5003) {}
};

// --- Ticket Specifics ---
class InvalidTicketException : public TicketException {
public:
    InvalidTicketException(const std::string& src = "Unknown")
        : TicketException("This ticket is not valid.", src, 6001) {}
};

class TicketAlreadyUsedException : public TicketException {
public:
    TicketAlreadyUsedException(const std::string& src = "Unknown")
        : TicketException("This ticket has already been used.", src, 6002) {}
};

// --- File Specifics ---
class FileNotFoundException : public FileException {
public:
    FileNotFoundException(const std::string& src = "Unknown")
        : FileException("Data file not found.", src, 7001) {}
};

class FileCorruptionException : public FileException {
public:
    FileCorruptionException(const std::string& src = "Unknown")
        : FileException("Data file is corrupted.", src, 7002) {}
};

// --- Validation Specifics ---
class InvalidDateException : public ValidationException {
public:
    InvalidDateException(const std::string& src = "Unknown")
        : ValidationException("Invalid date provided.", src, 8001) {}
};

class InvalidTimeException : public ValidationException {
public:
    InvalidTimeException(const std::string& src = "Unknown")
        : ValidationException("Invalid time provided.", src, 8002) {}
};

class InvalidCoordinateException : public ValidationException {
public:
    InvalidCoordinateException(const std::string& src = "Unknown")
        : ValidationException("GPS coordinate is out of valid range.", src, 8003) {}
};

class EmptyStringException : public ValidationException {
public:
    EmptyStringException(const std::string& src = "Unknown")
        : ValidationException("String cannot be empty.", src, 8004) {}
};

// --- Container Specifics ---
class IndexOutOfBoundsException : public ContainerException {
public:
    IndexOutOfBoundsException(const std::string& src = "Unknown")
        : ContainerException("Index is out of bounds.", src, 9001) {}
};

class NullNodeException : public ContainerException {
public:
    NullNodeException(const std::string& src = "Unknown")
        : ContainerException("Null node encountered.", src, 9002) {}
};

class GraphVertexNotFoundException : public ContainerException {
public:
    GraphVertexNotFoundException(const std::string& src = "Unknown")
        : ContainerException("Graph vertex not found.", src, 9003) {}
};
