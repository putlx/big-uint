from operator import *
from random import *

sns = tuple(int(random() * 10 ** randint(0, 3)) for _ in range(64))
ns = tuple(int(random() * 10 ** randint(0, 32)) for _ in range(256)) + sns
b = 10
assert b in (2, 8, 10, 16)


def wrap(n):
    if b == 10:
        if n < 65536 and random() < 0.7:
            return n
        return f'"{n}"'
    to_s = {2: bin, 8: oct, 16: hex}
    return f'"{to_s[b](n)[2:]}"'


with open("test.cpp", "w") as out:
    out.write('#include "big_uint.hpp"\n')
    out.write("#include <cassert>\n\n")
    out.write("int main() {\n\n")

    def comparison_operator_test(c, f):
        for _ in range(16):
            x = choice(ns)
            if random() < 0.5:
                y = choice(ns)
            else:
                y = x
            z = str(f(x, y)).lower()
            x, y = map(wrap, (x, y))
            out.write(f"assert( ( big_uint({x}, {b}) {c} big_uint({y}, {b}) ) == {z} );\n")
        out.write("\n")

    comparison_operator_test(">", gt)
    comparison_operator_test(">=", ge)
    comparison_operator_test("<", lt)
    comparison_operator_test("<=", le)
    comparison_operator_test("!=", ne)
    comparison_operator_test("==", eq)

    def binary_operator_test(c, f):
        for _ in range(16):
            x = choice(ns)
            y = choice(ns)
            if f is sub:
                while x < y:
                    if random() < 0.5:
                        x = choice(ns)
                    else:
                        y = choice(ns)
            elif f is floordiv or f is mod:
                while y == 0:
                    y = choice(ns)
            elif f is pow:
                x = choice(sns)
                y = choice(sns)
                while x == 0 and y == 0:
                    if random() < 0.5:
                        x = choice(sns)
                    else:
                        y = choice(sns)
            z = f(x, y)
            x, y, z = map(wrap, (x, y, z))
            out.write(f"assert( big_uint({x}, {b}) {c}(big_uint({y}, {b})) == big_uint({z}, {b}) );\n")
        out.write("\n")

    binary_operator_test("+", add)
    binary_operator_test("-", sub)
    binary_operator_test("*", mul)
    binary_operator_test("/", floordiv)
    binary_operator_test("%", mod)
    binary_operator_test(".pow", pow)

    out.write("big_uint tmp(0, 10);\n\n")

    def unary_operator_test(c, f):
        for _ in range(8):
            x = choice(ns)
            z = f(x)
            x, z = map(wrap, (x, z))
            out.write(f"assert( {c}(tmp = big_uint({x}, {b})) == big_uint({z}, {b}) );\n")
        out.write("\n")

    unary_operator_test("++", lambda x: x + 1)
    ns = tuple(filter(bool, ns))
    unary_operator_test("--", lambda x: x - 1)

    out.write("}\n")
