#include <iostream>
#include "BitMapHeader.h"
int main() {
    std::string input;
    std::string output;
    std::cout << "Please enter path to bmp file:"<< std::endl;
    std::cin >> input;
    BmpImage bmp;
    try {
        bmp.load(input);
        bmp.outputOnDisplay();

        bmp.drawCross();
        bmp.outputOnDisplay();
        std::cout << "Enter new name for file:" << std::endl;
        std::cin >> output;
        bmp.save(output);
    } catch (std::exception& e) {
        std::cerr <<"Error: "<< e.what() << std::endl;
    }

    return 0;
}

