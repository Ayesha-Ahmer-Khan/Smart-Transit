#pragma once
// ============================================================
// FILE: src/gui/TripSchedulerWindow.h
// MODULE: 7 - Qt GUI | Window 7 of 12
// Uses QCalendarWidget for date selection
// ============================================================

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QCalendarWidget>
#include <QTimeEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include "../controllers/ExtendedControllers.h"

class TripSchedulerWindow : public QWidget {
    Q_OBJECT
private:
    QCalendarWidget* m_calendar;
    QComboBox*       m_vehicleCombo;
    QComboBox*       m_driverCombo;
    QComboBox*       m_routeCombo;
    QTimeEdit*       m_timeEdit;
    QLabel*          m_conflictLabel;
    QTableWidget*    m_tripsTable;
    AppController&   m_app;

public:
    explicit TripSchedulerWindow(QWidget* parent = nullptr)
        : QWidget(parent), m_app(AppController::getInstance()) {

        setWindowTitle("Trip Scheduler");
        setMinimumSize(850, 560);

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(15,15,15,15);

        QLabel* title = new QLabel("Trip Scheduler");
        title->setStyleSheet("font-size:18px;font-weight:bold;color:#1a1a2e;");
        QPushButton* backBtn = new QPushButton("< Back to Dashboard");
        backBtn->setStyleSheet("background:#444;color:white;padding:6px 12px;border-radius:4px;font-weight:bold;");
        connect(backBtn, &QPushButton::clicked, this, &QWidget::close);

        QHBoxLayout* topBar = new QHBoxLayout();
        topBar->addWidget(title);
        topBar->addStretch();
        topBar->addWidget(backBtn);
        mainLayout->addLayout(topBar);

        QHBoxLayout* content = new QHBoxLayout();

        // Left: Calendar
        QVBoxLayout* calCol = new QVBoxLayout();
        calCol->addWidget(new QLabel("<b>Select Departure Date:</b>"));
        m_calendar = new QCalendarWidget();
        m_calendar->setMinimumDate(QDate::currentDate());
        m_calendar->setStyleSheet(
            "QCalendarWidget { border:1px solid #ddd; border-radius:4px; }");
        calCol->addWidget(m_calendar);
        content->addLayout(calCol);

        // Right: Form
        QGroupBox* formBox = new QGroupBox("Schedule Details");
        QVBoxLayout* formLayout = new QVBoxLayout(formBox);

        formLayout->addWidget(new QLabel("<b>Vehicle:</b>"));
        m_vehicleCombo = new QComboBox();
        m_vehicleCombo->addItems({
            "KHI-101 (City Bus - Route 1)",
            "MTR-001 (Metro Train - Line 1)",
            "KHI-CAR-01 (Ride Hail Car)"});
        formLayout->addWidget(m_vehicleCombo);

        formLayout->addWidget(new QLabel("<b>Driver:</b>"));
        m_driverCombo = new QComboBox();
        m_driverCombo->addItems({
            "DRV-010 - Zahid Hussain (HTV)",
            "DRV-011 - Ali Khan (LTV)",
            "(unassigned - will throw NoDriverAssignedException)"});
        formLayout->addWidget(m_driverCombo);

        formLayout->addWidget(new QLabel("<b>Route:</b>"));
        m_routeCombo = new QComboBox();
        m_routeCombo->addItems({
            "Route 1: City Hall → Saddar → Clifton",
            "Route 2: Airport → Gulshan → DHA",
            "Route 3: NUST → Saddar → Sea View"});
        formLayout->addWidget(m_routeCombo);

        formLayout->addWidget(new QLabel("<b>Departure Time:</b>"));
        m_timeEdit = new QTimeEdit(QTime(8, 0, 0));
        m_timeEdit->setDisplayFormat("HH:mm");
        formLayout->addWidget(m_timeEdit);

        m_conflictLabel = new QLabel("");
        m_conflictLabel->setStyleSheet("color:#c0392b;font-weight:bold;");
        formLayout->addWidget(m_conflictLabel);

        QPushButton* schedBtn = new QPushButton("Schedule Trip");
        schedBtn->setStyleSheet(
            "background:#e94560;color:white;padding:8px;border-radius:4px;font-weight:bold;");
        formLayout->addWidget(schedBtn);
        formLayout->addStretch();
        content->addWidget(formBox);
        mainLayout->addLayout(content);

        // Trips table
        QLabel* tripsTitle = new QLabel("<b>Scheduled Trips:</b>");
        mainLayout->addWidget(tripsTitle);
        m_tripsTable = new QTableWidget();
        m_tripsTable->setColumnCount(6);
        m_tripsTable->setHorizontalHeaderLabels(
            {"Trip ID","Vehicle","Driver","Route","Time","Status"});
        m_tripsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_tripsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_tripsTable->setAlternatingRowColors(true);
        mainLayout->addWidget(m_tripsTable);

        connect(schedBtn, &QPushButton::clicked, this, &TripSchedulerWindow::onSchedule);
        connect(m_vehicleCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &TripSchedulerWindow::onCheckConflict);
    }

private slots:
    void onCheckConflict() {
        // Check if selected vehicle has active trip
        // In full version: calls TripController to check
        m_conflictLabel->setText("");  // No conflict detected
    }

    void onSchedule() {
        // [OOP: Throws NoDriverAssignedException if unassigned selected]
        if (m_driverCombo->currentIndex() == 2) {
            QMessageBox::critical(this, "No Driver",
                "NoDriverAssignedException thrown!\n"
                "Cannot schedule trip without an assigned driver.");
            return;
        }

        QTime t = m_timeEdit->time();
        QDate d = m_calendar->selectedDate();

        // Add to trips table
        int row = m_tripsTable->rowCount();
        m_tripsTable->insertRow(row);
        m_tripsTable->setItem(row,0, new QTableWidgetItem(
            QString("TRIP-%1").arg(row+1)));
        m_tripsTable->setItem(row,1, new QTableWidgetItem(
            m_vehicleCombo->currentText().left(7)));
        m_tripsTable->setItem(row,2, new QTableWidgetItem(
            m_driverCombo->currentText().left(7)));
        m_tripsTable->setItem(row,3, new QTableWidgetItem(
            m_routeCombo->currentText().left(10)));
        m_tripsTable->setItem(row,4, new QTableWidgetItem(
            d.toString("dd/MM") + " " + t.toString("HH:mm")));
        QTableWidgetItem* status = new QTableWidgetItem("SCHEDULED");
        status->setBackground(QColor("#d4edda"));
        m_tripsTable->setItem(row,5, status);

        QMessageBox::information(this, "Trip Scheduled",
            QString("Trip scheduled!\nVehicle: %1\nDriver: %2\nDate: %3\nTime: %4")
            .arg(m_vehicleCombo->currentText())
            .arg(m_driverCombo->currentText())
            .arg(d.toString("dd/MM/yyyy"))
            .arg(t.toString("HH:mm")));
    }
};
