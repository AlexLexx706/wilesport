
#ifndef __MY_RF24_H__
#define __MY_RF24_H__

#include <RF24.h>
class MyRF24:public RF24
{
public:
    MyRF24(uint8_t _cepin, uint8_t _cspin);
    bool SendPacket(const uint8_t * data, int8_t data_size);
    uint8_t GetPacket(const uint8_t * data, int8_t data_size);

protected:
    uint8_t GetChecksum(const uint8_t* data, uint8_t count);
    bool SendPacket_internal(const uint8_t * data, int8_t data_size);
};

#endif // __MY_RF24_H__


