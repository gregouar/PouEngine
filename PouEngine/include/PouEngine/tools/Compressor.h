#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "PouEngine/Types.h"

#include <vector>

namespace pou
{

class AbstractCompressor
{
    public:
        AbstractCompressor();
        virtual ~AbstractCompressor();

        bool compress(uint8_t *srcBuffer, size_t srcBufferSize, std::vector<uint8_t> &dstBuffer);
        bool decompress(uint8_t *srcBuffer, size_t srcBufferSize, std::vector<uint8_t> &dstBuffer);

    protected:
        virtual bool compressImpl(uint8_t *srcBuffer, size_t srcBufferSize, std::vector<uint8_t> &dstBuffer) = 0;
        virtual bool decompressImpl(uint8_t *srcBuffer, size_t srcBufferSize, std::vector<uint8_t> &dstBuffer) = 0;


    private:
};

class LZ4_Compressor : public AbstractCompressor
{

    public:
        LZ4_Compressor();
        virtual ~LZ4_Compressor();

    protected:
        bool compressImpl(uint8_t *srcBuffer, size_t srcBufferSize, std::vector<uint8_t> &dstBuffer);
        bool decompressImpl(uint8_t *srcBuffer, size_t srcBufferSize, std::vector<uint8_t> &dstBuffer);

    private:
};

}

#endif // COMPRESSOR_H
