// ============================================================
// FILE: CustomString.cpp
// MODULE: 1 - Utility Layer
// Implements all methods declared in CustomString.h
// ============================================================

#include "CustomString.h"
#include "../exceptions/TransitExceptions.h"
#include <cstring>   // strlen, strcpy, strcat, strcmp, strstr
#include <cctype>    // toupper, tolower, isspace
#include <cstdlib>   // atoi, atof

// -----------------------------------------------------------
// PRIVATE HELPER
// -----------------------------------------------------------
void CustomString::copyFrom(const char* src) {
    if (src == nullptr) {
        m_length = 0;
        m_data   = new char[1];
        m_data[0] = '\0';
    } else {
        m_length = static_cast<int>(strlen(src));
        m_data   = new char[m_length + 1]; // +1 for null terminator
        strcpy(m_data, src);
    }
}

// -----------------------------------------------------------
// CONSTRUCTORS
// -----------------------------------------------------------

// Default: creates empty string ""
CustomString::CustomString() {
    m_length = 0;
    m_data   = new char[1];
    m_data[0] = '\0';
}

// From C-string: CustomString s("hello")
CustomString::CustomString(const char* str) {
    copyFrom(str);
}

// Copy constructor: called when you do CustomString b = a;
// [OOP: Deep copy - copies the actual data, not just pointer]
CustomString::CustomString(const CustomString& other) {
    copyFrom(other.m_data);
}

// Move constructor: steals resources from a temporary object
// [OOP: Move semantics - avoids unnecessary copying]
CustomString::CustomString(CustomString&& other) noexcept {
    m_data   = other.m_data;    // steal the pointer
    m_length = other.m_length;
    other.m_data   = nullptr;   // leave other empty
    other.m_length = 0;
}

// -----------------------------------------------------------
// DESTRUCTOR - frees heap memory
// -----------------------------------------------------------
CustomString::~CustomString() {
    delete[] m_data;  // MUST use delete[] for arrays
}

// -----------------------------------------------------------
// ASSIGNMENT OPERATORS
// -----------------------------------------------------------

// Copy assignment: s1 = s2;
CustomString& CustomString::operator=(const CustomString& other) {
    if (this == &other) return *this;  // self-assignment guard
    delete[] m_data;                   // free old memory
    copyFrom(other.m_data);            // copy new data
    return *this;
}

// Move assignment: s1 = std::move(s2);
CustomString& CustomString::operator=(CustomString&& other) noexcept {
    if (this == &other) return *this;
    delete[] m_data;
    m_data         = other.m_data;
    m_length       = other.m_length;
    other.m_data   = nullptr;
    other.m_length = 0;
    return *this;
}

// -----------------------------------------------------------
// CONCATENATION
// -----------------------------------------------------------

// s1 + s2 -> returns new string
CustomString CustomString::operator+(const CustomString& other) const {
    int newLen  = m_length + other.m_length;
    char* buf   = new char[newLen + 1];
    strcpy(buf, m_data);
    strcat(buf, other.m_data);
    CustomString result(buf);
    delete[] buf;
    return result;
}

// s1 += s2 -> appends to this
CustomString& CustomString::operator+=(const CustomString& other) {
    *this = *this + other;  // reuse operator+
    return *this;
}

// -----------------------------------------------------------
// COMPARISON OPERATORS
// -----------------------------------------------------------
bool CustomString::operator==(const CustomString& other) const {
    return strcmp(m_data, other.m_data) == 0;
}

bool CustomString::operator!=(const CustomString& other) const {
    return !(*this == other);  // reuse operator==
}

bool CustomString::operator<(const CustomString& other) const {
    return strcmp(m_data, other.m_data) < 0;
}

bool CustomString::operator>(const CustomString& other) const {
    return strcmp(m_data, other.m_data) > 0;
}

// -----------------------------------------------------------
// INDEX OPERATOR - bounds checked
// -----------------------------------------------------------
char& CustomString::operator[](int index) {
    if (index < 0 || index >= m_length) {
        throw IndexOutOfBoundsException("CustomString::operator[]");
    }
    return m_data[index];
}

const char& CustomString::operator[](int index) const {
    if (index < 0 || index >= m_length) {
        throw IndexOutOfBoundsException("CustomString::operator[]");
    }
    return m_data[index];
}

// -----------------------------------------------------------
// STREAM OPERATORS (Friend)
// -----------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const CustomString& s) {
    os << s.m_data;
    return os;
}

std::istream& operator>>(std::istream& is, CustomString& s) {
    char buffer[1024];
    is >> buffer;
    s = CustomString(buffer);
    return is;
}

// -----------------------------------------------------------
// UTILITY METHODS
// -----------------------------------------------------------

CustomString CustomString::toUpperCase() const {
    char* buf = new char[m_length + 1];
    for (int i = 0; i < m_length; i++) {
        buf[i] = static_cast<char>(toupper(m_data[i]));
    }
    buf[m_length] = '\0';
    CustomString result(buf);
    delete[] buf;
    return result;
}

CustomString CustomString::toLowerCase() const {
    char* buf = new char[m_length + 1];
    for (int i = 0; i < m_length; i++) {
        buf[i] = static_cast<char>(tolower(m_data[i]));
    }
    buf[m_length] = '\0';
    CustomString result(buf);
    delete[] buf;
    return result;
}

CustomString CustomString::trim() const {
    int start = 0;
    int end   = m_length - 1;
    while (start <= end && isspace(m_data[start])) start++;
    while (end >= start && isspace(m_data[end]))   end--;
    return substring(start, end - start + 1);
}

CustomString CustomString::substring(int start, int len) const {
    if (start < 0 || start >= m_length || len <= 0) return CustomString("");
    if (start + len > m_length) len = m_length - start;
    char* buf = new char[len + 1];
    strncpy(buf, m_data + start, len);
    buf[len] = '\0';
    CustomString result(buf);
    delete[] buf;
    return result;
}

bool CustomString::contains(const CustomString& sub) const {
    return find(sub) != -1;
}

int CustomString::find(const CustomString& sub) const {
    const char* pos = strstr(m_data, sub.m_data);
    if (pos == nullptr) return -1;
    return static_cast<int>(pos - m_data);
}

bool CustomString::startsWith(const CustomString& prefix) const {
    if (prefix.m_length > m_length) return false;
    return strncmp(m_data, prefix.m_data, prefix.m_length) == 0;
}

bool CustomString::endsWith(const CustomString& suffix) const {
    if (suffix.m_length > m_length) return false;
    int offset = m_length - suffix.m_length;
    return strcmp(m_data + offset, suffix.m_data) == 0;
}

int CustomString::toInt() const {
    return atoi(m_data);
}

double CustomString::toDouble() const {
    return atof(m_data);
}

// Replace all occurrences of oldText with newText. Required by specification.
CustomString CustomString::replace(const CustomString& oldText, const CustomString& newText) const {
    if (oldText.m_length == 0) return *this;

    int count = 0;
    for (int i = 0; i <= m_length - oldText.m_length; ) {
        if (strncmp(m_data + i, oldText.m_data, oldText.m_length) == 0) {
            ++count;
            i += oldText.m_length;
        } else {
            ++i;
        }
    }

    int newLen = m_length + count * (newText.m_length - oldText.m_length);
    char* buf = new char[newLen + 1];
    int out = 0;
    for (int i = 0; i < m_length; ) {
        if (i <= m_length - oldText.m_length &&
            strncmp(m_data + i, oldText.m_data, oldText.m_length) == 0) {
            for (int j = 0; j < newText.m_length; ++j) buf[out++] = newText.m_data[j];
            i += oldText.m_length;
        } else {
            buf[out++] = m_data[i++];
        }
    }
    buf[out] = '\0';
    CustomString result(buf);
    delete[] buf;
    return result;
}

// Split on a single delimiter and return a custom DynamicArray of CustomString.
DynamicArray<CustomString> CustomString::split(char delimiter) const {
    DynamicArray<CustomString> parts;
    int start = 0;
    for (int i = 0; i <= m_length; ++i) {
        if (i == m_length || m_data[i] == delimiter) {
            parts.append(substring(start, i - start));
            start = i + 1;
        }
    }
    return parts;
}
