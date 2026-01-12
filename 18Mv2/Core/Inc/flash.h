/*
 * flash.h
 *
 *  Created on: Jan 11, 2026
 *      Author: jacobyakubisin
 */
#include "stdio.h"
#include "main.h"

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

static uint32_t JACOB_UID = 0x0A844CF3;
static uint32_t CAMERON_UID = 0x0ACD51F3;
static uint32_t AMY_UID = 0x0A844CF3;
static uint32_t DEREK_UID = 0x1A356AF3;

uint32_t initialize_accounts();

uint32_t add_account(uint32_t fourbUID);

uint32_t get_money_in_account(uint32_t fourbUID);

uint32_t reset_accounts();

#endif /* INC_FLASH_H_ */
