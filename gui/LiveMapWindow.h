#pragma once
// ============================================================
// FILE: LiveMapWindow.h
// MODULE: 7 - Qt GUI (Window 3 of 12)
// OUTPUT-ID: M7-MAP-001
// COLLEAGUE'S FILE
// Expected Output: Schematic city map drawn with QPainter,
//                  stops as circles, routes as coloured lines,
//                  vehicle dots moving every 5 seconds
// OOP CONCEPT: Qt event system - paintEvent is called by Qt
//              whenever the widget needs to redraw itself
// ============================================================
#include <QWidget>
#include <QDateTime>
#include <QPainterPath>
#include <QPainter>
#include <QLinearGradient>
#include <QTime>
#include <QTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <cmath>

// Represents a vehicle dot on the map
struct MapVehicle {
    int    id;
    double x, y;       // Pixel coordinates on map
    double lat, lon;   // GPS coordinates
    QString type;      // "Bus", "Metro", "Car"
    QColor  color;
    double dx, dy;     // Movement delta per refresh
};

// Represents a stop circle on the map
struct MapStop {
    double x, y;
    QString name;
};

class LiveMapWindow : public QWidget {
    Q_OBJECT

private:
    QTimer*             m_refreshTimer; // [Qt: fires every 5000ms = 5 seconds]
    QLabel*             m_statusLabel;
    QLabel*             m_vehicleCountLabel;
    QList<MapVehicle>   m_vehicles;
    QList<MapStop>      m_stops;
    int                 m_selectedVehicleId;

    // Map bounds (Karachi area)
    double m_minLat = 24.75, m_maxLat = 24.98;
    double m_minLon = 66.95, m_maxLon = 67.22;

public:
    explicit LiveMapWindow(QWidget* parent = nullptr)
        : QWidget(parent), m_selectedVehicleId(-1) {

        setWindowTitle("Live Operations Map - SmartTransit");
        setMinimumSize(900, 620);

        // Status bar at bottom
        m_statusLabel       = new QLabel("Click a vehicle dot to see its details.");
        m_vehicleCountLabel = new QLabel("Vehicles: 0");
        m_statusLabel->setStyleSheet("color:#444;");

        QHBoxLayout* statusRow = new QHBoxLayout();
        statusRow->addWidget(m_vehicleCountLabel);
        statusRow->addStretch();
        statusRow->addWidget(m_statusLabel);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0,0,0,0);

        QPushButton* backBtn = new QPushButton("< Back to Dashboard");
        backBtn->setStyleSheet("background:#444;color:white;padding:6px 12px;border-radius:4px;font-weight:bold;");
        connect(backBtn, &QPushButton::clicked, this, &QWidget::close);

        QLabel* mapTitle = new QLabel("Live Operations Map");
        mapTitle->setStyleSheet("font-size:18px;font-weight:bold;color:#102033;background:rgba(255,255,255,210);padding:8px 12px;border-radius:6px;");

        QHBoxLayout* topBar = new QHBoxLayout();
        topBar->setContentsMargins(8,6,8,0);
        topBar->addWidget(mapTitle);
        topBar->addStretch();
        topBar->addWidget(backBtn);
        layout->addLayout(topBar);

        layout->addStretch(); // Map fills top part
        layout->addLayout(statusRow);

        // Load demo stops (using real Karachi coordinates)
        m_stops << MapStop{0,0,"Karachi Central"};
        m_stops << MapStop{0,0,"Saddar"};
        m_stops << MapStop{0,0,"Clifton"};
        m_stops << MapStop{0,0,"Gulshan"};
        m_stops << MapStop{0,0,"Airport"};
        // Positions set in paintEvent based on widget size

        // Load demo vehicles
        m_vehicles << MapVehicle{1, 0.3,0.4, 24.85,67.01, "Bus",   Qt::blue,    0.002, 0.001};
        m_vehicles << MapVehicle{2, 0.5,0.3, 24.90,67.05, "Metro", Qt::darkGreen,0.001,-0.001};
        m_vehicles << MapVehicle{3, 0.7,0.6, 24.82,67.12, "Car",   Qt::red,    -0.002, 0.002};
        m_vehicles << MapVehicle{4, 0.2,0.7, 24.88,66.98, "Truck", Qt::darkMagenta,0.003,0.0};

        m_vehicleCountLabel->setText(QString("Vehicles: %1").arg(m_vehicles.size()));

        // [Qt: QTimer - update vehicle positions every 5 seconds]
        m_refreshTimer = new QTimer(this);
        connect(m_refreshTimer, &QTimer::timeout, this, &LiveMapWindow::updateVehiclePositions);
        m_refreshTimer->start(5000); // 5000ms = 5 seconds
    }

protected:
    // [Qt: paintEvent is called by Qt automatically when the widget needs redrawing]
    // We override it to draw our custom map
    void paintEvent(QPaintEvent* /*event*/) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);  // Smooth edges

        int W = width();
        int H = height() - 42;  // Leave space for status bar

        // --- BACKGROUND: soft city map colours ---
        QLinearGradient sky(0, 0, W, H);
        sky.setColorAt(0.0, QColor("#f7fbff"));
        sky.setColorAt(0.55, QColor("#eef7f4"));
        sky.setColorAt(1.0, QColor("#f8fafc"));
        painter.fillRect(0, 0, W, H, sky);

        // Draw quiet city districts.
        painter.setPen(Qt::NoPen);
        for (int gx = 0; gx < 8; gx++) {
            for (int gy = 0; gy < 6; gy++) {
                QColor block = ((gx + gy) % 2 == 0) ? QColor("#e8eef3") : QColor("#ddeee8");
                painter.setBrush(block);
                painter.drawRoundedRect(gx*(W/8)+9, gy*(H/6)+12, W/8-18, H/6-22, 8, 8);
            }
        }

        // Draw water/park zones for real texture.
        painter.setBrush(QColor(184, 219, 231, 170));
        painter.drawRoundedRect(int(0.03*W), int(0.68*H), int(0.25*W), int(0.22*H), 18, 18);
        painter.setBrush(QColor(190, 224, 197, 170));
        painter.drawRoundedRect(int(0.66*W), int(0.10*H), int(0.24*W), int(0.20*H), 18, 18);

        // Main roads.
        painter.setPen(QPen(QColor("#ffffff"), 13, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawLine(int(0.05*W), int(0.22*H), int(0.94*W), int(0.34*H));
        painter.drawLine(int(0.12*W), int(0.82*H), int(0.92*W), int(0.45*H));
        painter.drawLine(int(0.43*W), int(0.08*H), int(0.55*W), int(0.92*H));
        painter.setPen(QPen(QColor("#c8d4df"), 2, Qt::DashLine, Qt::RoundCap));
        painter.drawLine(int(0.05*W), int(0.22*H), int(0.94*W), int(0.34*H));
        painter.drawLine(int(0.12*W), int(0.82*H), int(0.92*W), int(0.45*H));
        painter.drawLine(int(0.43*W), int(0.08*H), int(0.55*W), int(0.92*H));

        // --- DRAW ROUTES: coloured lines connecting stops ---
        QPen routePen;
        routePen.setWidth(8);
        routePen.setCapStyle(Qt::RoundCap);
        routePen.setJoinStyle(Qt::RoundJoin);

        // Route 1: Blue bus route
        routePen.setColor(QColor(28,113,216,190));
        painter.setPen(routePen);
        painter.drawLine(int(0.15*W),int(0.5*H), int(0.35*W),int(0.4*H));
        painter.drawLine(int(0.35*W),int(0.4*H), int(0.55*W),int(0.6*H));

        // Route 2: Green metro route
        routePen.setColor(QColor(31,147,109,190));
        painter.setPen(routePen);
        painter.drawLine(int(0.2*W),int(0.3*H), int(0.5*W),int(0.3*H));
        painter.drawLine(int(0.5*W),int(0.3*H), int(0.75*W),int(0.5*H));

        // --- DRAW STOPS: circles with names ---
        QList<QPair<double,double>> stopPositions = {
            {0.15,0.50},{0.35,0.40},{0.55,0.60},{0.20,0.30},{0.75,0.50}
        };
        painter.setFont(QFont("Arial",8,QFont::Bold));
        int si = 0;
        for (const auto& pos : stopPositions) {
            int sx = int(pos.first  * W);
            int sy = int(pos.second * H);
            // White station node with dark border
            painter.setPen(QPen(QColor("#243447"), 2));
            painter.setBrush(Qt::white);
            painter.drawEllipse(sx-9, sy-9, 18, 18);
            // Stop name
            painter.setPen(QColor("#102033"));
            if (si < m_stops.size())
                painter.drawText(sx+10, sy+4, m_stops[si].name);
            si++;
        }

        // --- DRAW VEHICLES: coloured dots ---
        painter.setFont(QFont("Arial",9,QFont::Bold));
        for (const auto& v : m_vehicles) {
            int vx = int(v.x * W);
            int vy = int(v.y * H);

            // Glow effect for selected vehicle
            if (v.id == m_selectedVehicleId) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(255,255,0,80));
                painter.drawEllipse(vx-15, vy-15, 30, 30);
            }

            // Vehicle dot
            painter.setPen(QPen(Qt::white,3));
            painter.setBrush(v.color);
            painter.drawEllipse(vx-10, vy-10, 20, 20);

            // Type label
            painter.setPen(v.color.darker(150));
            painter.drawText(vx+10, vy+4, v.type);
        }

        // --- LEGEND ---
        painter.setPen(QPen(QColor("#d9e2ec"), 1));
        painter.setBrush(QColor(255,255,255,225));
        painter.drawRoundedRect(W-170, 14, 154, 108, 8, 8);
        painter.setPen(QColor("#102033"));
        painter.setFont(QFont("Arial",8));
        painter.drawText(W-158,32, "LIVE LEGEND");
        drawLegendItem(painter, W-158,50, Qt::blue,     "City Bus");
        drawLegendItem(painter, W-158,68, Qt::darkGreen,"Metro Train");
        drawLegendItem(painter, W-158,86, Qt::red,      "Ride Hail");
        drawLegendItem(painter, W-158,104, Qt::darkMagenta,"Freight");
    }

    void drawLegendItem(QPainter& p, int x, int y, QColor c, const QString& label) {
        p.setBrush(c); p.setPen(Qt::NoPen);
        p.drawEllipse(x,y-6,10,10);
        p.setPen(Qt::black);
        p.drawText(x+14, y+3, label);
    }

    // [Qt: mousePressEvent - detect click on vehicle]
    void mousePressEvent(QMouseEvent* event) override {
        int W = width(), H = height()-30;
        for (const auto& v : m_vehicles) {
            int vx = int(v.x*W), vy = int(v.y*H);
            // Check if click is within 12 pixels of vehicle dot
            int dx = event->pos().x()-vx;
            int dy = event->pos().y()-vy;
            if (sqrt(dx*dx+dy*dy) < 12) {
                m_selectedVehicleId = v.id;
                m_statusLabel->setText(QString("Selected: %1 Vehicle #%2 | GPS: (%.4f, %.4f)")
                    .arg(v.type).arg(v.id).arg(v.lat).arg(v.lon));
                update(); // Trigger repaint
                return;
            }
        }
        m_selectedVehicleId = -1;
        m_statusLabel->setText("Click a vehicle dot to see its details.");
        update();
    }

private slots:
    // Called every 5 seconds by QTimer to simulate vehicle movement
    void updateVehiclePositions() {
        for (auto& v : m_vehicles) {
            v.x += v.dx * 0.1;
            v.y += v.dy * 0.1;
            // Bounce off edges
            if (v.x < 0.05 || v.x > 0.95) v.dx = -v.dx;
            if (v.y < 0.05 || v.y > 0.90) v.dy = -v.dy;
        }
        update(); // [Qt: triggers paintEvent to redraw the widget]
        m_statusLabel->setText(QString("Last updated: %1 (auto-refresh every 5s)")
                               .arg(QTime::currentTime().toString("HH:mm:ss")));
    }
};
// ============================================================
// StaffManagementWindow
// Window 5 of 12
// OUTPUT-ID: M7-STAFF-001
