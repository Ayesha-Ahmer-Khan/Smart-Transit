#pragma once
// ============================================================
// FILE: src/gui/MaintenanceWindow.h
// MODULE: 7 - Qt GUI | Window 11 of 12
// Schedule and track vehicle maintenance jobs
// ============================================================

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>
#include "../controllers/ExtendedControllers.h"

class MaintenanceWindow : public QWidget {
    Q_OBJECT
private:
    QTableWidget*         m_jobsTable;
    QLabel*               m_costLabel;
    MaintenanceController m_maint;

public:
    explicit MaintenanceWindow(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowTitle("Maintenance Management");
        setMinimumSize(800, 520);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(15,15,15,15);

        QLabel* title = new QLabel("Maintenance Management");
        title->setStyleSheet("font-size:18px;font-weight:bold;color:#1a1a2e;");
        QPushButton* backBtn = new QPushButton("< Back to Dashboard");
        backBtn->setStyleSheet("background:#444;color:white;padding:6px 12px;border-radius:4px;font-weight:bold;");
        connect(backBtn, &QPushButton::clicked, this, &QWidget::close);

        QHBoxLayout* topBar = new QHBoxLayout();
        topBar->addWidget(title);
        topBar->addStretch();
        topBar->addWidget(backBtn);
        layout->addLayout(topBar);

        QHBoxLayout* toolbar = new QHBoxLayout();
        QPushButton* schedBtn    = new QPushButton("+ Schedule Job");
        QPushButton* startBtn    = new QPushButton("Start Job");
        QPushButton* completeBtn = new QPushButton("Complete Job");
        QPushButton* refreshBtn  = new QPushButton("Refresh");
        schedBtn->setStyleSheet(
            "background:#0f3460;color:white;padding:5px 12px;border-radius:4px;");
        startBtn->setStyleSheet(
            "background:#533483;color:white;padding:5px 12px;border-radius:4px;");
        completeBtn->setStyleSheet(
            "background:#1b4332;color:white;padding:5px 12px;border-radius:4px;");
        refreshBtn->setStyleSheet(
            "background:#444;color:white;padding:5px 12px;border-radius:4px;");
        toolbar->addWidget(schedBtn);
        toolbar->addWidget(startBtn);
        toolbar->addWidget(completeBtn);
        toolbar->addWidget(refreshBtn);
        toolbar->addStretch();
        layout->addLayout(toolbar);

        m_jobsTable = new QTableWidget();
        m_jobsTable->setColumnCount(6);
        m_jobsTable->setHorizontalHeaderLabels(
            {"Job ID","Vehicle","Technician","Description","Status","Cost (Rs)"});
        m_jobsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_jobsTable->setAlternatingRowColors(true);
        m_jobsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_jobsTable->horizontalHeader()->setSectionsClickable(true);
        layout->addWidget(m_jobsTable);

        m_costLabel = new QLabel("Total Maintenance Cost: Rs 0.00");
        m_costLabel->setStyleSheet(
            "font-weight:bold;padding:8px;background:#f0f0f0;border-radius:4px;");
        layout->addWidget(m_costLabel);

        connect(schedBtn,    &QPushButton::clicked, this, &MaintenanceWindow::onSchedule);
        connect(startBtn,    &QPushButton::clicked, this, &MaintenanceWindow::onStart);
        connect(completeBtn, &QPushButton::clicked, this, &MaintenanceWindow::onComplete);
        connect(refreshBtn,  &QPushButton::clicked, this, &MaintenanceWindow::refreshTable);
    }

private slots:
    void onSchedule() {
        try {
            MaintenanceJob& job = m_maint.scheduleJob(
                1, 1, CustomString("Scheduled via GUI"), CustomDate::today());
            job.addPart(CustomString("Filter"));
            job.addPart(CustomString("Engine Oil"));
            refreshTable();
            QMessageBox::information(this,"Scheduled","Maintenance job scheduled!");
        } catch (const TransitException& e) {
            QMessageBox::critical(this,"Error",e.getMessage().c_str());
        }
    }

    void onStart() {
        QMessageBox::information(this, "Start Job",
            "Select a SCHEDULED job then this button calls:\n"
            "maintenanceController.startJob(jobId)\n"
            "Status changes to IN PROGRESS.");
    }

    void onComplete() {
        if (m_maint.getTotalJobs() == 0) {
            QMessageBox::warning(this,"No Jobs","Schedule a job first!"); return;
        }
        m_maint.completeJob(1, 3500.0);
        refreshTable();
        QMessageBox::information(this,"Completed",
            "Job completed!\nCost: Rs 3,500.00");
    }

    void refreshTable() {
        m_jobsTable->setRowCount(m_maint.getTotalJobs());
        for (int i = 0; i < m_maint.getTotalJobs(); i++) {
            m_jobsTable->setItem(i,0,new QTableWidgetItem(QString("J-%1").arg(i+1)));
            m_jobsTable->setItem(i,1,new QTableWidgetItem("Vehicle #1"));
            m_jobsTable->setItem(i,2,new QTableWidgetItem("Tech #1"));
            m_jobsTable->setItem(i,3,new QTableWidgetItem("Scheduled via GUI"));

            QTableWidgetItem* statusItem = new QTableWidgetItem("SCHEDULED");
            statusItem->setBackground(QColor("#fff3cd"));
            m_jobsTable->setItem(i,4,statusItem);
            m_jobsTable->setItem(i,5,new QTableWidgetItem(
                QString::number(m_maint.getTotalMaintenanceCost(),'f',2)));
        }
        m_costLabel->setText(
            QString("Total Maintenance Cost: Rs%1")
            .arg(m_maint.getTotalMaintenanceCost(),0,'f',2));
    }
};
