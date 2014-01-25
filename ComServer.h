
#ifndef __COM_SERVER__
#define __COM_SERVER__

#include "RF24.h"

class ComServer
{
public:
    ComServer();
    void run(void);
private:
    enum Status{GetType, GetSize, BuildData};
    enum CmdType{TranssmitPacket, Echo};

    RF24 radio;
    uint8_t size;
    uint8_t index;
    Status status;
    CmdType type;
    uint8_t buffer[32];
    uint8_t buffer_index;
    uint8_t packet_index;
    bool send_ok;

};

#endif // __COM_SERVER__


