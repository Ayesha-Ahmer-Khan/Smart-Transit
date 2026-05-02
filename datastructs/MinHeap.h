#pragma once
// ============================================================
// FILE: MinHeap.h
// MODULE: 2 - Custom Data Structures
// OUTPUT-ID: M2-HEAP-001
// Expected Output: Smallest element always at top, extracted first
// OOP CONCEPTS: Templates, array-based heap, heapify operations
// WHERE USED: DispatchController - vehicles dispatched by priority
//             Dijkstra algorithm in RouteController
// ============================================================
// HOW A HEAP WORKS:
//   Imagine a tree stored as an array.
//   Parent at index i → Children at 2i+1 and 2i+2
//   In MinHeap: parent is ALWAYS smaller than its children.
//   So m_data[0] is ALWAYS the smallest element.
//   Insert: add to end, "bubble up" until heap property restored.
//   Extract: remove root, put last element at root, "sink down".
// ============================================================

#include <iostream>
#include "../exceptions/TransitExceptions.h"

template <typename T>
class MinHeap {
private:
    T*  m_data;      // Array storing heap elements
    int m_size;
    int m_capacity;

    // --- HEAP HELPER FUNCTIONS ---

    // Get parent/child indices
    int parent(int i)    const { return (i - 1) / 2; }
    int leftChild(int i) const { return 2 * i + 1;   }
    int rightChild(int i)const { return 2 * i + 2;   }

    // Swap two elements
    void swapElements(int i, int j) {
        T temp    = m_data[i];
        m_data[i] = m_data[j];
        m_data[j] = temp;
    }

    // After INSERT: bubble the last element UP until heap property holds
    // [Uses T::operator< to compare]
    void heapifyUp(int index) {
        while (index > 0 && m_data[index] < m_data[parent(index)]) {
            swapElements(index, parent(index));
            index = parent(index);
        }
    }

    // After EXTRACT: sink the root element DOWN until heap property holds
    void heapifyDown(int index) {
        int smallest = index;
        int left     = leftChild(index);
        int right    = rightChild(index);

        // Find smallest among current, left child, right child
        if (left < m_size && m_data[left] < m_data[smallest])
            smallest = left;
        if (right < m_size && m_data[right] < m_data[smallest])
            smallest = right;

        // If smallest isn't current, swap and recurse
        if (smallest != index) {
            swapElements(index, smallest);
            heapifyDown(smallest);
        }
    }

    void resize() {
        m_capacity *= 2;
        T* newData   = new T[m_capacity];
        for (int i = 0; i < m_size; i++) newData[i] = m_data[i];
        delete[] m_data;
        m_data = newData;
    }

public:
    MinHeap() : m_size(0), m_capacity(8) {
        m_data = new T[m_capacity];
    }

    MinHeap(const MinHeap& other)
        : m_size(other.m_size), m_capacity(other.m_capacity) {
        m_data = new T[m_capacity];
        for (int i = 0; i < m_size; i++) m_data[i] = other.m_data[i];
    }

    ~MinHeap() { delete[] m_data; }

    // INSERT: O(log n)
    // Add element and bubble it up to correct position
    void insert(const T& value) {
        if (m_size >= m_capacity) resize();
        m_data[m_size] = value;  // Put at end
        heapifyUp(m_size);       // Bubble up
        m_size++;
    }

    // PEEK: O(1) - look at minimum without removing
    const T& getMin() const {
        if (m_size == 0) throw IndexOutOfBoundsException("MinHeap::getMin");
        return m_data[0];  // Root is always minimum!
    }

    // EXTRACT MINIMUM: O(log n)
    // Remove and return the smallest element
    T extractMin() {
        if (m_size == 0) throw IndexOutOfBoundsException("MinHeap::extractMin");
        T minValue   = m_data[0];      // Save the minimum
        m_data[0]    = m_data[m_size - 1]; // Move last to root
        m_size--;
        if (m_size > 0) heapifyDown(0); // Sink root down
        return minValue;
    }

    // DECREASE KEY: Used in Dijkstra algorithm
    // Update a value and fix heap property
    void decreaseKey(int index, const T& newValue) {
        if (index < 0 || index >= m_size) {
            throw IndexOutOfBoundsException("MinHeap::decreaseKey");
        }
        m_data[index] = newValue;
        heapifyUp(index);
    }

    bool isEmpty() const { return m_size == 0;  }
    int  size()    const { return m_size;        }
    void clear()         { m_size = 0;           }

    friend std::ostream& operator<<(std::ostream& os, const MinHeap<T>& h) {
        os << "MinHeap[";
        for (int i = 0; i < h.m_size; i++) {
            os << h.m_data[i];
            if (i < h.m_size - 1) os << ", ";
        }
        os << "]";
        return os;
    }
};

// ============================================================
// DispatchEntry: used in the dispatch priority queue
// Lower priority number = dispatched first
// ============================================================
struct DispatchEntry {
    int    vehicleId;
    int    priority;     // Lower = more urgent
    double distanceKm;   // Distance to incident location

    DispatchEntry() : vehicleId(-1), priority(99), distanceKm(0.0) {}
    DispatchEntry(int vid, int p, double dist)
        : vehicleId(vid), priority(p), distanceKm(dist) {}

    // [OOP: operator< required by MinHeap for heapify comparisons]
    bool operator<(const DispatchEntry& other) const {
        if (priority != other.priority) return priority < other.priority;
        return distanceKm < other.distanceKm;  // Tie-break: closer vehicle first
    }

    bool operator==(const DispatchEntry& other) const {
        return vehicleId == other.vehicleId;
    }

    friend std::ostream& operator<<(std::ostream& os, const DispatchEntry& e) {
        char buf[100];
        std::snprintf(buf, sizeof(buf), "Vehicle#%d P%d %.1fkm",
                 e.vehicleId, e.priority, e.distanceKm);
        os << buf;
        return os;
    }
};
