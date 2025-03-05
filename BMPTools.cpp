//
// Created by roach on 05.03.2025.
//

#include "BitMapHeader.h"
#include <fstream>

void BmpImage::load(const std::filesystem::path &path) {
    std::ifstream bmpFile(path, std::ios::binary);
    if (bmpFile) {
        bmpFile.read((char* ))
        if(bmpFile.file_type != 0x4D42) {
            throw std::runtime_error("Error! Unrecognized file format.");
        }
    }
    else
        throw std::runtime_error("File not found");
    if (path.extension() != ".bmp")
        throw std::invalid_argument("Is not BMP file");


}

void BmpImage::save(const std::filesystem::path &fileBmp) {

}

void BmpImage::drawLine(int x, int y) {

}

void BmpImage::drawCross() {

}

void BmpImage::outputOnDisplay() {

}
