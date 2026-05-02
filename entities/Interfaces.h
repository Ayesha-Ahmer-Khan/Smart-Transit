#pragma once
// ============================================================
// FILE: Interfaces.h
// MODULE: 4 - Polymorphism Layer (Notification + Reports)
// OUTPUT-ID: M4-INTF-001
// Expected Output: Notifications sent, reports generated
// OOP CONCEPTS:
//   - Pure abstract interfaces
//   - Multiple inheritance (AdminUser implements BOTH)
//   - Runtime polymorphism for notification dispatch
// ============================================================

#include "../utilities/CustomString.h"
#include "../utilities/CustomDate.h"
#include "../datastructs/DynamicArray.h"
#include <iostream>
#include <fstream>
#include <cstdio>

// ============================================================
// ABSTRACT INTERFACE: INotifiable
// [OOP: Pure interface - only pure virtual methods]
// ============================================================
class INotifiable {
public:
    virtual ~INotifiable() = default;

    virtual void         sendNotification(const CustomString& msg, int priority) = 0;
    virtual DynamicArray<CustomString> getDeliveryLog() const = 0;
    virtual void         clearLog()                          = 0;
    virtual CustomString getChannelName()              const = 0;
};

// ============================================================
// CONCRETE: SMSNotifier
// Sends notifications via SMS
// ============================================================
class SMSNotifier : public INotifiable {
private:
    CustomString             m_phoneNumber;
    DynamicArray<CustomString> m_log;

public:
    explicit SMSNotifier(const CustomString& phone) : m_phoneNumber(phone) {}

    void sendNotification(const CustomString& msg, int priority) override {
        char buf[300];
        std::snprintf(buf, sizeof(buf), "[SMS→%s] P%d: %s",
                 m_phoneNumber.c_str(), priority, msg.c_str());
        CustomString entry(buf);
        m_log.append(entry);
        std::cout << entry << "\n";
    }

    DynamicArray<CustomString> getDeliveryLog() const override { return m_log; }
    void clearLog() override { m_log.clear(); }
    CustomString getChannelName() const override { return CustomString("SMS"); }
};

// ============================================================
// CONCRETE: EmailNotifier
// ============================================================
class EmailNotifier : public INotifiable {
private:
    CustomString             m_emailAddress;
    DynamicArray<CustomString> m_log;

public:
    explicit EmailNotifier(const CustomString& email) : m_emailAddress(email) {}

    void sendNotification(const CustomString& msg, int priority) override {
        char buf[300];
        std::snprintf(buf, sizeof(buf), "[EMAIL→%s] P%d: %s",
                 m_emailAddress.c_str(), priority, msg.c_str());
        CustomString entry(buf);
        m_log.append(entry);
        std::cout << entry << "\n";
    }

    DynamicArray<CustomString> getDeliveryLog() const override { return m_log; }
    void clearLog() override { m_log.clear(); }
    CustomString getChannelName() const override { return CustomString("Email"); }
};

// ============================================================
// CONCRETE: PushNotifier (in-app notification)
// ============================================================
class PushNotifier : public INotifiable {
private:
    CustomString             m_deviceToken;
    DynamicArray<CustomString> m_log;

public:
    explicit PushNotifier(const CustomString& token) : m_deviceToken(token) {}

    void sendNotification(const CustomString& msg, int priority) override {
        char buf[300];
        std::snprintf(buf, sizeof(buf), "[PUSH→device] P%d: %s",
                 priority, msg.c_str());
        CustomString entry(buf);
        m_log.append(entry);
        std::cout << entry << "\n";
    }

    DynamicArray<CustomString> getDeliveryLog() const override { return m_log; }
    void clearLog() override { m_log.clear(); }
    CustomString getChannelName() const override { return CustomString("Push"); }
};

// ============================================================
// CONCRETE: DisplayBoardNotifier (station display screens)
// ============================================================
class DisplayBoardNotifier : public INotifiable {
private:
    int                      m_boardId;
    CustomString             m_currentMessage;
    DynamicArray<CustomString> m_log;

public:
    explicit DisplayBoardNotifier(int boardId) : m_boardId(boardId) {}

    void sendNotification(const CustomString& msg, int priority) override {
        m_currentMessage = msg;
        char buf[300];
        std::snprintf(buf, sizeof(buf), "[BOARD#%d] P%d: %s",
                 m_boardId, priority, msg.c_str());
        CustomString entry(buf);
        m_log.append(entry);
        std::cout << entry << "\n";
    }

    DynamicArray<CustomString> getDeliveryLog() const override { return m_log; }
    void clearLog() override { m_log.clear(); m_currentMessage = CustomString(""); }
    CustomString getChannelName()    const override { return CustomString("DisplayBoard"); }
    CustomString getCurrentMessage() const { return m_currentMessage; }
};

// ============================================================
// ABSTRACT INTERFACE: IReportGenerator
// ============================================================
class IReportGenerator {
public:
    virtual ~IReportGenerator() = default;

    virtual CustomString generateReport()                            = 0;
    virtual void         exportCSV(const CustomString& path)        = 0;
    virtual CustomString getTitle()                            const = 0;
    virtual void         getDateRange(CustomDate& from,
                                      CustomDate& to)          const = 0;
};

// ============================================================
// CONCRETE: FleetUtilisationReport
// ============================================================
class FleetUtilisationReport : public IReportGenerator {
private:
    int    m_totalVehicles;
    int    m_activeVehicles;
    double m_avgOccupancy;

public:
    FleetUtilisationReport(int total, int active, double avgOcc)
        : m_totalVehicles(total), m_activeVehicles(active), m_avgOccupancy(avgOcc) {}

    CustomString generateReport() override {
        char buf[400];
        std::snprintf(buf, sizeof(buf),
                "=== FLEET UTILISATION REPORT ===\n"
                "Generated: %s\n"
                "Total Vehicles : %d\n"
                "Active Vehicles: %d\n"
                "Utilisation    : %.1f%%\n"
                "Avg Occupancy  : %.1f%%\n"
                "================================",
                CustomDate::today().toString().c_str(),
                m_totalVehicles, m_activeVehicles,
                (m_totalVehicles > 0) ? (m_activeVehicles * 100.0 / m_totalVehicles) : 0.0,
                m_avgOccupancy);
        return CustomString(buf);
    }

    void exportCSV(const CustomString& path) override {
        std::ofstream f(path.c_str());
        if (!f.is_open()) return;
        f << "Metric,Value\n";
        f << "Total Vehicles," << m_totalVehicles << "\n";
        f << "Active Vehicles," << m_activeVehicles << "\n";
        f << "Avg Occupancy," << m_avgOccupancy << "\n";
        std::cout << "[Report] Fleet CSV exported to: " << path << "\n";
    }

    CustomString getTitle() const override {
        return CustomString("Fleet Utilisation Report");
    }

    void getDateRange(CustomDate& from, CustomDate& to) const override {
        from = CustomDate::today();
        to   = CustomDate::today();
    }
};

// ============================================================
// CONCRETE: RevenueReport
// ============================================================
class RevenueReport : public IReportGenerator {
private:
    double m_totalRevenue;
    int    m_ticketCount;
    double m_averageFare;

public:
    RevenueReport(double revenue, int tickets)
        : m_totalRevenue(revenue), m_ticketCount(tickets),
          m_averageFare(tickets > 0 ? revenue / tickets : 0.0) {}

    CustomString generateReport() override {
        char buf[400];
        std::snprintf(buf, sizeof(buf),
                "=== REVENUE REPORT ===\n"
                "Generated: %s\n"
                "Total Revenue : Rs%.2f\n"
                "Tickets Issued: %d\n"
                "Average Fare  : Rs%.2f\n"
                "======================",
                CustomDate::today().toString().c_str(),
                m_totalRevenue, m_ticketCount, m_averageFare);
        return CustomString(buf);
    }

    void exportCSV(const CustomString& path) override {
        std::ofstream f(path.c_str());
        if (!f.is_open()) return;
        f << "Metric,Value\n";
        f << "Total Revenue," << m_totalRevenue << "\n";
        f << "Ticket Count,"  << m_ticketCount  << "\n";
        f << "Average Fare,"  << m_averageFare  << "\n";
        std::cout << "[Report] Revenue CSV exported to: " << path << "\n";
    }

    CustomString getTitle() const override {
        return CustomString("Revenue Report");
    }

    void getDateRange(CustomDate& from, CustomDate& to) const override {
        from = CustomDate::today();
        to   = CustomDate::today();
    }
};

// ============================================================
// CONCRETE: IncidentReport
// ============================================================
class IncidentReport : public IReportGenerator {
private:
    int m_totalIncidents;
    int m_resolvedIncidents;
    int m_criticalIncidents;

public:
    IncidentReport(int total, int resolved, int critical)
        : m_totalIncidents(total), m_resolvedIncidents(resolved),
          m_criticalIncidents(critical) {}

    CustomString generateReport() override {
        char buf[400];
        std::snprintf(buf, sizeof(buf),
                "=== INCIDENT REPORT ===\n"
                "Generated: %s\n"
                "Total Incidents   : %d\n"
                "Resolved          : %d\n"
                "Pending           : %d\n"
                "Critical          : %d\n"
                "=======================",
                CustomDate::today().toString().c_str(),
                m_totalIncidents, m_resolvedIncidents,
                m_totalIncidents - m_resolvedIncidents,
                m_criticalIncidents);
        return CustomString(buf);
    }

    void exportCSV(const CustomString& path) override {
        std::ofstream f(path.c_str());
        if (!f.is_open()) return;
        f << "Metric,Value\n";
        f << "Total Incidents,"   << m_totalIncidents   << "\n";
        f << "Resolved,"          << m_resolvedIncidents<< "\n";
        f << "Critical,"          << m_criticalIncidents<< "\n";
        std::cout << "[Report] Incident CSV exported to: " << path << "\n";
    }

    CustomString getTitle() const override {
        return CustomString("Incident Report");
    }

    void getDateRange(CustomDate& from, CustomDate& to) const override {
        from = CustomDate::today();
        to   = CustomDate::today();
    }
};

// ============================================================
// CONCRETE: EmissionsReport
// Required report generator for environmental analysis.
// ============================================================
class EmissionsReport : public IReportGenerator {
private:
    double m_totalKgCO2;
    double m_averageKgCO2;

public:
    EmissionsReport(double totalKgCO2 = 0.0, double averageKgCO2 = 0.0)
        : m_totalKgCO2(totalKgCO2), m_averageKgCO2(averageKgCO2) {}

    CustomString generateReport() override {
        char buf[360];
        std::snprintf(buf, sizeof(buf),
            "=== EMISSIONS REPORT ===\nGenerated: %s\nTotal CO2: %.2f kg\nAverage CO2/vehicle: %.2f kg\n========================",
            CustomDate::today().toString().c_str(), m_totalKgCO2, m_averageKgCO2);
        return CustomString(buf);
    }

    void exportCSV(const CustomString& path) override {
        std::ofstream f(path.c_str());
        if (!f.is_open()) return;
        f << "Metric,Value\nTotal CO2 kg," << m_totalKgCO2 << "\nAverage CO2 kg," << m_averageKgCO2 << "\n";
    }

    CustomString getTitle() const override { return CustomString("Emissions Report"); }
    void getDateRange(CustomDate& from, CustomDate& to) const override { from = CustomDate::today(); to = CustomDate::today(); }
};

// ============================================================
// CONCRETE: DriverPerformanceReport
// Required report generator for staff performance.
// ============================================================
class DriverPerformanceReport : public IReportGenerator {
private:
    int m_totalDrivers;
    int m_incidents;
    double m_averageHours;

public:
    DriverPerformanceReport(int totalDrivers = 0, int incidents = 0, double averageHours = 0.0)
        : m_totalDrivers(totalDrivers), m_incidents(incidents), m_averageHours(averageHours) {}

    CustomString generateReport() override {
        char buf[360];
        std::snprintf(buf, sizeof(buf),
            "=== DRIVER PERFORMANCE REPORT ===\nGenerated: %s\nDrivers: %d\nIncidents: %d\nAverage Weekly Hours: %.1f\n=================================",
            CustomDate::today().toString().c_str(), m_totalDrivers, m_incidents, m_averageHours);
        return CustomString(buf);
    }

    void exportCSV(const CustomString& path) override {
        std::ofstream f(path.c_str());
        if (!f.is_open()) return;
        f << "Metric,Value\nDrivers," << m_totalDrivers << "\nIncidents," << m_incidents << "\nAverage Hours," << m_averageHours << "\n";
    }

    CustomString getTitle() const override { return CustomString("Driver Performance Report"); }
    void getDateRange(CustomDate& from, CustomDate& to) const override { from = CustomDate::today(); to = CustomDate::today(); }
};

// ============================================================
// CONCRETE: RouteEfficiencyReport
// Required report generator for route performance.
// ============================================================
class RouteEfficiencyReport : public IReportGenerator {
private:
    int m_totalRoutes;
    double m_averageDelayMinutes;
    double m_averageOccupancy;

public:
    RouteEfficiencyReport(int totalRoutes = 0, double averageDelayMinutes = 0.0, double averageOccupancy = 0.0)
        : m_totalRoutes(totalRoutes), m_averageDelayMinutes(averageDelayMinutes), m_averageOccupancy(averageOccupancy) {}

    CustomString generateReport() override {
        char buf[380];
        std::snprintf(buf, sizeof(buf),
            "=== ROUTE EFFICIENCY REPORT ===\nGenerated: %s\nRoutes: %d\nAverage Delay: %.1f min\nAverage Occupancy: %.1f%%\n===============================",
            CustomDate::today().toString().c_str(), m_totalRoutes, m_averageDelayMinutes, m_averageOccupancy);
        return CustomString(buf);
    }

    void exportCSV(const CustomString& path) override {
        std::ofstream f(path.c_str());
        if (!f.is_open()) return;
        f << "Metric,Value\nRoutes," << m_totalRoutes << "\nAverage Delay," << m_averageDelayMinutes << "\nAverage Occupancy," << m_averageOccupancy << "\n";
    }

    CustomString getTitle() const override { return CustomString("Route Efficiency Report"); }
    void getDateRange(CustomDate& from, CustomDate& to) const override { from = CustomDate::today(); to = CustomDate::today(); }
};
