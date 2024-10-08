// PR c++/66635
// { dg-do compile { target c++11 } }
// { dg-skip-if "requires hosted libstdc++ for cassert" { ! hostedlib } }

#include <cassert>

struct Foo {
    constexpr Foo(const unsigned i) : val(i) {}
    constexpr Foo operator-(const Foo &rhs) const {
      return assert(val >= rhs.val), Foo(val - rhs.val); // { dg-error "call to non-.constexpr." }
    }
    unsigned val;
};

constexpr Foo foo(Foo(1) - Foo(2)); // { dg-message "in .constexpr. expansion of " }
