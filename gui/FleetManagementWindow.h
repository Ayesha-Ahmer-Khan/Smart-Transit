#pragma once
// ============================================================
// FILE: FleetManagementWindow.h
// MODULE: 7 - Qt GUI (Window 3 of 12)
// OUTPUT-ID: M7-FLEET-001
// COLLEAGUE'S FILE
// Expected Output: Table of all vehicles, add/decommission buttons
// ============================================================
#include <QWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include "../controllers/Controllers.h"
#include "../entities/CityBus.h"
#include "../entities/Level5Vehicles.h"
#include "../entities/FreightTruck.h"

class FleetManagementWindow : public QWidget {
    Q_OBJECT

private:
    QTableWidget* m_table;
    QPushButton*  m_addBusBtn;
    QPushButton*  m_addMetroBtn;
    QPushButton*  m_addTruckBtn;
    QPushButton*  m_decommissionBtn;
    QPushButton*  m_refreshBtn;
    QLabel*       m_countLabel;
    AppController& m_app;

public:
    explicit FleetManagementWindow(QWidget* parent = nullptr)
        : QWidget(parent), m_app(AppController::getInstance()) {

        setWindowTitle("Fleet Management");

        // Title
        QLabel* title = new QLabel("Fleet Management");
        title->setStyleSheet("font-size:22px; font-weight:bold; color:#1a1a2e;");

        // Toolbar
        m_addBusBtn      = new QPushButton("+ City Bus");
        m_addMetroBtn    = new QPushButton("+ Metro Train");
        m_addTruckBtn    = new QPushButton("+ Freight Truck");
        m_decommissionBtn= new QPushButton("Decommission");
        m_refreshBtn     = new QPushButton("Refresh");
        m_countLabel     = new QLabel("Vehicles: 0");

        for (auto* btn : {m_addBusBtn, m_addMetroBtn, m_addTruckBtn})
            btn->setStyleSheet("background:#0f3460; color:white; padding:6px 12px; border-radius:4px;");
        m_decommissionBtn->setStyleSheet("background:#e94560; color:white; padding:6px 12px; border-radius:4px;");
        m_refreshBtn->setStyleSheet("background:#444; color:white; padding:6px 12px; border-radius:4px;");

        QHBoxLayout* toolbar = new QHBoxLayout();
        toolbar->addWidget(m_addBusBtn);
        toolbar->addWidget(m_addMetroBtn);
        toolbar->addWidget(m_addTruckBtn);
        toolbar->addWidget(m_decommissionBtn);
        toolbar->addWidget(m_refreshBtn);
        toolbar->addStretch();
        toolbar->addWidget(m_countLabel);

        // Table
        m_table = new QTableWidget();
        m_table->setColumnCount(7);
        m_table->setHorizontalHeaderLabels(
            {"ID","Reg No","Type","Make/Model","Status","Passengers","Fuel"});
        m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        // [Qt: clicking column header sorts the table]
        m_table->setSortingEnabled(true);
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_table->setAlternatingRowColors(true);

        QVBoxLayout* layout = new QVBoxLayout(this);
        QPushButton* backBtn = new QPushButton("< Back to Dashboard");
        backBtn->setStyleSheet("background:#444;color:white;padding:6px 12px;border-radius:4px;font-weight:bold;");
        connect(backBtn, &QPushButton::clicked, this, &QWidget::close);

        QHBoxLayout* topBar = new QHBoxLayout();
        topBar->addWidget(title);
        topBar->addStretch();
        topBar->addWidget(backBtn);
        layout->addLayout(topBar);
        layout->addLayout(toolbar);
        layout->addWidget(m_table);

        connect(m_addBusBtn,       &QPushButton::clicked, this, &FleetManagementWindow::onAddBus);
        connect(m_addMetroBtn,     &QPushButton::clicked, this, &FleetManagementWindow::onAddMetro);
        connect(m_addTruckBtn,     &QPushButton::clicked, this, &FleetManagementWindow::onAddTruck);
        connect(m_decommissionBtn, &QPushButton::clicked, this, &FleetManagementWindow::onDecommission);
        connect(m_refreshBtn,      &QPushButton::clicked, this, &FleetManagementWindow::refreshTable);

        refreshTable();
    }

    // Add a row to the table with vehicle data
    void addTableRow(int id, const QString& reg, const QString& type,
                     const QString& makeModel, const QString& status,
                     const QString& passengers, const QString& fuel) {
        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
        m_table->setItem(row, 1, new QTableWidgetItem(reg));
        m_table->setItem(row, 2, new QTableWidgetItem(type));
        m_table->setItem(row, 3, new QTableWidgetItem(makeModel));
        m_table->setItem(row, 4, new QTableWidgetItem(status));
        m_table->setItem(row, 5, new QTableWidgetItem(passengers));
        m_table->setItem(row, 6, new QTableWidgetItem(fuel));

        // Color by status
        QColor color = (status == "ACTIVE") ? QColor(220,255,220)
                     : (status == "UNDER MAINTENANCE") ? QColor(255,255,200)
                     : QColor(255,220,220);
        for (int c = 0; c < 7; c++) m_table->item(row,c)->setBackground(color);
    }

private:
    QString statusText(AssetStatus status) const {
        switch (status) {
            case AssetStatus::ACTIVE: return "ACTIVE";
            case AssetStatus::UNDER_MAINTENANCE: return "UNDER MAINTENANCE";
            case AssetStatus::DECOMMISSIONED: return "DECOMMISSIONED";
        }
        return "UNKNOWN";
    }

    void addVehicleToTable(Vehicle* vehicle) {
        QString makeModel = QString("%1/%2")
            .arg(vehicle->getMake().c_str())
            .arg(vehicle->getModel().c_str());
        QString passengers = QString("%1/%2")
            .arg(vehicle->getCurrentPassengers())
            .arg(vehicle->getCapacity());
        addTableRow(vehicle->getId(),
                    vehicle->getRegistrationNo().c_str(),
                    vehicle->getVehicleCategory().c_str(),
                    makeModel,
                    statusText(vehicle->getStatus()),
                    passengers,
                    vehicle->getFuelTypeString().c_str());
    }

private slots:
    void refreshTable() {
        m_table->setRowCount(0);
        int count = m_app.getFleet().getTotalVehicles();
        m_countLabel->setText(QString("Vehicles: %1").arg(count));
        m_table->setSortingEnabled(false);
        for (int i = 0; i < count; ++i) {
            addVehicleToTable(m_app.getFleet().getVehicleAt(i));
        }
        m_table->setSortingEnabled(true);
    }

    void onAddBus() {
        // Simple input dialog
        QDialog dlg(this);
        dlg.setWindowTitle("Add City Bus");
        QFormLayout* form = new QFormLayout(&dlg);
        auto* regEdit  = new QLineEdit("KHI-NEW-01");
        auto* busEdit  = new QLineEdit("B-100");
        auto* acCheck  = new QCheckBox("Air Conditioned");
        auto* valEdit  = new QDoubleSpinBox();
        valEdit->setRange(100000, 100000000); valEdit->setValue(4500000);
        form->addRow("Registration:", regEdit);
        form->addRow("Bus Number:",   busEdit);
        form->addRow("AC:",           acCheck);
        form->addRow("Purchase Value:", valEdit);
        auto* btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        form->addRow(btns);
        connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

        if (dlg.exec() == QDialog::Accepted) {
            try {
                if (regEdit->text().trimmed().isEmpty()) {
                    QMessageBox::warning(this, "Validation",
                                         "Please enter a registration number, for example KHI-101.");
                    return;
                }
                if (busEdit->text().trimmed().isEmpty()) {
                    QMessageBox::warning(this, "Validation",
                                         "Please enter a bus number, for example B-100.");
                    return;
                }
                m_app.getFleet().addVehicle(new CityBus(
                    CustomString(regEdit->text().toStdString().c_str()),
                    CustomString(busEdit->text().toStdString().c_str()),
                    1, acCheck->isChecked(), valEdit->value()));
                refreshTable();
                QMessageBox::information(this,"Success","Bus added to fleet!");
            } catch (const TransitException& e) {
                QMessageBox::critical(this,"Error", e.getMessage().c_str());
            }
        }
    }

    void onAddMetro() {
        try {
            static int c = 10;
            char buf[20]; std::snprintf(buf,sizeof(buf),"MTR-00%d",++c);
            m_app.getFleet().addVehicle(new MetroTrain(
                CustomString(buf),2,4,80.0,false,70000000.0));
            refreshTable();
            QMessageBox::information(this,"Success",QString("Metro train %1 added!").arg(buf));
        } catch (const TransitException& e) {
            QMessageBox::critical(this,"Error",e.getMessage().c_str());
        }
    }

    void onAddTruck() {
        try {
            static int c = 1;
            char buf[20]; std::snprintf(buf,sizeof(buf),"TRK-00%d",++c);
            m_app.getFleet().addVehicle(new FreightTruck(
                CustomString(buf),CustomString("Hino"),CustomString("700"),
                15000.0,CustomString("General"),false,8000000.0));
            refreshTable();
            QMessageBox::information(this,"Success","Freight truck added!");
        } catch (const TransitException& e) {
            QMessageBox::critical(this,"Error",e.getMessage().c_str());
        }
    }

    void onDecommission() {
        int row = m_table->currentRow();
        if (row < 0) { QMessageBox::warning(this,"Select","Please select a vehicle."); return; }
        int id = m_table->item(row,0)->text().toInt();
        auto reply = QMessageBox::question(this,"Confirm",
            QString("Decommission vehicle ID %1?").arg(id));
        if (reply == QMessageBox::Yes) {
            try {
                m_app.getFleet().removeVehicle(id);
                refreshTable();
            } catch (const TransitException& e) {
                QMessageBox::critical(this,"Error",e.getMessage().c_str());
            }
        }
    }
};

// ============================================================
// RouteBuilderWindow - Build and view city routes
// Window 6 of 12
// OUTPUT-ID: M7-ROUTE-001
