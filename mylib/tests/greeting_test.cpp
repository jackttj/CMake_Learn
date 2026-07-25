#include <mylib/greeting.hpp>

#include <iostream>

int main()
{
    const auto actual = mylib::greeting("CMake");
    const auto expected = std::string{"Hello, CMake!"};

    if (actual != expected) {
        std::cerr << "expected: " << expected << "\nactual: " << actual << '\n';
        return 1;
    }
    return 0;
}
