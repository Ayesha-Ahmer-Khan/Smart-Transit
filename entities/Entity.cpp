// ============================================================
// FILE: Entity.cpp
// MODULE: 3 - Entity Layer
// ============================================================

#include "Entity.h"

// [OOP: Define static member OUTSIDE the class]
// This is the ONE shared counter for all Entity objects
int Entity::s_nextId = 1;

// Default constructor: auto-assign next ID
Entity::Entity()
    : m_id(s_nextId++),           // use current ID, then increment
      m_createdOn(CustomDate::today()),
      m_updatedOn(CustomDate::today()) {
}

// Constructor with specific ID (for loading from file)
Entity::Entity(int id)
    : m_id(id),
      m_createdOn(CustomDate::today()),
      m_updatedOn(CustomDate::today()) {
}

// Copy constructor: copies ID and dates but doesn't increment counter
Entity::Entity(const Entity& other)
    : m_id(other.m_id),
      m_createdOn(other.m_createdOn),
      m_updatedOn(other.m_updatedOn) {
}

// Equality: two entities are equal if they have the same ID
bool Entity::operator==(const Entity& other) const {
    return m_id == other.m_id;
}

// Less than: compare by ID (for sorting)
bool Entity::operator<(const Entity& other) const {
    return m_id < other.m_id;
}

// Stream output: calls toString() which is polymorphic!
// [OOP: Polymorphism in action - prints the RIGHT thing based on actual type]
std::ostream& operator<<(std::ostream& os, const Entity& e) {
    os << e.toString().c_str();
    return os;
}
