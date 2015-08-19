// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: t -*-

/// @file	GCS_MAVLink.cpp

/*
This provides some support code and variables for MAVLink enabled sketches

This firmware is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
*/

#include <FastSerial.h>
#include <AP_Common.h>
#include <GCS_MAVLink.h>


BetterStream	*mavlink_comm_0_port;

mavlink_system_t mavlink_system = {97,94};

static const uint8_t mavlink_message_crc_progmem[256] PROGMEM = MAVLINK_MESSAGE_CRCS;

uint8_t mavlink_get_message_crc(uint8_t msgid)
{
    return pgm_read_byte(&mavlink_message_crc_progmem[msgid]);
}
