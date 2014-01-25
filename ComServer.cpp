#include "ComServer.h"
#include "nRF24L01.h"
#include "printf.h"

const uint64_t pipes[2] = {0X0101010101LL, 0X0202020202LL};

ComServer::ComServer():
    radio(9,10),
    size(0),
    index(0),
    status(ComServer::GetType),
    type(ComServer::TranssmitPacket),
    packet_index(0),
    buffer_index(0),
    send_ok(true)
{
    Serial.begin(115200);
    //printf_begin();
    //printf("\n\rRF24 server started/\n\r");
    
    radio.begin();
    //настройка параметров.
    radio.setChannel(66);
    radio.setDataRate(RF24_2MBPS);
    radio.setAutoAck(true);
    radio.setCRCLength(RF24_CRC_8);
    radio.setRetries(15,15);
    radio.setPayloadSize(32);
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1, pipes[1]);
    radio.startListening();
    //radio.printDetails();
}

void ComServer::run(void)
{
    uint8_t available;

    if ( available = Serial.available() )
    {
        while (available)
        {
            available--;
            uint8_t ch = Serial.read();

            //анализ данных
            //Сборка пакета
            if ( status == BuildData )
            {
                //Пакет на передачу.
                if ( type == TranssmitPacket )
                {
                    //игнорирование данных, т.к была потеря пакета.
                    if ( send_ok )
                    {
                        //Добавим заголоков в буффер
                        if (buffer_index == 0 )
                        {
                            //Добавим индекс пакета
                            buffer[buffer_index] = packet_index;
                            buffer_index++;

                            //в первом пакете посылаем размер.
                            if (packet_index == 0)
                            {
                                buffer[buffer_index] = size;
                                buffer_index++;
                            }
                            packet_index++;
                        }
                        
                        //add data to buffer
                        buffer[buffer_index] = ch;
                        buffer_index++;
                        index++;

                        //Пакет пролезает в payload
                        if (size <= 32 - 2)
                        {
                            //send packet
                            if (index == size)
                            {
                                radio.stopListening();
                                send_ok = radio.write(buffer, buffer_index);
                                radio.startListening();
                            }
                        }
                        //Посылка данных кусками
                        else if (buffer_index == 32)
                        {
                            if (packet_index == 1)
                                radio.stopListening();
                            send_ok = radio.write(buffer, buffer_index);
                            buffer_index = 0;
                        }
                        //Остаток.
                        else if (index == size)
                        {
                            send_ok = radio.write(buffer, buffer_index);
                            radio.startListening();
                        }
                    }
                    else
                    {
                        index++;
                    }
                }
                //Делаем ехо в ком порт.
                else if ( type == Echo )
                {
                    //отправим заголовок.
                    if ( index == 0 )
                    {
                        Serial.write(uint8_t(type));
                        Serial.write(size);
                    }

                    Serial.write(ch);
                    index++;
                }
                else
                {
                    index++;                
                }

                //завершение приёма
                if (index == size)
                {
                    status = GetType;

                    if ( type == TranssmitPacket )
                    {
                        //вернём результат.
                        Serial.write(uint8_t(type));
                        Serial.write(1);
                        Serial.write(uint8_t(send_ok));
                    }
                }
            }
            else if ( status == GetType )
            {
                type = CmdType(ch);
                status = GetSize;
            }
            else if ( status == GetSize )
            {
                size = ch;
                status = BuildData;

                //завершение обработку, нет данных.
                if (!size)
                    status = GetType;

                //инициализация данных
                index = 0;
                packet_index = 0;
                buffer_index = 0;
                send_ok = true;
            }
        }
    }
}
