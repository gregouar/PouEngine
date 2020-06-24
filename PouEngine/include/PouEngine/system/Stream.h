#ifndef STREAM_H
#define STREAM_H

#include <vector>
#include <cstdint>
#include <string>

namespace pou
{

class Stream
{
    public:
        Stream();
        virtual ~Stream();

        virtual bool isWriting() = 0;
        virtual bool isReading() = 0;

        virtual void setBuffer(std::vector<uint8_t> &buffer);
        virtual void setBuffer(uint8_t *buffer, int bytes) = 0;

        int computeBytes();
        int bitsRequired(int32_t min, int32_t max);
        virtual void flush();

        virtual bool memcpy(uint8_t *data, int data_size) = 0;
        virtual bool serializeBits(int32_t &value, int bits) = 0;

        virtual bool serializeInt(int32_t &value, int32_t min, int32_t max) = 0;
        virtual bool serializeUint32(uint32_t &value) = 0;
        virtual bool serializeFloat(float &value) = 0;
        virtual bool serializeFloat(float &value, float min, float max, uint8_t decimals) = 0;
        virtual bool serializeBool(bool &value) = 0;
        virtual bool serializeChar(char &value) = 0;

        virtual bool serializeString(std::string &str) = 0;

        void padZeroes();

    protected:
        int m_bits;

    private:
};

}

#endif // STREAM_H
