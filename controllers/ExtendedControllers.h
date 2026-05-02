#pragma once
// ============================================================
// FILE: ExtendedControllers.h
// MODULE: 5 - Controller Layer (8 Remaining Controllers)
// OUTPUT-ID: M5-EXT-001
// Expected Output: Route management, trip scheduling, dispatch,
//                  maintenance tracking, incident logging, reports
// OOP CONCEPTS: Polymorphism, Singleton, Graph+Dijkstra usage,
//               MinHeap dispatch queue, file I/O
// ============================================================

#include "Controllers.h"
#include "../entities/Level5Vehicles.h"
#include "../entities/FreightTruck.h"
#include "../entities/ExtendedDomainEntities.h"
#include "../entities/Interfaces.h"
#include "../datastructs/Graph.h"
#include "../datastructs/MinHeap.h"
#include <fstream>
#include <sstream>

// ============================================================
// ROUTECONTROLLER: Manages routes and the city graph
// ============================================================
class RouteController {
private:
    DynamicArray<Route>  m_routes;
    DynamicArray<Stop>   m_stops;

    // [OOP: Graph stores the entire city network as vertices+edges]
    // Vertices = Stops, Edges = road/track connections with km weights
    Graph<Stop> m_cityGraph;

public:
    RouteController() {}

    // Add a stop to the system
    int addStop(const CustomString& name, double lat, double lon, bool shelter = false) {
        Stop s(name, lat, lon, shelter);
        m_stops.append(s);
        int graphIdx = m_cityGraph.addVertex(s);
        std::cout << "[RouteController] Added stop: " << name
                  << " (Graph index " << graphIdx << ")\n";
        return graphIdx;
    }

    // Add a route
    Route& addRoute(const CustomString& name, int frequencyMins) {
        Route r(name, frequencyMins);
        m_routes.append(r);
        std::cout << "[RouteController] Added route: " << name << "\n";
        return m_routes[m_routes.size() - 1];
    }

    // Connect two stops with a directed edge (automatically calculates distance)
    void connectStops(int fromStopGraphIdx, int toStopGraphIdx) {
        const Stop& from = m_cityGraph.getVertex(fromStopGraphIdx);
        const Stop& to   = m_cityGraph.getVertex(toStopGraphIdx);
        // [OOP: GeoCoordinate::operator- gives distance via Haversine]
        double distKm = from.getPosition() - to.getPosition();
        m_cityGraph.addEdge(fromStopGraphIdx, toStopGraphIdx, distKm);
        std::cout << "[RouteController] Connected stops "
                  << from.getStopName() << " → " << to.getStopName()
                  << " (" << distKm << " km)\n";
    }

    // Find shortest path between two stops using Dijkstra
    void findShortestPath(int fromIdx, int toIdx) {
        DynamicArray<int> path = m_cityGraph.shortestPath(fromIdx, toIdx);
        if (path.size() == 0) {
            std::cout << "[RouteController] No path found!\n";
            return;
        }
        std::cout << "[RouteController] Shortest path: ";
        DynamicArray<double> allDists = m_cityGraph.dijkstra(fromIdx);
        for (int i = 0; i < path.size(); i++) {
            std::cout << m_cityGraph.getVertex(path[i]).getStopName().c_str();
            if (i < path.size() - 1) std::cout << " → ";
        }
        if (toIdx < allDists.size() && allDists[toIdx] >= 0) {
            std::cout << " (" << allDists[toIdx] << " km)\n";
        } else {
            std::cout << "\n";
        }
    }

    void printGraph() const { m_cityGraph.printGraph(); }

    int getTotalRoutes() const { return m_routes.size(); }
    int getTotalStops()  const { return m_stops.size();  }
};

// ============================================================
// TRIPCONTROLLER: Schedule and manage vehicle trips
// ============================================================
class TripController {
private:
    DynamicArray<Trip> m_trips;

public:
    TripController() {}

    // Schedule a new trip
    Trip& scheduleTrip(int vehicleId, int driverId, int routeId,
                       const CustomTime& scheduledTime) {
        if (driverId == -1) {
            throw NoDriverAssignedException("TripController::scheduleTrip");
        }
        // Check if vehicle already has an active trip
        for (int i = 0; i < m_trips.size(); i++) {
            if (m_trips[i].getVehicleId() == vehicleId &&
                m_trips[i].getStatus() == TripStatus::IN_PROGRESS) {
                throw TripAlreadyActiveException("TripController::scheduleTrip");
            }
        }
        Trip t(vehicleId, driverId, routeId, scheduledTime);
        m_trips.append(t);
        std::cout << "[TripController] Trip " << t.getTripId()
                  << " scheduled for vehicle " << vehicleId << "\n";
        return m_trips[m_trips.size() - 1];
    }

    // Start a trip
    void startTrip(int tripId) {
        for (int i = 0; i < m_trips.size(); i++) {
            if (m_trips[i].getTripId() == tripId) {
                m_trips[i].startTrip();
                std::cout << "[TripController] Trip " << tripId << " started.\n";
                return;
            }
        }
        throw TripCancelledException("TripController::startTrip");
    }

    // End a trip
    void endTrip(int tripId) {
        for (int i = 0; i < m_trips.size(); i++) {
            if (m_trips[i].getTripId() == tripId) {
                m_trips[i].endTrip();
                std::cout << "[TripController] Trip " << tripId << " completed. Passengers: "
                          << m_trips[i].getPassengersBoardedTotal() << "\n";
                return;
            }
        }
    }

    void printAllTrips() const {
        std::cout << "\n=== TRIPS (" << m_trips.size() << " total) ===\n";
        for (int i = 0; i < m_trips.size(); i++) {
            std::cout << m_trips[i] << "\n";
        }
    }

    int getTotalTrips() const { return m_trips.size(); }
};

// ============================================================
// DISPATCHCONTROLLER: Priority-based vehicle dispatch
// ============================================================
class DispatchController {
private:
    // [OOP: MinHeap used as priority queue for dispatch]
    // Vehicles with highest priority (lowest priority number) dispatched first
    MinHeap<DispatchEntry> m_dispatchQueue;
    int m_emergencyCount;

public:
    DispatchController() : m_emergencyCount(0) {}

    // Add vehicle to dispatch queue
    void queueVehicle(int vehicleId, int priority, double distanceKm) {
        DispatchEntry entry(vehicleId, priority, distanceKm);
        m_dispatchQueue.insert(entry);
        std::cout << "[Dispatch] Queued vehicle " << vehicleId
                  << " Priority:" << priority << " Dist:" << distanceKm << "km\n";
    }

    // Dispatch the highest-priority vehicle
    int dispatchNext() {
        if (m_dispatchQueue.isEmpty()) {
            throw VehicleNotFoundException("DispatchController::dispatchNext");
        }
        DispatchEntry entry = m_dispatchQueue.extractMin();
        std::cout << "[Dispatch] Dispatching vehicle " << entry.vehicleId
                  << " (Priority:" << entry.priority << ")\n";
        return entry.vehicleId;
    }

    // Emergency: dispatch the nearest available vehicle
    void triggerEmergency(const CustomString& description) {
        m_emergencyCount++;
        std::cout << "[EMERGENCY #" << m_emergencyCount << "] " << description << "\n";
        if (!m_dispatchQueue.isEmpty()) {
            int vehicleId = dispatchNext();
            std::cout << "[EMERGENCY] Vehicle " << vehicleId << " dispatched!\n";
        }
    }

    void printQueue() const {
        std::cout << "\n=== DISPATCH QUEUE ===\n";
        std::cout << m_dispatchQueue << "\n";
        std::cout << "Queue size: " << m_dispatchQueue.size() << "\n";
    }

    int getQueueSize() const { return m_dispatchQueue.size(); }
};

// ============================================================
// PAYMENTCONTROLLER: Process all payments
// ============================================================
class PaymentController {
private:
    DynamicArray<IPaymentProcessor*> m_processors;
    double m_dailyRevenue;
    int    m_transactionCount;

public:
    PaymentController() : m_dailyRevenue(0.0), m_transactionCount(0) {
        m_processors.append(new CashProcessor());
        m_processors.append(new TransitCardProcessor(CustomString("CARD-SYS"), 100000.0));
        m_processors.append(new QRCodeProcessor(CustomString("SMART-QR")));
        m_processors.append(new CorporateAccountProcessor(CustomString("CORP-ABC"), 500000.0));
        m_processors.append(new LoyaltyPointsProcessor(10000));
    }

    ~PaymentController() {
        for (int i = 0; i < m_processors.size(); i++) delete m_processors[i];
    }

    // [OOP: polymorphic payment - works for any IPaymentProcessor]
    bool processPayment(double amount, int processorIndex) {
        if (processorIndex < 0 || processorIndex >= m_processors.size()) {
            throw PaymentDeclinedException("PaymentController::processPayment");
        }
        IPaymentProcessor* proc = m_processors[processorIndex];
        if (!proc->validateDetails()) {
            throw InvalidCardException("PaymentController::processPayment");
        }
        bool success = proc->processPayment(amount);
        if (!success) {
            throw InsufficientFundsException("PaymentController::processPayment");
        }
        m_dailyRevenue += amount;
        m_transactionCount++;
        CustomString txId = proc->generateTransactionId();
        std::cout << "[Payment] Rs" << amount << " via "
                  << proc->getPaymentMethod() << " TxID:" << txId << "\n";
        return true;
    }

    double getDailyRevenue()    const { return m_dailyRevenue;     }
    int    getTransactionCount()const { return m_transactionCount; }
    void   resetDailyRevenue()        { m_dailyRevenue = 0.0;      }
};

// ============================================================
// MAINTENANCECONTROLLER: Schedule and track maintenance
// ============================================================
class MaintenanceController {
private:
    DynamicArray<MaintenanceJob> m_jobs;

public:
    MaintenanceController() {}

    MaintenanceJob& scheduleJob(int vehicleId, int techId,
                                const CustomString& desc, const CustomDate& date) {
        MaintenanceJob job(vehicleId, techId, desc, date);
        m_jobs.append(job);
        std::cout << "[Maintenance] Job " << job.getJobId()
                  << " scheduled for vehicle " << vehicleId << "\n";
        return m_jobs[m_jobs.size() - 1];
    }

    void completeJob(int jobId, double cost) {
        for (int i = 0; i < m_jobs.size(); i++) {
            if (m_jobs[i].getJobId() == jobId) {
                m_jobs[i].completeJob(cost);
                std::cout << "[Maintenance] Job " << jobId
                          << " completed. Cost: Rs" << cost << "\n";
                return;
            }
        }
    }

    double getTotalMaintenanceCost() const {
        double total = 0.0;
        for (int i = 0; i < m_jobs.size(); i++) {
            if (m_jobs[i].getStatus() == JobStatus::COMPLETED)
                total += m_jobs[i].getCost();
        }
        return total;
    }

    void printAllJobs() const {
        std::cout << "\n=== MAINTENANCE JOBS (" << m_jobs.size() << ") ===\n";
        for (int i = 0; i < m_jobs.size(); i++) std::cout << m_jobs[i] << "\n";
    }

    int getTotalJobs() const { return m_jobs.size(); }
};

// ============================================================
// INCIDENTCONTROLLER: Log and manage incidents
// ============================================================
class IncidentController {
private:
    DynamicArray<Incident>    m_incidents;
    DynamicArray<INotifiable*> m_notifiers;

public:
    IncidentController() {
        // Set up notification channels
        m_notifiers.append(new SMSNotifier(CustomString("+92-300-TRANSIT")));
        m_notifiers.append(new EmailNotifier(CustomString("ops@abccity.gov")));
        m_notifiers.append(new DisplayBoardNotifier(1));
    }

    ~IncidentController() {
        for (int i = 0; i < m_notifiers.size(); i++) delete m_notifiers[i];
    }

    Incident& logIncident(int vehicleId, int driverId, const CustomString& desc,
                          const GeoCoordinate& loc, IncidentSeverity severity) {
        Incident inc(vehicleId, driverId, desc, loc, severity);
        m_incidents.append(inc);
        std::cout << "[Incident] Logged: " << desc << "\n";

        // [OOP: Polymorphic notification - sends via ALL channels]
        int priority = (severity == IncidentSeverity::CRITICAL) ? 1 :
                       (severity == IncidentSeverity::HIGH)     ? 2 : 3;
        CustomString msg = CustomString("INCIDENT: ") + desc;
        for (int i = 0; i < m_notifiers.size(); i++) {
            m_notifiers[i]->sendNotification(msg, priority);
        }
        return m_incidents[m_incidents.size() - 1];
    }

    void resolveIncident(int incidentId) {
        for (int i = 0; i < m_incidents.size(); i++) {
            if (m_incidents[i].getIncidentId() == incidentId) {
                m_incidents[i].resolve();
                std::cout << "[Incident] Incident " << incidentId << " resolved.\n";
                return;
            }
        }
    }

    void printAllIncidents() const {
        std::cout << "\n=== INCIDENTS (" << m_incidents.size() << ") ===\n";
        for (int i = 0; i < m_incidents.size(); i++) std::cout << m_incidents[i] << "\n";
    }

    int getTotalIncidents()   const { return m_incidents.size(); }
    int getResolvedCount()    const {
        int count = 0;
        for (int i = 0; i < m_incidents.size(); i++)
            if (m_incidents[i].isResolved()) count++;
        return count;
    }
};

// ============================================================
// REPORTCONTROLLER: Generates all reports polymorphically
// ============================================================
class ReportController {
private:
    // [OOP: LinkedList of interface pointers - polymorphic report generation]
    LinkedList<IReportGenerator*> m_generators;

public:
    ReportController() {}

    ~ReportController() {
        for (int i = 0; i < m_generators.size(); i++) delete m_generators[i];
    }

    void addGenerator(IReportGenerator* gen) { m_generators.append(gen); }

    // [OOP: Polymorphic call - each generator produces different report]
    void generateAllReports() {
        std::cout << "\n=== GENERATING ALL REPORTS ===\n";
        for (int i = 0; i < m_generators.size(); i++) {
            std::cout << "\n" << m_generators[i]->generateReport().c_str() << "\n";
        }
    }

    void exportAllCSV(const CustomString& folder) {
        for (int i = 0; i < m_generators.size(); i++) {
            CustomString path = folder + m_generators[i]->getTitle()
                              + CustomString(".csv");
            m_generators[i]->exportCSV(path);
        }
    }

    int getReportCount() const { return m_generators.size(); }
};

// ============================================================
// FILECONTROLLER: Binary I/O for all entities
// ============================================================
class FileController {
private:
    CustomString m_dataFolder;

    // Simple checksum: sum of all bytes modulo 65536
    int computeChecksum(const std::string& content) const {
        int checksum = 0;
        for (char c : content) checksum = (checksum + c) % 65536;
        return checksum;
    }

public:
    explicit FileController(const CustomString& folder = CustomString("data/"))
        : m_dataFolder(folder) {}

    // Save any text content to file with checksum
    void saveWithChecksum(const CustomString& filename, const std::string& content) {
        CustomString fullPath = m_dataFolder + filename;
        std::ofstream out(fullPath.c_str());
        if (!out.is_open()) throw FileNotFoundException("FileController::saveWithChecksum");

        int checksum = computeChecksum(content);
        out << checksum << "\n";  // Write checksum first
        out << content;
        out.close();
        std::cout << "[FileController] Saved: " << fullPath << " (checksum:" << checksum << ")\n";
    }

    // Load file and verify checksum
    std::string loadWithChecksum(const CustomString& filename) {
        CustomString fullPath = m_dataFolder + filename;
        std::ifstream in(fullPath.c_str());
        if (!in.is_open()) throw FileNotFoundException("FileController::loadWithChecksum");

        int storedChecksum;
        in >> storedChecksum;
        in.ignore();  // Skip newline after checksum

        std::string content((std::istreambuf_iterator<char>(in)),
                             std::istreambuf_iterator<char>());

        int actualChecksum = computeChecksum(content);
        if (actualChecksum != storedChecksum) {
            throw FileCorruptionException("FileController::loadWithChecksum");
        }

        std::cout << "[FileController] Loaded: " << fullPath << " (checksum OK)\n";
        return content;
    }

    // Write activity log entry
    void logActivity(const CustomString& user, const CustomString& action) {
        CustomString logPath = m_dataFolder + CustomString("transit_log.txt");
        std::ofstream log(logPath.c_str(), std::ios::app);  // Append mode
        if (!log.is_open()) return;
        log << CustomDate::today().toString().c_str() << " | "
            << CustomTime::now().toString24h().c_str() << " | "
            << user.c_str() << " | " << action.c_str() << "\n";
    }

    bool fileExists(const CustomString& filename) const {
        CustomString fullPath = m_dataFolder + filename;
        std::ifstream f(fullPath.c_str());
        return f.good();
    }
};
