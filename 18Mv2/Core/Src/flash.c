/*
 * flash.c
 *
 *  Created on: Jan 11, 2026
 *      Author: jacobyakubisin
 */

#include "flash.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"

uint32_t initialize_accounts()
{
  uint32_t jacob = *(__IO uint32_t *)(SECTOR_7_ADDR+0x4);
  uint32_t cameron = *(__IO uint32_t *)(SECTOR_7_ADDR+0xC);
  uint32_t amy = *(__IO uint32_t *)(SECTOR_7_ADDR+0x14);
  uint32_t derek = *(__IO uint32_t *)(SECTOR_7_ADDR+0x1C);

  if(jacob != JACOB_UID || cameron != CAMERON_UID ||
	 amy != AMY_UID || derek != DEREK_UID) {
	return reset_accounts();
  }

  return 0;
}

uint32_t add_account(uint32_t fourbUID)
{
  uint32_t numUsers = *(__IO uint32_t *)SECTOR_7_ADDR;
  numUsers++;
  uint32_t users[numUsers];
  uint32_t money[numUsers];
  uint32_t addr = SECTOR_7_ADDR+4;

  for(uint32_t i = 0; i < numUsers-1; i++)
  {
  	users[i] = *(__IO uint32_t *)(addr+(8*i));

  	if(users[i] == fourbUID) {
  	  return 0xFFFFFFFF;
  	}

  	money[i] = *(__IO uint32_t *)(addr+(8*i)+4);
  }
  users[numUsers-1] = fourbUID;
  addr += 4;

  money[numUsers-1] = 0x64;

  static FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SECTORError;

  HAL_FLASH_Unlock();

  EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInitStruct.Sector = FLASH_SECTOR_7;
  EraseInitStruct.NbSectors = 1;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
  {
	return HAL_FLASH_GetError();
  }

  addr = SECTOR_7_ADDR;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, numUsers) != HAL_OK) return HAL_FLASH_GetError();

  addr += 4;
  for(uint32_t i = 0; i<numUsers; i++)
  {
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, users[i]) != HAL_OK) return HAL_FLASH_GetError();
	addr += 4;
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, money[i]) != HAL_OK) return HAL_FLASH_GetError();
	addr += 4;
  }

  HAL_FLASH_Lock();

  return 0;
}

uint32_t get_money_in_account(uint32_t fourbUID)
{
  uint32_t numUsers = *(__IO uint32_t *)SECTOR_7_ADDR;
  uint32_t currentUser;
  uint32_t addr = SECTOR_7_ADDR+4;

  for(uint32_t i = 0; i < numUsers; i++)
  {
	currentUser = *(__IO uint32_t *)(addr+(8*i));
	if(currentUser == fourbUID) {
	  return *(__IO uint32_t *)(addr+4);
	}
  }

  return 0xFFFFFFFF;
}

uint32_t set_money_in_account(uint32_t fourbUID, uint32_t newMoney)
{
  uint32_t numUsers = *(__IO uint32_t *)SECTOR_7_ADDR;
  uint32_t users[numUsers];
  uint32_t money[numUsers];
  uint32_t addr = SECTOR_7_ADDR+4;

  for(uint32_t i = 0; i < numUsers; i++)
  {
	users[i] = *(__IO uint32_t *)addr;
	addr += 4;

	money[i] = *(__IO uint32_t *)addr;
	addr += 4;

	if(users[i] == fourbUID) {
	  money[i] = newMoney;
	}
  }

  static FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SECTORError;

  HAL_FLASH_Unlock();

  EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInitStruct.Sector = FLASH_SECTOR_7;
  EraseInitStruct.NbSectors = 1;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
  {
	return HAL_FLASH_GetError();
  }

  addr = SECTOR_7_ADDR;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, numUsers) != HAL_OK) return HAL_FLASH_GetError();

  addr += 4;
  for(uint32_t i = 0; i<numUsers; i++)
  {
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, users[i]) != HAL_OK) return HAL_FLASH_GetError();
	addr += 4;
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, money[i]) != HAL_OK) return HAL_FLASH_GetError();
	addr += 4;
  }

  HAL_FLASH_Lock();

  return 0;
}

uint32_t reset_accounts()
{
  static FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SECTORError;

  HAL_FLASH_Unlock();

  EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
  EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
  EraseInitStruct.Sector = FLASH_SECTOR_7;
  EraseInitStruct.NbSectors = 1;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
  {
	return HAL_FLASH_GetError();
  }

  uint32_t addr = SECTOR_7_ADDR;

  // add number of accounts
  uint32_t numAcc = 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, numAcc) != HAL_OK) return HAL_FLASH_GetError();

  // write UID for Jacob
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, JACOB_UID) != HAL_OK) return HAL_FLASH_GetError();
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, 0x00000100) != HAL_OK) return HAL_FLASH_GetError();

  // write UID for Cameron
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, CAMERON_UID) != HAL_OK) return HAL_FLASH_GetError();
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, 0x00000100) != HAL_OK) return HAL_FLASH_GetError();

  // write UID for Amy
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, AMY_UID) != HAL_OK) return HAL_FLASH_GetError();
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, 0x00000100) != HAL_OK) return HAL_FLASH_GetError();

  // write UID for Derek
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, DEREK_UID) != HAL_OK) return HAL_FLASH_GetError();
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, 0x00000100) != HAL_OK) return HAL_FLASH_GetError();

  HAL_FLASH_Lock();

  return 0;
}
