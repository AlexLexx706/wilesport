#include "ComServer.h"
#include "nRF24L01.h"
#include "printf.h"

const uint64_t pipes[2] = {0X0101010101LL, 0X0202020202LL};

ComServer::ComServer():
    radio(9,10),
    size(0),
    type(ComServer::TranssmitPacket),
    packet_index(0),
    buffer_index(0),
    send_ok(true)
{
    Serial.begin(115200);
   
    //настройка параметров.
    radio.begin();
    radio.setChannel(66);
    radio.setDataRate(RF24_2MBPS);
    radio.setAutoAck(true);
    radio.setCRCLength(RF24_CRC_8);
    radio.setRetries(15,15);
    radio.setPayloadSize(32);
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1, pipes[1]);
    radio.startListening();
}

void ComServer::run(void)
{
    if ( Serial.available() )
    {
        //1. читаем тип
        type = ComServer::CmdType(Serial.read());

        if (type == 0xff)
          return;
       
        while(!Serial.available()){}

        //2. читаем размер данных.        
        size = CmdType(Serial.read());

        //передача данных
        if (type == TranssmitPacket)
        {
            //читаем все данные из ком порта.
            for (uint8_t i = 0; i < size; i++ )
            {
                while(!Serial.available()){}
                temp_buffer[i] = Serial.read();
            }

            temp_buffer_index = 0;
            packet_index = 0;
            send_ok = true;
           
            //подготовка данных к передаче.
            for(;;)
            {
                //1. создадим заголовок пекета.
                buffer_index = 0;
                buffer[buffer_index] = packet_index;
                buffer_index++;
               
                //первый пакет
                if (packet_index == 0 )
                {
                    buffer[buffer_index] = size;
                    buffer_index++;
                    radio.stopListening();
                }
  
                //первый/паследний пакет.
                if ( size + buffer_index <= 32 )
                {
                    size = size + buffer_index;

                    while(buffer_index < size)
                    {
                        buffer[buffer_index] = temp_buffer[temp_buffer_index];
                        temp_buffer_index++;
                        buffer_index++;
                    }

                    //запись данных                    
                    send_ok = radio.write(buffer, buffer_index);
                    break;
                }
                //промежуточный пакет
                else
                {
                    while(buffer_index < 31)
                    {
                        buffer[buffer_index] = temp_buffer[temp_buffer_index];
                        temp_buffer_index++;
                        buffer_index++;
                    }
                    size = size - 31;
                    send_ok = radio.write(buffer, buffer_index);

                    //прекратим передачу пакетов.
                    if (!send_ok)
                        break;
                }
                
                //увеличим номер пакета.
                packet_index++;
            }
            radio.startListening();

            //запись резульата.
            Serial.write(type);
            Serial.write(1);
            Serial.write(uint8_t(send_ok));
        }
        //Делаем ехо в ком порт.
        else if ( type == Echo )
        {
            Serial.write(type);
            Serial.write(size);
            
            while(size)
            {
                while(!Serial.available()){}
                Serial.write(Serial.read());
                size--;
            }
        }
    }
}
