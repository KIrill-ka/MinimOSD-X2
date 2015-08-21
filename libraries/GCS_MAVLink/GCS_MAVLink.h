// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: t -*-

/// @file	GCS_MAVLink.h
/// @brief	One size fits all header for MAVLink integration.

#ifndef GCS_MAVLink_h
#define GCS_MAVLink_h

#include <BetterStream.h>

// we have separate helpers disabled to make it possible
// to select MAVLink 1.0 in the arduino GUI build
//#define MAVLINK_SEPARATE_HELPERS

#include "include/mavlink/v1.0/ardupilotmega/version.h"

// this allows us to make mavlink_message_t much smaller
//
// Mavlink uses two mavlink_message_t structures - one as parse
// buffer, one is in stack.
// Normal size is 255. Value of 101 will not allow to read
// large messages which we don't need.
// FILE_TRANSFER_PROTOCOL (254), V2_EXTENSION (254),
// ENCAPSULATED_DATA (255)
// LOCAL_POSITION_NED_COV (181), GLOBAL_POSITION_INT_COV (185)
#define MAVLINK_MAX_PAYLOAD_LEN 101

#define MAVLINK_COMM_NUM_BUFFERS 1
#include "include/mavlink/v1.0/mavlink_types.h"

/// MAVLink stream used for HIL interaction
extern BetterStream	*mavlink_comm_0_port;

/// MAVLink system definition
extern mavlink_system_t mavlink_system;

/// Send a byte to the nominated MAVLink channel
///
/// @param chan		Channel to send to
/// @param ch		Byte to send
///
static inline void comm_send_ch(mavlink_channel_t chan, uint8_t ch)
{
	if(chan != MAVLINK_COMM_0) return;
	mavlink_comm_0_port->write(ch);
}

/// Read a byte from the nominated MAVLink channel
///
/// @param chan		Channel to receive on
/// @returns		Byte read
///
static inline uint8_t comm_receive_ch(mavlink_channel_t chan)
{
	if(chan != MAVLINK_COMM_0) return 0;
	return mavlink_comm_0_port->read();
}

/// Check for available data on the nominated MAVLink channel
///
/// @param chan		Channel to check
/// @returns		Number of bytes available
static inline uint16_t comm_get_available(mavlink_channel_t chan)
{
	if(chan != MAVLINK_COMM_0) return 0;
	return mavlink_comm_0_port->available();
}


/// Check for available transmit space on the nominated MAVLink channel
///
/// @param chan		Channel to check
/// @returns		Number of bytes available, -1 for error
static inline int comm_get_txspace(mavlink_channel_t chan)
{
	if(chan != MAVLINK_COMM_0) return -1;
	return mavlink_comm_0_port->txspace();
}

uint8_t mavlink_get_message_crc(uint8_t msgid);

#define MAVLINK_MESSAGE_CRC(msgid) mavlink_get_message_crc(msgid)
#define MAVLINK_USE_CONVENIENCE_FUNCTIONS
#include "include/mavlink/v1.0/ardupilotmega/mavlink.h"

uint8_t mavlink_check_target(uint8_t sysid, uint8_t compid);

// return a MAVLink variable type given a AP_Param type
uint8_t mav_var_type(enum ap_var_type t);

#endif // GCS_MAVLink_h
