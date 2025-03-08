#include <iostream>
#include "BitMapHeader.h"
int main() {
    std::string input;
    std::string output;
    std::cout << "Please enter path to bmp file"<< std::endl;
    std::cin >> input;
    BmpImage a(input);
    a.outputOnDisplay();
    a.drawCross();
    a.outputOnDisplay();
    std::cin >> output;
    a.save(output);
    return 0;
}

