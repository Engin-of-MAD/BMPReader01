#include <iostream>
#include "BitMapHeader.h"
int main() {
    std::string input;
    std::cout << "Please enter path to bmp file"<< std::endl;
    std::cin >> input;
    BmpImage a(input);
    return 0;
}

