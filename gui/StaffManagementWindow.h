#pragma once
// ============================================================
// FILE: src/gui/StaffManagementWindow.h
// MODULE: 7 - Qt GUI
// Window 10 of 12: Staff Management
// ============================================================

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QColor>

#include "../controllers/Controllers.h"
#include "../entities/StaffMember.h"
#include "../entities/Level5Vehicles.h"

// --- Add Staff Dialog ---
class AddStaffDialog : public QDialog {
    Q_OBJECT
private:
    QLineEdit*    m_firstNameInput;
    QLineEdit*    m_lastNameInput;
    QLineEdit*    m_cnicInput;
    QComboBox*    m_roleCombo;
    QDoubleSpinBox* m_salaryInput;
    QLineEdit*    m_empIdInput;

public:
    explicit AddStaffDialog(QWidget* parent = nullptr) : QDialog(parent) {
        setWindowTitle("Add New Staff Member");
        setFixedSize(380, 300);

        QFormLayout* form = new QFormLayout(this);
        form->setSpacing(10);
        form->setContentsMargins(20, 20, 20, 20);

        m_firstNameInput = new QLineEdit();
        m_firstNameInput->setPlaceholderText("First Name");
        m_lastNameInput  = new QLineEdit();
        m_lastNameInput->setPlaceholderText("Last Name");
        m_cnicInput      = new QLineEdit();
        m_cnicInput->setPlaceholderText("42201-1234567-1");
        m_empIdInput     = new QLineEdit();
        m_empIdInput->setPlaceholderText("DRV-001");
        m_roleCombo      = new QComboBox();
        m_roleCombo->addItems({"Driver","Conductor","Dispatcher",
                               "Maintenance Tech","Admin"});
        m_salaryInput    = new QDoubleSpinBox();
        m_salaryInput->setRange(20000, 200000);
        m_salaryInput->setValue(40000);
        m_salaryInput->setSuffix(" Rs");

        form->addRow("First Name:", m_firstNameInput);
        form->addRow("Last Name:",  m_lastNameInput);
        form->addRow("CNIC:",       m_cnicInput);
        form->addRow("Employee ID:",m_empIdInput);
        form->addRow("Role:",       m_roleCombo);
        form->addRow("Base Salary:",m_salaryInput);

        QHBoxLayout* btnRow = new QHBoxLayout();
        QPushButton* addBtn    = new QPushButton("Add");
        QPushButton* cancelBtn = new QPushButton("Cancel");
        addBtn->setStyleSheet(
            "background:#0f3460; color:white; padding:6px 16px; border-radius:4px;");
        cancelBtn->setStyleSheet(
            "background:#888; color:white; padding:6px 16px; border-radius:4px;");
        btnRow->addWidget(addBtn);
        btnRow->addWidget(cancelBtn);
        form->addRow(btnRow);

        connect(addBtn,    &QPushButton::clicked, this, &QDialog::accept);
        connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    }

    QString getFirstName() const { return m_firstNameInput->text(); }
    QString getLastName()  const { return m_lastNameInput->text();  }
    QString getCnic()      const { return m_cnicInput->text();      }
    QString getEmpId()     const { return m_empIdInput->text();     }
    int     getRole()      const { return m_roleCombo->currentIndex(); }
    double  getSalary()    const { return m_salaryInput->value();   }
};

// --- Main Staff Window ---
class StaffManagementWindow : public QWidget {
    Q_OBJECT

private:
    QTableWidget*  m_table;
    QLabel*        m_totalLabel;
    AppController& m_app;

public:
    explicit StaffManagementWindow(QWidget* parent = nullptr)
        : QWidget(parent), m_app(AppController::getInstance()) {

        setWindowTitle("Staff Management");
        setMinimumSize(860, 560);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(15, 15, 15, 15);

        QLabel* title = new QLabel("Staff Management");
        title->setStyleSheet("font-size:18px; font-weight:bold; color:#1a1a2e;");
        QPushButton* backBtn = new QPushButton("< Back to Dashboard");
        backBtn->setStyleSheet("background:#444;color:white;padding:6px 12px;border-radius:4px;font-weight:bold;");
        connect(backBtn, &QPushButton::clicked, this, &QWidget::close);

        QHBoxLayout* topBar = new QHBoxLayout();
        topBar->addWidget(title);
        topBar->addStretch();
        topBar->addWidget(backBtn);
        layout->addLayout(topBar);

        // Static counter display
        m_totalLabel = new QLabel();
        m_totalLabel->setStyleSheet(
            "background:#e8f4f8; padding:8px; border-radius:4px;");
        layout->addWidget(m_totalLabel);

        // Toolbar
        QHBoxLayout* toolbar = new QHBoxLayout();
        QPushButton* addBtn      = new QPushButton("+ Add Staff");
        QPushButton* clockInBtn  = new QPushButton("Clock In");
        QPushButton* clockOutBtn = new QPushButton("Clock Out");
        QPushButton* refreshBtn  = new QPushButton("Refresh");

        addBtn->setStyleSheet(
            "background:#0f3460; color:white; padding:5px 12px; border-radius:4px;");
        clockInBtn->setStyleSheet(
            "background:#1b4332; color:white; padding:5px 12px; border-radius:4px;");
        clockOutBtn->setStyleSheet(
            "background:#c0392b; color:white; padding:5px 12px; border-radius:4px;");
        refreshBtn->setStyleSheet(
            "background:#444; color:white; padding:5px 12px; border-radius:4px;");

        toolbar->addWidget(addBtn);
        toolbar->addWidget(clockInBtn);
        toolbar->addWidget(clockOutBtn);
        toolbar->addWidget(refreshBtn);
        toolbar->addStretch();
        layout->addLayout(toolbar);

        // Table
        m_table = new QTableWidget();
        m_table->setColumnCount(6);
        m_table->setHorizontalHeaderLabels(
            {"ID", "Full Name", "Role", "Employee ID",
             "Salary (Rs)", "On Duty"});
        m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_table->setAlternatingRowColors(true);
        m_table->horizontalHeader()->setSectionsClickable(true);
        layout->addWidget(m_table);

        // License expiry warning
        QLabel* warning = new QLabel(
            "Important: check license expiry dates before assigning drivers.");
        warning->setStyleSheet("color:#c0392b; padding:4px;");
        layout->addWidget(warning);

        connect(addBtn,     &QPushButton::clicked, this, &StaffManagementWindow::onAddStaff);
        connect(clockInBtn, &QPushButton::clicked, this, &StaffManagementWindow::onClockIn);
        connect(clockOutBtn,&QPushButton::clicked, this, &StaffManagementWindow::onClockOut);
        connect(refreshBtn, &QPushButton::clicked, this, &StaffManagementWindow::refreshTable);

        refreshTable();
    }

private slots:
    void onAddStaff() {
        AddStaffDialog dlg(this);
        if (dlg.exec() == QDialog::Accepted) {
            try {
                QString fn  = dlg.getFirstName();
                QString ln  = dlg.getLastName();
                QString emp = dlg.getEmpId();

                if (fn.trimmed().isEmpty()) {
                    QMessageBox::warning(this, "Validation",
                                         "Please enter the driver's first name.");
                    return;
                }
                if (ln.trimmed().isEmpty()) {
                    QMessageBox::warning(this, "Validation",
                                         "Please enter the driver's last name.");
                    return;
                }
                if (emp.trimmed().isEmpty()) {
                    QMessageBox::warning(this, "Validation",
                                         "Please enter an employee ID, for example DRV-001.");
                    return;
                }

                StaffMember* s = nullptr;
                switch (dlg.getRole()) {
                    case 0: s = new Driver(
                        CustomString(fn.toStdString().c_str()),
                        CustomString(ln.toStdString().c_str()),
                        CustomString("42201-0000001-1"),
                        CustomDate(1,1,1990),
                        CustomString(emp.toStdString().c_str()),
                        CustomString("HTV"), dlg.getSalary()); break;
                    case 1: s = new Conductor(
                        CustomString(fn.toStdString().c_str()),
                        CustomString(ln.toStdString().c_str()),
                        CustomString("42201-0000002-2"),
                        CustomDate(1,1,1990),
                        CustomString(emp.toStdString().c_str()),
                        dlg.getSalary()); break;
                    default: s = new Dispatcher(
                        CustomString(fn.toStdString().c_str()),
                        CustomString(ln.toStdString().c_str()),
                        CustomString("42201-0000003-3"),
                        CustomDate(1,1,1990),
                        CustomString(emp.toStdString().c_str()),
                        CustomString("ZONE-A"), dlg.getSalary()); break;
                }

                m_app.getStaff().addStaff(s);
                refreshTable();
                QMessageBox::information(this, "Added",
                    QString("Staff member %1 %2 added!").arg(fn, ln));

            } catch (const TransitException& e) {
                QMessageBox::critical(this, "Error", e.getMessage().c_str());
            }
        }
    }

    void onClockIn() {
        int row = m_table->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Select Staff",
                                 "Please select a staff member first.");
            return;
        }
        try {
            StaffMember* staff = m_app.getStaff().getStaffAt(row);
            staff->clockIn();
            refreshTable();
            QMessageBox::information(this, "Clock In",
                                     "Selected staff member is now ON duty.");
        } catch (const TransitException& e) {
            QMessageBox::critical(this, "Error", e.getMessage().c_str());
        }
    }

    void onClockOut() {
        int row = m_table->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Select Staff",
                                 "Please select a staff member first.");
            return;
        }
        try {
            StaffMember* staff = m_app.getStaff().getStaffAt(row);
            staff->clockOut();
            refreshTable();
            QMessageBox::information(this, "Clock Out",
                                     "Selected staff member is now OFF duty.");
        } catch (const TransitException& e) {
            QMessageBox::critical(this, "Error", e.getMessage().c_str());
        }
    }

    void refreshTable() {
        // Update static counter label
        m_totalLabel->setText(
            QString("Total Staff (static counter): %1  |  "
                    "Use static method: StaffMember::getTotalStaff()")
            .arg(StaffMember::getTotalStaff()));

        int total = m_app.getStaff().getTotalStaff();
        m_table->setSortingEnabled(false);
        m_table->setRowCount(total);

        for (int i = 0; i < total; i++) {
            StaffMember* staff = m_app.getStaff().getStaffAt(i);
            m_table->setItem(i, 0, new QTableWidgetItem(QString::number(staff->getId())));
            m_table->setItem(i, 1, new QTableWidgetItem(staff->getFullName().c_str()));
            m_table->setItem(i, 2, new QTableWidgetItem(staff->getRoleString().c_str()));
            m_table->setItem(i, 3, new QTableWidgetItem(staff->getEmployeeId().c_str()));
            m_table->setItem(i, 4, new QTableWidgetItem(
                QString::number(staff->calculateMonthlySalary(), 'f', 0)));
            QTableWidgetItem* duty = new QTableWidgetItem(staff->isOnDuty() ? "ON" : "OFF");
            duty->setBackground(staff->isOnDuty() ? QColor("#d8f7df") : QColor("#ffdddd"));
            m_table->setItem(i, 5, duty);
        }
        m_table->setSortingEnabled(true);
    }
};
