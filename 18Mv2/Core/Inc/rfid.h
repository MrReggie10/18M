/*
 * rfid.h
 *
 *  Created on: Jan 13, 2026
 *      Author: jacobyakubisin
 */

#ifndef INC_RFID_H_
#define INC_RFID_H_

#include "stdio.h"
#include "main.h"

void rfid_init();

uint8_t *rfid_read();

#define PN532_PREAMBLE (0x00)   ///< Command sequence start, byte 1/3
#define PN532_STARTCODE1 (0x00) ///< Command sequence start, byte 2/3
#define PN532_STARTCODE2 (0xFF) ///< Command sequence start, byte 3/3
#define PN532_POSTAMBLE (0x00)  ///< EOD

#define PN532_HOSTTOPN532 (0xD4) ///< Host-to-PN532
#define PN532_PN532TOHOST (0xD5) ///< PN532-to-host

#define PN532_COMMAND_GETFIRMWAREVERSION (0x02)    ///< Get firmware version
#define PN532_COMMAND_INLISTPASSIVETARGET (0x4A)   ///< List passive target
#define PN532_COMMAND_SAMCONFIGURATION (0x14)      ///< SAM configuration

#define PN532_I2C_ADDRESS (0x48 >> 1) ///< Default I2C address
#define PN532_I2C_READBIT (0x01)      ///< Read bit
#define PN532_I2C_BUSY (0x00)         ///< Busy
#define PN532_I2C_READY (0x01)        ///< Ready
#define PN532_I2C_READYTIMEOUT (20)   ///< Ready timeout

#define PN532_MIFARE_ISO14443A (0x00) ///< MiFare

#define PN532DEBUGPRINT Serial ///< Fixed name for debug Serial instance

#endif /* INC_RFID_H_ */
