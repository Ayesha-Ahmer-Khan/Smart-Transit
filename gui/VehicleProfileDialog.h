#pragma once
// ============================================================
// FILE: src/gui/VehicleProfileDialog.h
// MODULE: 7 - Qt GUI
// Window 5 of 12: Vehicle Profile (popup dialog)
// Shows tabbed details: Info, Maintenance, Fuel Log
// ============================================================

#include <QDialog>
#include <QTabWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QFrame>
#include <QGridLayout>

#include "../entities/Vehicle.h"
#include "../entities/ExtendedDomainEntities.h"

class VehicleProfileDialog : public QDialog {
    Q_OBJECT

private:
    Vehicle*    m_vehicle;
    QTabWidget* m_tabs;

public:
    explicit VehicleProfileDialog(Vehicle* vehicle, QWidget* parent = nullptr)
        : QDialog(parent), m_vehicle(vehicle) {

        setWindowTitle(QString("Vehicle Profile - %1")
                       .arg(vehicle->getRegistrationNo().c_str()));
        setMinimumSize(600, 450);
        setModal(true);

        QVBoxLayout* layout = new QVBoxLayout(this);

        // Header
        QFrame* header = new QFrame();
        header->setStyleSheet(
            "background:#1a1a2e; border-radius:6px; padding:10px;");
        QHBoxLayout* hLayout = new QHBoxLayout(header);

        QLabel* regLabel = new QLabel(
            QString("🚌 %1").arg(vehicle->getRegistrationNo().c_str()));
        regLabel->setStyleSheet("color:white; font-size:18px; font-weight:bold;");

        QLabel* statusLabel = new QLabel(
            vehicle->getStatusString().c_str());
        statusLabel->setStyleSheet(
            "background:#e94560; color:white; padding:4px 10px; "
            "border-radius:4px; font-weight:bold;");

        QPushButton* backBtn = new QPushButton("< Back to Dashboard");
        backBtn->setStyleSheet(
            "background:#444; color:white; padding:5px 12px; border-radius:4px; font-weight:bold;");

        hLayout->addWidget(regLabel);
        hLayout->addStretch();
        hLayout->addWidget(statusLabel);
        hLayout->addWidget(backBtn);
        layout->addWidget(header);

        connect(backBtn, &QPushButton::clicked, this, &QDialog::accept);

        // Tabs
        m_tabs = new QTabWidget();
        m_tabs->addTab(buildDetailsTab(),     "Details");
        m_tabs->addTab(buildMaintenanceTab(), "Maintenance Log");
        m_tabs->addTab(buildFuelLogTab(),     "Fuel Log");
        layout->addWidget(m_tabs);

        // Close button
        QPushButton* closeBtn = new QPushButton("Close");
        closeBtn->setStyleSheet(
            "background:#444; color:white; padding:6px 20px; border-radius:4px;");
        QHBoxLayout* btnRow = new QHBoxLayout();
        btnRow->addStretch();
        btnRow->addWidget(closeBtn);
        layout->addLayout(btnRow);

        connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    }

private:
    QWidget* buildDetailsTab() {
        QWidget* tab = new QWidget();
        QGridLayout* grid = new QGridLayout(tab);
        grid->setSpacing(10);
        grid->setContentsMargins(15, 15, 15, 15);

        auto addRow = [&](int row, const QString& label, const QString& value) {
            QLabel* lbl = new QLabel(label + ":");
            lbl->setStyleSheet("font-weight:bold; color:#555;");
            QLabel* val = new QLabel(value);
            val->setStyleSheet("color:#1a1a2e;");
            grid->addWidget(lbl, row, 0);
            grid->addWidget(val, row, 1);
        };

        addRow(0, "Registration No", m_vehicle->getRegistrationNo().c_str());
        addRow(1, "Make",           m_vehicle->getMake().c_str());
        addRow(2, "Model",          m_vehicle->getModel().c_str());
        addRow(3, "Category",       m_vehicle->getVehicleCategory().c_str());
        addRow(4, "Fuel Type",      m_vehicle->getFuelTypeString().c_str());
        addRow(5, "Capacity",       QString::number(m_vehicle->getCapacity()) + " passengers");
        addRow(6, "Current Pass",   QString::number(m_vehicle->getCurrentPassengers()));
        addRow(7, "Occupancy",      QString::number(m_vehicle->getOccupancyPercent()) + "%");
        addRow(8, "Odometer",       QString::number(m_vehicle->getOdometer(), 'f', 1) + " km");
        addRow(9, "Fuel Cost/km",   "Rs" + QString::number(m_vehicle->getFuelCostPerKm(), 'f', 2));
        addRow(10,"Daily Check",    m_vehicle->performDailyCheck() ? "✅ PASS" : "❌ FAIL");
        addRow(11,"CO2 per 100km",  QString::number(m_vehicle->calculateEmissions(100.0), 'f', 1) + " kg");
        addRow(12,"Status",         m_vehicle->getStatusString().c_str());
        addRow(13,"Entity ID",      QString::number(m_vehicle->getId()));
        addRow(14,"Created On",     m_vehicle->getCreatedOn().toString().c_str());

        grid->setRowStretch(15, 1);
        return tab;
    }

    QWidget* buildMaintenanceTab() {
        QWidget* tab = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(tab);

        QLabel* info = new QLabel(
            "Maintenance jobs for this vehicle.\n"
            "Connect to MaintenanceController to load real data.");
        info->setStyleSheet("color:#666; padding:10px;");
        layout->addWidget(info);

        QTableWidget* table = new QTableWidget(3, 4);
        table->setHorizontalHeaderLabels(
            {"Job ID", "Description", "Status", "Cost (Rs)"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);

        // Demo rows
        QStringList jobs[] = {
            {"J-001", "Engine Oil Change",    "COMPLETED", "3500.00"},
            {"J-002", "Tyre Replacement",     "COMPLETED", "12000.00"},
            {"J-003", "Brake Inspection",     "SCHEDULED", "0.00"}
        };
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 4; c++) {
                table->setItem(r, c, new QTableWidgetItem(jobs[r][c]));
            }
        }
        layout->addWidget(table);
        return tab;
    }

    QWidget* buildFuelLogTab() {
        QWidget* tab = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(tab);

        QTableWidget* table = new QTableWidget(3, 5);
        table->setHorizontalHeaderLabels(
            {"Date", "Litres", "Rs/Litre", "Total Rs", "Odometer"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);

        // Demo rows using FuelLog
        FuelLog logs[3] = {
            FuelLog(m_vehicle->getId(), 45.5, 260.0, 10500.0),
            FuelLog(m_vehicle->getId(), 50.0, 258.0, 11200.0),
            FuelLog(m_vehicle->getId(), 40.0, 265.0, 11800.0)
        };
        QString dates[] = {"01/04/2026", "10/04/2026", "20/04/2026"};
        for (int r = 0; r < 3; r++) {
            table->setItem(r, 0, new QTableWidgetItem(dates[r]));
            table->setItem(r, 1, new QTableWidgetItem(
                QString::number(logs[r].getLitresFilled(), 'f', 1)));
            table->setItem(r, 2, new QTableWidgetItem(
                QString::number(logs[r].getCostPerLitre(), 'f', 2)));
            table->setItem(r, 3, new QTableWidgetItem(
                QString::number(logs[r].getTotalCost(), 'f', 2)));
            table->setItem(r, 4, new QTableWidgetItem(
                QString::number(logs[r].getOdometerAtFill(), 'f', 0) + " km"));
        }
        layout->addWidget(table);

        QLabel* total = new QLabel(
            QString("Total fuel cost: Rs%1")
            .arg(logs[0].getTotalCost()+logs[1].getTotalCost()+logs[2].getTotalCost(),
                 0, 'f', 2));
        total->setStyleSheet("font-weight:bold; padding:8px; background:#f0f0f0;");
        layout->addWidget(total);
        return tab;
    }
};
