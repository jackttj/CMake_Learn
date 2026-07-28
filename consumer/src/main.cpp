#include <mylib/greeting.hpp>
#include<mymath/mymath.h>
using namespace Mylib;

#include <iostream>

#if defined(_WIN32)
#include <windows.h>
#endif

int main()
{
#if defined(_WIN32)
    // MyLib returns UTF-8 text. Make the Windows console decode it as UTF-8.
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::cout << mylib::greeting("downstream consumer") << '\n';
    std::cout << mylib::formatted_greeting("downstream consumer", 1) << '\n';
    std::cout << mylib::formatted_today() << '\n';
    Mymath mymath;
    std::cout << "1 Add 2 equal to" << mymath.add(1 , 2) << '\n';
    return 0;
}
