// ============================================================
// FILE: Person.cpp
// ============================================================

#include "Person.h"
#include <cstdio>

Person::Person()
    : Entity(),
      m_firstName("Unknown"), m_lastName("Unknown"),
      m_cnic("00000-0000000-0"),
      m_contactNumber(""), m_email(""), m_address(""),
      m_dob(1, 1, 1990) {
}

Person::Person(const CustomString& firstName, const CustomString& lastName,
               const CustomString& cnic, const CustomDate& dob)
    : Entity(),
      m_firstName(firstName), m_lastName(lastName),
      m_cnic(cnic), m_dob(dob) {
}

Person::Person(const Person& other)
    : Entity(other),
      m_firstName(other.m_firstName),
      m_lastName(other.m_lastName),
      m_cnic(other.m_cnic),
      m_contactNumber(other.m_contactNumber),
      m_email(other.m_email),
      m_address(other.m_address),
      m_dob(other.m_dob) {
}

// [OOP: Uses CustomString operator+ for concatenation]
CustomString Person::getFullName() const {
    return m_firstName + CustomString(" ") + m_lastName;
}

int Person::getAge() const {
    CustomDate today = CustomDate::today();
    int daysDiff     = today - m_dob;   // Uses CustomDate::operator-
    return daysDiff / 365;              // Approximate years
}

bool Person::operator<(const Person& other) const {
    // Alphabetic order by last name
    return m_lastName < other.m_lastName;  // Uses CustomString::operator<
}

CustomString Person::toString() const {
    char buf[256];
    sprintf(buf, "Person[%d] %s | CNIC: %s | Age: %d",
            m_id, getFullName().c_str(), m_cnic.c_str(), getAge());
    return CustomString(buf);
}

void Person::serialize(std::ofstream& out) const {
    out << m_id << "\n";
    out << m_firstName.c_str() << "\n";
    out << m_lastName.c_str()  << "\n";
    out << m_cnic.c_str()      << "\n";
}

void Person::deserialize(std::ifstream& in) {
    std::string fn, ln, cnic;
    in >> m_id >> fn >> ln >> cnic;
    m_firstName = CustomString(fn.c_str());
    m_lastName  = CustomString(ln.c_str());
    m_cnic      = CustomString(cnic.c_str());
}
