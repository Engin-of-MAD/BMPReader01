//
// Created by roach on 05.03.2025.
//

#include "BitMapHeader.h"
#include <fstream>
#include <iostream>

void BmpImage::load() {

    std::ifstream bmpFile(_pathToBmp, std::ios::binary);
    if (!bmpFile)
        throw std::runtime_error("File not found: " + _pathToBmp.string());

     bmpFile.read(reinterpret_cast<char *>(&fileHeader), sizeof(BmpFileHeader));
     bmpFile.read(reinterpret_cast<char *>(&infoHeader), sizeof(BmpInfoHeader));
     if (fileHeader.bfType != 0x4D42)
         throw std::runtime_error("Invalid BMP format");
     if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32)
         throw std::runtime_error("Only 24/32-bit BMP supported");
    bmpFile.seekg(fileHeader.bfOffBits, bmpFile.beg);
    data.resize(infoHeader.biWidth * infoHeader.biHeight * (infoHeader.biBitCount / 8));
    if (infoHeader.biWidth % 4 == 0) {
        bmpFile.read(reinterpret_cast<char*>(data.data()), data.size());
        fileHeader.bfSize += data.size();
    } else {
        const uint32_t stride = row_stride();
        std::vector<uint8_t> padding_row(stride - infoHeader.biWidth * 3);
        for (int y = 0; y < infoHeader.biHeight; ++y) {
            bmpFile.read(reinterpret_cast<char *>(data.data()+ y * infoHeader.biWidth * 3)
                         , infoHeader.biWidth * 3);
            bmpFile.read(reinterpret_cast<char *>(padding_row.data()), padding_row.size());
        }
    }
}

void BmpImage::save(const std::filesystem::path &fileBmp) {

}

void BmpImage::drawLine(int x, int y) {

}

void BmpImage::drawCross() {

}

void BmpImage::outputOnDisplay() {

}

BmpImage::BmpImage(const std::string& path) : _pathToBmp(path) {
    load();
}

uint32_t BmpImage::row_stride() const {
    if (infoHeader.biBitCount == 24) {
        // Выравнивание до 4 байт: (width*3 + 3) & ~3
        return (infoHeader.biWidth * 3 + 3) & ~3;
    } else { // 32-битные не требуют выравнивания
        return infoHeader.biWidth * 4;
    }
}
