#include <string>
#include <fstream>
#include <vector>
#include "zipUtilsH.hpp"

class FLVCEncoder {
public:
    FLVCEncoder(uint16_t width, uint16_t height, uint16_t fps, const std::filesystem::path& outputFilePath)
        : width(width), height(height), fps(fps), frameSize(width * height * 3), frameCount(0), outputFilePath(outputFilePath) {
        outputFile.open(outputFilePath, std::ios::binary);
        if (!outputFile.is_open()) {
            return;
        }

        writeHeader();
    }

    ~FLVCEncoder() {
        if (outputFile.is_open()) {
            updateHeader();
            outputFile.close();
        }
    }

    void close() {
        if (outputFile.is_open()) {
            updateHeader();
            outputFile.close();
        }
    }

    bool writeFrame(const std::vector<uint8_t>& frameData) {
        if (frameData.size() != frameSize) {
            return false;
        }
        if (!outputFile.is_open()) {
            return false;
        }

        std::vector<uint8_t> compressedData = zipUtilsH::compress(frameData);
        if (compressedData.empty()) {
            return false;
        }

        size_t compressedSize = compressedData.size();
        outputFile.write(reinterpret_cast<const char*>(&compressedSize), sizeof(compressedSize));

        outputFile.write(reinterpret_cast<const char*>(compressedData.data()), compressedSize);
        if (!outputFile) {
            return false;
        }
        frameCount++;

        return true;
    }

private:
    uint16_t width;
    uint16_t height;
    uint16_t fps;
    size_t frameSize;
    size_t frameCount;
    std::filesystem::path outputFilePath;
    std::ofstream outputFile;

    void writeHeader() {
        outputFile.write(reinterpret_cast<const char*>(&width), sizeof(width));
        outputFile.write(reinterpret_cast<const char*>(&height), sizeof(height));
        outputFile.write(reinterpret_cast<const char*>(&fps), sizeof(fps));
        size_t placeholderFrameCount = 0;
        outputFile.write(reinterpret_cast<const char*>(&placeholderFrameCount), sizeof(placeholderFrameCount));
    }

    void updateHeader() {
        outputFile.seekp(sizeof(width) + sizeof(height) + sizeof(fps), std::ios::beg);
        outputFile.write(reinterpret_cast<const char*>(&frameCount), sizeof(frameCount));
    }
};


class FLVCDecoder {
public:
    FLVCDecoder(const std::filesystem::path& inputFilePath) : inputFilePath(inputFilePath) {
        inputFile.open(inputFilePath, std::ios::binary);
        if (!inputFile.is_open()) {
            return;
        }

        readHeader();
    }

    ~FLVCDecoder() {
        if (inputFile.is_open()) {
            inputFile.close();
        }
    }

    void close() {
        if (inputFile.is_open()) {
            inputFile.close();
        }
    }

    bool getCurrentRawByFrame(size_t frameIndex, std::vector<unsigned char>& frameData) {
        if (frameIndex >= frameCount) {
            return false;
        }

        inputFile.seekg(sizeof(width) + sizeof(height) + sizeof(fps) + sizeof(frameCount), std::ios::beg);

        size_t currentFramePos = sizeof(width) + sizeof(height) + sizeof(fps) + sizeof(frameCount) + frameIndex * sizeof(frameSize);
        inputFile.seekg(currentFramePos);

        size_t compressedSize = 0;
        inputFile.read(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize));
        if (!inputFile) {
            return false;
        }

        std::vector<uint8_t> compressedData(compressedSize);
        inputFile.read(reinterpret_cast<char*>(compressedData.data()), compressedSize);
        if (!inputFile) {
            return false;
        }

        std::vector<uint8_t> decompressedData = zipUtilsH::decompress(compressedData);
        if (decompressedData.empty()) {
            return false;
        }

        frameData = std::move(decompressedData);
        return true;
    }

    uint16_t getWidth() { return width; }
    uint16_t getHeight() { return height; }
    uint16_t getFPS() { return fps; }
    size_t getFrameCount() { return frameCount; }

    double getDuration() {
        if (fps == 0) {
            return 0.0;
        }
        return static_cast<double>(frameCount) / fps;
    }

private:
    uint16_t width;
    uint16_t height;
    uint16_t fps;
    size_t frameSize;
    size_t frameCount;
    std::filesystem::path inputFilePath;
    std::ifstream inputFile;

    void readHeader() {
        inputFile.read(reinterpret_cast<char*>(&width), sizeof(width));
        inputFile.read(reinterpret_cast<char*>(&height), sizeof(height));
        inputFile.read(reinterpret_cast<char*>(&fps), sizeof(fps));
        inputFile.read(reinterpret_cast<char*>(&frameCount), sizeof(frameCount));

        if (!inputFile) {
            return;
        }

        frameSize = width * height * 3;
    }
};