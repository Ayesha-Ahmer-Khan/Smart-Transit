#pragma once
// ============================================================
// FILE: src/gui/LoginWindow.h
// MODULE: 7 - Qt GUI Layer
// OUTPUT-ID: M7-LOGIN-001
// COLLEAGUE'S FILE: Implement this
// Expected Output: Login window appears, accept admin/admin,
//                  opens MainDashboard on success
// OOP CONCEPTS: Qt signals/slots, Q_OBJECT macro
// ============================================================

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFrame>
#include <QComboBox>
#include "../controllers/AuthController.h"

// [OOP: Inherits from QWidget - Qt's base class for all windows]
class LoginWindow : public QWidget {
    Q_OBJECT   // [Qt: MUST have this - enables signals/slots]

private:
    QLabel*      m_titleLabel;
    QLabel*      m_subtitleLabel;
    QLabel*      m_usernameLabel;
    QLabel*      m_passwordLabel;
    QLabel*      m_roleLabel;
    QLineEdit*   m_usernameInput;
    QLineEdit*   m_passwordInput;
    QComboBox*   m_roleInput;
    QPushButton* m_loginButton;
    QPushButton* m_exitButton;

public:
    // Constructor: sets up the window
    explicit LoginWindow(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowTitle("SmartTransit EMS - Login");
        setFixedSize(520, 390);
        setStyleSheet("background-color: #1a1a2e;");

        // Title
        m_titleLabel = new QLabel("SmartTransit EMS");
        m_titleLabel->setAlignment(Qt::AlignCenter);
        m_titleLabel->setStyleSheet("color: #e94560; font-size: 24px; font-weight: bold;");

        m_subtitleLabel = new QLabel("ABC City Transit Authority");
        m_subtitleLabel->setAlignment(Qt::AlignCenter);
        m_subtitleLabel->setStyleSheet("color: #a0a0b0; font-size: 12px;");

        // Username input
        m_usernameLabel = new QLabel("Username:");
        m_usernameLabel->setStyleSheet("color: white;");
        m_usernameInput = new QLineEdit();
        m_usernameInput->setPlaceholderText("Enter username");
        m_usernameInput->setText("admin");  // Pre-fill for demo
        m_usernameInput->setMinimumHeight(30);
        m_usernameInput->setStyleSheet("background: #16213e; color: white; border: 1px solid #e94560; padding: 6px; border-radius: 4px;");

        // Password input
        m_passwordLabel = new QLabel("Password:");
        m_passwordLabel->setStyleSheet("color: white;");
        m_passwordInput = new QLineEdit();
        m_passwordInput->setPlaceholderText("Enter password");
        m_passwordInput->setEchoMode(QLineEdit::Password);  // [Qt: hides password]
        m_passwordInput->setText("admin");  // Pre-fill for demo
        m_passwordInput->setMinimumHeight(30);
        m_passwordInput->setStyleSheet("background: #16213e; color: white; border: 1px solid #e94560; padding: 6px; border-radius: 4px;");

        // Role selector required by specification
        m_roleLabel = new QLabel("Role:");
        m_roleLabel->setStyleSheet("color: white;");
        m_roleInput = new QComboBox();
        m_roleInput->addItems({"Admin", "Driver", "Dispatcher", "Maintenance"});
        m_roleInput->setMinimumHeight(32);
        m_roleInput->setStyleSheet("QComboBox { background: #16213e; color: white; border: 1px solid #e94560; padding: 6px; border-radius: 4px; } QComboBox QAbstractItemView { background: #16213e; color: white; selection-background-color: #e94560; }");

        // Buttons
        m_loginButton = new QPushButton("Login");
        m_loginButton->setMinimumHeight(36);
        m_loginButton->setStyleSheet("QPushButton { background: #e94560; color: white; padding: 8px; border-radius: 4px; font-weight: bold; } QPushButton:hover { background: #ff5572; }");

        m_exitButton = new QPushButton("Exit");
        m_exitButton->setMinimumHeight(36);
        m_exitButton->setStyleSheet("QPushButton { background: #444; color: white; padding: 8px; border-radius: 4px; font-weight: bold; } QPushButton:hover { background: #666; }");

        // Layout: arrange widgets vertically
        QVBoxLayout* mainLayout = new QVBoxLayout();
        mainLayout->setSpacing(10);
        mainLayout->setContentsMargins(50, 24, 50, 24);
        mainLayout->addWidget(m_titleLabel);
        mainLayout->addWidget(m_subtitleLabel);
        mainLayout->addSpacing(10);
        mainLayout->addWidget(m_usernameLabel);
        mainLayout->addWidget(m_usernameInput);
        mainLayout->addWidget(m_passwordLabel);
        mainLayout->addWidget(m_passwordInput);
        mainLayout->addWidget(m_roleLabel);
        mainLayout->addWidget(m_roleInput);

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(m_loginButton);
        buttonLayout->addWidget(m_exitButton);
        mainLayout->addLayout(buttonLayout);

        setLayout(mainLayout);

        // [Qt: Connect signals to slots]
        // When loginButton is clicked, call our onLogin() slot
        connect(m_loginButton, &QPushButton::clicked, this, &LoginWindow::onLogin);
        connect(m_exitButton,  &QPushButton::clicked, this, &QWidget::close);

        // Also allow pressing Enter to login
        connect(m_passwordInput, &QLineEdit::returnPressed, this, &LoginWindow::onLogin);
    }

// [Qt: slots = functions that respond to signals]
private slots:
    void onLogin() {
        QString username = m_usernameInput->text();
        QString password = m_passwordInput->text();

        // Validate input
        if (username.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "Login", "Please enter username and password.");
            return;
        }

        try {
            AuthController& auth = AuthController::getInstance();
            auth.login(CustomString(username.toStdString().c_str()),
                       CustomString(password.toStdString().c_str()));
            QString role = m_roleInput->currentText();
            emit loginSuccess(username, role);   // [Qt: emit signal - broadcast event]
            this->hide();
        } catch (const TransitException& e) {
            QMessageBox::critical(this, "Login Failed",
                QString("Authentication failed: %1\n\nHint: Use admin / admin").arg(e.getMessage().c_str()));
            m_passwordInput->clear();
            m_passwordInput->setFocus();
        }
    }

// [Qt: signals = events this window can broadcast]
signals:
    void loginSuccess(const QString& username, const QString& role);  // Emitted when login works
};
