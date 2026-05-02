#pragma once
// ============================================================
// FILE: src/gui/MainDashboard.h
// MODULE: 7 - Qt GUI Layer
// OUTPUT-ID: M7-DASH-001
// COLLEAGUE'S FILE: Implement this
// Expected Output: Main window with left nav panel,
//                  shows different pages based on selection
// OOP CONCEPTS: Qt signals/slots, role-based navigation
// ============================================================

#include <QMainWindow>
#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QDateTime>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QTextBrowser>
#include <QSpinBox>
#include <QFrame>
#include <QDialog>

#include "LiveMapWindow.h"
#include "FleetManagementWindow.h"
#include "VehicleProfileDialog.h"
#include "RouteBuilderWindow.h"
#include "TripSchedulerWindow.h"
#include "DispatchConsoleWindow.h"
#include "TicketingWindow.h"
#include "StaffManagementWindow.h"
#include "MaintenanceWindow.h"
#include "ReportDashboardWindow.h"

// Include backend
#include "../controllers/Controllers.h"
#include "../controllers/AuthController.h"

// [OOP: Inherits from QMainWindow - has menu bar, status bar, central widget]
class MainDashboard : public QMainWindow {
    Q_OBJECT

private:
    QString           m_currentUser;
    QString           m_currentRole;
    QListWidget*      m_navPanel;         // Left navigation
    QStackedWidget*   m_contentArea;      // Right content (swaps pages)
    QLabel*           m_statusUserLabel;
    QLabel*           m_statusTimeLabel;
    QTimer*           m_clockTimer;       // Updates clock every second

    // Page widgets
    QWidget*          m_homePage;
    QWidget*          m_fleetPage;
    QWidget*          m_staffPage;
    QWidget*          m_ticketPage;
    QWidget*          m_reportPage;

    // Fleet table
    QTableWidget*     m_fleetTable;
    QTableWidget*     m_staffTable;
    QTableWidget*     m_ticketTable;

    AppController&    m_app;  // Reference to singleton

public:
    explicit MainDashboard(const QString& username, const QString& role = "Admin", QWidget* parent = nullptr)
        : QMainWindow(parent),
          m_currentUser(username),
          m_currentRole(role),
          m_app(AppController::getInstance()) {

        setWindowTitle("SmartTransit EMS - Main Dashboard");
        setMinimumSize(1000, 650);

        m_app.initialise();  // Initialize backend

        setupUI();
        setupStatusBar();
        setupTimer();

        // Load initial data
        refreshFleetTable();
        refreshStaffTable();
    }

    ~MainDashboard() {
        // Qt handles child widget deletion automatically
    }

private:
    void setupUI() {
        QWidget* central = new QWidget(this);
        setCentralWidget(central);

        QHBoxLayout* mainLayout = new QHBoxLayout(central);
        mainLayout->setSpacing(0);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        // === LEFT NAV PANEL ===
        m_navPanel = new QListWidget();
        m_navPanel->setFixedWidth(200);
        m_navPanel->setStyleSheet(
            "QListWidget { background: #16213e; color: white; border: none; font-size: 14px; }"
            "QListWidget::item { padding: 12px 20px; }"
            "QListWidget::item:selected { background: #e94560; color: white; }");

        m_navPanel->addItem("🏠  Home");
        m_navPanel->addItem("🚌  Fleet Summary");
        m_navPanel->addItem("👤  Staff Summary");
        m_navPanel->addItem("🎫  Ticketing Summary");
        m_navPanel->addItem("📊  Reports Summary");
        m_navPanel->addItem("🗺️  Live Map Window");
        m_navPanel->addItem("🚦  Dispatch Console");
        m_navPanel->addItem("🛣️  Route Builder");
        m_navPanel->addItem("📅  Trip Scheduler");
        m_navPanel->addItem("🧰  Maintenance Window");
        m_navPanel->addItem("🚌  Full Fleet Window");
        m_navPanel->addItem("👤  Full Staff Window");
        m_navPanel->addItem("🎫  Full Ticketing Window");
        m_navPanel->addItem("📄  Full Report Dashboard");
        m_navPanel->addItem("🔎  Vehicle Profile Dialog");
        m_navPanel->addItem("🔒  Logout");
        m_navPanel->setCurrentRow(0);

        // === RIGHT CONTENT AREA ===
        m_contentArea = new QStackedWidget();
        m_contentArea->setStyleSheet("background: #f0f2f5;");

        m_contentArea->addWidget(buildHomePage());
        m_contentArea->addWidget(buildFleetPage());
        m_contentArea->addWidget(buildStaffPage());
        m_contentArea->addWidget(buildTicketPage());
        m_contentArea->addWidget(buildReportPage());

        mainLayout->addWidget(m_navPanel);
        mainLayout->addWidget(m_contentArea);

        // [Qt: Connect nav selection to page switch]
        connect(m_navPanel, &QListWidget::currentRowChanged,
                this,        &MainDashboard::onNavChanged);
    }

    // Build home/summary page
    QWidget* buildHomePage() {
        m_homePage = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(m_homePage);
        layout->setContentsMargins(30, 30, 30, 30);
        layout->setSpacing(20);

        QLabel* title = new QLabel("SmartTransit EMS");
        title->setStyleSheet("font-size: 28px; font-weight: bold; color: #1a1a2e;");

        QLabel* subtitle = new QLabel("ABC City Transit Authority");
        subtitle->setStyleSheet("font-size: 14px; color: #666;");

        QLabel* welcome = new QLabel("Welcome, " + m_currentUser + "!");
        welcome->setStyleSheet("font-size: 16px; color: #e94560;");

        // Stats cards row
        QHBoxLayout* statsRow = new QHBoxLayout();
        statsRow->addWidget(makeStatCard("Total Vehicles", "4",  "#e94560"));
        statsRow->addWidget(makeStatCard("Active Staff",   "2",  "#0f3460"));
        statsRow->addWidget(makeStatCard("Tickets Issued", "2",  "#533483"));
        statsRow->addWidget(makeStatCard("Revenue (Rs)",   "100","#1b4332"));

        layout->addWidget(title);
        layout->addWidget(subtitle);
        layout->addWidget(welcome);
        layout->addLayout(statsRow);
        layout->addStretch();
        return m_homePage;
    }

    // Helper: create a colored stat card
    QFrame* makeStatCard(const QString& label, const QString& value, const QString& color) {
        QFrame* card = new QFrame();
        card->setStyleSheet(QString("background: %1; border-radius: 8px; padding: 10px;").arg(color));
        card->setMinimumSize(150, 100);

        QVBoxLayout* cl = new QVBoxLayout(card);
        QLabel* val = new QLabel(value);
        val->setStyleSheet("color: white; font-size: 32px; font-weight: bold;");
        val->setAlignment(Qt::AlignCenter);

        QLabel* lbl = new QLabel(label);
        lbl->setStyleSheet("color: rgba(255,255,255,0.8); font-size: 12px;");
        lbl->setAlignment(Qt::AlignCenter);

        cl->addWidget(val);
        cl->addWidget(lbl);
        return card;
    }

    QString statusText(AssetStatus status) const {
        switch (status) {
            case AssetStatus::ACTIVE: return "ACTIVE";
            case AssetStatus::UNDER_MAINTENANCE: return "UNDER MAINTENANCE";
            case AssetStatus::DECOMMISSIONED: return "DECOMMISSIONED";
        }
        return "UNKNOWN";
    }

    // Build fleet management page
    QWidget* buildFleetPage() {
        m_fleetPage = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(m_fleetPage);
        layout->setContentsMargins(20, 20, 20, 20);

        QLabel* title = new QLabel("Fleet Management");
        title->setStyleSheet("font-size: 20px; font-weight: bold;");

        // Toolbar
        QHBoxLayout* toolbar = new QHBoxLayout();
        QPushButton* addBusBtn = new QPushButton("+ Add City Bus");
        QPushButton* addMetroBtn = new QPushButton("+ Add Metro Train");
        QPushButton* refreshBtn = new QPushButton("Refresh");
        addBusBtn->setStyleSheet("background: #e94560; color: white; padding: 6px 14px; border-radius: 4px;");
        addMetroBtn->setStyleSheet("background: #0f3460; color: white; padding: 6px 14px; border-radius: 4px;");
        refreshBtn->setStyleSheet("background: #444; color: white; padding: 6px 14px; border-radius: 4px;");

        toolbar->addWidget(addBusBtn);
        toolbar->addWidget(addMetroBtn);
        toolbar->addWidget(refreshBtn);
        toolbar->addStretch();

        // Fleet table
        m_fleetTable = new QTableWidget();
        m_fleetTable->setColumnCount(6);
        m_fleetTable->setHorizontalHeaderLabels({"ID", "Reg No", "Type", "Status", "Passengers", "Fuel Type"});
        m_fleetTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_fleetTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_fleetTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_fleetTable->setAlternatingRowColors(true);
        // [Qt: clicking column header sorts the table]
        m_fleetTable->horizontalHeader()->setSectionsClickable(true);
        connect(m_fleetTable->horizontalHeader(), &QHeaderView::sectionClicked,
                this, [this](int col) { m_fleetTable->sortItems(col); });

        layout->addWidget(title);
        layout->addLayout(toolbar);
        layout->addWidget(m_fleetTable);

        // Connect buttons
        connect(addBusBtn,   &QPushButton::clicked, this, &MainDashboard::onAddBus);
        connect(addMetroBtn, &QPushButton::clicked, this, &MainDashboard::onAddMetro);
        connect(refreshBtn,  &QPushButton::clicked, this, &MainDashboard::refreshFleetTable);

        return m_fleetPage;
    }

    // Build staff page
    QWidget* buildStaffPage() {
        m_staffPage = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(m_staffPage);
        layout->setContentsMargins(20, 20, 20, 20);

        QLabel* title = new QLabel("Staff Management");
        title->setStyleSheet("font-size: 20px; font-weight: bold;");

        QHBoxLayout* toolbar = new QHBoxLayout();
        QPushButton* addDriverBtn = new QPushButton("+ Add Driver");
        addDriverBtn->setStyleSheet("background: #0f3460; color: white; padding: 6px 14px; border-radius: 4px;");
        toolbar->addWidget(addDriverBtn);
        toolbar->addStretch();

        m_staffTable = new QTableWidget();
        m_staffTable->setColumnCount(5);
        m_staffTable->setHorizontalHeaderLabels({"ID", "Name", "Role", "Employee ID", "On Duty"});
        m_staffTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_staffTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_staffTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_staffTable->setAlternatingRowColors(true);
        m_staffTable->horizontalHeader()->setSectionsClickable(true);
        connect(m_staffTable->horizontalHeader(), &QHeaderView::sectionClicked,
                this, [this](int col) { m_staffTable->sortItems(col); });

        layout->addWidget(title);
        layout->addLayout(toolbar);
        layout->addWidget(m_staffTable);

        connect(addDriverBtn, &QPushButton::clicked, this, &MainDashboard::onAddDriver);

        return m_staffPage;
    }

    // Build ticketing page
    QWidget* buildTicketPage() {
        m_ticketPage = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(m_ticketPage);
        layout->setContentsMargins(20, 20, 20, 20);
        layout->setSpacing(10);

        QLabel* title = new QLabel("Ticketing");
        title->setStyleSheet("font-size: 20px; font-weight: bold;");

        // Issue ticket form
        QFrame* form = new QFrame();
        form->setStyleSheet("background: white; border-radius: 8px; padding: 15px;");
        QVBoxLayout* formLayout = new QVBoxLayout(form);

        QLabel* formTitle = new QLabel("Issue New Ticket");
        formTitle->setStyleSheet("font-weight: bold; font-size: 14px;");

        QComboBox* fareType = new QComboBox();
        fareType->addItems({"Bus (Flat Fare)", "Metro (Zone Based)", "Ride Hail (Surge)"});

        QDoubleSpinBox* distanceSpin = new QDoubleSpinBox();
        distanceSpin->setRange(0.1, 100.0);
        distanceSpin->setValue(10.0);
        distanceSpin->setSuffix(" km");

        QCheckBox* peakCheck = new QCheckBox("Peak Hour?");
        QSpinBox* loyaltySpin = new QSpinBox();
        loyaltySpin->setRange(0, 1000);
        loyaltySpin->setSuffix(" points");

        QPushButton* issueBtn = new QPushButton("Issue Ticket");
        issueBtn->setStyleSheet("background: #e94560; color: white; padding: 8px; border-radius: 4px; font-weight: bold;");

        QLabel* receiptLabel = new QLabel("Receipt will appear here...");
        receiptLabel->setStyleSheet("background: #f0f0f0; padding: 10px; border-radius: 4px; font-family: monospace;");
        receiptLabel->setWordWrap(true);

        formLayout->addWidget(formTitle);
        formLayout->addWidget(new QLabel("Fare Type:"));
        formLayout->addWidget(fareType);
        formLayout->addWidget(new QLabel("Distance:"));
        formLayout->addWidget(distanceSpin);
        formLayout->addWidget(peakCheck);
        formLayout->addWidget(new QLabel("Loyalty Points:"));
        formLayout->addWidget(loyaltySpin);
        formLayout->addWidget(issueBtn);
        formLayout->addWidget(receiptLabel);

        // Connect issue button
        connect(issueBtn, &QPushButton::clicked, this, [=]() {
            try {
                int fareIndex = fareType->currentIndex();
                double dist   = distanceSpin->value();
                bool isPeak   = peakCheck->isChecked();
                int loyalty   = loyaltySpin->value();

                Ticket t = m_app.getTicketing().issueTicket(1, 999, dist, fareIndex, isPeak, loyalty);
                receiptLabel->setText(
                    QString("✓ Ticket #%1 issued!\nFare: Rs%2\nType: %3")
                    .arg(t.getTicketId())
                    .arg(t.getFareAmount())
                    .arg(t.getTypeString().c_str()));

            } catch (const TransitException& e) {
                QMessageBox::critical(this, "Error", e.getMessage().c_str());
            }
        });

        layout->addWidget(title);
        layout->addWidget(form);
        layout->addStretch();
        return m_ticketPage;
    }

    // Build report page
    QWidget* buildReportPage() {
        m_reportPage = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(m_reportPage);
        layout->setContentsMargins(20, 20, 20, 20);

        QLabel* title = new QLabel("Reports Dashboard");
        title->setStyleSheet("font-size: 20px; font-weight: bold;");

        QComboBox* reportType = new QComboBox();
        reportType->addItems({"System Summary", "Fleet Utilisation", "Revenue Report"});

        QPushButton* generateBtn = new QPushButton("Generate Report");
        generateBtn->setStyleSheet("background: #0f3460; color: white; padding: 8px 20px; border-radius: 4px;");

        QTextBrowser* reportView = new QTextBrowser();
        reportView->setStyleSheet("background: white; font-family: monospace;");

        connect(generateBtn, &QPushButton::clicked, this, [=]() {
            QString report;
            report += "========================================\n";
            report += "  SmartTransit EMS - System Report\n";
            report += "  Generated: " + QDateTime::currentDateTime().toString() + "\n";
            report += "========================================\n\n";
            report += QString("Total Entities Created : %1\n").arg(Entity::getTotalCreated());
            report += QString("Total Staff Members    : %1\n").arg(StaffMember::getTotalStaff());
            report += QString("Fleet Size             : %1\n").arg(m_app.getFleet().getTotalVehicles());
            report += QString("Active Vehicles        : %1\n").arg(m_app.getFleet().getActiveCount());
            report += QString("Tickets Issued         : %1\n").arg(m_app.getTicketing().getTotalTickets());
            report += QString("Total Revenue          : Rs%1\n").arg(m_app.getTicketing().getTotalRevenue());
            reportView->setText(report);
        });

        layout->addWidget(title);
        layout->addWidget(new QLabel("Report Type:"));
        layout->addWidget(reportType);
        layout->addWidget(generateBtn);
        layout->addWidget(reportView);
        return m_reportPage;
    }

    void setupStatusBar() {
        m_statusUserLabel = new QLabel("User: " + m_currentUser + " | Role: " + m_currentRole);
        m_statusTimeLabel = new QLabel(QDateTime::currentDateTime().toString("hh:mm:ss"));
        statusBar()->addWidget(m_statusUserLabel);
        statusBar()->addPermanentWidget(m_statusTimeLabel);
        statusBar()->setStyleSheet("background: #1a1a2e; color: white;");
    }

    void setupTimer() {
        m_clockTimer = new QTimer(this);
        // [Qt: QTimer - calls updateClock() every 1000ms (1 second)]
        connect(m_clockTimer, &QTimer::timeout, this, &MainDashboard::updateClock);
        m_clockTimer->start(1000);
    }

    void openStandaloneWindow(int index) {
        QWidget* win = nullptr;
        switch (index) {
            case 5:  win = new LiveMapWindow(); break;
            case 6:  win = new DispatchConsoleWindow(); break;
            case 7:  win = new RouteBuilderWindow(); break;
            case 8:  win = new TripSchedulerWindow(); break;
            case 9:  win = new MaintenanceWindow(); break;
            case 10: win = new FleetManagementWindow(); break;
            case 11: win = new StaffManagementWindow(); break;
            case 12: win = new TicketingWindow(); break;
            case 13: win = new ReportDashboardWindow(); break;
            case 14: {
                CityBus* demoVehicle = new CityBus(CustomString("DEMO-101"), CustomString("B-101"), 1, true, 4200000.0);
                VehicleProfileDialog* dialog = new VehicleProfileDialog(demoVehicle, this);
                connect(dialog, &QDialog::finished, this, [dialog, demoVehicle]() {
                    delete demoVehicle;
                    dialog->deleteLater();
                });
                dialog->show();
                return;
            }
            default: return;
        }
        win->setAttribute(Qt::WA_DeleteOnClose);
        win->show();
    }


private slots:
    // Called when nav item is clicked
    void onNavChanged(int index) {
        const int logoutIndex = m_navPanel->count() - 1;
        if (index == logoutIndex) {
            AuthController::getInstance().logout();
            emit userLoggedOut();
            this->hide();
            return;
        }

        if (index >= 0 && index < 5) {
            m_contentArea->setCurrentIndex(index);
            return;
        }

        openStandaloneWindow(index);
        // Return selection to Home so the same external window can be opened again.
        m_navPanel->setCurrentRow(0);
    }

    void updateClock() {
        m_statusTimeLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss"));
    }

    // Add a city bus via dialog-style input
    void onAddBus() {
        // In full version: open AddVehicleDialog
        // For demo: add directly
        try {
            static int busCount = 100;
            busCount++;
            char regBuf[20], busBuf[20];
            std::snprintf(regBuf, sizeof(regBuf), "KHI-%d", busCount);
            std::snprintf(busBuf, sizeof(busBuf), "B-%d", busCount);
            m_app.getFleet().addVehicle(
                new CityBus(CustomString(regBuf), CustomString(busBuf), 1, true, 4000000.0)
            );
            refreshFleetTable();
            QMessageBox::information(this, "Success",
                QString("Bus %1 added to fleet!").arg(regBuf));
        } catch (const TransitException& e) {
            QMessageBox::critical(this, "Error", e.getMessage().c_str());
        }
    }

    void onAddMetro() {
        try {
            static int metroCount = 10;
            metroCount++;
            char regBuf[20];
            std::snprintf(regBuf, sizeof(regBuf), "MTR-%d", metroCount);
            m_app.getFleet().addVehicle(
                new MetroTrain(CustomString(regBuf), 2, 4, 80.0, false, 60000000.0)
            );
            refreshFleetTable();
            QMessageBox::information(this, "Success", "Metro train added!");
        } catch (const TransitException& e) {
            QMessageBox::critical(this, "Error", e.getMessage().c_str());
        }
    }

    void onAddDriver() {
        AddStaffDialog dlg(this);
        if (dlg.exec() == QDialog::Accepted) {
            QString fn = dlg.getFirstName().trimmed();
            QString ln = dlg.getLastName().trimmed();
            QString emp = dlg.getEmpId().trimmed();
            if (fn.isEmpty()) {
                QMessageBox::warning(this, "Validation",
                                     "Please enter the driver's first name.");
                return;
            }
            if (ln.isEmpty()) {
                QMessageBox::warning(this, "Validation",
                                     "Please enter the driver's last name.");
                return;
            }
            if (emp.isEmpty()) {
                QMessageBox::warning(this, "Validation",
                                     "Please enter an employee ID, for example DRV-001.");
                return;
            }
            try {
                m_app.getStaff().addStaff(new Driver(
                    CustomString(fn.toStdString().c_str()),
                    CustomString(ln.toStdString().c_str()),
                    CustomString(dlg.getCnic().toStdString().c_str()),
                    CustomDate(1, 1, 1990),
                    CustomString(emp.toStdString().c_str()),
                    CustomString("HTV"),
                    dlg.getSalary()));
                refreshStaffTable();
                QMessageBox::information(this, "Success",
                    QString("Driver %1 %2 added.").arg(fn, ln));
            } catch (const TransitException& e) {
                QMessageBox::critical(this, "Error", e.getMessage().c_str());
            }
        }
    }

    void refreshFleetTable() {
        int count = m_app.getFleet().getTotalVehicles();
        m_fleetTable->setSortingEnabled(false);
        m_fleetTable->setRowCount(count);
        for (int i = 0; i < count; ++i) {
            Vehicle* vehicle = m_app.getFleet().getVehicleAt(i);
            m_fleetTable->setItem(i, 0, new QTableWidgetItem(QString::number(vehicle->getId())));
            m_fleetTable->setItem(i, 1, new QTableWidgetItem(vehicle->getRegistrationNo().c_str()));
            m_fleetTable->setItem(i, 2, new QTableWidgetItem(vehicle->getVehicleCategory().c_str()));
            m_fleetTable->setItem(i, 3, new QTableWidgetItem(statusText(vehicle->getStatus())));
            m_fleetTable->setItem(i, 4, new QTableWidgetItem(
                QString("%1/%2").arg(vehicle->getCurrentPassengers()).arg(vehicle->getCapacity())));
            m_fleetTable->setItem(i, 5, new QTableWidgetItem(vehicle->getFuelTypeString().c_str()));
        }
        m_fleetTable->setSortingEnabled(true);
        statusBar()->showMessage(
            QString("Fleet: %1 vehicles loaded").arg(count), 3000);
    }

    void refreshStaffTable() {
        int count = m_app.getStaff().getTotalStaff();
        m_staffTable->setSortingEnabled(false);
        m_staffTable->setRowCount(count);
        for (int i = 0; i < count; ++i) {
            StaffMember* staff = m_app.getStaff().getStaffAt(i);
            m_staffTable->setItem(i, 0, new QTableWidgetItem(QString::number(staff->getId())));
            m_staffTable->setItem(i, 1, new QTableWidgetItem(staff->getFullName().c_str()));
            m_staffTable->setItem(i, 2, new QTableWidgetItem(staff->getRoleString().c_str()));
            m_staffTable->setItem(i, 3, new QTableWidgetItem(staff->getEmployeeId().c_str()));
            m_staffTable->setItem(i, 4, new QTableWidgetItem(staff->isOnDuty() ? "YES" : "NO"));
        }
        m_staffTable->setSortingEnabled(true);
        statusBar()->showMessage(
            QString("Staff: %1 members loaded").arg(count), 3000);
    }

signals:
    void userLoggedOut();  // [Qt: Custom signal - emitted when user logs out]
};
