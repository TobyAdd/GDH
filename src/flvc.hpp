#include <string>
#include <fstream>
#include <vector>
#define QOI_IMPLEMENTATION
#include <qoi/qoi.h>

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
        if (frameData.size() != frameSize || !outputFile.is_open()) {
            return false;
        }

        qoi_desc desc = {width, height, 3, QOI_SRGB};

        int qoiSize;
        void* qoiData = qoi_encode(frameData.data(), &desc, &qoiSize);
        if (!qoiData) {
            return false;
        }

        outputFile.write(reinterpret_cast<const char*>(&qoiSize), sizeof(qoiSize));
        outputFile.write(reinterpret_cast<const char*>(qoiData), qoiSize);

        free(qoiData);

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