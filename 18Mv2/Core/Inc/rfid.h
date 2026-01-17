/*
 * rfid.h
 *
 *  Created on: Jan 13, 2026
 *      Author: jacobyakubisin
 */

#ifndef INC_RFID_H_
#define INC_RFID_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

// PN532 Commands
#define PN532_CMD_GETFIRMWAREVERSION    0x02
#define PN532_CMD_SAMCONFIGURATION      0x14
#define PN532_CMD_INLISTPASSIVETARGET   0x4A

// PN532 Frame identifiers
#define PN532_PREAMBLE                  0x00
#define PN532_STARTCODE1                0x00
#define PN532_STARTCODE2                0xFF
#define PN532_POSTAMBLE                 0x00
#define PN532_HOSTTOPN532               0xD4
#define PN532_PN532TOHOST               0xD5

// I2C Address (7-bit address shifted left)
#define PN532_I2C_ADDRESS               (0x24 << 1)

// Timeouts
#define PN532_I2C_TIMEOUT               100
#define PN532_ACK_WAIT_TIME             50

// Max UID length
#define PN532_MAX_UID_LENGTH            10

// Return codes
#define PN532_OK                        0
#define PN532_ERROR                     -1
#define PN532_TIMEOUT                   -2
#define PN532_NO_TAG                    -3

// PN532 handle structure
typedef struct {
    I2C_HandleTypeDef *hi2c;
} pn532_t;

// Tag information structure
typedef struct {
    uint8_t uid[PN532_MAX_UID_LENGTH];
    uint8_t uid_len;
    uint8_t sak;
    uint16_t atqa;
} pn532_tag_info_t;

// Public API
void pn532_SetI2C(I2C_HandleTypeDef *hi2c);
int pn532_init(pn532_t *dev, I2C_HandleTypeDef *hi2c);
int pn532_get_firmware_version(pn532_t *dev, uint32_t *version);
int pn532_sam_config(pn532_t *dev);
int pn532_read_passive_target(pn532_t *dev, pn532_tag_info_t *tag_info, uint32_t timeout_ms);

#endif // PN532_H
