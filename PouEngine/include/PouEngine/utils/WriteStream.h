#ifndef WRITESTREAM_H
#define WRITESTREAM_H

#include <memory>
#include <cstdint>

#include "PouEngine/utils/Stream.h"

namespace pou
{

class BitWriter
{
    public:
        BitWriter(uint8_t *buffer, int bytes);
        virtual ~BitWriter();

        bool writeBits(uint32_t unsigned_value, int bits);
        void memcpy(const uint8_t *data, int data_size/*, int bytes_shift*/);
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
        int m_byte_index;
};

class WriteStream : public Stream
{
    public:
        WriteStream();
        virtual ~WriteStream();

        virtual bool isWriting();
        virtual bool isReading();

        virtual void setBuffer(std::vector<uint8_t> &buffer);
        virtual void setBuffer(uint8_t *buffer, int bytes);

        virtual void flush();

        virtual void memcpy(uint8_t *data, int data_size);
        virtual void const_memcpy(const uint8_t *data, int data_size);
        virtual void serializeBits(int32_t &value, int bits);
        virtual void serializeInt(int32_t &value, int32_t min, int32_t max);
        virtual void serializeFloat(float &value, float min, float max, uint8_t decimals);
        virtual void serializeBool(bool &value);
        virtual void serializeChar(char &value);

        virtual void serializeString(std::string &str);

    protected:

    private:
        std::unique_ptr<BitWriter> m_writer;
};

}


#endif // WRITESTREAM_H


