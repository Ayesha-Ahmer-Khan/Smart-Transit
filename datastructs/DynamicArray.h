#pragma once
// ============================================================
// FILE: DynamicArray.h
// MODULE: 2 - Custom Data Structures
// OUTPUT-ID: M2-DA-001
// OOP CONCEPTS: Templates, operator overloading, constructors,
// heap memory management, iterator support, quicksort.
// ============================================================

#include <iostream>
#include "../exceptions/TransitExceptions.h"

template <typename T>
class DynamicArray {
private:
    T*  m_data;
    int m_size;
    int m_capacity;

    void resize() {
        int newCapacity = m_capacity * 2;
        T* newData = new T[newCapacity];
        for (int i = 0; i < m_size; ++i) newData[i] = m_data[i];
        delete[] m_data;
        m_data = newData;
        m_capacity = newCapacity;
    }

    void swapItems(int a, int b) {
        T temp = m_data[a];
        m_data[a] = m_data[b];
        m_data[b] = temp;
    }

    int partition(int low, int high) {
        T pivot = m_data[high];
        int i = low - 1;
        for (int j = low; j < high; ++j) {
            if (m_data[j] < pivot || m_data[j] == pivot) {
                ++i;
                swapItems(i, j);
            }
        }
        swapItems(i + 1, high);
        return i + 1;
    }

    void quickSort(int low, int high) {
        if (low < high) {
            int pi = partition(low, high);
            quickSort(low, pi - 1);
            quickSort(pi + 1, high);
        }
    }

public:
    DynamicArray() : m_data(new T[4]), m_size(0), m_capacity(4) {}

    explicit DynamicArray(int initialCapacity)
        : m_data(new T[initialCapacity > 0 ? initialCapacity : 4]),
          m_size(0),
          m_capacity(initialCapacity > 0 ? initialCapacity : 4) {}

    DynamicArray(const DynamicArray& other)
        : m_data(new T[other.m_capacity]),
          m_size(other.m_size),
          m_capacity(other.m_capacity) {
        for (int i = 0; i < m_size; ++i) m_data[i] = other.m_data[i];
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this == &other) return *this;
        delete[] m_data;
        m_size = other.m_size;
        m_capacity = other.m_capacity;
        m_data = new T[m_capacity];
        for (int i = 0; i < m_size; ++i) m_data[i] = other.m_data[i];
        return *this;
    }

    ~DynamicArray() { delete[] m_data; }

    void append(const T& value) {
        if (m_size >= m_capacity) resize();
        m_data[m_size++] = value;
    }

    void insert(int index, const T& value) {
        if (index < 0 || index > m_size) throw IndexOutOfBoundsException("DynamicArray::insert");
        if (m_size >= m_capacity) resize();
        for (int i = m_size; i > index; --i) m_data[i] = m_data[i - 1];
        m_data[index] = value;
        ++m_size;
    }

    void remove(int index) {
        if (index < 0 || index >= m_size) throw IndexOutOfBoundsException("DynamicArray::remove");
        for (int i = index; i < m_size - 1; ++i) m_data[i] = m_data[i + 1];
        --m_size;
    }

    T& get(int index) {
        if (index < 0 || index >= m_size) throw IndexOutOfBoundsException("DynamicArray::get");
        return m_data[index];
    }

    const T& get(int index) const {
        if (index < 0 || index >= m_size) throw IndexOutOfBoundsException("DynamicArray::get const");
        return m_data[index];
    }

    void set(int index, const T& value) {
        if (index < 0 || index >= m_size) throw IndexOutOfBoundsException("DynamicArray::set");
        m_data[index] = value;
    }

    int  size() const { return m_size; }
    int  capacity() const { return m_capacity; }
    bool isEmpty() const { return m_size == 0; }

    void clear() { m_size = 0; }

    bool contains(const T& value) const {
        for (int i = 0; i < m_size; ++i) if (m_data[i] == value) return true;
        return false;
    }

    // Required quicksort operation.
    void sort() {
        if (m_size > 1) quickSort(0, m_size - 1);
    }

    T& operator[](int index) { return get(index); }
    const T& operator[](int index) const { return get(index); }

    bool operator==(const DynamicArray& other) const {
        if (m_size != other.m_size) return false;
        for (int i = 0; i < m_size; ++i) if (!(m_data[i] == other.m_data[i])) return false;
        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const DynamicArray<T>& arr) {
        os << "[";
        for (int i = 0; i < arr.m_size; ++i) {
            os << arr.m_data[i];
            if (i < arr.m_size - 1) os << ", ";
        }
        os << "]";
        return os;
    }

    T* begin() { return m_data; }
    T* end() { return m_data + m_size; }
    const T* begin() const { return m_data; }
    const T* end() const { return m_data + m_size; }
};
