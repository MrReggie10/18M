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
  uint32_t jacob = *(__IO uint32_t *)0x08060004;
  uint32_t cameron = *(__IO uint32_t *)0x0806000C;
  uint32_t amy = *(__IO uint32_t *)0x08060014;
  uint32_t derek = *(__IO uint32_t *)0x0806001C;

  if(jacob != JACOB_UID || cameron != CAMERON_UID ||
	 amy != AMY_UID || derek != DEREK_UID) {
	return reset_accounts();
  }

  return 0;
}

uint32_t add_account(uint32_t fourbUID)
{
  uint32_t numUsers = *(__IO uint32_t *)0x08060000;
  uint32_t users[numUsers];
  uint32_t money[numUsers];
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

//  for(int i = 0; i<numberOfWords; i++)
//  {
//	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startSectorAddress, data[i]) == HAL_OK)
//	{
//	  startSectorAddress += 4;  // use StartPageAddress += 2 for half word and 8 for double word
//	}
//	else
//	{
//	  return HAL_FLASH_GetError();
//	}
//  }

  HAL_FLASH_Lock();

  return 0;
}

uint32_t get_money_in_account(uint32_t fourbUID);

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

  uint32_t addr = 0x08060000;

  // add number of accounts
  uint32_t numAcc = 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, numAcc) != HAL_OK)
  {
	return HAL_FLASH_GetError();
  }

  // write UID for Jacob
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, JACOB_UID) != HAL_OK) return HAL_FLASH_GetError();
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, 0x00000064) != HAL_OK) return HAL_FLASH_GetError();

  // write UID for Cameron
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, CAMERON_UID) != HAL_OK) return HAL_FLASH_GetError();
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, 0x00000064) != HAL_OK) return HAL_FLASH_GetError();

  // write UID for Amy
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, AMY_UID) != HAL_OK) return HAL_FLASH_GetError();
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, 0x00000064) != HAL_OK) return HAL_FLASH_GetError();

  // write UID for Derek
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, DEREK_UID) != HAL_OK) return HAL_FLASH_GetError();
  addr += 4;
  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, 0x00000064) != HAL_OK) return HAL_FLASH_GetError();

  HAL_FLASH_Lock();

  return 0;
}
