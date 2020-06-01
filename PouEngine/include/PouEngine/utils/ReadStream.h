#ifndef READSTREAM_H
#define READSTREAM_H

#include <memory>

#include "PouEngine/utils/Stream.h"

namespace pou
{

class BitReader
{
    public:
        BitReader(const uint8_t *buffer, int bytes);
        virtual ~BitReader();

        bool wouldReadPastEnd(int bits);
        uint32_t readBits(int bits);
        bool memcpy(uint8_t *data, int data_size/*, int bytes_shift*/);

    private:
        const uint8_t *m_buffer;
        int m_bytes;

        uint64_t m_scratch;
        int m_scratch_bits;
        //int m_total_bits;
        //int m_num_bits_read;
        int m_byte_index;
};

class ReadStream : public Stream
{
    public:
        ReadStream();
        virtual ~ReadStream();

        virtual bool isWriting();
        virtual bool isReading();

        virtual void setBuffer(std::vector<uint8_t> &buffer);
        virtual void setBuffer(uint8_t *buffer, int bytes);

        virtual bool memcpy(uint8_t *data, int data_size);
        virtual bool serializeBits(int32_t &value, int bits);

        virtual bool serializeInt(int32_t &value, int32_t min, int32_t max);
        virtual bool serializeUint32(uint32_t &value);
        virtual bool serializeFloat(float &value);
        virtual bool serializeFloat(float &value, float min, float max, uint8_t decimals);
        virtual bool serializeBool(bool &value);
        virtual bool serializeChar(char &value);

        virtual bool serializeString(std::string &str);

    protected:

    private:
        std::unique_ptr<BitReader> m_reader;
};

}

#endif // READSTREAM_H

