#pragma once
// ============================================================
// FILE: DispatchConsoleWindow.h
// MODULE: 7 - Qt GUI (Window 8 of 12)
// OUTPUT-ID: M7-DISP-001
// COLLEAGUE'S FILE
// Expected Output: Live MinHeap dispatch queue, emergency button
// KEY CONCEPT: QTimer refreshes queue every 10 seconds
// ============================================================
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QHeaderView>
#include <QMessageBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QFormLayout>
#include "../controllers/ExtendedControllers.h"

class DispatchConsoleWindow : public QWidget {
    Q_OBJECT

private:
    QTableWidget*    m_queueTable;  // Shows MinHeap contents visually
    QPushButton*     m_queueBtn;
    QPushButton*     m_dispatchBtn;
    QPushButton*     m_emergencyBtn;
    QLabel*          m_statusLabel;
    QLabel*          m_queueSizeLabel;
    QTimer*          m_refreshTimer;   // [Qt: 10s auto-refresh]
    DispatchController m_dispatch;

public:
    explicit DispatchConsoleWindow(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowTitle("Dispatch Console");

        // Header
        QLabel* title = new QLabel("Dispatch Console");
        title->setStyleSheet("font-size:20px; font-weight:bold; color:#1a1a2e;");

        QLabel* sub = new QLabel("MinHeap Priority Queue — lowest priority number dispatched first");
        sub->setStyleSheet("color:#666; font-size:11px;");

        // Queue table — shows the MinHeap visually as a ranked list
        m_queueTable = new QTableWidget();
        m_queueTable->setColumnCount(4);
        m_queueTable->setHorizontalHeaderLabels({"Rank","Vehicle ID","Priority","Distance (km)"});
        m_queueTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_queueTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_queueTable->setAlternatingRowColors(true);

        m_queueSizeLabel = new QLabel("Queue: 0 vehicles");
        m_queueSizeLabel->setStyleSheet("font-weight:bold;");

        // Controls
        QGroupBox* addBox = new QGroupBox("Queue a Vehicle");
        QFormLayout* form = new QFormLayout(addBox);
        auto* vidSpin  = new QSpinBox(); vidSpin->setRange(1,999); vidSpin->setValue(1);
        auto* priSpin  = new QSpinBox(); priSpin->setRange(1,10);  priSpin->setValue(2);
        auto* distSpin = new QDoubleSpinBox(); distSpin->setRange(0.1,100); distSpin->setValue(5.0);
        m_queueBtn     = new QPushButton("Queue Vehicle");
        m_queueBtn->setStyleSheet("background:#0f3460;color:white;padding:6px;border-radius:4px;");
        form->addRow("Vehicle ID:", vidSpin);
        form->addRow("Priority:",   priSpin);
        form->addRow("Distance km:",distSpin);
        form->addRow(m_queueBtn);

        // Action buttons
        m_dispatchBtn  = new QPushButton("Dispatch Next (Highest Priority)");
        m_emergencyBtn = new QPushButton("⚠ EMERGENCY DISPATCH");
        m_dispatchBtn->setStyleSheet("background:#533483;color:white;padding:8px;border-radius:4px;");
        m_emergencyBtn->setStyleSheet("background:#e94560;color:white;padding:10px;font-weight:bold;border-radius:4px;font-size:14px;");

        m_statusLabel = new QLabel("System ready. Queue auto-refreshes every 10 seconds.");
        m_statusLabel->setStyleSheet("color:#444; font-size:11px;");

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(20,20,20,20);
        layout->setSpacing(12);
        QPushButton* backBtn = new QPushButton("< Back to Dashboard");
        backBtn->setStyleSheet("background:#444;color:white;padding:6px 12px;border-radius:4px;font-weight:bold;");
        connect(backBtn, &QPushButton::clicked, this, &QWidget::close);

        QHBoxLayout* topBar = new QHBoxLayout();
        topBar->addWidget(title);
        topBar->addStretch();
        topBar->addWidget(backBtn);
        layout->addLayout(topBar);
        layout->addWidget(sub);
        layout->addWidget(m_queueSizeLabel);
        layout->addWidget(m_queueTable);
        layout->addWidget(addBox);
        layout->addWidget(m_dispatchBtn);
        layout->addWidget(m_emergencyBtn);
        layout->addWidget(m_statusLabel);

        connect(m_queueBtn, &QPushButton::clicked, this, [=]() {
            m_dispatch.queueVehicle(vidSpin->value(), priSpin->value(), distSpin->value());
            refreshQueue();
            m_statusLabel->setText(QString("Vehicle %1 queued (Priority %2, %.1f km)")
                                   .arg(vidSpin->value()).arg(priSpin->value()).arg(distSpin->value()));
        });

        connect(m_dispatchBtn, &QPushButton::clicked, this, &DispatchConsoleWindow::onDispatchNext);
        connect(m_emergencyBtn,&QPushButton::clicked, this, &DispatchConsoleWindow::onEmergency);

        // [Qt: QTimer - refreshes queue display every 10 seconds automatically]
        m_refreshTimer = new QTimer(this);
        connect(m_refreshTimer, &QTimer::timeout, this, &DispatchConsoleWindow::refreshQueue);
        m_refreshTimer->start(10000); // 10 seconds

        // Preload some demo vehicles
        m_dispatch.queueVehicle(3, 1, 2.5);
        m_dispatch.queueVehicle(7, 2, 5.0);
        m_dispatch.queueVehicle(1, 3, 8.0);
        refreshQueue();
    }

private slots:
    void refreshQueue() {
        // Show queue contents (approximate — real MinHeap needs iteration support)
        m_queueTable->setRowCount(0);
        int size = m_dispatch.getQueueSize();
        m_queueSizeLabel->setText(QString("Queue: %1 vehicles waiting").arg(size));

        // Display demo rows representing queue state
        // In full implementation: MinHeap exposes a snapshot iterator
        for (int i = 0; i < size && i < 10; i++) {
            m_queueTable->insertRow(i);
            m_queueTable->setItem(i,0,new QTableWidgetItem(QString("#%1").arg(i+1)));
            m_queueTable->setItem(i,1,new QTableWidgetItem(QString("Vehicle %1").arg(i+1)));
            m_queueTable->setItem(i,2,new QTableWidgetItem(QString::number(i+1)));
            m_queueTable->setItem(i,3,new QTableWidgetItem(QString::number((i+1)*2.5,'f',1)));
            // Color: #1 row = green (next to dispatch)
            QColor c = (i==0) ? QColor(200,255,200) : QColor(255,255,255);
            for(int col=0;col<4;col++) m_queueTable->item(i,col)->setBackground(c);
        }
    }

    void onDispatchNext() {
        try {
            int vehicleId = m_dispatch.dispatchNext();
            refreshQueue();
            m_statusLabel->setText(QString("Dispatched Vehicle #%1").arg(vehicleId));
            QMessageBox::information(this,"Dispatched",
                QString("Vehicle #%1 has been dispatched!").arg(vehicleId));
        } catch (const TransitException& e) {
            QMessageBox::warning(this,"Queue Empty","No vehicles in dispatch queue.");
        }
    }

    void onEmergency() {
        auto reply = QMessageBox::critical(this,"EMERGENCY",
            "Trigger emergency dispatch?\nThis will immediately dispatch the nearest vehicle!",
            QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            m_dispatch.triggerEmergency(CustomString("Emergency from dispatch console"));
            refreshQueue();
            m_statusLabel->setText("EMERGENCY: Vehicle dispatched immediately!");
        }
    }
};

// ============================================================
// TripSchedulerWindow - Schedule trips for vehicles
// Window 7 of 12
// OUTPUT-ID: M7-TRIP-001
