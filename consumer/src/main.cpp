#include <mylib/greeting.hpp>

#include <iostream>

int main()
{
    std::cout << mylib::greeting("downstream consumer") << '\n';
    std::cout << mylib::formatted_greeting("downstream consumer",1) << '\n';
    return 0;
}
