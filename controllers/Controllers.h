#pragma once
// ============================================================
// FILE: Controllers.h
// MODULE: 5 - Controller Layer
// OUTPUT-ID: M5-CTRL-001
// Expected Output: CRUD operations on fleet, tickets, staff
// OOP CONCEPTS:
//   - Singleton Pattern (AppController)
//   - Polymorphic containers: DynamicArray<Vehicle*>
//   - Runtime polymorphism: fare calculation, payment
//   - Exception throwing on invalid operations
// ============================================================

#include "../entities/Entity.h"
#include "../entities/CityBus.h"
#include "../entities/StaffMember.h"
#include "../entities/DomainEntities.h"
#include "../entities/IFareCalculator.h"
#include "../datastructs/DynamicArray.h"
#include "../exceptions/TransitExceptions.h"
#include <iostream>
#include <fstream>

// ============================================================
// FLEETCONTROLLER: Manages all vehicles in the system
// ============================================================
class FleetController {
private:
    // [OOP: Polymorphic array - holds pointers to ANY Vehicle subclass]
    // CityBus*, MetroTrain*, RideHailCar* all stored here as Vehicle*
    DynamicArray<Vehicle*> m_vehicles;

public:
    FleetController() {}

    // Destructor: must delete all Vehicle objects (we own the memory)
    ~FleetController() {
        for (int i = 0; i < m_vehicles.size(); i++) {
            delete m_vehicles[i];
        }
    }

    // Add any vehicle type (polymorphism: accepts any Vehicle subclass)
    void addVehicle(Vehicle* v) {
        if (v == nullptr) throw NullNodeException("FleetController::addVehicle");
        m_vehicles.append(v);
        std::cout << "[FleetController] Added: " << v->getRegistrationNo() << "\n";
    }

    // Find vehicle by registration number
    Vehicle* findByRegNo(const CustomString& regNo) {
        for (int i = 0; i < m_vehicles.size(); i++) {
            if (m_vehicles[i]->getRegistrationNo() == regNo) {
                return m_vehicles[i];
            }
        }
        throw VehicleNotFoundException("FleetController::findByRegNo");
    }

    // Find vehicle by ID
    Vehicle* findById(int id) {
        for (int i = 0; i < m_vehicles.size(); i++) {
            if (m_vehicles[i]->getId() == id) {
                return m_vehicles[i];
            }
        }
        throw VehicleNotFoundException("FleetController::findById");
    }

    // Remove vehicle from fleet (decommission)
    void removeVehicle(int id) {
        for (int i = 0; i < m_vehicles.size(); i++) {
            if (m_vehicles[i]->getId() == id) {
                m_vehicles[i]->decommission();
                std::cout << "[FleetController] Decommissioned vehicle ID:" << id << "\n";
                return;
            }
        }
        throw VehicleNotFoundException("FleetController::removeVehicle");
    }

    // Assign driver to vehicle
    void assignDriver(int vehicleId, int driverId) {
        Vehicle* v = findById(vehicleId);
        v->assignDriver(driverId);
        std::cout << "[FleetController] Driver " << driverId
                  << " assigned to vehicle " << vehicleId << "\n";
    }

    // Print all vehicles - [OOP: Calls toString() polymorphically]
    void printAll() const {
        std::cout << "\n=== FLEET (" << m_vehicles.size() << " vehicles) ===\n";
        for (int i = 0; i < m_vehicles.size(); i++) {
            // [OOP: POLYMORPHISM - calls the correct toString() based on ACTUAL type]
            std::cout << m_vehicles[i]->toString().c_str() << "\n";
        }
        std::cout << "================================\n";
    }

    // Get count of active vehicles
    int getActiveCount() const {
        int count = 0;
        for (int i = 0; i < m_vehicles.size(); i++) {
            if (m_vehicles[i]->getStatus() == AssetStatus::ACTIVE) count++;
        }
        return count;
    }

    int getTotalVehicles() const { return m_vehicles.size(); }

    Vehicle* getVehicleAt(int index) const {
        return m_vehicles.get(index);
    }

    // Save all vehicles to file
    void saveToFile(const CustomString& filename) const {
        std::ofstream out(filename.c_str());
        if (!out.is_open()) throw FileNotFoundException("FleetController::saveToFile");
        out << m_vehicles.size() << "\n";
        for (int i = 0; i < m_vehicles.size(); i++) {
            out << m_vehicles[i]->getEntityType().c_str() << "\n";
            m_vehicles[i]->serialize(out);
        }
        std::cout << "[FleetController] Saved " << m_vehicles.size() << " vehicles.\n";
    }
};

// ============================================================
// STAFFCONTROLLER: Manages all staff members
// ============================================================
class StaffController {
private:
    DynamicArray<StaffMember*> m_staff;

public:
    StaffController() {}

    ~StaffController() {
        for (int i = 0; i < m_staff.size(); i++) {
            delete m_staff[i];
        }
    }

    void addStaff(StaffMember* s) {
        if (s == nullptr) throw NullNodeException("StaffController::addStaff");
        m_staff.append(s);
        std::cout << "[StaffController] Added: " << s->getFullName() << "\n";
    }

    StaffMember* findById(int id) {
        for (int i = 0; i < m_staff.size(); i++) {
            if (m_staff[i]->getId() == id) return m_staff[i];
        }
        throw DriverNotFoundException("StaffController::findById");
    }

    // Find a driver and assign vehicle
    void assignVehicleToDriver(int driverId, int vehicleId) {
        for (int i = 0; i < m_staff.size(); i++) {
            // [OOP: dynamic_cast - safely check if StaffMember is actually a Driver]
            Driver* driver = dynamic_cast<Driver*>(m_staff[i]);
            if (driver != nullptr && driver->getId() == driverId) {
                if (!driver->isLicenseValid()) {
                    throw LicenseExpiredException("StaffController::assignVehicleToDriver");
                }
                driver->assignVehicle(vehicleId);
                driver->clockIn();
                std::cout << "[StaffController] Driver " << driverId
                          << " assigned to vehicle " << vehicleId << "\n";
                return;
            }
        }
        throw DriverNotFoundException("StaffController::assignVehicleToDriver");
    }

    void printAll() const {
        std::cout << "\n=== STAFF (" << m_staff.size() << " members) ===\n";
        for (int i = 0; i < m_staff.size(); i++) {
            std::cout << m_staff[i]->toString().c_str() << "\n";
        }
        std::cout << "=================================\n";
        std::cout << "Total staff (static): " << StaffMember::getTotalStaff() << "\n";
    }

    int getTotalStaff() const { return m_staff.size(); }

    StaffMember* getStaffAt(int index) const {
        return m_staff.get(index);
    }
};

// ============================================================
// TICKETINGCONTROLLER: Issues tickets using fare polymorphism
// ============================================================
class TicketingController {
private:
    // [OOP: Array of INTERFACE pointers - can hold any fare type]
    DynamicArray<IFareCalculator*> m_fareCalculators;
    DynamicArray<Ticket>           m_tickets;
    DynamicArray<Passenger>        m_passengers;

public:
    TicketingController() {
        // Pre-load available fare strategies
        m_fareCalculators.append(new FlatFareBus());
        m_fareCalculators.append(new ZoneBasedMetro());
        m_fareCalculators.append(new SurgeRideHail(1.0));
        m_fareCalculators.append(new WeightFreight());
        m_fareCalculators.append(new MicroMobility());
    }

    ~TicketingController() {
        for (int i = 0; i < m_fareCalculators.size(); i++) {
            delete m_fareCalculators[i];
        }
    }

    // Register a new passenger
    int registerPassenger(const CustomString& name, const CustomString& contact) {
        Passenger p(name, contact);
        m_passengers.append(p);
        std::cout << "[Ticketing] Registered passenger: " << name << " ID:"
                  << m_passengers[m_passengers.size()-1].getPassengerId() << "\n";
        return m_passengers[m_passengers.size()-1].getPassengerId();
    }

    // Issue a ticket using the correct fare calculator
    // fareIndex: 0=Bus, 1=Metro, 2=RideHail
    Ticket issueTicket(int passengerId, int tripId, double distanceKm,
                       int fareIndex, bool isPeakHour, int loyaltyPoints = 0) {
        if (fareIndex < 0 || fareIndex >= m_fareCalculators.size()) {
            throw InvalidTicketException("TicketingController::issueTicket");
        }

        // [OOP: RUNTIME POLYMORPHISM]
        // m_fareCalculators[fareIndex] is IFareCalculator*
        // but actually points to FlatFareBus or ZoneBasedMetro etc.
        // Correct calculateFare() is called at RUNTIME!
        IFareCalculator* calc = m_fareCalculators[fareIndex];
        double baseFare = calc->calculateFare(distanceKm, 1, isPeakHour);
        double finalFare = calc->applyDiscount(baseFare, loyaltyPoints);

        Ticket t(passengerId, tripId, finalFare, TicketType::SINGLE_JOURNEY);
        m_tickets.append(t);

        std::cout << "[Ticketing] Issued ticket. Fare:Rs" << finalFare
                  << " via " << calc->getFareType() << "\n";

        // Generate and print receipt
        char ticketIdStr[20];
        std::snprintf(ticketIdStr, sizeof(ticketIdStr), "TKT-%d", t.getTicketId());
        std::cout << calc->generateReceipt(finalFare, CustomString(ticketIdStr)).c_str() << "\n";

        return t;
    }

    // Validate a ticket (mark as used)
    void validateTicket(int ticketId) {
        for (int i = 0; i < m_tickets.size(); i++) {
            if (m_tickets[i].getTicketId() == ticketId) {
                if (m_tickets[i].isValidated()) {
                    throw TicketAlreadyUsedException("TicketingController::validateTicket");
                }
                m_tickets[i].validate();
                std::cout << "[Ticketing] Ticket " << ticketId << " validated.\n";
                return;
            }
        }
        throw InvalidTicketException("TicketingController::validateTicket");
    }

    // Get total revenue collected
    double getTotalRevenue() const {
        double total = 0.0;
        for (int i = 0; i < m_tickets.size(); i++) {
            total += m_tickets[i].getFareAmount();
        }
        return total;
    }

    void printAllTickets() const {
        std::cout << "\n=== TICKETS (" << m_tickets.size() << " total) ===\n";
        for (int i = 0; i < m_tickets.size(); i++) {
            std::cout << m_tickets[i] << "\n";
        }
        std::cout << "Total Revenue: Rs" << getTotalRevenue() << "\n";
        std::cout << "================================\n";
    }

    int getTotalTickets() const { return m_tickets.size(); }
};

// ============================================================
// APPCONTROLLER: Singleton - owns everything
// [OOP: Singleton Pattern - only ONE instance of this class exists]
// ============================================================
class AppController {
private:
    FleetController*     m_fleet;
    StaffController*     m_staff;
    TicketingController* m_ticketing;

    bool m_isInitialized;

    // [OOP: Private constructor = nobody can do AppController c; from outside]
    AppController()
        : m_fleet(nullptr), m_staff(nullptr),
          m_ticketing(nullptr), m_isInitialized(false) {}

    // Prevent copying the singleton
    AppController(const AppController&) = delete;
    AppController& operator=(const AppController&) = delete;

public:
    // [OOP: Static method returns the ONE shared instance]
    static AppController& getInstance() {
        static AppController instance;  // Created once, lives forever
        return instance;
    }

    // Initialize all subsystems
    void initialise() {
        if (m_isInitialized) return;  // Don't initialize twice

        m_fleet     = new FleetController();
        m_staff     = new StaffController();
        m_ticketing = new TicketingController();

        m_isInitialized = true;
        std::cout << "[AppController] SmartTransit EMS initialized.\n";
        std::cout << "[AppController] ABC City Transit Authority - Ready.\n";
    }

    // Access subsystems
    FleetController&     getFleet()     { return *m_fleet;     }
    StaffController&     getStaff()     { return *m_staff;     }
    TicketingController& getTicketing() { return *m_ticketing; }

    bool isInitialized() const { return m_isInitialized; }

    // Cleanup
    void shutdown() {
        delete m_fleet;
        delete m_staff;
        delete m_ticketing;
        m_fleet     = nullptr;
        m_staff     = nullptr;
        m_ticketing = nullptr;
        std::cout << "[AppController] System shut down cleanly.\n";
    }

    ~AppController() {
        if (m_isInitialized) shutdown();
    }

    // Print system summary
    void printSummary() const {
        std::cout << "\n========= SMARTTRANSIT EMS - SYSTEM SUMMARY =========\n";
        std::cout << "Total Entities Created : " << Entity::getTotalCreated() << "\n";
        std::cout << "Total Staff Members    : " << StaffMember::getTotalStaff() << "\n";
        if (m_isInitialized) {
            std::cout << "Fleet Size             : " << m_fleet->getTotalVehicles() << "\n";
            std::cout << "Active Vehicles        : " << m_fleet->getActiveCount()   << "\n";
            std::cout << "Tickets Issued         : " << m_ticketing->getTotalTickets() << "\n";
            std::cout << "Total Revenue          : Rs" << m_ticketing->getTotalRevenue() << "\n";
        }
        std::cout << "======================================================\n";
    }
};
