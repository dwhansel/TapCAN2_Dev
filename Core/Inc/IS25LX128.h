/**
  ******************************************************************************
  * @file    IS25LX.h
  * @modify  MCD Application Team
  * @brief   This file contains all the description of the
  *          IS25LX OSPI memory.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IS25LX128_H
#define IS25LX128_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* IS25LX Size configuration */
#define IS25LX_PAGE_SIZE                    (uint32_t)256    
#define IS25LX_SECTOR_SIZE                  (uint32_t)(128 * 1024)        
#define IS25LX_FLASH_SIZE                   (uint32_t)(128 * IS25LX_SECTOR_SIZE)  /* 128 Mbits => 16MBytes        */
   
/* IS25LX Comm config items */
#define DUMMY_CYCLES_READ_REG_OCTAL_DTR   8U
#define DUMMY_CYCLES_READ_MEM_OCTAL_DTR  16U
#define DUMMY_CYCLES_READ_ID_OCTAL_DTR  8U
#define DUMMY_CYCLES_WRITE_ENBL_OCTAL_DTR  0U
#define DUMMY_CYCLES_ERASE_MEM_OCTAL_DTR  0U
#define DUMMY_CYCLES_PROGRAM_MEM_OCTAL_DTR  0U

#define IS25LX_AUTOPOLLING_INTERVAL_TIME    0x10U

/* IS25LX Indirect SDR COMMANDS */
#define IS25LX_READ_STATUS_REG_CMD   0x05
#define IS25LX_WRITE_ENABLE_CMD      0x06

/* IS25LX Octal DDR COMMANDS */
#define IS25LX_OCTA_READ_STATUS_REG_CMD   0x0505   /*!< Octa Read Status Register */
#define IS25LX_OCTA_WRITE_ENABLE_CMD      0x0606
#define IS25LX_OCTA_PAGE_PROG_CMD         0x8e8e  /*!< Octa Page Program */    
#define IS25LX_OCTA_BULK_ERASE_CMD        0xc7c7  /*!< Octa Bulk Erase */
#define IS25LX_OCTA_READ_FLAG_REG_CMD     0x7070  /* Read Flag Status Register */
#define IS25LX_OCTA_SECTOR_ERASE_128K_CMD    0xDCDC
#define IS25LX_OCTA_SECTOR_ERASE_32K_CMD  0x5C5C
#define IS25LX_OCTA_SECTOR_ERASE_4K_CMD   0x2121
/* Error codes */
#define IS25LX_OK                           (0)
#define IS25LX_ERROR                        (1)

/* Status Register */
#define IS25LX_SR_WIP                               0x01U   /*!< Write in progress                                       */
#define IS25LX_SR_WEL                               0x02U   /*!< Write enable latch                                      */
#define IS25LX_SR_BP                                0x3CU   /*!< Block protected against program and erase operations    */

/* Flag Register */
#define IS25LX_FLAG_P_FAIL 0x10
#define IS25LX_FLAG_E_FAIL 0x20
#define IS25LX_FLAG_P_SUSP 0x04
#define IS25LX_FLAG_E_SUSP 0x40

extern uint8_t ExtFLASH_Init();
extern uint8_t IS25LX_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
extern uint8_t IS25LX_MassErase();
extern uint8_t IS25LX_EraseBlock(uint32_t BlockAddress);
extern uint8_t IS25LX_EnableMemoryMappedMode();
extern uint8_t IS25LX_EnableMemoryMappedModeSDR();
extern uint8_t IS25LX_GetStatus();
extern uint8_t IS25LX_ReadMemory_1S_1S_1S(uint32_t ReadAddr, uint32_t Size);
extern void Flash_MAXTRAN_Cfg();
extern void EnblFlash_MemMapped();

#ifdef __cplusplus
}
#endif

#endif