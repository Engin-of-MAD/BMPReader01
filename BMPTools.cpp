//
// Created by roach on 05.03.2025.
//

#include "BitMapHeader.h"
#include <fstream>
#include <iostream>
#ifdef _WIN32
#endif

void BmpImage::load(const std::filesystem::path& path) {

    std::ifstream bmpFile(path, std::ios::binary);
    if (!bmpFile)
        throw std::runtime_error("File not found: " + path.string());

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
    bmpFile.close();
}

void BmpImage::outputOnDisplay() {
    const int bytes_per_pixel = infoHeader.biBitCount / 8;
    const uint32_t stride = row_stride();

    for (int y = infoHeader.biHeight - 1; y >= 0; --y) {
        const uint8_t* row = data.data() + y * stride;
        for (int x = 0; x < infoHeader.biWidth; ++x) {
            const uint8_t* pixel = row + x * bytes_per_pixel;
            //
            if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0) {
                std::cout << "\033[40m \033[0m";
            } else if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255) {
                std::cout << "\033[47m \033[0m";
            }
        }
        std::cout << '\n'; // Новая строка изображения
    }
    std::cout << '\n';
}


uint32_t BmpImage::row_stride() const {
    if (infoHeader.biBitCount == 24) {
        // Выравнивание до 4 байт: (width*3 + 3) & ~3
        return (infoHeader.biWidth * 3 + 3) & ~3;
    } else { // 32-битные не требуют выравнивания
        return infoHeader.biWidth * 4;
    }
}

void BmpImage::drawLine(std::pair<int, int> pointA, std::pair<int,int> pointB, bool color) {
    const int bytes_per_pixel = infoHeader.biBitCount / 8;
    int dx = abs(pointB.first - pointA.first);
    int dy = -abs(pointB.second - pointA.second);
    int sx = pointA.first < pointB.first ? 1 : -1;
    int sy = pointA.second < pointB.second ? 1 : -1;
    int error = dx + dy;

    // Проверка границ точек
    if (pointA.first < 0 || pointA.first >=  infoHeader.biWidth ||
        pointA.second < 0 || pointA.second >= infoHeader.biHeight ||
        pointB.first < 0 || pointB.first >=  infoHeader.biWidth ||
        pointB.second < 0 || pointB.second >= infoHeader.biHeight) {
        throw std::invalid_argument("Point out border of image");
    }

    // Учет выравнивания BMP
    int rowSize = (infoHeader.biWidth * bytes_per_pixel + 3) & ~3;

    while (true) {
        // Проверка текущих координат
        bool current_inside = (pointA.first >= 0 && pointA.first < infoHeader.biWidth && pointA.second >= 0 && pointA.second < infoHeader.biHeight);

        if (current_inside) {
            int invertedY = infoHeader.biHeight - 1 - pointA.second;
            int index = invertedY * rowSize + pointA.first * bytes_per_pixel;

            if (color){
                data[index] = 255;
                data[index + 1] = 255;
                data[index + 2] = 255;
            } else {
                data[index] = 0;
                data[index + 1] = 0;
                data[index + 2] = 0;
            }

            if (bytes_per_pixel == 4) {
                data[index + 3] = 255; // Альфа
            }
        }

        if (pointA.first == pointB.first && pointA.second == pointB.second) break;

        int e2 = 2 * error;

        if (e2 >= dy) { // Движение по X
            error += dy;
            pointA.first += sx;
        }

        if (e2 <= dx) { // Движение по Y
            error += dx;
            pointA.second += sy;
        }
    }

}

void BmpImage::drawCross() {
    drawLine({0, 0}, {infoHeader.biWidth - 1, infoHeader.biHeight - 1});
    drawLine({infoHeader.biWidth - 1, 0}, {0, infoHeader.biHeight - 1});
}


void BmpImage::save(const std::filesystem::path &file) {
    std::ofstream of(file, std::ios::binary);
    if (infoHeader.biBitCount == 32 )
        writeData(of);
    else if (infoHeader.biBitCount == 24) {
        if (infoHeader.biWidth % 4 == 0)
            writeData(of);
    }
    of.close();
}

void BmpImage::writeData(std::ofstream &of) {
    of.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
    of.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));
    of.write(reinterpret_cast<const char*>(data.data()), data.size());
}
