
#ifndef __COM_SERVER__
#define __COM_SERVER__

#include "RF24.h"

class ComServer
{
public:
    ComServer();
    void run(void);
private:
    enum CmdType{TranssmitPacket, Echo};

    RF24 radio;
    uint8_t size;
    CmdType type;
    uint8_t buffer[32];
    uint8_t buffer_index;
    uint8_t packet_index;
    uint8_t avalable_bytes;
    bool send_ok;

};

#endif // __COM_SERVER__


