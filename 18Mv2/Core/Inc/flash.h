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

uint32_t initialize_accounts();

uint32_t add_account(uint32_t fourbUID);

uint32_t get_money_in_account(uint32_t fourbUID);

uint32_t set_money_in_account(uint32_t fourbUID, uint32_t newMoney);

uint32_t reset_accounts();

#endif /* INC_FLASH_H_ */
