#include <lz4frame.h>
#include <vector>

namespace zipUtilsH {
    static std::vector<uint8_t> compress(const std::vector<uint8_t>& data) {
        size_t maxDstSize = LZ4F_compressFrameBound(data.size(), nullptr);
        std::vector<uint8_t> compressedData(maxDstSize);

        size_t compressedSize = LZ4F_compressFrame(compressedData.data(), maxDstSize, data.data(), data.size(), nullptr);
        if (LZ4F_isError(compressedSize)) {
            return {};
        }

        compressedData.resize(compressedSize);
        return compressedData;
    }

    static std::vector<uint8_t> decompress(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> decompressedData(data.size() * 10);

        LZ4F_decompressionContext_t dctx;
        LZ4F_errorCode_t result = LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION);
        if (LZ4F_isError(result)) {
            return {};
        }

        size_t srcSize = data.size();
        size_t dstSize = decompressedData.size();
        size_t decompressedSize = 0;
        const uint8_t* srcPtr = data.data();
        uint8_t* dstPtr = decompressedData.data();

        while (srcSize > 0) {
            size_t srcConsumed = srcSize;
            size_t dstAvailable = dstSize - decompressedSize;
            result = LZ4F_decompress(dctx, dstPtr + decompressedSize, &dstAvailable, srcPtr, &srcConsumed, nullptr);
            if (LZ4F_isError(result)) {
                LZ4F_freeDecompressionContext(dctx);
                return {};
            }

            srcPtr += srcConsumed;
            srcSize -= srcConsumed;
            decompressedSize += dstAvailable;

            if (srcSize > 0 && decompressedSize == dstSize) {
                dstSize *= 2;
                decompressedData.resize(dstSize);
                dstPtr = decompressedData.data();
            }
        }

        decompressedData.resize(decompressedSize);
        LZ4F_freeDecompressionContext(dctx);

        return decompressedData;
    }

}
