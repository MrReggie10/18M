/*
 * rfid.c
 *
 *  Created on: Jan 13, 2026
 *      Author: jacobyakubisin
 */
#include "rfid.h"
#include <string.h>
#include "claude_lcd.h"

// ACK frame
static const uint8_t PN532_ACK[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};

static UART_HandleTypeDef *rfid_uart;  // Private pointer to SPI handle

void pn532_SetUART(UART_HandleTypeDef *huart) {
    rfid_uart = huart;
}

// Helper: Calculate checksum
static uint8_t pn532_checksum(const uint8_t *data, uint8_t len) {
    uint8_t sum = 0;
    for (uint8_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return (~sum + 1);
}

// Helper: Wait for PN532 to be ready
static int pn532_wait_ready(pn532_t *dev, uint32_t timeout_ms) {
    uint8_t status;
    uint32_t tickstart = HAL_GetTick();

    while ((HAL_GetTick() - tickstart) < timeout_ms) {
        if (HAL_I2C_Master_Receive(dev->hi2c, PN532_I2C_ADDRESS, &status, 1,
                                   PN532_I2C_TIMEOUT) == HAL_OK) {
            if (status == 0x01) {
                return PN532_OK;
            }
        }
        HAL_Delay(10);
    }
    return PN532_TIMEOUT;
}

// Helper: Read ACK frame
static int pn532_read_ack(pn532_t *dev) {
    uint8_t ack[7];

    HAL_Delay(PN532_ACK_WAIT_TIME);

    if (pn532_wait_ready(dev, 1000) != PN532_OK) {
        return PN532_TIMEOUT;
    }

    if (HAL_I2C_Master_Receive(dev->hi2c, PN532_I2C_ADDRESS, ack, 7,
                               PN532_I2C_TIMEOUT) != HAL_OK) {
        return PN532_ERROR;
    }

    if (memcmp(&ack[1], PN532_ACK, 6) != 0) {
    	char buf[15];
		sprintf(buf, "%x\r\n", ack[0]);
		HAL_UART_Transmit(rfid_uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
		sprintf(buf, "%x\r\n", ack[1]);
		HAL_UART_Transmit(rfid_uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
		sprintf(buf, "%x\r\n", ack[2]);
		HAL_UART_Transmit(rfid_uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
		sprintf(buf, "%x\r\n", ack[3]);
		HAL_UART_Transmit(rfid_uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
		sprintf(buf, "%x\r\n", ack[4]);
		HAL_UART_Transmit(rfid_uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
		sprintf(buf, "%x\r\n", ack[5]);
		HAL_UART_Transmit(rfid_uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
		sprintf(buf, "%x\r\n", ack[6]);
		HAL_UART_Transmit(rfid_uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
//    	ST7796S_DrawString(10, 10, buf, &Font24, WHITE, BLACK);
        return PN532_ERROR;
    }

    return PN532_OK;
}

// Helper: Write command to PN532
static int pn532_write_command(pn532_t *dev, const uint8_t *cmd, uint8_t cmd_len) {
    uint8_t frame[64];
    uint8_t idx = 0;

    // Build frame: PREAMBLE + START + LEN + LCS + TFI + DATA + DCS + POSTAMBLE
    frame[idx++] = PN532_PREAMBLE;
    frame[idx++] = PN532_STARTCODE1;
    frame[idx++] = PN532_STARTCODE2;

    uint8_t length = cmd_len + 1;  // +1 for TFI
    frame[idx++] = length;
    frame[idx++] = (~length + 1);  // Length checksum

    frame[idx++] = PN532_HOSTTOPN532;

    for (uint8_t i = 0; i < cmd_len; i++) {
        frame[idx++] = cmd[i];
    }

    uint8_t dcs = PN532_HOSTTOPN532;
    for (uint8_t i = 0; i < cmd_len; i++) {
        dcs += cmd[i];
    }
    frame[idx++] = (~dcs + 1);

    frame[idx++] = PN532_POSTAMBLE;

    // Send frame
    if (HAL_I2C_Master_Transmit(dev->hi2c, PN532_I2C_ADDRESS, frame, idx,
                                PN532_I2C_TIMEOUT) != HAL_OK) {
    	ST7796S_DrawString(10, 10, "Nuh uh transmit", &Font24, WHITE, BLACK);
        return PN532_ERROR;
    }

    // Wait for and read ACK
    return pn532_read_ack(dev);
}

// Helper: Read response from PN532
static int pn532_read_response(pn532_t *dev, uint8_t *buf, uint8_t len, uint32_t timeout_ms) {
	if (pn532_wait_ready(dev, timeout_ms) != PN532_OK) {
	  return PN532_TIMEOUT;
	}

    uint8_t frame[64];
    uint8_t frame_len = len + 9;  // Data + overhead

    if (HAL_I2C_Master_Receive(dev->hi2c, PN532_I2C_ADDRESS, frame, frame_len,
                               PN532_I2C_TIMEOUT) != HAL_OK) {
    	ST7796S_DrawString(10, 10, "Nuh uh receive", &Font24, WHITE, BLACK);
        return PN532_ERROR;
    }

    // Verify frame structure
    if (frame[1] != PN532_PREAMBLE || frame[2] != PN532_STARTCODE1 ||
        frame[3] != PN532_STARTCODE2) {
    	char buf[15];
		sprintf(buf, "%x\r\n", frame[0]);
		HAL_UART_Transmit(rfid_uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
		sprintf(buf, "%x\r\n", frame[1]);
		HAL_UART_Transmit(rfid_uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
		sprintf(buf, "%x\r\n", frame[2]);
		HAL_UART_Transmit(rfid_uart, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);
        return PN532_ERROR;
    }

    uint8_t length = frame[4];
    uint8_t lcs = frame[5];

    if ((uint8_t)(length + lcs) != 0) {
    	ST7796S_DrawString(10, 10, "Nuh uh wtf", &Font24, WHITE, BLACK);
        return PN532_ERROR;
    }

    if (frame[6] != PN532_PN532TOHOST) {
    	ST7796S_DrawString(10, 10, "Nuh uh host", &Font24, WHITE, BLACK);
        return PN532_ERROR;
    }

    // Copy data
    memcpy(buf, &frame[7], len);

    return PN532_OK;
}

// Initialize PN532
int pn532_init(pn532_t *dev, I2C_HandleTypeDef *hi2c) {
    dev->hi2c = hi2c;

    HAL_Delay(200);  // Power-up delay

    // Wake up PN532
    uint8_t dummy = 0x00;
    HAL_I2C_Master_Transmit(dev->hi2c, PN532_I2C_ADDRESS, &dummy, 1, PN532_I2C_TIMEOUT);
    HAL_Delay(50);

    // Get firmware version to verify communication
    uint32_t version;
    if (pn532_get_firmware_version(dev, &version) != PN532_OK) {
        return PN532_ERROR;
    }

    // Configure SAM (Security Access Module)
    if (pn532_sam_config(dev) != PN532_OK) {
    	ST7796S_DrawString(10, 10, "Nuh uh sam", &Font24, WHITE, BLACK);
        return PN532_ERROR;
    }

    return PN532_OK;
}

// Get firmware version
int pn532_get_firmware_version(pn532_t *dev, uint32_t *version) {
    uint8_t cmd[] = {PN532_CMD_GETFIRMWAREVERSION};
    uint8_t response[4];

    if (pn532_write_command(dev, cmd, sizeof(cmd)) != PN532_OK) {

        return PN532_ERROR;
        return PN532_ERROR;
    }

    if (pn532_read_response(dev, response, sizeof(response), 1000) != PN532_OK) {
        return PN532_ERROR;
        return PN532_ERROR;
    }

    if (version != NULL) {
        *version = ((uint32_t)response[0] << 24) |
                   ((uint32_t)response[1] << 16) |
                   ((uint32_t)response[2] << 8) |
                   response[3];
    }

    return PN532_OK;
}

// Configure SAM
int pn532_sam_config(pn532_t *dev) {
    uint8_t cmd[] = {
        PN532_CMD_SAMCONFIGURATION,
        0x01,  // Normal mode
        0x14,  // Timeout (20ms * 50 = 1 second)
        0x01   // Use IRQ pin
    };
    uint8_t response[1];

    if (pn532_write_command(dev, cmd, sizeof(cmd)) != PN532_OK) {
        return PN532_ERROR;
    }

    if (pn532_read_response(dev, response, sizeof(response), 1000) != PN532_OK) {
        return PN532_ERROR;
    }

    return PN532_OK;
}

// Read passive target (RFID tag)
int pn532_read_passive_target(pn532_t *dev, pn532_tag_info_t *tag_info, uint32_t timeout_ms) {
    uint8_t cmd[] = {
        PN532_CMD_INLISTPASSIVETARGET,
        0x01,  // Max 1 card
        0x00   // 106 kbps type A (ISO/IEC14443 Type A)
    };
    uint8_t response[32];

    if (pn532_write_command(dev, cmd, sizeof(cmd)) != PN532_OK) {
    	ST7796S_DrawString(10, 10, "Nuh uh write", &Font24, WHITE, BLACK);
        return PN532_ERROR;
    }

    if (pn532_read_response(dev, response, sizeof(response), timeout_ms) != PN532_OK) {
        return PN532_TIMEOUT;
    }

    // Check if tag was found
    if (response[1] != 0x01) {  // Number of targets
        return PN532_NO_TAG;
    }

    // Parse response
    tag_info->atqa = (response[3] << 8) | response[4];
    tag_info->sak = response[5];
    tag_info->uid_len = response[6];

    if (tag_info->uid_len > PN532_MAX_UID_LENGTH) {
    	ST7796S_DrawString(10, 10, "Nuh uh uidlen", &Font24, WHITE, BLACK);
        return PN532_ERROR;
    }

    memcpy(tag_info->uid, &response[7], tag_info->uid_len);

    return PN532_OK;
}
