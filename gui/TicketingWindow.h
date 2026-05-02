#pragma once
// ============================================================
// FILE: src/gui/TicketingWindow.h
// MODULE: 7 - Qt GUI
// Window 9 of 12: Ticketing
// Issue tickets with polymorphic fare calculation
// ============================================================

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QGroupBox>

#include "../controllers/Controllers.h"
#include "../entities/IFareCalculator.h"

class TicketingWindow : public QWidget {
    Q_OBJECT

private:
    QComboBox*      m_fareTypeCombo;
    QDoubleSpinBox* m_distanceSpin;
    QCheckBox*      m_peakCheck;
    QSpinBox*       m_loyaltySpin;
    QLabel*         m_farePreviewLabel;
    QLabel*         m_receiptLabel;
    QTableWidget*   m_ticketsTable;
    AppController&  m_app;

public:
    explicit TicketingWindow(QWidget* parent = nullptr)
        : QWidget(parent), m_app(AppController::getInstance()) {

        setWindowTitle("Ticketing System");
        setMinimumSize(800, 600);

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(15, 15, 15, 15);
        mainLayout->setSpacing(10);

        QLabel* title = new QLabel("Ticketing System");
        title->setStyleSheet("font-size:18px; font-weight:bold; color:#1a1a2e;");

        QPushButton* backBtn = new QPushButton("< Back to Dashboard");
        backBtn->setStyleSheet("background:#444;color:white;padding:6px 12px;border-radius:4px;font-weight:bold;");
        connect(backBtn, &QPushButton::clicked, this, &QWidget::close);

        QHBoxLayout* topBar = new QHBoxLayout();
        topBar->addWidget(title);
        topBar->addStretch();
        topBar->addWidget(backBtn);
        mainLayout->addLayout(topBar);

        // === Issue Ticket Form ===
        QGroupBox* issueBox = new QGroupBox("Issue New Ticket");
        issueBox->setStyleSheet("QGroupBox { font-weight:bold; }");
        QVBoxLayout* issueLayout = new QVBoxLayout(issueBox);

        // Fare type
        QHBoxLayout* row1 = new QHBoxLayout();
        row1->addWidget(new QLabel("Fare Type:"));
        m_fareTypeCombo = new QComboBox();
        m_fareTypeCombo->addItem("Bus - Flat Fare (Rs30 flat)");
        m_fareTypeCombo->addItem("Metro - Zone Based (Rs4/km)");
        m_fareTypeCombo->addItem("Ride Hail - Surge Pricing (Rs25/km x1.5)");
        row1->addWidget(m_fareTypeCombo);
        issueLayout->addLayout(row1);

        // Distance
        QHBoxLayout* row2 = new QHBoxLayout();
        row2->addWidget(new QLabel("Distance (km):"));
        m_distanceSpin = new QDoubleSpinBox();
        m_distanceSpin->setRange(0.5, 100.0);
        m_distanceSpin->setValue(10.0);
        m_distanceSpin->setSingleStep(0.5);
        row2->addWidget(m_distanceSpin);
        m_peakCheck = new QCheckBox("Peak Hour? (+20%)");
        row2->addWidget(m_peakCheck);
        issueLayout->addLayout(row2);

        // Loyalty points
        QHBoxLayout* row3 = new QHBoxLayout();
        row3->addWidget(new QLabel("Loyalty Points to Use:"));
        m_loyaltySpin = new QSpinBox();
        m_loyaltySpin->setRange(0, 500);
        m_loyaltySpin->setValue(0);
        row3->addWidget(m_loyaltySpin);
        issueLayout->addLayout(row3);

        // Fare preview
        m_farePreviewLabel = new QLabel("Estimated fare: Rs --");
        m_farePreviewLabel->setStyleSheet(
            "background:#e8f4f8; padding:8px; border-radius:4px; font-weight:bold;");
        issueLayout->addWidget(m_farePreviewLabel);

        // Buttons
        QHBoxLayout* btnRow = new QHBoxLayout();
        QPushButton* calcBtn  = new QPushButton("Calculate Fare");
        QPushButton* issueBtn = new QPushButton("Issue Ticket");
        calcBtn->setStyleSheet(
            "background:#0f3460; color:white; padding:6px 16px; border-radius:4px;");
        issueBtn->setStyleSheet(
            "background:#e94560; color:white; padding:6px 16px; border-radius:4px; font-weight:bold;");
        btnRow->addWidget(calcBtn);
        btnRow->addWidget(issueBtn);
        btnRow->addStretch();
        issueLayout->addLayout(btnRow);

        // Receipt
        m_receiptLabel = new QLabel("Receipt will appear here after issuing ticket...");
        m_receiptLabel->setStyleSheet(
            "background:#f8f8f8; padding:10px; border:1px solid #ddd; "
            "border-radius:4px; font-family:Courier New;");
        m_receiptLabel->setWordWrap(true);
        issueLayout->addWidget(m_receiptLabel);

        mainLayout->addWidget(issueBox);

        // === Tickets Table ===
        QLabel* tableTitle = new QLabel("Issued Tickets:");
        tableTitle->setStyleSheet("font-weight:bold; margin-top:5px;");
        mainLayout->addWidget(tableTitle);

        m_ticketsTable = new QTableWidget();
        m_ticketsTable->setColumnCount(5);
        m_ticketsTable->setHorizontalHeaderLabels(
            {"Ticket ID", "Fare (Rs)", "Type", "Validated", "Issued"});
        m_ticketsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_ticketsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_ticketsTable->setAlternatingRowColors(true);
        mainLayout->addWidget(m_ticketsTable);

        // Totals
        QLabel* totalLabel = new QLabel("Total Revenue: Rs 0.00");
        totalLabel->setStyleSheet("font-weight:bold; padding:6px; background:#f0f0f0;");
        mainLayout->addWidget(totalLabel);

        // Connections
        connect(calcBtn,  &QPushButton::clicked, this, &TicketingWindow::onCalculate);
        connect(issueBtn, &QPushButton::clicked, this, &TicketingWindow::onIssue);
        connect(m_fareTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &TicketingWindow::onCalculate);
        connect(m_distanceSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                [this](double) { onCalculate(); });
    }

private slots:
    void onCalculate() {
        try {
            int    fareIdx  = m_fareTypeCombo->currentIndex();
            double dist     = m_distanceSpin->value();
            bool   isPeak   = m_peakCheck->isChecked();
            int    loyalty  = m_loyaltySpin->value();

            // [OOP: Polymorphic fare calculation]
            IFareCalculator* calcs[3] = {
                new FlatFareBus(),
                new ZoneBasedMetro(),
                new SurgeRideHail(1.5)
            };

            double base  = calcs[fareIdx]->calculateFare(dist, 1, isPeak);
            double final_= calcs[fareIdx]->applyDiscount(base, loyalty);

            m_farePreviewLabel->setText(
                QString("Estimated fare: Rs%1  (Base: Rs%2, Discount: Rs%3)")
                .arg(final_, 0, 'f', 2)
                .arg(base,   0, 'f', 2)
                .arg(base - final_, 0, 'f', 2));

            for (int i = 0; i < 3; i++) delete calcs[i];

        } catch (...) {}
    }

    void onIssue() {
        try {
            int fareIdx = m_fareTypeCombo->currentIndex();
            double dist = m_distanceSpin->value();
            bool isPeak = m_peakCheck->isChecked();
            int loyalty = m_loyaltySpin->value();

            // [OOP: Calls TicketingController which uses IFareCalculator polymorphically]
            Ticket t = m_app.getTicketing().issueTicket(
                1, 1, dist, fareIdx, isPeak, loyalty);

            // Show receipt
            m_receiptLabel->setText(
                QString("=== TICKET ISSUED ===\n"
                        "Ticket ID : TKT-%1\n"
                        "Fare Type : %2\n"
                        "Distance  : %3 km\n"
                        "Amount    : Rs %4\n"
                        "Peak Hour : %5\n"
                        "====================")
                .arg(t.getTicketId())
                .arg(t.getTypeString().c_str())
                .arg(dist, 0, 'f', 1)
                .arg(t.getFareAmount(), 0, 'f', 2)
                .arg(isPeak ? "Yes" : "No"));

            refreshTable();
            QMessageBox::information(this, "Ticket Issued",
                QString("Ticket #%1 issued!\nFare: Rs%2")
                .arg(t.getTicketId())
                .arg(t.getFareAmount(), 0, 'f', 2));

        } catch (const TransitException& e) {
            QMessageBox::critical(this, "Error", e.getMessage().c_str());
        }
    }

    void refreshTable() {
        int total = m_app.getTicketing().getTotalTickets();
        m_ticketsTable->setRowCount(total);
        // In full version: iterate tickets from controller
        // For demo show count row
        if (total > 0) {
            m_ticketsTable->setItem(total-1, 0,
                new QTableWidgetItem(QString::number(total)));
            m_ticketsTable->setItem(total-1, 1,
                new QTableWidgetItem(
                    QString::number(m_app.getTicketing().getTotalRevenue(),'f',2)));
            m_ticketsTable->setItem(total-1, 2,
                new QTableWidgetItem("Single Journey"));
            m_ticketsTable->setItem(total-1, 3,
                new QTableWidgetItem("NO"));
            m_ticketsTable->setItem(total-1, 4,
                new QTableWidgetItem("Today"));
        }
    }
};
