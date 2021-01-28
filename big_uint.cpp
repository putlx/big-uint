#include "big_uint.hpp"

const char *big_uint::ntable = "0123456789abcdef";

big_uint::big_uint(int n, const int &base) : base(base) {
    assert(base > 1);
    assert(n >= 0);

    for (; n; n /= base)
        x.push_back(n % base);
}

big_uint::big_uint(const char *n, const int &base) : base(base) {
    assert(base > 1 && base <= 16);

    for (; *n; ++n) {
        auto c = *n;
        if (c >= '0' && c <= '9') {
            c -= '0';
            assert(c < base);
            x.push_front(c);
        } else {
            c = std::tolower(c);
            assert(c >= 'a' && c <= 'f');
            x.push_front(c - 'a' + 10);
        }
    }
    while (!x.empty() && x.back() == 0)
        x.pop_back();
}

big_uint &big_uint::operator+=(const big_uint &rhs) {
    assert(base == rhs.base);

    if (!*this)
        x = rhs.x;
    else if (rhs) {
        auto cy = false;
        auto lit = x.begin();
        auto rit = rhs.x.cbegin();
        while (true) {
            if (lit == x.end()) {
                if (rit != rhs.x.cend()) {
                    x.push_back((*rit + cy) % base);
                    lit = x.end();
                    cy = *rit + cy >= base;
                    ++rit;
                } else {
                    if (cy)
                        x.push_back(1);
                    break;
                }
            } else if (cy || rit != rhs.x.cend()) {
                if (rit != rhs.x.cend()) {
                    *lit += *rit;
                    ++rit;
                }
                *lit += cy;
                cy = *lit >= base;
                *lit %= base;
                ++lit;
            } else
                break;
        }
    }
    return *this;
}

big_uint &big_uint::operator-=(const big_uint &rhs) {
    assert(base == rhs.base);

    if (!rhs)
        return *this;
    auto cy = false;
    auto rit = rhs.x.cbegin();
    for (auto &lhs : x) {
        if (rit != rhs.x.cend()) {
            lhs -= *rit;
            ++rit;
        } else if (!cy)
            break;
        lhs -= cy;
        cy = lhs < 0;
        lhs = (lhs + base) % base;
    }
    assert(rit == rhs.x.cend());
    assert(!cy);

    while (!x.empty() && x.back() == 0)
        x.pop_back();
    return *this;
}

big_uint &big_uint::operator*=(const big_uint &rhs) {
    assert(base == rhs.base);

    if (!*this)
        return *this;
    else if (!rhs) {
        x.clear();
        return *this;
    }

    const auto smaller = *this < rhs;
    big_uint tmp(0, base);
    auto pos = 0;
    for (const auto &n : (smaller ? x : rhs.x)) {
        if (n > 0) {
            big_uint temp(smaller ? rhs : *this);
            auto cy = 0;
            for (auto &bit : temp.x) {
                bit = bit * n + cy;
                cy = bit / base;
                bit %= base;
            }
            if (cy != 0)
                temp.x.push_back(cy);

            for (auto i = pos; i; --i)
                temp.x.push_front(0);
            tmp += temp;
        }
        ++pos;
    }
    x = std::move(tmp.x);
    return *this;
}

big_uint &big_uint::operator/=(const big_uint &rhs) {
    assert(base == rhs.base);
    assert(rhs);

    if (size() < rhs.size()) {
        x.clear();
        return *this;
    }

    std::deque<int> x;
    while (*this >= rhs) {
        auto temp(rhs);
        std::size_t n = 0;
        do {
            temp.x.push_front(0);
            ++n;
        } while (*this >= temp);
        temp.x.pop_front();
        --n;

        while (x.size() <= n)
            x.push_back(0);
        ++x[n];
        *this -= temp;
    }
    this->x = x;
    return *this;
}

big_uint &big_uint::operator%=(const big_uint &rhs) {
    assert(base == rhs.base);
    assert(rhs);

    if (size() < rhs.size())
        return *this;

    while (*this >= rhs) {
        auto temp(rhs);
        do {
            temp.x.push_front(0);
        } while (*this >= temp);
        temp.x.pop_front();
        *this -= temp;
    }
    return *this;
}

big_uint big_uint::operator+(const big_uint &rhs) const {
    big_uint lhs(*this);
    lhs += rhs;
    return lhs;
}

big_uint big_uint::operator-(const big_uint &rhs) const {
    auto lhs(*this);
    lhs -= rhs;
    return lhs;
}

big_uint big_uint::operator*(const big_uint &rhs) const {
    auto lhs(*this);
    lhs *= rhs;
    return lhs;
}

big_uint big_uint::operator/(const big_uint &rhs) const {
    auto lhs(*this);
    lhs /= rhs;
    return lhs;
}

big_uint big_uint::operator%(const big_uint &rhs) const {
    auto lhs(*this);
    lhs %= rhs;
    return lhs;
}

big_uint big_uint::pow(const big_uint &rhs) const {
    assert(*this || rhs);

    if (!rhs) {
        return big_uint(1, base);
    } else if (!*this) {
        return big_uint(0, base);
    } else if (rhs.size() == 1) {
        auto rst(*this);
        for (int i = rhs.x.front() - 1; i; --i)
            rst *= *this;
        return rst;
    }

    auto temp(rhs);
    temp.x.pop_front();
    auto rst(pow(temp));
    temp.x.clear();
    temp.x.push_front(base);
    rst = rst.pow(temp);
    if (rhs.x.front() != 0) {
        temp.x.front() = rhs.x.front();
        return rst * pow(temp);
    }
    return rst;
}

bool big_uint::operator>(const big_uint &rhs) const { return compare(rhs) > 0; }

bool big_uint::operator>=(const big_uint &rhs) const {
    return compare(rhs) >= 0;
}

bool big_uint::operator<(const big_uint &rhs) const { return compare(rhs) < 0; }

bool big_uint::operator<=(const big_uint &rhs) const {
    return compare(rhs) <= 0;
}

bool big_uint::operator==(const big_uint &rhs) const {
    return compare(rhs) == 0;
}

bool big_uint::operator!=(const big_uint &rhs) const {
    return compare(rhs) != 0;
}

big_uint::operator bool() const { return !x.empty(); }

int big_uint::size() const { return x.size(); }

std::ostream &operator<<(std::ostream &out, const big_uint &src) {
    assert(src.base <= 16);

    if (src)
        for (auto it = src.x.crbegin(); it != src.x.crend(); ++it)
            out << big_uint::ntable[*it];
    else
        out << '0';
    return out;
}

int big_uint::compare(const big_uint &rhs) const {
    assert(base == rhs.base);

    if (size() != rhs.size())
        return size() - rhs.size();

    for (auto lit = x.crbegin(), rit = rhs.x.crbegin(); lit != x.crend();
         ++lit, ++rit)
        if (*lit != *rit)
            return *lit - *rit;
    return 0;
}
