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

    const auto today = mylib::formatted_today();
    if (today.find(u8"年") == std::string::npos ||
        today.find(u8"月") == std::string::npos ||
        today.find(u8"日") == std::string::npos ||
        today.find(u8"时") == std::string::npos ||
        today.find(u8"分") == std::string::npos ||
        today.find(u8"秒") == std::string::npos) {
        std::cerr << "unexpected Chinese time format: " << today << '\n';
        return 1;
    }

    return 0;
}
