//
// Created by roach on 05.03.2025.
//

#include "BitMapHeader.h"
#include <fstream>
#include <iostream>
#ifdef _WIN32
#endif

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

void BmpImage::outputOnDisplay() {
    const int bytes_per_pixel = infoHeader.biBitCount / 8;
    const uint32_t stride = row_stride();

    // Перебор строк снизу вверх (как хранится в BMP)
    for (int y = infoHeader.biHeight - 1; y >= 0; --y) {
        const uint8_t* row = data.data() + y * stride;
        for (int x = 0; x < infoHeader.biWidth; ++x) {
            const uint8_t* pixel = row + x * bytes_per_pixel;
            // Проверка цвета (BGR-формат)
            if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0) { // Черный
                std::cout << "\033[40m \033[0m";
            } else { // Белый или другие цвета (по условию только черный/белый)
                std::cout << "\033[47m \033[0m";
            }
        }
        std::cout << '\n'; // Новая строка изображения
    }
    std::cout << '\n';
}



void BmpImage::debugOutput() {
    for (int i = 0; i < data.size(); ++i) {
        uint8_t tmp = data[i];
        std::cout << tmp << std::endl;
    }
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

void BmpImage::drawLine(int x1, int y1, int x2, int y2) {
    const int bytes_per_pixel = infoHeader.biBitCount / 8;
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int error = dx + dy;

    // Учет выравнивания BMP
    int rowSize = (infoHeader.biWidth * bytes_per_pixel + 3) & ~3;

    while (true) {
        // Проверка текущих координат
        bool current_inside = (x1 >= 0 && x1 < infoHeader.biWidth && y1 >= 0 && y1 < infoHeader.biHeight);

        if (current_inside) {
            int invertedY = infoHeader.biHeight - 1 - y1;
            int index = invertedY * rowSize + x1 * bytes_per_pixel;

            data[index] = 0;     // Синий
            data[index + 1] = 0; // Зеленый
            data[index + 2] = 0; // Красный

            if (bytes_per_pixel == 4) {
                data[index + 3] = 255; // Альфа
            }
        }

        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * error;

        if (e2 >= dy) { // Движение по X
            error += dy;
            x1 += sx;
        }

        if (e2 <= dx) { // Движение по Y
            error += dx;
            y1 += sy;
        }
    }

}

void BmpImage::drawCross() {
    drawLine(0, 0, infoHeader.biWidth - 1, infoHeader.biHeight - 1);
    drawLine(1, 0, infoHeader.biWidth - 2, infoHeader.biHeight - 1);
    drawLine(infoHeader.biWidth - 1, 0, 0, infoHeader.biHeight - 1);
    drawLine(infoHeader.biWidth - 2, 0, 1, infoHeader.biHeight - 1);
}

void BmpImage::save(const std::filesystem::path &file) {
    std::ofstream of(file, std::ios::binary);
    if (infoHeader.biBitCount == 32 ) {
        of.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
        of.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));
        of.write(reinterpret_cast<const char*>(data.data()), data.size());
    } else if (infoHeader.biBitCount == 24) {
        if (infoHeader.biWidth % 4 == 0) {
            of.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
            of.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));
            of.write(reinterpret_cast<const char*>(data.data()), data.size());
        }
    }
}
