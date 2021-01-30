#include <cassert>
#include <deque>
#include <iostream>
#include <string>
#pragma once

class big_uint {
  public:
    big_uint(int n, const int &base);
    big_uint(const char *n, const int &base);
    big_uint(const big_uint &) = default;
    big_uint(big_uint &&) noexcept = default;
    big_uint &operator=(const big_uint &) = default;
    big_uint &operator=(big_uint &&) noexcept = default;

    big_uint &operator++();
    big_uint &operator--();
    big_uint &operator+=(const big_uint &rhs);
    big_uint &operator-=(const big_uint &rhs);
    big_uint &operator*=(const big_uint &rhs);
    big_uint &operator/=(const big_uint &rhs);
    big_uint &operator%=(const big_uint &rhs);

    big_uint operator+(const big_uint &rhs) const;
    big_uint operator-(const big_uint &rhs) const;
    big_uint operator*(const big_uint &rhs) const;
    big_uint operator/(const big_uint &rhs) const;
    big_uint operator%(const big_uint &rhs) const;
    big_uint pow(const big_uint &rhs) const;

    bool operator>(const big_uint &rhs) const;
    bool operator>=(const big_uint &rhs) const;
    bool operator<(const big_uint &rhs) const;
    bool operator<=(const big_uint &rhs) const;
    bool operator==(const big_uint &rhs) const;
    bool operator!=(const big_uint &rhs) const;
    operator bool() const;
    int size() const;

    friend std::ostream &operator<<(std::ostream &out, const big_uint &src);

  private:
    int compare(const big_uint &rhs) const;

    int base;
    std::deque<int> x;
    static const char *ntable;
};
