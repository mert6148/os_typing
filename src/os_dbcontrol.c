#include <cstdio>
#include <format>
#include <initializer_list>
#include <iostream>
#include <string>
 
#if __cpp_lib_to_string >= 202306L
constexpr auto revision() { return " (post C++26)"; }
#else
constexpr auto revision() { return " (pre C++26)"; }
#endif
 
int main()
{
    for (const double f : {1.23456789555555, 23.43, 1e-9, 1e40, 1e-40, 123456789.0})
    {
        std::cout << "to_string:\t" << std::to_string(f) << revision() << '\n';
 
        // Before C++26, the output of std::to_string matches std::printf.
        std::printf("printf:\t\t%f\n", f);
 
        // As of C++26, the output of std::to_string matches std::format.
        std::cout << std::format("format:\t\t{}\n", f);
 
        std::cout << "std::cout:\t" << f << "\n\n";
    }
}