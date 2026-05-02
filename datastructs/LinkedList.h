#pragma once
// ============================================================
// FILE: LinkedList.h
// MODULE: 2 - Custom Data Structures
// OUTPUT-ID: M2-LL-001
// Expected Output: List operations - add, remove, traverse
// OOP CONCEPTS: Templates, nested class (Node), linked pointers
// ============================================================

#include <iostream>
#include "../exceptions/TransitExceptions.h"

template <typename T>
class LinkedList {
private:
    // -----------------------------------------------------------
    // NESTED CLASS: Node
    // [OOP: A class inside a class = nested class]
    // -----------------------------------------------------------
    struct Node {
        T     data;   // The actual value stored
        Node* next;   // Pointer to next node

        // Node constructor
        Node(const T& val) : data(val), next(nullptr) {}
    };

    Node* m_head;  // Pointer to first node
    Node* m_tail;  // Pointer to last node (for fast append)
    int   m_size;

public:
    // -----------------------------------------------------------
    // CONSTRUCTORS & DESTRUCTOR
    // -----------------------------------------------------------
    LinkedList() : m_head(nullptr), m_tail(nullptr), m_size(0) {}

    // Copy constructor
    LinkedList(const LinkedList& other) : m_head(nullptr), m_tail(nullptr), m_size(0) {
        Node* current = other.m_head;
        while (current != nullptr) {
            append(current->data);
            current = current->next;
        }
    }

    // Copy assignment
    LinkedList& operator=(const LinkedList& other) {
        if (this == &other) return *this;
        clear();
        Node* current = other.m_head;
        while (current != nullptr) {
            append(current->data);
            current = current->next;
        }
        return *this;
    }

    // Destructor - MUST free all nodes
    ~LinkedList() {
        clear();
    }

    // -----------------------------------------------------------
    // CORE OPERATIONS
    // -----------------------------------------------------------

    // Add to end
    void append(const T& value) {
        Node* newNode = new Node(value);
        if (m_tail == nullptr) {
            m_head = m_tail = newNode;
        } else {
            m_tail->next = newNode;
            m_tail       = newNode;
        }
        m_size++;
    }

    // Add to front
    void prepend(const T& value) {
        Node* newNode = new Node(value);
        newNode->next = m_head;
        m_head        = newNode;
        if (m_tail == nullptr) m_tail = m_head;
        m_size++;
    }

    // Remove first occurrence of value
    void remove(const T& value) {
        Node* current  = m_head;
        Node* previous = nullptr;

        while (current != nullptr) {
            if (current->data == value) {
                if (previous == nullptr) {
                    m_head = current->next;  // remove head
                } else {
                    previous->next = current->next;
                }
                if (current == m_tail) m_tail = previous;
                delete current;
                m_size--;
                return;
            }
            previous = current;
            current  = current->next;
        }
    }

    // Get by index
    T& get(int index) {
        if (index < 0 || index >= m_size) {
            throw IndexOutOfBoundsException("LinkedList::get");
        }
        Node* current = m_head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->data;
    }

    const T& get(int index) const {
        if (index < 0 || index >= m_size) {
            throw IndexOutOfBoundsException("LinkedList::get const");
        }
        Node* current = m_head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->data;
    }

    bool contains(const T& value) const {
        Node* current = m_head;
        while (current != nullptr) {
            if (current->data == value) return true;
            current = current->next;
        }
        return false;
    }

    // Free all nodes
    void clear() {
        Node* current = m_head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        m_head = m_tail = nullptr;
        m_size = 0;
    }

    int  size()    const { return m_size;   }
    bool isEmpty() const { return m_size == 0; }

    // -----------------------------------------------------------
    // OPERATORS
    // -----------------------------------------------------------
    T& operator[](int index) { return get(index); }
    const T& operator[](int index) const { return get(index); }

    // Concatenate two lists (returns new list)
    LinkedList operator+(const LinkedList& other) const {
        LinkedList result(*this);
        Node* current = other.m_head;
        while (current != nullptr) {
            result.append(current->data);
            current = current->next;
        }
        return result;
    }

    bool operator==(const LinkedList& other) const {
        if (m_size != other.m_size) return false;
        Node* a = m_head;
        Node* b = other.m_head;
        while (a != nullptr) {
            if (!(a->data == b->data)) return false;
            a = a->next;
            b = b->next;
        }
        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const LinkedList<T>& list) {
        os << "{";
        Node* current = list.m_head;
        while (current != nullptr) {
            os << current->data;
            if (current->next) os << " -> ";
            current = current->next;
        }
        os << "}";
        return os;
    }
};
