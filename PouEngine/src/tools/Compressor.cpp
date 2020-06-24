#include "PouEngine/tools/Compressor.h"

#include "PouEngine/tools/Logger.h"

#include <iostream>

namespace pou
{

AbstractCompressor::AbstractCompressor()
{
    //ctor
}

AbstractCompressor::~AbstractCompressor()
{
    //dtor
}

bool AbstractCompressor::compress(uint8_t *srcBuffer, size_t srcBufferSize, std::vector<uint8_t> &dstBuffer)
{
    return this->compressImpl(srcBuffer, srcBufferSize, dstBuffer);
}

bool AbstractCompressor::decompress(uint8_t *srcBuffer, size_t srcBufferSize, std::vector<uint8_t> &dstBuffer)
{
    return this->decompressImpl(srcBuffer, srcBufferSize, dstBuffer);
}

///
/// LZ4_Compressor
///

#include "lz4.h"
#include "lz4hc.h"

LZ4_Compressor::LZ4_Compressor()
{

}

LZ4_Compressor::~LZ4_Compressor()
{

}

bool LZ4_Compressor::compressImpl(uint8_t *srcBuffer, size_t srcBufferSize, std::vector<uint8_t> &dstBuffer)
{
    int maxSize = LZ4_compressBound(srcBufferSize);
    dstBuffer.resize(maxSize);

    //int dstSize = LZ4_compress_default((char*)srcBuffer, (char*)dstBuffer.data(), srcBufferSize, maxSize);
    int dstSize = LZ4_compress_HC((char*)srcBuffer, (char*)dstBuffer.data(), srcBufferSize, maxSize, LZ4HC_CLEVEL_MAX);
    dstBuffer.resize(dstSize);

    return (true);
}

bool LZ4_Compressor::decompressImpl(uint8_t *srcBuffer, size_t srcBufferSize, std::vector<uint8_t> &dstBuffer)
{
    int dstSize = LZ4_decompress_safe((char*)srcBuffer, (char*)dstBuffer.data(), srcBufferSize, dstBuffer.size());

    if(dstSize != (int)dstBuffer.size())
        Logger::warning("Decompressed size do not match with expected buffer size");

    return (true);
}


}
