#pragma once
// ============================================================
// FILE: Person.h
// MODULE: 3 - Entity Layer (LEVEL 2)
// OUTPUT-ID: M3-PER-001
// Expected Output: Full name, age, contact info
// OOP CONCEPT: Inheritance (extends Entity at Level 2)
//              CustomString operator+ used for full name
// ============================================================

#include "Entity.h"

class Person : public Entity {
protected:
    CustomString m_firstName;
    CustomString m_lastName;
    CustomString m_cnic;           // ID number
    CustomString m_contactNumber;
    CustomString m_email;
    CustomString m_address;
    CustomDate   m_dob;            // Date of birth

public:
    Person();
    Person(const CustomString& firstName, const CustomString& lastName,
           const CustomString& cnic, const CustomDate& dob);
    Person(const Person& other);
    virtual ~Person() = default;

    // Getters
    CustomString getFirstName()     const { return m_firstName;     }
    CustomString getLastName()      const { return m_lastName;      }
    CustomString getCnic()          const { return m_cnic;          }
    CustomString getContactNumber() const { return m_contactNumber; }
    CustomString getEmail()         const { return m_email;         }
    CustomDate   getDob()           const { return m_dob;           }

    // Setters
    void setContactNumber(const CustomString& contact) { m_contactNumber = contact; }
    void setEmail(const CustomString& email)           { m_email = email;           }
    void setAddress(const CustomString& address)       { m_address = address;       }

    // Methods
    // [OOP: CustomString operator+ used here for concatenation]
    CustomString getFullName() const;   // firstName + " " + lastName
    int          getAge()      const;   // today - dob in years

    // Less than by last name (alphabetic)
    bool operator<(const Person& other) const;

    // Override pure virtuals
    CustomString toString()      const override;
    CustomString getEntityType() const override { return CustomString("Person"); }
    void serialize(std::ofstream& out)   const override;
    void deserialize(std::ifstream& in)        override;
};
