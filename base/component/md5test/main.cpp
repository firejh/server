#include "md5.h"

#include <iostream>

int main()
{
    std::string test = "12345";

    std::cout << md5(test) << std::endl;
}
