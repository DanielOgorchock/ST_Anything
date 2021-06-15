/*
  wifi_spi.h - Library for Arduino Wifi shield.
  Copyright (c) 2018 Arduino SA. All rights reserved.
  Copyright (c) 2011-2014 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef WiFi_Spi_h
#define WiFi_Spi_h

#include <inttypes.h>
#include "utility/wl_definitions.h"

#define CMD_FLAG        0
#define REPLY_FLAG      1<<7
#define DATA_FLAG 		0x40

#define WIFI_SPI_ACK        1
#define WIFI_SPI_ERR        0xFF

#define TIMEOUT_CHAR    1000

//#define	MAX_SOCK_NUM		4	/**< Maxmium number of socket  */
#define NO_SOCKET_AVAIL     255

#define START_CMD   0xE0
#define END_CMD     0xEE
#define ERR_CMD   	0xEF
#define CMD_POS			1		// Position of Command OpCode on SPI stream
#define PARAM_LEN_POS 	2		// Position of Param len on SPI stream

  
enum {
	SET_NET_CMD 		= 0x10,
	SET_PASSPHRASE_CMD	= 0x11,
	SET_KEY_CMD	        = 0x12,
//	TEST_CMD	        = 0x13,
	SET_IP_CONFIG_CMD	= 0x14,
	SET_DNS_CONFIG_CMD	= 0x15,
	SET_HOSTNAME_CMD	= 0x16,
	SET_POWER_MODE_CMD	= 0x17,
	SET_AP_NET_CMD		= 0x18,
	SET_AP_PASSPHRASE_CMD = 0x19,
	SET_DEBUG_CMD		= 0x1A,
	GET_TEMPERATURE_CMD = 0x1B,
	GET_REASON_CODE_CMD = 0x1F,

	GET_CONN_STATUS_CMD	= 0x20,
	GET_IPADDR_CMD		= 0x21,
	GET_MACADDR_CMD		= 0x22,
	GET_CURR_SSID_CMD	= 0x23,
	GET_CURR_BSSID_CMD	= 0x24,
	GET_CURR_RSSI_CMD	= 0x25,
	GET_CURR_ENCT_CMD	= 0x26,
	SCAN_NETWORKS		= 0x27,
	START_SERVER_TCP_CMD= 0x28,
	GET_STATE_TCP_CMD   = 0x29,
	DATA_SENT_TCP_CMD	= 0x2A,
    AVAIL_DATA_TCP_CMD	= 0x2B,
    GET_DATA_TCP_CMD	= 0x2C,
    START_CLIENT_TCP_CMD= 0x2D,
    STOP_CLIENT_TCP_CMD = 0x2E,
    GET_CLIENT_STATE_TCP_CMD= 0x2F,
    DISCONNECT_CMD		= 0x30,
//	GET_IDX_SSID_CMD	= 0x31,
	GET_IDX_RSSI_CMD	= 0x32,
	GET_IDX_ENCT_CMD	= 0x33,
	REQ_HOST_BY_NAME_CMD= 0x34,
	GET_HOST_BY_NAME_CMD= 0x35,
	START_SCAN_NETWORKS	= 0x36,
	GET_FW_VERSION_CMD	= 0x37,
//	GET_TEST_CMD		= 0x38,
	SEND_DATA_UDP_CMD	= 0x39,
	GET_REMOTE_DATA_CMD = 0x3A,
	GET_TIME_CMD		= 0x3B,
	GET_IDX_BSSID		= 0x3C,
	GET_IDX_CHANNEL_CMD = 0x3D,
	PING_CMD			= 0x3E,
	GET_SOCKET_CMD		= 0x3F,

    // All command with DATA_FLAG 0x40 send a 16bit Len
	SET_ENT_CMD			= 0x40,

	SEND_DATA_TCP_CMD		= 0x44,
    GET_DATABUF_TCP_CMD		= 0x45,
    INSERT_DATABUF_CMD		= 0x46,

    // regular format commands
    SET_PIN_MODE		= 0x50,
    SET_DIGITAL_WRITE	= 0x51,
    SET_ANALOG_WRITE	= 0x52,

    // regular format commands
    WRITE_FILE			= 0x60,
    READ_FILE			= 0x61,
    DELETE_FILE			= 0x62,
    EXISTS_FILE			= 0x63,
    DOWNLOAD_FILE		= 0x64,
    APPLY_OTA_COMMAND	= 0x65,
	RENAME_FILE			= 0x66,
	DOWNLOAD_OTA		= 0x67,
};


enum wl_tcp_state {
  CLOSED      = 0,
  LISTEN      = 1,
  SYN_SENT    = 2,
  SYN_RCVD    = 3,
  ESTABLISHED = 4,
  FIN_WAIT_1  = 5,
  FIN_WAIT_2  = 6,
  CLOSE_WAIT  = 7,
  CLOSING     = 8,
  LAST_ACK    = 9,
  TIME_WAIT   = 10
};


enum numParams{
    PARAM_NUMS_0,
    PARAM_NUMS_1,
    PARAM_NUMS_2,
    PARAM_NUMS_3,
    PARAM_NUMS_4,
    PARAM_NUMS_5,
    PARAM_NUMS_6,
    MAX_PARAM_NUMS
};

#define MAX_PARAMS MAX_PARAM_NUMS-1
#define PARAM_LEN_SIZE 1

typedef struct  __attribute__((__packed__))
{
	uint8_t     paramLen;
	char*	    param;
}tParam;

typedef struct  __attribute__((__packed__))
{
	uint16_t     dataLen;
	char*	     data;
}tDataParam;


typedef struct  __attribute__((__packed__))
{
	unsigned char	cmd;
	unsigned char	tcmd;
	unsigned char	nParam;
	tParam	params[MAX_PARAMS];
}tSpiMsg;

typedef struct  __attribute__((__packed__))
{
	unsigned char	cmd;
	unsigned char	tcmd;
	unsigned char	nParam;
	tDataParam		params[MAX_PARAMS];
}tSpiMsgData;


typedef struct  __attribute__((__packed__))
{
	unsigned char	cmd;
	unsigned char	tcmd;
	//unsigned char	totLen;
	unsigned char	nParam;
}tSpiHdr;

typedef struct  __attribute__((__packed__))
{
	uint8_t     paramLen;
	uint32_t	param;
}tLongParam;

typedef struct  __attribute__((__packed__))
{
	uint8_t     paramLen;
	uint16_t	param;
}tIntParam;

typedef struct  __attribute__((__packed__))
{
	uint8_t     paramLen;
	uint8_t	param;
}tByteParam;

#endif
