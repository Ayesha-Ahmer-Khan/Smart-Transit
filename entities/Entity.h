#pragma once
// ============================================================
// FILE: Entity.h
// MODULE: 3 - Entity Layer (LEVEL 1 of hierarchy)
// OUTPUT-ID: M3-ENT-001
// Expected Output: Cannot instantiate - it's abstract.
//                  Subclasses will show output.
// OOP CONCEPTS:
//   - Abstract class (has pure virtual methods)
//   - Static member (s_nextId shared by all objects)
//   - Protected access (children can access)
//   - Virtual destructor (MUST for polymorphism)
// ============================================================

#include "../utilities/CustomString.h"
#include "../utilities/CustomDate.h"
#include <iostream>
#include <fstream>

class Entity {
protected:
    // [OOP: static member - ONE copy shared by ALL Entity objects]
    // Used to auto-generate unique IDs
    static int s_nextId;

    int        m_id;         // Unique identifier
    CustomDate m_createdOn;  // When was this object created?
    CustomDate m_updatedOn;  // When was it last changed?

public:
    // -----------------------------------------------------------
    // CONSTRUCTORS
    // [OOP: constructor chaining - each sets up its own part]
    // -----------------------------------------------------------
    Entity();                      // Auto-assigns next ID
    explicit Entity(int id);       // Set specific ID
    Entity(const Entity& other);   // Copy

    // [OOP: virtual destructor - ESSENTIAL for inheritance]
    // Without this, deleting a Vehicle* that points to CityBus
    // would only call Vehicle's destructor, not CityBus's!
    virtual ~Entity() = default;

    // -----------------------------------------------------------
    // PURE VIRTUAL METHODS
    // [OOP: These make Entity ABSTRACT - cannot create Entity objects]
    // Every subclass MUST implement these
    // -----------------------------------------------------------
    virtual CustomString toString()       const = 0;  // Text representation
    virtual CustomString getEntityType()  const = 0;  // "Vehicle", "Driver", etc.
    virtual void serialize(std::ofstream& out) const = 0; // Save to file
    virtual void deserialize(std::ifstream& in)      = 0; // Load from file

    // -----------------------------------------------------------
    // CONCRETE METHODS (these work the same for all subclasses)
    // -----------------------------------------------------------
    int        getId()          const { return m_id;        }
    CustomDate getCreatedOn()   const { return m_createdOn; }
    CustomDate getUpdatedOn()   const { return m_updatedOn; }
    void       markUpdated()          { m_updatedOn = CustomDate::today(); }

    // [OOP: Static method - can call without an object: Entity::getTotalCreated()]
    static int getTotalCreated() { return s_nextId - 1; }

    // -----------------------------------------------------------
    // OPERATORS
    // -----------------------------------------------------------
    bool operator==(const Entity& other) const;
    bool operator< (const Entity& other) const;

    // [OOP: Friend function - can access private members of Entity]
    friend std::ostream& operator<<(std::ostream& os, const Entity& e);
};
