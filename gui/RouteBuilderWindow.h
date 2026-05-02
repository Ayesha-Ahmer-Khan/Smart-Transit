#pragma once
// ============================================================
// FILE: src/gui/RouteBuilderWindow.h
// MODULE: 7 - Qt GUI | Window 6 of 12
// ============================================================
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QMessageBox>
#include "../controllers/ExtendedControllers.h"

class RouteBuilderWindow : public QWidget {
    Q_OBJECT
private:
    QListWidget*    m_stopList;
    QLabel*         m_distLabel;
    QLineEdit*      m_stopNameInput;
    QDoubleSpinBox* m_latInput;
    QDoubleSpinBox* m_lonInput;
    double          m_totalDist;
    double          m_lastLat, m_lastLon;
    bool            m_hasLast;
    RouteController m_routeCtrl;

public:
    explicit RouteBuilderWindow(QWidget* parent = nullptr)
        : QWidget(parent), m_totalDist(0.0),
          m_lastLat(0), m_lastLon(0), m_hasLast(false) {

        setWindowTitle("Route Builder");
        setMinimumSize(760, 500);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(15,15,15,15);

        QLabel* title = new QLabel("Route Builder");
        title->setStyleSheet("font-size:18px;font-weight:bold;color:#1a1a2e;");
        QPushButton* backBtn = new QPushButton("< Back to Dashboard");
        backBtn->setStyleSheet("background:#444;color:white;padding:6px 12px;border-radius:4px;font-weight:bold;");
        connect(backBtn, &QPushButton::clicked, this, &QWidget::close);

        QHBoxLayout* topBar = new QHBoxLayout();
        topBar->addWidget(title);
        topBar->addStretch();
        topBar->addWidget(backBtn);
        layout->addLayout(topBar);

        // Add stop form
        QGroupBox* addBox = new QGroupBox("Add Stop to Route");
        QHBoxLayout* addRow = new QHBoxLayout(addBox);

        m_stopNameInput = new QLineEdit();
        m_stopNameInput->setPlaceholderText("Stop Name (e.g. Saddar)");

        m_latInput = new QDoubleSpinBox();
        m_latInput->setRange(-90, 90);
        m_latInput->setDecimals(4);
        m_latInput->setValue(24.8607);
        m_latInput->setPrefix("Lat: ");

        m_lonInput = new QDoubleSpinBox();
        m_lonInput->setRange(-180, 180);
        m_lonInput->setDecimals(4);
        m_lonInput->setValue(67.0011);
        m_lonInput->setPrefix("Lon: ");

        QPushButton* addBtn = new QPushButton("+ Add Stop");
        addBtn->setStyleSheet(
            "background:#0f3460;color:white;padding:5px 12px;border-radius:4px;");

        addRow->addWidget(m_stopNameInput);
        addRow->addWidget(m_latInput);
        addRow->addWidget(m_lonInput);
        addRow->addWidget(addBtn);
        layout->addWidget(addBox);

        // Stop list
        QLabel* listTitle = new QLabel("Route Stops (in order):");
        listTitle->setStyleSheet("font-weight:bold;");
        layout->addWidget(listTitle);

        m_stopList = new QListWidget();
        m_stopList->setStyleSheet(
            "QListWidget::item { padding:6px; border-bottom:1px solid #eee; }");
        layout->addWidget(m_stopList);

        m_distLabel = new QLabel("Total route distance: 0.00 km");
        m_distLabel->setStyleSheet(
            "font-weight:bold;background:#e8f4f8;padding:8px;border-radius:4px;");
        layout->addWidget(m_distLabel);

        QHBoxLayout* ctrlRow = new QHBoxLayout();
        QPushButton* removeBtn = new QPushButton("Remove Selected");
        QPushButton* clearBtn  = new QPushButton("Clear All");
        QPushButton* saveBtn   = new QPushButton("Save Route");
        removeBtn->setStyleSheet("background:#c0392b;color:white;padding:5px 10px;border-radius:4px;");
        clearBtn->setStyleSheet ("background:#888;color:white;padding:5px 10px;border-radius:4px;");
        saveBtn->setStyleSheet  ("background:#1b4332;color:white;padding:5px 10px;border-radius:4px;");
        ctrlRow->addWidget(removeBtn);
        ctrlRow->addWidget(clearBtn);
        ctrlRow->addStretch();
        ctrlRow->addWidget(saveBtn);
        layout->addLayout(ctrlRow);

        connect(addBtn,    &QPushButton::clicked, this, &RouteBuilderWindow::onAddStop);
        connect(removeBtn, &QPushButton::clicked, this, &RouteBuilderWindow::onRemoveStop);
        connect(clearBtn,  &QPushButton::clicked, this, &RouteBuilderWindow::onClear);
        connect(saveBtn,   &QPushButton::clicked, this, &RouteBuilderWindow::onSave);
    }

private slots:
    void onAddStop() {
        QString name = m_stopNameInput->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(this, "Input", "Please enter a stop name."); return;
        }
        double lat = m_latInput->value();
        double lon = m_lonInput->value();

        // [OOP: GeoCoordinate::distanceTo = Haversine formula]
        if (m_hasLast) {
            GeoCoordinate prev(m_lastLat, m_lastLon);
            GeoCoordinate curr(lat, lon);
            double dist = prev.distanceTo(curr);
            m_totalDist += dist;
            m_stopList->addItem(
                QString("  → %1  [%2, %3]  +%4 km")
                .arg(name).arg(lat,0,'f',4).arg(lon,0,'f',4).arg(dist,0,'f',2));
        } else {
            m_stopList->addItem(
                QString("  ★ %1  [%2, %3]  (start)").arg(name).arg(lat,0,'f',4).arg(lon,0,'f',4));
        }
        m_lastLat = lat; m_lastLon = lon; m_hasLast = true;
        m_distLabel->setText(
            QString("Total route distance: %1 km  |  Stops: %2")
            .arg(m_totalDist,0,'f',2).arg(m_stopList->count()));
        m_stopNameInput->clear();
    }

    void onRemoveStop() {
        if (m_stopList->currentRow() < 0) {
            QMessageBox::warning(this,"Select","Select a stop first."); return;
        }
        delete m_stopList->takeItem(m_stopList->currentRow());
    }

    void onClear() {
        m_stopList->clear();
        m_totalDist = 0; m_hasLast = false;
        m_distLabel->setText("Total route distance: 0.00 km");
    }

    void onSave() {
        if (m_stopList->count() < 2) {
            QMessageBox::warning(this,"Route","Need at least 2 stops!"); return;
        }
        QMessageBox::information(this, "Route Saved",
            QString("Route saved!\n%1 stops | Total: %2 km")
            .arg(m_stopList->count()).arg(m_totalDist,0,'f',2));
    }
};
