#include "MyRF24.h"

MyRF24::MyRF24(uint8_t _cepin, uint8_t _cspin):RF24(_cepin, _cspin)
{
}

uint8_t MyRF24::GetChecksum(const uint8_t* Data, uint8_t DataCount)
{
    volatile uint8_t checksum = DataCount;
    uint8_t i;
    for (i = 0; i < DataCount; i++)
    {
        checksum += Data[i];
    }
    return ~checksum;
}


bool MyRF24::SendPacket_internal(const uint8_t * data, int8_t data_size)
{
    int8_t temp_data[32];
    temp_data[0] = data_size;
    uint8_t i = 1;
    uint8_t j = 1;

    for (; i < data_size + 1; i++, j++)
        temp_data[i] = data[j];

    temp_data[i] = GetChecksum(data, data_size);
    return write(temp_data, data_size + 2);
}


bool MyRF24::SendPacket(const uint8_t * data, int8_t data_size)
{
    if ( data_size + 2 <= 32 )
        return SendPacket_internal(data, data_size);
    else
    {
        uint8_t cur_size;
        do 
        {
            cur_size = data_size ? data_size + 2 <= 32 : 30;

            if ( !SendPacket_internal(data, cur_size) )
                return false;
            data = data + cur_size;
            data_size = data_size - cur_size;
        }
        while(data_size);
        return true;
    }
}


uint8_t MyRF24::GetPacket(const uint8_t * data, int8_t data_size)
{

}
