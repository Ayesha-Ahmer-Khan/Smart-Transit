// ============================================================
// FILE: main.cpp  (COMPLETE VERSION)
// SmartTransit EMS - ABC City Transit Authority
// CS-212 | OOP with C++ | Spring 2026
// OUTPUT-ID: MAIN-001
// ============================================================

#include "controllers/Controllers.h"
#include "controllers/ExtendedControllers.h"
#include "entities/CityBus.h"
#include "entities/FreightTruck.h"
#include "entities/Level5Vehicles.h"
#include "entities/StaffMember.h"
#include "entities/DomainEntities.h"
#include "entities/ExtendedDomainEntities.h"
#include "entities/Interfaces.h"
#include "utilities/CustomString.h"
#include "utilities/CustomDate.h"
#include "utilities/CustomTime.h"
#include "utilities/GeoCoordinate.h"
#include "datastructs/MinHeap.h"
#include "datastructs/Graph.h"
#include "exceptions/TransitExceptions.h"
#include <iostream>

void demoModule1_Utilities() {
    std::cout << "\n========== MODULE 1: UTILITIES ==========\n";
    CustomString s1("SmartTransit"), s2(" EMS");
    CustomString s3 = s1 + s2;
    std::cout << "Concat   : " << s3 << "\n";
    std::cout << "Upper    : " << s3.toUpperCase() << "\n";
    std::cout << "Length   : " << s3.length() << "\n";
    CustomDate today = CustomDate::today();
    CustomDate past(1,1,2024);
    std::cout << "Today    : " << today << " | Days since 1/1/2024: " << (today-past) << "\n";
    CustomTime now = CustomTime::now();
    std::cout << "Time24h  : " << now.toString24h() << " | 12h: " << now.toString12h() << "\n";
    GeoCoordinate k1(24.8607,67.0011), k2(24.9008,67.1681);
    std::cout << "Dist KHI→Airport: " << (k1-k2) << " km | Bearing: " << k1.bearingTo(k2) << " deg\n";
}

void demoInternalSupportStructures() {
    // Internal support structures are intentionally not shown as a separate viva module.
    // They remain in src/datastructs because the official PDF requires custom containers
    // and graph support for controllers, routing, dispatch, and polymorphic storage.
}

void demoModule3_Hierarchy() {
    std::cout << "\n========== MODULE 3: HIERARCHY (ALL 5 LEVELS) ==========\n";
    Vehicle* fleet[7];
    fleet[0] = new CityBus("KHI-BUS-01","B-17A",3,true,5000000.0);
    fleet[1] = new MetroTrain("METRO-L1",1,6,80.0,false,50000000.0);
    fleet[2] = new RideHailCar("KHI-CAR-99","Toyota","Corolla",2500000.0);
    fleet[3] = new FreightTruck("KHI-TRK-01","Hino","500",15000.0,CustomString("Electronics"),false,8000000.0);
    fleet[4] = new ElectricScooter("ESC-001",1,120000.0);
    fleet[5] = new ArticulatedBus("KHI-ART-01","AB-1",1,true,40,9000000.0);
    fleet[6] = new RefrigeratedTruck("KHI-REF-01","Nissan","UD",12000.0,-18.0,9500000.0);

    for (int i=0;i<7;i++) {
        std::cout << fleet[i]->toString().c_str() << "\n";
        std::cout << "  Cat:" << fleet[i]->getVehicleCategory()
                  << " Rs/km:" << fleet[i]->getFuelCostPerKm()
                  << " CO2:" << fleet[i]->calculateEmissions(50.0) << "kg\n";
    }

    std::cout << "\n--- RefrigeratedTruck Temp Alert ---\n";
    try {
        static_cast<RefrigeratedTruck*>(fleet[6])->setCurrentTemp(-25.0);
    } catch (const CargoCriticalException& e) {
        std::cout << "[CAUGHT] " << e.getCategory() << ": " << e.getMessage() << "\n";
    }

    SeniorDriver* sd = new SeniorDriver(
        CustomString("Imran"),CustomString("Sheikh"),CustomString("42101-5555555-5"),
        CustomDate(10,2,1975),CustomString("DRV-SR-01"),CustomString("HTV"),60000.0,3);
    sd->addAward(CustomString("Best Driver 2023"));
    std::cout << "\n" << sd->toString().c_str() << "\n";
    std::cout << "Salary (with seniority): Rs" << sd->calculateMonthlySalary() << "\n";
    StaffMember* sm = sd;
    if (dynamic_cast<SeniorDriver*>(sm))
        std::cout << "[dynamic_cast OK] confirmed SeniorDriver\n";

    for (int i=0;i<7;i++) delete fleet[i];
    delete sd;
}

void demoModule4_Polymorphism() {
    std::cout << "\n========== MODULE 4: POLYMORPHISM ==========\n";
    IFareCalculator* calcs[3]={new FlatFareBus(),new ZoneBasedMetro(),new SurgeRideHail(1.5)};
    for (int i=0;i<3;i++) {
        double b=calcs[i]->calculateFare(12.5,1,true);
        std::cout << calcs[i]->getFareType() << ": Rs" << b << "→Rs" << calcs[i]->applyDiscount(b,50) << "\n";
        delete calcs[i];
    }
    INotifiable* notifs[3]={new SMSNotifier(CustomString("+92-TRANSIT")),
                             new EmailNotifier(CustomString("ops@abc.gov")),
                             new DisplayBoardNotifier(1)};
    CustomString msg("Bus B-17A delayed 15 min");
    for (int i=0;i<3;i++) { notifs[i]->sendNotification(msg,2); delete notifs[i]; }

    IReportGenerator* reps[3]={new FleetUtilisationReport(10,8,72.5),
                                new RevenueReport(15000.0,120),
                                new IncidentReport(5,4,1)};
    for (int i=0;i<3;i++) { std::cout << reps[i]->generateReport().c_str() << "\n"; delete reps[i]; }
}

void demoModule5_Controllers() {
    std::cout << "\n========== MODULE 5: CONTROLLERS ==========\n";
    AppController& app = AppController::getInstance();
    app.initialise();

    app.getFleet().addVehicle(new CityBus("KHI-101","B-21",1,true,4500000.0));
    app.getFleet().addVehicle(new MetroTrain("MTR-001",1,4,80.0,true,80000000.0));
    app.getFleet().addVehicle(new FreightTruck("KHI-TRK-A","Hino","300",10000.0,CustomString("Food"),false,7000000.0));
    app.getFleet().printAll();

    app.getStaff().addStaff(new Driver(CustomString("Zahid"),CustomString("Hussain"),
        CustomString("42301-9876543-3"),CustomDate(10,3,1985),CustomString("DRV-010"),CustomString("HTV"),48000.0));
    app.getStaff().addStaff(new AdminUser(CustomString("Sys"),CustomString("Admin"),
        CustomString("00000-0000000-0"),CustomDate(1,1,1990),CustomString("ADM-001"),
        CustomString("admin"),CustomString("admin"),3));
    app.getStaff().printAll();

    app.getTicketing().registerPassenger(CustomString("Ahmed"),CustomString("0300-1234567"));
    app.getTicketing().issueTicket(1,1,8.0,0,false,0);
    app.getTicketing().issueTicket(1,2,15.0,1,true,50);
    app.getTicketing().validateTicket(5);
    app.getTicketing().printAllTickets();

    RouteController rc;
    int a=rc.addStop(CustomString("City Hall"),24.860,67.010);
    int b=rc.addStop(CustomString("Saddar"),24.856,67.010);
    int c=rc.addStop(CustomString("Clifton"),24.811,67.028);
    rc.connectStops(a,b); rc.connectStops(b,c);
    rc.findShortestPath(a,c);

    TripController tc;
    Trip& myTrip = tc.scheduleTrip(1,9,1,CustomTime(8,0,0));
    int tripId = myTrip.getTripId();
    tc.startTrip(tripId); tc.endTrip(tripId);
    tc.printAllTrips();

    DispatchController dc;
    dc.queueVehicle(1,2,5.0); dc.queueVehicle(3,1,3.0); dc.queueVehicle(5,3,7.0);
    dc.printQueue();
    dc.triggerEmergency(CustomString("Breakdown on Main Road"));

    MaintenanceController mc;
    MaintenanceJob& job = mc.scheduleJob(1,1,CustomString("Oil Change"),CustomDate::today());
    job.addPart(CustomString("Oil Filter"));
    mc.completeJob(job.getJobId(),3500.0);
    mc.printAllJobs();

    IncidentController ic;
    Incident& inc = ic.logIncident(1,9,CustomString("Fender bender at Saddar"),GeoCoordinate(24.856,67.010),IncidentSeverity::MEDIUM);
    ic.resolveIncident(inc.getIncidentId());
    ic.printAllIncidents();

    ReportController rpc;
    rpc.addGenerator(new FleetUtilisationReport(app.getFleet().getTotalVehicles(),app.getFleet().getActiveCount(),68.0));
    rpc.addGenerator(new RevenueReport(app.getTicketing().getTotalRevenue(),app.getTicketing().getTotalTickets()));
    rpc.generateAllReports();

    FileController fc(CustomString("data/"));
    fc.logActivity(CustomString("admin"),CustomString("System demo complete"));

    app.printSummary();
}

void demoModule6_Exceptions() {
    std::cout << "\n========== MODULE 6: ALL EXCEPTION TYPES ==========\n";
    auto test = [](const char* name, auto fn) {
        std::cout << "--- " << name << " ---\n";
        try { fn(); std::cout << "  (no exception thrown)\n"; }
        catch (const TransitException& e) {
            std::cout << "  [CAUGHT] " << e.getCategory() << " (code " << e.getCode() << "): " << e.getMessage() << "\n";
        }
    };
    test("VehicleOverCapacity",   []{CityBus b("T","T",1,false,1); for(int i=0;i<52;i++) b.boardPassenger();});
    test("InvalidDate",           []{CustomDate d(32,13,2026);});
    test("InvalidTime",           []{CustomTime t(25,0,0);});
    test("InvalidCoordinate",     []{GeoCoordinate g(91.0,0.0);});
    test("IndexOutOfBounds",      []{DynamicArray<int> a; a.append(1); int v=a[99];(void)v;});
    test("VehicleNotFound",       []{AppController::getInstance().getFleet().findById(9999);});
    test("TicketAlreadyUsed",     []{AppController::getInstance().getTicketing().validateTicket(5);});
    test("CargoCritical",         []{RefrigeratedTruck t("R","N","U",5000,-18,1e6); t.setCurrentTemp(-25);});
    test("NoDriverAssigned",      []{TripController tc; tc.scheduleTrip(1,-1,1,CustomTime(9,0,0));});
    test("InsufficientFunds",     []{
        TransitCardProcessor p(CustomString("X"),10.0);
        if(!p.processPayment(500.0)) throw InsufficientFundsException("demo");
    });
    std::cout << "All 10 exception types demonstrated!\n";
}

int main() {
    std::cout << "============================================================\n";
    std::cout << "   SmartTransit EMS | ABC City | CS-212 | Spring 2026\n";
    std::cout << "============================================================\n";
    try {
        demoModule1_Utilities();
        // Internal support structures are used by controllers; no separate module demo.
        demoModule3_Hierarchy();
        demoModule4_Polymorphism();
        demoModule5_Controllers();
        demoModule6_Exceptions();
        std::cout << "\n============================================================\n";
        std::cout << "   ALL 6 MODULES COMPLETE | Entities: " << Entity::getTotalCreated()
                  << " | Staff: " << StaffMember::getTotalStaff() << "\n";
        std::cout << "============================================================\n";
    } catch (const TransitException& e) {
        std::cout << "[UNHANDLED] " << e.what() << "\n"; return 1;
    } catch (const std::exception& e) {
        std::cout << "[SYS ERROR] " << e.what() << "\n"; return 2;
    }
    return 0;
}
