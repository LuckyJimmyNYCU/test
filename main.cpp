#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#pragma pack(push, 1) // To ensure no padding in the struct

struct BMPHeader {
    uint16_t fileType;       // BMP file type ('BM')
    uint32_t fileSize;       // Size of the file in bytes
    uint16_t reserved1;      // Reserved
    uint16_t reserved2;      // Reserved
    uint32_t offsetData;     // Offset to the start of the pixel data
    uint32_t size;           // Header size
    int32_t width;           // Width of the image
    int32_t height;          // Height of the image
    uint16_t planes;         // Number of planes
    uint16_t bitCount;       // Bits per pixel (1, 4, 8, 24, or 32)
    uint32_t compression;    // Compression type
    uint32_t sizeImage;      // Size of the pixel data
    int32_t xPixelsPerMeter; // Horizontal resolution
    int32_t yPixelsPerMeter; // Vertical resolution
    uint32_t colorsUsed;     // Number of colors used
    uint32_t colorsImportant;// Number of important colors
};

#pragma pack(pop)

void flipHorizontal(std::vector<uint8_t>& imageData, int width, int height, int bytesPerPixel) {
    // Process each row of the image
    int rowSize = width * bytesPerPixel;

    for (int y = 0; y < height; ++y) {
        uint8_t* rowStart = &imageData[y * rowSize];
        for (int x = 0; x < width / 2; ++x) {
            for (int b = 0; b < bytesPerPixel; ++b) {
                std::swap(rowStart[x * bytesPerPixel + b], rowStart[(width - x - 1) * bytesPerPixel + b]);
            }
        }
    }
}

void readBMP(const std::string& filename, BMPHeader& header, std::vector<uint8_t>& imageData) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    // Move to pixel data position
    file.seekg(header.offsetData, std::ios::beg);

    // Calculate total pixel data size (including possible padding for each row)
    int rowSize = (header.width * header.bitCount + 31) / 32 * 4;
    imageData.resize(rowSize * header.height);

    file.read(reinterpret_cast<char*>(imageData.data()), imageData.size());
    file.close();
}

void writeBMP(const std::string& filename, const BMPHeader& header, const std::vector<uint8_t>& imageData) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not write file " << filename << std::endl;
        return;
    }

    // Write the header
    file.write(reinterpret_cast<const char*>(&header), sizeof(header));

    // Write the pixel data
    file.write(reinterpret_cast<const char*>(imageData.data()), imageData.size());
    file.close();
}

int main() {
    BMPHeader header1, header2;
    std::vector<uint8_t> imageData1, imageData2;

    // Read input BMP files
    readBMP("input1.bmp", header1, imageData1);
    readBMP("input2.bmp", header2, imageData2);

    // Flip the images horizontally
    int bytesPerPixel1 = header1.bitCount / 8;
    int bytesPerPixel2 = header2.bitCount / 8;

    flipHorizontal(imageData1, header1.width, header1.height, bytesPerPixel1);
    flipHorizontal(imageData2, header2.width, header2.height, bytesPerPixel2);

    // Write output BMP files
    writeBMP("output1_flip.bmp", header1, imageData1);
    writeBMP("output2_flip.bmp", header2, imageData2);

    std::cout << "Images flipped and saved as output1_flip.bmp and output2_flip.bmp." << std::endl;

    return 0;
}
