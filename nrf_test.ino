/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example for Getting Started with nRF24L01+ radios. 
 *
 * This is an example of how to use the RF24 class.    Write this sketch to two 
 * different nodes.    Put one of the nodes into 'transmit' mode by connecting 
 * with the serial monitor and sending a 'T'.    The ping node sends the current 
 * time to the pong node, which responds by sending the value back.    The ping 
 * node can then see how long the whole cycle took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "ComServer.h"

ComServer * server;

void setup(void)
{
    server = new ComServer();
}

void loop(void)
{
    server->run();
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
