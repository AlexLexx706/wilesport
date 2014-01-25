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

/*

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 

RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0X0101010101LL, 0X0202020202LL};

//
// Role management
//
// Set up role.    This sketch uses the same software for all the nodes
// in this system.    Doing so greatly simplifies testing.    
//

// The various roles supported by this sketch
typedef enum {role_ping_out = 1, role_pong_back, role_send_data, role_stop, role_read} role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};

// The role of the current running sketch
role_e role = role_pong_back;
uint8_t cur_char = 0;

void setup(void)
{
    //
    // Print preamble
    //
    Serial.begin(57600);
    printf_begin();
    printf("\n\rRF24/examples/GettingStarted/\n\r");
    printf("ROLE: %s\n\r",role_friendly_name[role]);
    printf("*** PRESS 'T' to begin transmitting to the other node\n\r");
    printf("*** PRESS 'S' to send message\n\r");

    //
    // Setup and configure rf radio
    //

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

    radio.printDetails();
}

void loop(void)
{
    //
    // Ping out role.    Repeatedly send the current time
    //
    if (role == role_ping_out)
    {
        // First, stop listening so we can talk.
        radio.stopListening();

        // Take the time, and send it.    This will block until complete
        unsigned long time = millis();
        printf("Now sending %lu...",time);
        bool ok = radio.write( &time, sizeof(unsigned long) );
        
        if (ok)
            printf("ok...");
        else
            printf("failed.\n\r");

        // Now, continue listening
        radio.startListening();

        // Wait here until we get a response, or timeout (250ms)
        unsigned long started_waiting_at = millis();
        bool timeout = false;
        while ( ! radio.available() && ! timeout )
            if (millis() - started_waiting_at > 10000 )
                timeout = true;

        // Describe the results
        if ( timeout )
        {
            printf("Failed, response timed out.\n\r");
        }
        else
        {
            // Grab the response, compare, and send to debugging spew
            unsigned long got_time;
            radio.read( &got_time, sizeof(unsigned long) );

            // Spew it
            printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
        }

        // Try again 1s later
        delay(1000);
    }
    
    if ( role == role_send_data )
    {
        radio.stopListening();

        // Take the time, and send it.    This will block until complete
        printf("Now sending %c...", cur_char);
        bool ok = radio.write(&cur_char, sizeof(cur_char));

        if (ok)
            printf("ok...\r\n");
        else
            printf("failed.\n\r");
      
        // Now, continue listening
        radio.startListening();
        role = role_stop;
    }

    //
    // Pong back role.    Receive each packet, dump it out, and send it back
    //

    if ( role == role_pong_back )
    {
        // if there is data ready
        if ( radio.available() )
        {
            // Dump the payloads until we've gotten everything
            unsigned long got_time;
            bool done = false;

            while (!done)
            {
                // Fetch the payload, and see if this was the last one.
                done = radio.read( &got_time, sizeof(unsigned long) );

                // Spew it
                printf("Got payload %lu...",got_time);

                // Delay just a little bit to let the other unit
                // make the transition to receiver
                delay(20);
            }

            // First, stop listening so we can talk
            radio.stopListening();

            // Send the final one back.
            radio.write( &got_time, sizeof(unsigned long) );
            printf("Sent response.\n\r");

            // Now, resume listening so we catch the next packets.
            radio.startListening();
        }
    }
    if ( role == role_read )
    {
        // if there is data ready
        if ( radio.available() )
        {
            unsigned long got_time;
            bool done = false;
            while (!done)
            {
                // Fetch the payload, and see if this was the last one.
                done = radio.read( &got_time, sizeof(unsigned long) );

                // Spew it
                printf("Got payload %lu...",got_time);

                // Delay just a little bit to let the other unit
                // make the transition to receiver
                delay(20);
            }
        }
    }
    //
    // Change roles
    //

    if ( Serial.available() )
    {
        cur_char = toupper(Serial.read());

        if ( cur_char == 'T' )
        {
            printf("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK\n\r");

            // Become the primary transmitter (ping out)
            role = role_ping_out;
            radio.openWritingPipe(pipes[0]);
            radio.openReadingPipe(1,pipes[1]);
        }
        else if ( cur_char == 'R' )
        {
            printf("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK\n\r");
            
            // Become the primary receiver (pong back)
            role = role_pong_back;
            radio.openWritingPipe(pipes[1]);
            radio.openReadingPipe(1,pipes[0]);
        }
        else if ( cur_char == 'L')
        {
            // Now, resume listening so we catch the next packets.
            radio.openWritingPipe(pipes[1]);
            radio.openReadingPipe(1,pipes[0]);
            radio.startListening();
            
            role = role_read;
        }
        else
        {
            // Become the primary transmitter (ping out)
            role = role_send_data;
            radio.openWritingPipe(pipes[0]);
            radio.openReadingPipe(1, pipes[1]);
        }
    }
}
// vim:cin:ai:sts=2 sw=2 ft=cpp

**/
