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
        bool memcpy(const uint8_t *data, int data_size/*, int bytes_shift*/);
        void flush();

        static void printBitCode(uint8_t v);
        static void printBitCode(uint16_t v);
        static void printBitCode(uint32_t v);
        static void printBitCode(uint64_t v);

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

        virtual bool memcpy(uint8_t *data, int data_size);
        virtual bool const_memcpy(const uint8_t *data, int data_size);
        virtual bool serializeBits(int32_t &value, int bits);
        virtual bool serializeInt(int32_t &value, int32_t min, int32_t max);
        virtual bool serializeFloat(float &value);
        virtual bool serializeFloat(float &value, float min, float max, uint8_t decimals);
        virtual bool serializeBool(bool &value);
        virtual bool serializeChar(char &value);

        virtual bool serializeString(std::string &str);

    protected:

    private:
        std::unique_ptr<BitWriter> m_writer;
};

}


#endif // WRITESTREAM_H


