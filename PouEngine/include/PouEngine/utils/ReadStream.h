#ifndef READSTREAM_H
#define READSTREAM_H

#include <memory>
#include <cstdint>

namespace pou
{

class BitReader
{
    public:
        BitReader(const uint32_t *buffer, int bytes);
        virtual ~BitReader();

        bool wouldReadPastEnd(int bits);
        uint32_t readBits(int bits);

    private:
        const uint32_t *m_buffer;
        int m_bytes;

        uint64_t m_scratch;
        int m_scratch_bits;
        int m_total_bits;
        int m_num_bits_read;
        int m_word_index;
};

class ReadStream
{
    public:
        enum {IsWriting = 0};
        enum {IsReading = 1};

        ReadStream();
        virtual ~ReadStream();

        void setBuffer(const uint32_t *buffer, int bytes);

        int computeBytes(int bits);
        int bitsRequired(int32_t min, int32_t max);

        int serializeBits(int32_t &value, int bits);
        int serializeInteger(int32_t &value, int32_t min, int32_t max);
        int serializeBool(bool &value);
        int serializeChar(char &value);

    protected:

    private:
        std::unique_ptr<BitReader> m_reader;
};

}

#endif // READSTREAM_H

