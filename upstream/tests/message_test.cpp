#include <upstream/message.hpp>

#include <iostream>

int main()
{
    const auto actual = upstream::message("Upstream");
    const auto expected = std::string{"Hello, Upstream"};

    if (actual != expected) {
        std::cerr << "expected: " << expected << "\nactual: " << actual << '\n';
        return 1;
    }
    return 0;
}
