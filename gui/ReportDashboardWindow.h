#pragma once
// ============================================================
// FILE: src/gui/ReportDashboardWindow.h
// MODULE: 7 - Qt GUI | Window 12 of 12
// Polymorphic report generation + CSV export
// ============================================================

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTextBrowser>
#include <QDateEdit>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include "../controllers/Controllers.h"
#include "../controllers/ExtendedControllers.h"
#include "../entities/Interfaces.h"

class ReportDashboardWindow : public QWidget {
    Q_OBJECT
private:
    QComboBox*   m_reportTypeCombo;
    QDateEdit*   m_fromDateEdit;
    QDateEdit*   m_toDateEdit;
    QTextBrowser* m_reportView;
    AppController& m_app;

public:
    explicit ReportDashboardWindow(QWidget* parent = nullptr)
        : QWidget(parent), m_app(AppController::getInstance()) {

        setWindowTitle("Report Dashboard");
        setMinimumSize(820, 560);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(15,15,15,15);

        QLabel* title = new QLabel("Reports Dashboard");
        title->setStyleSheet("font-size:18px;font-weight:bold;color:#1a1a2e;");
        QPushButton* backBtn = new QPushButton("< Back to Dashboard");
        backBtn->setStyleSheet("background:#444;color:white;padding:6px 12px;border-radius:4px;font-weight:bold;");
        connect(backBtn, &QPushButton::clicked, this, &QWidget::close);

        QHBoxLayout* topBar = new QHBoxLayout();
        topBar->addWidget(title);
        topBar->addStretch();
        topBar->addWidget(backBtn);
        layout->addLayout(topBar);

        // Controls
        QGroupBox* ctrlBox = new QGroupBox("Report Settings");
        QHBoxLayout* ctrlRow = new QHBoxLayout(ctrlBox);

        ctrlRow->addWidget(new QLabel("Report Type:"));
        m_reportTypeCombo = new QComboBox();
        m_reportTypeCombo->addItems({
            "System Summary",
            "Fleet Utilisation Report",
            "Revenue Report",
            "Incident Report",
            "Driver Performance Report",
            "Route Efficiency Report"});
        ctrlRow->addWidget(m_reportTypeCombo);

        ctrlRow->addWidget(new QLabel("From:"));
        m_fromDateEdit = new QDateEdit(QDate::currentDate().addDays(-30));
        m_fromDateEdit->setDisplayFormat("dd/MM/yyyy");
        ctrlRow->addWidget(m_fromDateEdit);

        ctrlRow->addWidget(new QLabel("To:"));
        m_toDateEdit = new QDateEdit(QDate::currentDate());
        m_toDateEdit->setDisplayFormat("dd/MM/yyyy");
        ctrlRow->addWidget(m_toDateEdit);

        QPushButton* generateBtn = new QPushButton("Generate Report");
        QPushButton* exportBtn   = new QPushButton("Export CSV");
        generateBtn->setStyleSheet(
            "background:#0f3460;color:white;padding:6px 14px;border-radius:4px;");
        exportBtn->setStyleSheet(
            "background:#1b4332;color:white;padding:6px 14px;border-radius:4px;");
        ctrlRow->addWidget(generateBtn);
        ctrlRow->addWidget(exportBtn);
        layout->addWidget(ctrlBox);

        // Report view
        m_reportView = new QTextBrowser();
        m_reportView->setStyleSheet(
            "background:white; font-family:'Courier New'; font-size:12px; "
            "border:1px solid #ddd; border-radius:4px;");
        m_reportView->setPlaceholderText(
            "Select a report type and click 'Generate Report'...");
        layout->addWidget(m_reportView);

        connect(generateBtn, &QPushButton::clicked, this,
                &ReportDashboardWindow::onGenerate);
        connect(exportBtn,   &QPushButton::clicked, this,
                &ReportDashboardWindow::onExport);
    }

private slots:
    void onGenerate() {
        int type = m_reportTypeCombo->currentIndex();
        QString report;
        QString date = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm");

        // [OOP: Polymorphic report generation via IReportGenerator]
        IReportGenerator* gen = nullptr;

        switch (type) {
            case 0: { // System Summary
                report  = "========================================\n";
                report += "   SMARTTRANSIT EMS - SYSTEM SUMMARY\n";
                report += "========================================\n";
                report += "Generated: " + date + "\n\n";
                report += QString("Total Entities Created : %1\n")
                          .arg(Entity::getTotalCreated());
                report += QString("Total Staff Members    : %1\n")
                          .arg(StaffMember::getTotalStaff());
                report += QString("Fleet Size             : %1\n")
                          .arg(m_app.getFleet().getTotalVehicles());
                report += QString("Active Vehicles        : %1\n")
                          .arg(m_app.getFleet().getActiveCount());
                report += QString("Tickets Issued         : %1\n")
                          .arg(m_app.getTicketing().getTotalTickets());
                report += QString("Total Revenue          : Rs%1\n")
                          .arg(m_app.getTicketing().getTotalRevenue(), 0,'f',2);
                report += "========================================\n";
                break;
            }
            case 1: { // Fleet Utilisation
                gen = new FleetUtilisationReport(
                    m_app.getFleet().getTotalVehicles(),
                    m_app.getFleet().getActiveCount(), 72.5);
                report = gen->generateReport().c_str();
                break;
            }
            case 2: { // Revenue
                gen = new RevenueReport(
                    m_app.getTicketing().getTotalRevenue(),
                    m_app.getTicketing().getTotalTickets());
                report = gen->generateReport().c_str();
                break;
            }
            case 3: { // Incident
                gen = new IncidentReport(5, 4, 1);
                report = gen->generateReport().c_str();
                break;
            }
            default:
                report = "Report type: " + m_reportTypeCombo->currentText() + "\n";
                report += "Generated: " + date + "\n";
                report += "Full implementation connects to all controllers.\n";
        }

        delete gen;
        m_reportView->setPlainText(report);
        m_reportView->moveCursor(QTextCursor::Start);
    }

    void onExport() {
        QString fileName = QFileDialog::getSaveFileName(
            this, "Export CSV",
            m_reportTypeCombo->currentText().replace(" ", "_") + ".csv",
            "CSV Files (*.csv)");

        if (!fileName.isEmpty()) {
            IReportGenerator* gen = new RevenueReport(
                m_app.getTicketing().getTotalRevenue(),
                m_app.getTicketing().getTotalTickets());
            gen->exportCSV(CustomString(fileName.toStdString().c_str()));
            delete gen;
            QMessageBox::information(this, "Exported",
                "Report exported to:\n" + fileName);
        }
    }
};
