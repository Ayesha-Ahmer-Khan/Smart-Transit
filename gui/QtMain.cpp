#include <QApplication>
#include <QMessageBox>
#include "LoginWindow.h"
#include "MainDashboard.h"
#include "../controllers/Controllers.h"
#include "../controllers/ExtendedControllers.h"
#include "../exceptions/TransitExceptions.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("SmartTransit EMS");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("ABC City Transit Authority");
    app.setStyle("Fusion");

    try {
        AppController& ctrl = AppController::getInstance();
        ctrl.initialise();
        ctrl.getFleet().addVehicle(new CityBus("KHI-101","B-21",1,true,4500000.0));
        ctrl.getFleet().addVehicle(new MetroTrain("MTR-001",1,4,80.0,true,80000000.0));
        ctrl.getFleet().addVehicle(new RideHailCar("KHI-CAR-01","Toyota","Corolla",2200000.0));
        ctrl.getStaff().addStaff(new Driver(
            CustomString("Zahid"),CustomString("Hussain"),
            CustomString("42301-9876543-3"),CustomDate(10,3,1985),
            CustomString("DRV-010"),CustomString("HTV"),48000.0));
        ctrl.getTicketing().registerPassenger(
            CustomString("Ahmed"), CustomString("0300-1234567"));
    } catch (const TransitException& e) {
        QMessageBox::critical(nullptr,"Startup Error",e.getMessage().c_str());
        return 1;
    }

    LoginWindow* loginWin = new LoginWindow();
    MainDashboard* dashboard = nullptr;

    QObject::connect(loginWin, &LoginWindow::loginSuccess,
                     [&](const QString& username, const QString& role) {
        loginWin->hide();
        dashboard = new MainDashboard(username, role);
        QObject::connect(dashboard, &MainDashboard::userLoggedOut, [&]() {
            dashboard->hide();
            loginWin->show();
        });
        dashboard->show();
    });

    loginWin->show();
    return app.exec();
}
