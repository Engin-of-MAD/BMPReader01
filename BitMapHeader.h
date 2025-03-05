//
// Created by roach on 05.03.2025.
//

#ifndef BMPVIEWERCLI_BITMAPHEADER_H
#define BMPVIEWERCLI_BITMAPHEADER_H

#include <cstdint>
#include <string>
#include <filesystem>

#pragma pack(push, 1)
struct BmpFileHeader{
    uint16_t bfType{};        ///< Type file
    uint32_t bfSize{};        ///< Size in bytes
    uint16_t bfReserved1{};   ///< Reserved; must be 0
    uint16_t bfReserved2{};   ///< Reserved; must be 0
    uint32_t bfOffBits{};     /// Offset before the start of the img data
};
#pragma pack(pop)


struct BmpInfoHeader {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression{};
    uint32_t biSizeImage{};
    int32_t biXPixelsPerMeter{};
    int32_t biYPixelsPerMeter{};
    uint32_t biColorUsed{};
    uint32_t biColortImportant{};
};


class BmpCli{
public:
    void readBMP(std::filesystem::path& fileBmp);
    void writeLine(int x, int y);
private:
    BmpFileHeader bmpFile;
    BmpInfoHeader bmpInfo;
};
#endif //BMPVIEWERCLI_BITMAPHEADER_H
