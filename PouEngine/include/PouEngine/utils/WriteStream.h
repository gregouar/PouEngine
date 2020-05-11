#ifndef WRITESTREAM_H
#define WRITESTREAM_H

#include <memory>
#include <cstdint>

namespace pou
{

class BitWriter
{
    public:
        BitWriter(uint8_t *buffer, int bytes);
        virtual ~BitWriter();

        bool writeBits(uint32_t unsigned_value, int bits);
        void flush();

        void printBitCode(uint8_t v);
        void printBitCode(uint16_t v);
        void printBitCode(uint32_t v);
        void printBitCode(uint64_t v);

    private:
        uint8_t *m_buffer;
        int m_bytes;

        uint64_t m_scratch;
        int m_scratch_bits;
        int m_word_index;
};

class WriteStream
{
    public:
        enum {IsWriting = 1};
        enum {IsReading = 0};

        WriteStream();
        virtual ~WriteStream();

        void setBuffer(uint8_t *buffer, int bytes);

        int computeBytes(int bits);
        int bitsRequired(int32_t min, int32_t max);

        int serializeBits(int32_t value, int bits);
        int serializeInteger(int32_t value, int32_t min, int32_t max);
        int serializeBool(bool value);
        int serializeChar(char value);

    protected:

    private:
        std::unique_ptr<BitWriter> m_writer;
};

}


#endif // WRITESTREAM_H


