#pragma once
// ============================================================
// FILE: IFareCalculator.h
// MODULE: 4 - Polymorphism Layer
// OUTPUT-ID: M4-FARE-001
// Expected Output: Calculated fare amounts for different types
// OOP CONCEPTS:
//   - Pure Abstract Interface (all methods = 0)
//   - Runtime Polymorphism: IFareCalculator* can be any fare type
//   - Strategy Pattern: swap fare strategy at runtime
// ============================================================

#include "../utilities/CustomString.h"
#include <cstdio>

// ============================================================
// ABSTRACT INTERFACE: IFareCalculator
// [OOP: Interface = class with ONLY pure virtual methods]
// You CANNOT create: IFareCalculator f;
// You CAN do: IFareCalculator* f = new FlatFareBus();
// ============================================================
class IFareCalculator {
public:
    virtual ~IFareCalculator() = default;

    // [OOP: All pure virtual = interface]
    virtual double       calculateFare(double distanceKm, int passengerCount,
                                       bool isPeakHour) const = 0;
    virtual double       applyDiscount(double baseFare, int loyaltyPoints) const = 0;
    virtual CustomString getFareType()    const = 0;
    virtual double       getMinimumFare() const = 0;
    virtual CustomString generateReceipt(double fare, const CustomString& ticketId) const = 0;
};

// ============================================================
// CONCRETE: FlatFareBus
// All bus journeys cost the same flat rate regardless of distance
// [OOP: Implements all pure virtual methods = can be instantiated]
// ============================================================
class FlatFareBus : public IFareCalculator {
private:
    static constexpr double BASE_FARE      = 30.0;   // Rs 30 flat
    static constexpr double PEAK_SURCHARGE = 10.0;   // Rs 10 extra at peak
    static constexpr double MIN_FARE       = 20.0;

public:
    // [OOP: override confirms we are implementing the interface]
    double calculateFare(double distanceKm, int passengerCount,
                         bool isPeakHour) const override {
        (void)distanceKm;     // Bus fare doesn't depend on distance
        (void)passengerCount; // Per-ticket pricing
        double fare = BASE_FARE;
        if (isPeakHour) fare += PEAK_SURCHARGE;
        return fare;
    }

    double applyDiscount(double baseFare, int loyaltyPoints) const override {
        // 1 point = Rs 0.10 discount, max 50% off
        double discount = loyaltyPoints * 0.10;
        double maxDiscount = baseFare * 0.50;
        if (discount > maxDiscount) discount = maxDiscount;
        return baseFare - discount;
    }

    CustomString getFareType()    const override { return CustomString("Flat Fare Bus"); }
    double       getMinimumFare() const override { return MIN_FARE; }

    CustomString generateReceipt(double fare, const CustomString& ticketId) const override {
        char buf[200];
        std::snprintf(buf, sizeof(buf),
                "=== BUS TICKET ===\nID: %s\nType: Flat Fare\nAmount: Rs %.2f\n==================",
                ticketId.c_str(), fare);
        return CustomString(buf);
    }
};

// ============================================================
// CONCRETE: ZoneBasedMetro
// Metro fare depends on how many zones you cross
// ============================================================
class ZoneBasedMetro : public IFareCalculator {
private:
    static constexpr double PRICE_PER_KM  = 4.0;    // Rs 4 per km
    static constexpr double MIN_FARE      = 25.0;
    static constexpr double PEAK_MULT     = 1.20;   // 20% more at peak

public:
    double calculateFare(double distanceKm, int passengerCount,
                         bool isPeakHour) const override {
        (void)passengerCount;
        double fare = distanceKm * PRICE_PER_KM;
        if (isPeakHour) fare *= PEAK_MULT;
        if (fare < MIN_FARE) fare = MIN_FARE;
        return fare;
    }

    double applyDiscount(double baseFare, int loyaltyPoints) const override {
        double discount = loyaltyPoints * 0.05;  // 5 paisa per point
        return baseFare - (discount > baseFare * 0.30 ? baseFare * 0.30 : discount);
    }

    CustomString getFareType()    const override { return CustomString("Zone Based Metro"); }
    double       getMinimumFare() const override { return MIN_FARE; }

    CustomString generateReceipt(double fare, const CustomString& ticketId) const override {
        char buf[200];
        std::snprintf(buf, sizeof(buf),
                "=== METRO TICKET ===\nID: %s\nType: Zone Based\nAmount: Rs %.2f\n====================",
                ticketId.c_str(), fare);
        return CustomString(buf);
    }
};

// ============================================================
// CONCRETE: SurgeRideHail
// Ride-hail pricing with surge multiplier
// ============================================================
class SurgeRideHail : public IFareCalculator {
private:
    double m_surgeMultiplier;
    static constexpr double BASE_PER_KM = 25.0;
    static constexpr double MIN_FARE    = 80.0;

public:
    SurgeRideHail(double surge = 1.0) : m_surgeMultiplier(surge) {}

    double calculateFare(double distanceKm, int passengerCount,
                         bool isPeakHour) const override {
        (void)passengerCount;
        double fare = distanceKm * BASE_PER_KM * m_surgeMultiplier;
        if (isPeakHour) fare *= 1.10;
        if (fare < MIN_FARE) fare = MIN_FARE;
        return fare;
    }

    double applyDiscount(double baseFare, int loyaltyPoints) const override {
        double discount = loyaltyPoints * 0.20;
        return baseFare - (discount > baseFare * 0.20 ? baseFare * 0.20 : discount);
    }

    CustomString getFareType()    const override { return CustomString("Surge Ride Hail"); }
    double       getMinimumFare() const override { return MIN_FARE; }

    CustomString generateReceipt(double fare, const CustomString& ticketId) const override {
        char buf[200];
        std::snprintf(buf, sizeof(buf),
                "=== RIDE RECEIPT ===\nID: %s\nSurge: %.1fx\nAmount: Rs %.2f\n====================",
                ticketId.c_str(), m_surgeMultiplier, fare);
        return CustomString(buf);
    }
};

// ============================================================
// CONCRETE: WeightFreight
// Freight pricing based on distance, payload and peak surcharge.
// ============================================================
class WeightFreight : public IFareCalculator {
private:
    static constexpr double BASE_PER_KM = 60.0;
    static constexpr double WEIGHT_RATE = 0.015; // Rs per kg per km
    static constexpr double MIN_FARE = 500.0;

public:
    double calculateFare(double distanceKm, int passengerCount,
                         bool isPeakHour) const override {
        double cargoKg = passengerCount > 0 ? passengerCount : 1000;
        double fare = distanceKm * BASE_PER_KM + distanceKm * cargoKg * WEIGHT_RATE;
        if (isPeakHour) fare *= 1.15;
        return fare < MIN_FARE ? MIN_FARE : fare;
    }

    double applyDiscount(double baseFare, int loyaltyPoints) const override {
        double discount = loyaltyPoints * 0.25;
        double maxDiscount = baseFare * 0.15;
        return baseFare - (discount > maxDiscount ? maxDiscount : discount);
    }

    CustomString getFareType() const override { return CustomString("Weight Freight"); }
    double getMinimumFare() const override { return MIN_FARE; }

    CustomString generateReceipt(double fare, const CustomString& ticketId) const override {
        char buf[220];
        std::snprintf(buf, sizeof(buf),
            "=== FREIGHT RECEIPT ===\nID: %s\nType: Weight Freight\nAmount: Rs %.2f\n=======================",
            ticketId.c_str(), fare);
        return CustomString(buf);
    }
};

// ============================================================
// CONCRETE: MicroMobility
// Scooter/bike fare based on short urban distance.
// ============================================================
class MicroMobility : public IFareCalculator {
private:
    static constexpr double UNLOCK_FEE = 20.0;
    static constexpr double PRICE_PER_KM = 8.0;
    static constexpr double MIN_FARE = 25.0;

public:
    double calculateFare(double distanceKm, int passengerCount,
                         bool isPeakHour) const override {
        (void)passengerCount;
        double fare = UNLOCK_FEE + distanceKm * PRICE_PER_KM;
        if (isPeakHour) fare += 5.0;
        return fare < MIN_FARE ? MIN_FARE : fare;
    }

    double applyDiscount(double baseFare, int loyaltyPoints) const override {
        double discount = loyaltyPoints * 0.05;
        double maxDiscount = baseFare * 0.25;
        return baseFare - (discount > maxDiscount ? maxDiscount : discount);
    }

    CustomString getFareType() const override { return CustomString("Micro Mobility"); }
    double getMinimumFare() const override { return MIN_FARE; }

    CustomString generateReceipt(double fare, const CustomString& ticketId) const override {
        char buf[220];
        std::snprintf(buf, sizeof(buf),
            "=== MICRO MOBILITY RECEIPT ===\nID: %s\nAmount: Rs %.2f\n==============================",
            ticketId.c_str(), fare);
        return CustomString(buf);
    }
};

// ============================================================
// ABSTRACT INTERFACE: IPaymentProcessor
// [OOP: Another interface - how payment is processed]
// ============================================================
class IPaymentProcessor {
public:
    virtual ~IPaymentProcessor() = default;

    virtual bool         processPayment(double amount)              = 0;
    virtual bool         refund(const CustomString& transactionId)  = 0;
    virtual CustomString getPaymentMethod()                   const = 0;
    virtual CustomString generateTransactionId()              const = 0;
    virtual bool         validateDetails()                    const = 0;
};

// ============================================================
// CONCRETE: CashProcessor
// ============================================================
class CashProcessor : public IPaymentProcessor {
private:
    double m_cashReceived;
    double m_changeGiven;

public:
    CashProcessor() : m_cashReceived(0.0), m_changeGiven(0.0) {}

    bool processPayment(double amount) override {
        // Cash always succeeds (assume exact change for simplicity)
        m_cashReceived = amount;
        m_changeGiven  = 0.0;
        return true;
    }

    bool refund(const CustomString& transactionId) override {
        (void)transactionId;
        // Cash refund: just return the money
        return true;
    }

    CustomString getPaymentMethod() const override {
        return CustomString("Cash");
    }

    CustomString generateTransactionId() const override {
        static int counter = 1000;
        char buf[20];
        std::snprintf(buf, sizeof(buf), "CASH-%d", counter++);
        return CustomString(buf);
    }

    bool validateDetails() const override { return true; }  // Cash always valid

    double getCashReceived() const { return m_cashReceived; }
};

// ============================================================
// CONCRETE: TransitCardProcessor
// ============================================================
class TransitCardProcessor : public IPaymentProcessor {
private:
    double m_cardBalance;
    CustomString m_cardNumber;

public:
    TransitCardProcessor(const CustomString& cardNum, double balance)
        : m_cardBalance(balance), m_cardNumber(cardNum) {}

    bool processPayment(double amount) override {
        if (m_cardBalance < amount) return false;  // Insufficient funds
        m_cardBalance -= amount;
        return true;
    }

    bool refund(const CustomString& transactionId) override {
        (void)transactionId;
        return true;
    }

    CustomString getPaymentMethod() const override {
        return CustomString("Transit Card");
    }

    CustomString generateTransactionId() const override {
        static int counter = 2000;
        char buf[30];
        std::snprintf(buf, sizeof(buf), "CARD-%s-%d", m_cardNumber.c_str(), counter++);
        return CustomString(buf);
    }

    bool validateDetails() const override {
        return m_cardBalance > 0 && m_cardNumber.length() > 0;
    }

    double getBalance() const { return m_cardBalance; }
    void topUp(double amount) { m_cardBalance += amount; }
};

// ============================================================
// CONCRETE: QRCodeProcessor
// Mobile QR payment processor.
// ============================================================
class QRCodeProcessor : public IPaymentProcessor {
private:
    CustomString m_walletId;

public:
    explicit QRCodeProcessor(const CustomString& walletId = CustomString("QR-WALLET"))
        : m_walletId(walletId) {}

    bool processPayment(double amount) override {
        return amount > 0.0 && validateDetails();
    }

    bool refund(const CustomString& transactionId) override {
        return transactionId.length() > 0;
    }

    CustomString getPaymentMethod() const override { return CustomString("QR Code"); }

    CustomString generateTransactionId() const override {
        static int counter = 3000;
        char buf[40];
        std::snprintf(buf, sizeof(buf), "QR-%s-%d", m_walletId.c_str(), counter++);
        return CustomString(buf);
    }

    bool validateDetails() const override { return m_walletId.length() > 0; }
};

// ============================================================
// CONCRETE: CorporateAccountProcessor
// Monthly billing for registered freight/company accounts.
// ============================================================
class CorporateAccountProcessor : public IPaymentProcessor {
private:
    CustomString m_accountCode;
    double m_creditLimit;
    double m_usedCredit;

public:
    CorporateAccountProcessor(const CustomString& accountCode = CustomString("CORP-001"),
                              double creditLimit = 500000.0)
        : m_accountCode(accountCode), m_creditLimit(creditLimit), m_usedCredit(0.0) {}

    bool processPayment(double amount) override {
        if (amount <= 0.0) return false;
        if (m_usedCredit + amount > m_creditLimit) return false;
        m_usedCredit += amount;
        return true;
    }

    bool refund(const CustomString& transactionId) override {
        (void)transactionId;
        return true;
    }

    CustomString getPaymentMethod() const override { return CustomString("Corporate Account"); }

    CustomString generateTransactionId() const override {
        static int counter = 4000;
        char buf[45];
        std::snprintf(buf, sizeof(buf), "CORP-%s-%d", m_accountCode.c_str(), counter++);
        return CustomString(buf);
    }

    bool validateDetails() const override { return m_accountCode.length() > 0 && m_creditLimit > 0.0; }
};

// ============================================================
// CONCRETE: LoyaltyPointsProcessor
// Fare payment using accumulated loyalty points.
// ============================================================
class LoyaltyPointsProcessor : public IPaymentProcessor {
private:
    int m_pointsBalance;
    static constexpr double POINT_VALUE = 0.10; // Rs per point

public:
    explicit LoyaltyPointsProcessor(int pointsBalance = 10000)
        : m_pointsBalance(pointsBalance) {}

    bool processPayment(double amount) override {
        int needed = static_cast<int>(amount / POINT_VALUE + 0.5);
        if (needed <= 0 || needed > m_pointsBalance) return false;
        m_pointsBalance -= needed;
        return true;
    }

    bool refund(const CustomString& transactionId) override {
        return transactionId.length() > 0;
    }

    CustomString getPaymentMethod() const override { return CustomString("Loyalty Points"); }

    CustomString generateTransactionId() const override {
        static int counter = 5000;
        char buf[30];
        std::snprintf(buf, sizeof(buf), "LOYALTY-%d", counter++);
        return CustomString(buf);
    }

    bool validateDetails() const override { return m_pointsBalance > 0; }

    int getPointsBalance() const { return m_pointsBalance; }
};
