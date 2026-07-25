#include <mylib/greeting.hpp>

#include <iostream>

int main()
{
    std::cout << mylib::greeting("downstream consumer") << '\n';
    return 0;
}
