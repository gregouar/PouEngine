#ifndef STREAM_H
#define STREAM_H

#include <cstdint>

namespace pou
{

class Stream
{
    public:
        Stream();
        virtual ~Stream();

        virtual bool isWriting() = 0;
        virtual bool isReading() = 0;

        virtual void setBuffer(uint8_t *buffer, int bytes) = 0;

        int computeBytes();
        int bitsRequired(int32_t min, int32_t max);
        virtual void flush();

        virtual void memcpy(uint8_t *data, int data_size) = 0;
        virtual void serializeBits(int32_t &value, int bits) = 0;
        virtual void serializeInt(int32_t &value, int32_t min, int32_t max) = 0;
        virtual void serializeBool(bool &value) = 0;
        virtual void serializeChar(char &value) = 0;

        void padZeroes();

    protected:
        int m_bits;

    private:
};

}

#endif // STREAM_H
