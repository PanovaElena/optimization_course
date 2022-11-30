#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>


class BMPReader {
public:

    struct CieXYZ {
        uint32_t ciexyzX;
        uint32_t ciexyzY;
        uint32_t ciexyzZ;
    };

    struct CieXYZTriple {
        CieXYZ ciexyzRed;
        CieXYZ ciexyzGreen;
        CieXYZ ciexyzBlue;
    };

    // bitmap file header
    struct BMPFileHeader {
        uint16_t bfType;
        uint32_t bfSize;
        uint16_t bfReserved1;
        uint16_t bfReserved2;
        uint32_t bfOffBits;
    };

    // bitmap info header
    struct BMPInfoHeader {
        uint32_t biSize;
        uint32_t biWidth;
        uint32_t biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount;
        uint32_t biCompression;
        uint32_t biSizeImage;
        uint32_t biXPelsPerMeter;
        uint32_t biYPelsPerMeter;
        uint32_t biClrUsed;
        uint32_t biClrImportant;
        uint32_t biRedMask;
        uint32_t biGreenMask;
        uint32_t biBlueMask;
        uint32_t biAlphaMask;
        uint32_t biCSType;
        CieXYZTriple biEndpoints;
        uint32_t biGammaRed;
        uint32_t biGammaGreen;
        uint32_t biGammaBlue;
        uint32_t biIntent;
        uint32_t biProfileData;
        uint32_t biProfileSize;
        uint32_t biReserved;
    };

    // rgb quad
    struct RGBQuad {
        uint8_t rgbBlue;
        uint8_t rgbGreen;
        uint8_t rgbRed;
        uint8_t rgbReserved;
    };


    bool open(std::string fileName) {
        // открываем файл
        std::ifstream fileStream(fileName, std::ifstream::binary);
        if (!fileStream) {
            std::cout << "Error opening file '" << fileName << "'." << std::endl;
            return false;
        }

        // заголовок изображения
        read(fileStream, fileHeader.bfType);
        read(fileStream, fileHeader.bfSize);
        read(fileStream, fileHeader.bfReserved1);
        read(fileStream, fileHeader.bfReserved2);
        read(fileStream, fileHeader.bfOffBits);

        if (fileHeader.bfType != 0x4D42) {
            std::cout << "Error: '" << fileName << "' is not BMP file." << std::endl;
            return false;
        }

        // информация изображения
        read(fileStream, fileInfoHeader.biSize);

        // bmp core
        if (fileInfoHeader.biSize >= 12) {
            read(fileStream, fileInfoHeader.biWidth);
            read(fileStream, fileInfoHeader.biHeight);
            read(fileStream, fileInfoHeader.biPlanes);
            read(fileStream, fileInfoHeader.biBitCount);
        }

        // получаем информацию о битности
        int colorsCount = fileInfoHeader.biBitCount >> 3;
        if (colorsCount < 3) {
            colorsCount = 3;
        }

        int bitsOnColor = fileInfoHeader.biBitCount / colorsCount;
        int maskValue = (1 << bitsOnColor) - 1;

        // bmp v1
        if (fileInfoHeader.biSize >= 40) {
            read(fileStream, fileInfoHeader.biCompression);
            read(fileStream, fileInfoHeader.biSizeImage);
            read(fileStream, fileInfoHeader.biXPelsPerMeter);
            read(fileStream, fileInfoHeader.biYPelsPerMeter);
            read(fileStream, fileInfoHeader.biClrUsed);
            read(fileStream, fileInfoHeader.biClrImportant);
        }

        // bmp v2
        fileInfoHeader.biRedMask = 0;
        fileInfoHeader.biGreenMask = 0;
        fileInfoHeader.biBlueMask = 0;

        if (fileInfoHeader.biSize >= 52) {
            read(fileStream, fileInfoHeader.biRedMask);
            read(fileStream, fileInfoHeader.biGreenMask);
            read(fileStream, fileInfoHeader.biBlueMask);
        }

        // если маска не задана, то ставим маску по умолчанию
        if (fileInfoHeader.biRedMask == 0 || fileInfoHeader.biGreenMask == 0 || fileInfoHeader.biBlueMask == 0) {
            fileInfoHeader.biRedMask = maskValue << (bitsOnColor * 2);
            fileInfoHeader.biGreenMask = maskValue << bitsOnColor;
            fileInfoHeader.biBlueMask = maskValue;
        }

        // bmp v3
        if (fileInfoHeader.biSize >= 56) {
            read(fileStream, fileInfoHeader.biAlphaMask);
        }
        else {
            fileInfoHeader.biAlphaMask = maskValue << (bitsOnColor * 3);
        }

        // bmp v4
        if (fileInfoHeader.biSize >= 108) {
            read(fileStream, fileInfoHeader.biCSType);
            read(fileStream, fileInfoHeader.biEndpoints);
            read(fileStream, fileInfoHeader.biGammaRed);
            read(fileStream, fileInfoHeader.biGammaGreen);
            read(fileStream, fileInfoHeader.biGammaBlue);
        }

        // bmp v5
        if (fileInfoHeader.biSize >= 124) {
            read(fileStream, fileInfoHeader.biIntent);
            read(fileStream, fileInfoHeader.biProfileData);
            read(fileStream, fileInfoHeader.biProfileSize);
            read(fileStream, fileInfoHeader.biReserved);
        }

        // проверка на поддерку этой версии формата
        if (fileInfoHeader.biSize != 12 && fileInfoHeader.biSize != 40 && fileInfoHeader.biSize != 52 &&
            fileInfoHeader.biSize != 56 && fileInfoHeader.biSize != 108 && fileInfoHeader.biSize != 124) {
            std::cout << "Error: Unsupported BMP format." << std::endl;
            return false;
        }

        if (fileInfoHeader.biBitCount != 16 && fileInfoHeader.biBitCount != 24 && fileInfoHeader.biBitCount != 32) {
            std::cout << "Error: Unsupported BMP bit count." << std::endl;
            return false;
        }

        if (fileInfoHeader.biCompression != 0 && fileInfoHeader.biCompression != 3) {
            std::cout << "Error: Unsupported BMP compression." << std::endl;
            return false;
        }

        // rgb info
        rgbInfo.resize(fileInfoHeader.biHeight);
        for (uint32_t i = 0; i < fileInfoHeader.biHeight; i++) {
            rgbInfo[i].resize(fileInfoHeader.biWidth);
        }

        // определение размера отступа в конце каждой строки
        int linePadding = ((fileInfoHeader.biWidth * (fileInfoHeader.biBitCount / 8)) % 4) & 3;

        // чтение
        uint32_t buffer;

        for (uint32_t i = 0; i < fileInfoHeader.biHeight; i++) {
            for (uint32_t j = 0; j < fileInfoHeader.biWidth; j++) {
                read(fileStream, buffer, fileInfoHeader.biBitCount / 8);

                rgbInfo[i][j].rgbRed = bitextract(buffer, fileInfoHeader.biRedMask);
                rgbInfo[i][j].rgbGreen = bitextract(buffer, fileInfoHeader.biGreenMask);
                rgbInfo[i][j].rgbBlue = bitextract(buffer, fileInfoHeader.biBlueMask);
                rgbInfo[i][j].rgbReserved = bitextract(buffer, fileInfoHeader.biAlphaMask);
            }
            fileStream.seekg(linePadding, std::ios_base::cur);
        }

        return true;
    }


    void save(std::string fileName) {
        // открываем файл
        std::ofstream fileStream(fileName, std::ofstream::binary);
        
        // заголовок изображения
        write(fileStream, fileHeader.bfType);
        write(fileStream, fileHeader.bfSize);
        write(fileStream, fileHeader.bfReserved1);
        write(fileStream, fileHeader.bfReserved2);
        write(fileStream, fileHeader.bfOffBits);

        // информация изображения
        write(fileStream, fileInfoHeader.biSize);

        // bmp core
        if (fileInfoHeader.biSize >= 12) {
            write(fileStream, fileInfoHeader.biWidth);
            write(fileStream, fileInfoHeader.biHeight);
            write(fileStream, fileInfoHeader.biPlanes);
            write(fileStream, fileInfoHeader.biBitCount);
        }

        // получаем информацию о битности
        int colorsCount = fileInfoHeader.biBitCount >> 3;
        if (colorsCount < 3) {
            colorsCount = 3;
        }

        int bitsOnColor = fileInfoHeader.biBitCount / colorsCount;
        int maskValue = (1 << bitsOnColor) - 1;

        // bmp v1
        if (fileInfoHeader.biSize >= 40) {
            write(fileStream, fileInfoHeader.biCompression);
            write(fileStream, fileInfoHeader.biSizeImage);
            write(fileStream, fileInfoHeader.biXPelsPerMeter);
            write(fileStream, fileInfoHeader.biYPelsPerMeter);
            write(fileStream, fileInfoHeader.biClrUsed);
            write(fileStream, fileInfoHeader.biClrImportant);
        }

        // bmp v2
        fileInfoHeader.biRedMask = 0;
        fileInfoHeader.biGreenMask = 0;
        fileInfoHeader.biBlueMask = 0;

        if (fileInfoHeader.biSize >= 52) {
            write(fileStream, fileInfoHeader.biRedMask);
            write(fileStream, fileInfoHeader.biGreenMask);
            write(fileStream, fileInfoHeader.biBlueMask);
        }

        // если маска не задана, то ставим маску по умолчанию
        if (fileInfoHeader.biRedMask == 0 || fileInfoHeader.biGreenMask == 0 || fileInfoHeader.biBlueMask == 0) {
            fileInfoHeader.biRedMask = maskValue << (bitsOnColor * 2);
            fileInfoHeader.biGreenMask = maskValue << bitsOnColor;
            fileInfoHeader.biBlueMask = maskValue;
        }

        // bmp v3
        if (fileInfoHeader.biSize >= 56) {
            write(fileStream, fileInfoHeader.biAlphaMask);
        }

        // bmp v4
        if (fileInfoHeader.biSize >= 108) {
            write(fileStream, fileInfoHeader.biCSType);
            write(fileStream, fileInfoHeader.biEndpoints);
            write(fileStream, fileInfoHeader.biGammaRed);
            write(fileStream, fileInfoHeader.biGammaGreen);
            write(fileStream, fileInfoHeader.biGammaBlue);
        }

        // bmp v5
        if (fileInfoHeader.biSize >= 124) {
            write(fileStream, fileInfoHeader.biIntent);
            write(fileStream, fileInfoHeader.biProfileData);
            write(fileStream, fileInfoHeader.biProfileSize);
            write(fileStream, fileInfoHeader.biReserved);
        }

        // определение размера отступа в конце каждой строки
        int linePadding = ((fileInfoHeader.biWidth * (fileInfoHeader.biBitCount / 8)) % 4) & 3;

        // запись
        uint32_t buffer;

        for (uint32_t i = 0; i < fileInfoHeader.biHeight; i++) {
            for (uint32_t j = 0; j < fileInfoHeader.biWidth; j++) {
                buffer = bitpack(rgbInfo[i][j].rgbRed, fileInfoHeader.biRedMask) |
                    bitpack(rgbInfo[i][j].rgbGreen, fileInfoHeader.biGreenMask) |
                    bitpack(rgbInfo[i][j].rgbBlue, fileInfoHeader.biBlueMask) |
                    bitpack(rgbInfo[i][j].rgbReserved, fileInfoHeader.biAlphaMask);

                write(fileStream, buffer, fileInfoHeader.biBitCount / 8);
            }
            for (int i = 0; i < linePadding; i++) {
                uint8_t padv = 0;
                write(fileStream, padv);
            }
        }
    }

    int getHeight() const {
        return fileInfoHeader.biHeight;
    }

    int getWidth() const {
        return fileInfoHeader.biWidth;
    }

    template <class T>
    std::vector<T> getRGBPixels() const {
        const int h = getHeight(), w = getWidth();
        std::vector<T> pixels(h * w * 3);

        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) {
                pixels[3 * (i * w + j) + 0] = (T)rgbInfo[i][j].rgbRed;
                pixels[3 * (i * w + j) + 1] = (T)rgbInfo[i][j].rgbGreen;
                pixels[3 * (i * w + j) + 2] = (T)rgbInfo[i][j].rgbBlue;
        }

        return pixels;
    }

    template <class T>
    std::vector<T> getRGBAPixels() const {
        const int h = getHeight(), w = getWidth();
        std::vector<T> pixels(h * w * 4);

        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) {
                pixels[4 * (i * w + j) + 0] = (T)rgbInfo[i][j].rgbRed;
                pixels[4 * (i * w + j) + 1] = (T)rgbInfo[i][j].rgbGreen;
                pixels[4 * (i * w + j) + 2] = (T)rgbInfo[i][j].rgbBlue;
                pixels[4 * (i * w + j) + 3] = (T)rgbInfo[i][j].rgbReserved;
            }

        return pixels;
    }
	
	template <class T>
    std::vector<T> getGreyPixels() const {
        const int h = getHeight(), w = getWidth();
        std::vector<T> pixels(h * w);

        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) {
                pixels[i * w + j] = (T)(rgbInfo[i][j].rgbRed/3 +
					rgbInfo[i][j].rgbGreen/3 + rgbInfo[i][j].rgbBlue/3);
            }

        return pixels;
    }

    template <class T>
    void setRGBPixels(const std::vector<T>& pixels) {
        const int h = getHeight(), w = getWidth();

        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) {
                rgbInfo[i][j].rgbRed = (uint8_t)pixels[3 * (i * w + j) + 0];
                rgbInfo[i][j].rgbGreen = (uint8_t)pixels[3 * (i * w + j) + 1];
                rgbInfo[i][j].rgbBlue = (uint8_t)pixels[3 * (i * w + j) + 2];
            }
    }

    template <class T>
    void setRGBAPixels(const std::vector<T>& pixels) {
        const int h = getHeight(), w = getWidth();

        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) {
                rgbInfo[i][j].rgbRed = (uint8_t)pixels[4 * (i * w + j) + 0];
                rgbInfo[i][j].rgbGreen = (uint8_t)pixels[4 * (i * w + j) + 1];
                rgbInfo[i][j].rgbBlue = (uint8_t)pixels[4 * (i * w + j) + 2];
                rgbInfo[i][j].rgbReserved = (uint8_t)pixels[4 * (i * w + j) + 3];
            }
    }
	
	template <class T>
    void setGreyPixels(const std::vector<T>& pixels) {
        const int h = getHeight(), w = getWidth();

        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++) {
                rgbInfo[i][j].rgbRed = (uint8_t)pixels[i * w + j];
                rgbInfo[i][j].rgbGreen = (uint8_t)pixels[i * w + j];
                rgbInfo[i][j].rgbBlue = (uint8_t)pixels[i * w + j];
            }
    }

    template <class T>
    void saveRGB(const std::string& fileName, const std::vector<T>& pixels) {
        setRGBPixels<T>(pixels);
        save(fileName);
    }

    template <class T>
    void saveRGBA(const std::string& fileName, const std::vector<T>& pixels) {
        setRGBAPixels<T>(pixels);
        save(fileName);
    }
	
	template <class T>
    void saveGrey(const std::string& fileName, const std::vector<T>& pixels) {
        setGreyPixels<T>(pixels);
        save(fileName);
    }

    template <class T>
    void save(const std::string& fileName, const std::vector<T>& pixels, int nColors) {
        switch (nColors) {
		case 1:
			saveGrey(fileName, pixels);
            break;
        case 3:
            saveRGB(fileName, pixels);
            break;
        case 4:
            saveRGBA(fileName, pixels);
            break;
        default: break;
        }
    }

protected:

    BMPFileHeader fileHeader;
    BMPInfoHeader fileInfoHeader;
    std::vector<std::vector<RGBQuad>> rgbInfo;

    // определение количества нулевых бит справа от маски
    uint32_t getMaskPadding(const uint32_t mask) {
        uint32_t maskBuffer = mask, maskPadding = 0;

        while (!(maskBuffer & 1)) {
            maskBuffer >>= 1;
            maskPadding++;
        }

        return maskPadding;
    }

    uint8_t bitextract(const uint32_t byte, const uint32_t mask) {
        if (mask == 0) {
            return 0;
        }

        // применение маски и смещение
        return (byte & mask) >> getMaskPadding(mask);
    }

    uint32_t bitpack(const uint8_t s, const uint32_t mask) {
        if (mask == 0) {
            return 0;
        }

        return ((uint32_t)s) << getMaskPadding(mask);
    }

    // read bytes
    template <typename Type>
    void read(std::ifstream& fp, Type& result) {
        read<Type>(fp, result, sizeof(result));
    }

    template <typename Type>
    void read(std::ifstream& fp, Type& result, std::size_t size) {
        fp.read(reinterpret_cast<char*>(&result), size);
    }

    // write bytes
    template <typename Type>
    void write(std::ofstream& fp, Type& result) {
        write<Type>(fp, result, sizeof(result));
    }

    template <typename Type>
    void write(std::ofstream& fp, Type& result, std::size_t size) {
        fp.write(reinterpret_cast<char*>(&result), size);
    }

};




