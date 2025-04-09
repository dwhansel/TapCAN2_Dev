/**
 ******************************************************************************
 * @file    IS25LX.c
 * @modify  MCD Application Team
 * @brief   This file provides the IS25LX OSPI drivers.
 ******************************************************************************
 * IS25LX action :
 *   STR Octal IO protocol (SOPI) and DTR Octal IO protocol (DOPI) bits of
 *   Configuration Register 2 :
 *     DOPI = 1 and SOPI = 0: Operates in DTR Octal IO protocol (accepts 8-8-8 commands)
 *     DOPI = 0 and SOPI = 1: Operates in STR Octal IO protocol (accepts 8-8-8 commands)
 *     DOPI = 0 and SOPI = 0: Operates in Single IO protocol (accepts 1-1-1 commands)
 *   Enter SOPI mode by configuring DOPI = 0 and SOPI = 1 in CR2-Addr0
 *   Exit SOPI mode by configuring DOPI = 0 and SOPI = 0 in CR2-Addr0
 *   Enter DOPI mode by configuring DOPI = 1 and SOPI = 0 in CR2-Addr0
 *   Exit DOPI mode by configuring DOPI = 0 and SOPI = 0 in CR2-Addr0
 *
 *   Memory commands support STR(Single Transfer Rate) &
 *   DTR(Double Transfer Rate) modes in OPI
 *
 *   Memory commands support STR(Single Transfer Rate) &
 *   DTR(Double Transfer Rate) modes in SPI
 *
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

/* Includes ------------------------------------------------------------------*/
#include "IS25LX128.h"
#include "octospi.h"
#include <string.h>

/* Private variables ---------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/
uint8_t OCTOSPI1_Init();
uint8_t ResetMemory(OSPI_HandleTypeDef *);
uint8_t SetupDummyCycles(OSPI_HandleTypeDef *);
uint8_t SetupOctalDDRMode(OSPI_HandleTypeDef *);
uint8_t AutoPollingMemReady_1S_0_1S(OSPI_HandleTypeDef *Ctx);
uint8_t AutoPollingWriteEnabled_1S_0_1S(OSPI_HandleTypeDef *Ctx);
uint8_t AutoPollingWriteEnabledOctal(OSPI_HandleTypeDef *Ctx);
uint8_t WriteEnable_1S_0_0(OSPI_HandleTypeDef *Ctx);
uint8_t WriteEnableOctal(OSPI_HandleTypeDef *Ctx);
uint8_t PageProgram_1S_8S_8S(OSPI_HandleTypeDef *Ctx, uint8_t *pData, uint32_t WriteAddr, uint32_t Size);
uint8_t ReadFlagRegister(OSPI_HandleTypeDef *Ctx, uint8_t *Value);
uint8_t ReadStatusRegister(OSPI_HandleTypeDef *Ctx, uint8_t *Value);
uint8_t EraseSector_1S_1S_0(OSPI_HandleTypeDef* Ctx, uint32_t BlockAddress);
uint8_t ChipErase(OSPI_HandleTypeDef *Ctx);
uint8_t Enter_4Byte_Address_Mode(OSPI_HandleTypeDef *);

/* Polling WIP(Write In Progress) bit to become 0  */
uint8_t AutoPollingMemReady_1S_0_1S(OSPI_HandleTypeDef *Ctx)
{
  OSPI_RegularCmdTypeDef  s_command = {0};
  OSPI_AutoPollingTypeDef s_config = {0};

  /* Configure automatic polling mode to wait for memory ready */
  s_command.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  s_command.FlashId            = HAL_OSPI_FLASH_ID_1;
  s_command.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  s_command.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  s_command.Instruction        = 0x05; //IS25LX status register read
  s_command.AddressMode        = HAL_OSPI_ADDRESS_NONE;    
  s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = HAL_OSPI_DATA_1_LINE;
  s_command.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  s_command.DummyCycles        = 0;
  s_command.NbData             = 1; 
  s_command.DQSMode            = HAL_OSPI_DQS_DISABLE;
  s_command.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  s_config.Match         = 0U;
  s_config.Mask          = IS25LX_SR_WIP;
  s_config.MatchMode     = HAL_OSPI_MATCH_MODE_AND;
  s_config.Interval      = IS25LX_AUTOPOLLING_INTERVAL_TIME;
  s_config.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_OSPI_Command(Ctx, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  if (HAL_OSPI_AutoPolling(Ctx, &s_config, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  return IS25LX_OK;
}

uint8_t AutoPollingWriteEnabled_1S_0_1S(OSPI_HandleTypeDef *Ctx)
{
  OSPI_RegularCmdTypeDef  s_command = {0};
  OSPI_AutoPollingTypeDef s_config = {0};

  /* Configure automatic polling mode to wait for memory ready */
  s_command.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  s_command.FlashId            = HAL_OSPI_FLASH_ID_1;
  s_command.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  s_command.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  s_command.Instruction        = IS25LX_READ_STATUS_REG_CMD;
  s_command.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  s_command.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  s_command.AddressSize        = 0U;
  s_command.Address            = 0U;
  s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = HAL_OSPI_DATA_1_LINE;
  s_command.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  s_command.DummyCycles        = 0;
  s_command.NbData             = 1; 
  s_command.DQSMode            = HAL_OSPI_DQS_DISABLE;
  s_command.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  s_config.Match         = 0x02;
  s_config.Mask          = IS25LX_SR_WEL;
  s_config.MatchMode     = HAL_OSPI_MATCH_MODE_AND;
  s_config.Interval      = IS25LX_AUTOPOLLING_INTERVAL_TIME;
  s_config.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_OSPI_Command(Ctx, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  if (HAL_OSPI_AutoPolling(Ctx, &s_config, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  return IS25LX_OK;
}

/* Polling WEL(Write Enable) bit to become 1 */
uint8_t AutoPollingWriteEnabledOctal(OSPI_HandleTypeDef *Ctx)
{
  OSPI_RegularCmdTypeDef  s_command = {0};
  OSPI_AutoPollingTypeDef s_config = {0};

  /* Configure automatic polling mode to wait for memory ready */
  s_command.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  s_command.FlashId            = HAL_OSPI_FLASH_ID_1;
  s_command.InstructionMode    = HAL_OSPI_INSTRUCTION_8_LINES;
  s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_ENABLE;
  s_command.InstructionSize    = HAL_OSPI_INSTRUCTION_16_BITS;
  s_command.Instruction        = IS25LX_OCTA_READ_STATUS_REG_CMD;
  s_command.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  s_command.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  s_command.AddressSize        = 0U;
  s_command.Address            = 0U;
  s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = HAL_OSPI_DATA_8_LINES;
  s_command.DataDtrMode        = HAL_OSPI_DATA_DTR_ENABLE;
  s_command.DummyCycles        = DUMMY_CYCLES_READ_REG_OCTAL_DTR;
  s_command.NbData             = 2; //minimum 2 bytes in octal DDR mode
  s_command.DQSMode            = HAL_OSPI_DQS_ENABLE;
  s_command.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  s_config.Match         = 2;
  s_config.Mask          = IS25LX_SR_WEL;
  s_config.MatchMode     = HAL_OSPI_MATCH_MODE_AND;
  s_config.Interval      = IS25LX_AUTOPOLLING_INTERVAL_TIME;
  s_config.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_OSPI_Command(Ctx, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  if (HAL_OSPI_AutoPolling(Ctx, &s_config, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  return IS25LX_OK;
}

/* This function send a Write Enable and wait it is effective */
uint8_t WriteEnable_1S_0_0(OSPI_HandleTypeDef *Ctx)
{
  OSPI_RegularCmdTypeDef     s_command = {0};  

  /* Initialize the write enable command */
  s_command.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;  
  s_command.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  s_command.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  s_command.Instruction        = 0x06; //IS25LX WRITE ENABLE CMD
  s_command.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = HAL_OSPI_DATA_NONE;
  s_command.DummyCycles        = 0;
  s_command.DQSMode            = HAL_OSPI_DQS_DISABLE;
  s_command.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_OSPI_Command(Ctx, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  if(AutoPollingWriteEnabled_1S_0_1S(Ctx) != IS25LX_OK)  
  {
    return IS25LX_ERROR;
  }

  return IS25LX_OK;
}

uint8_t PageProgram1S_8S_8S(OSPI_HandleTypeDef *Ctx, uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{/*  Writes an amount of data to the OSPI memory on DTR mode */  
  
  OSPI_RegularCmdTypeDef s_command = {0};

  /* Initialize the program command */
  s_command.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  s_command.FlashId            = HAL_OSPI_FLASH_ID_1;
  s_command.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  s_command.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  s_command.Instruction        = 0x12;//IS25LX OCTAL Input Fast Program
  s_command.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
  s_command.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  s_command.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
  s_command.Address            = WriteAddr;
  s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = HAL_OSPI_DATA_1_LINE;
  s_command.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
  s_command.DummyCycles        = 0U;
  s_command.NbData             = Size;
  s_command.DQSMode            = HAL_OSPI_DQS_DISABLE;
  s_command.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_OSPI_Command(Ctx, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  /* Transmission of the data */
  if (HAL_OSPI_Transmit(Ctx, pData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  return IS25LX_OK;
}

uint8_t ChipErase(OSPI_HandleTypeDef *Ctx)
{
  OSPI_RegularCmdTypeDef s_command = {0};

  /* Initialize the erase command */
  s_command.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  s_command.FlashId            = HAL_OSPI_FLASH_ID_1;
  s_command.InstructionMode    = HAL_OSPI_INSTRUCTION_8_LINES;
  s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_ENABLE;
  s_command.InstructionSize    = HAL_OSPI_INSTRUCTION_16_BITS;
  s_command.Instruction        = IS25LX_OCTA_BULK_ERASE_CMD;
  s_command.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = HAL_OSPI_DATA_NONE;
  s_command.DummyCycles        = 0;
  s_command.DQSMode            = HAL_OSPI_DQS_ENABLE;
  s_command.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if(HAL_OSPI_Command(Ctx, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  return IS25LX_OK;
}

uint8_t ReadFlagRegister(OSPI_HandleTypeDef *Ctx, uint8_t *Value)
{
  OSPI_RegularCmdTypeDef s_command = {0};

  /* Initialize the reading of security register */
  s_command.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  s_command.FlashId            = HAL_OSPI_FLASH_ID_1;
  s_command.InstructionMode    = HAL_OSPI_INSTRUCTION_8_LINES;
  s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_ENABLE;
  s_command.InstructionSize    = HAL_OSPI_INSTRUCTION_16_BITS;
  s_command.Instruction        = IS25LX_OCTA_READ_FLAG_REG_CMD;
  s_command.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  s_command.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_ENABLE;
  s_command.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
  s_command.Address            = 0U;
  s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           =  HAL_OSPI_DATA_8_LINES;
  s_command.DataDtrMode        =  HAL_OSPI_DATA_DTR_ENABLE;
  s_command.DummyCycles        = DUMMY_CYCLES_READ_REG_OCTAL_DTR;
  s_command.NbData             = 2U;
  s_command.DQSMode            = HAL_OSPI_DQS_ENABLE;
  s_command.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_OSPI_Command(Ctx, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  /* Reception of the data */
  if (HAL_OSPI_Receive(Ctx, Value, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  return IS25LX_OK;
}

uint8_t ReadStatusRegister(OSPI_HandleTypeDef *Ctx, uint8_t *Value)
{
  OSPI_RegularCmdTypeDef s_command = {0};

  /* Initialize the reading of status register */
  s_command.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  s_command.FlashId            = HAL_OSPI_FLASH_ID_1;
  s_command.InstructionMode    = HAL_OSPI_INSTRUCTION_8_LINES;
  s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_ENABLE;
  s_command.InstructionSize    = HAL_OSPI_INSTRUCTION_16_BITS;
  s_command.Instruction        = IS25LX_OCTA_READ_STATUS_REG_CMD;
  s_command.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  s_command.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  s_command.AddressSize        = 0U;
  s_command.Address            = 0U;
  s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = HAL_OSPI_DATA_8_LINES;
  s_command.DataDtrMode        = HAL_OSPI_DATA_DTR_ENABLE;
  s_command.DummyCycles        = DUMMY_CYCLES_READ_REG_OCTAL_DTR;
  s_command.NbData             = 2; //minimum 2 bytes in octal DDR mode
  s_command.DQSMode            = HAL_OSPI_DQS_ENABLE;
  s_command.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_OSPI_Command(Ctx, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  /* Reception of the data */
  if (HAL_OSPI_Receive(Ctx, Value, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  return IS25LX_OK;
}

uint8_t EraseSector_1S_1S_0(OSPI_HandleTypeDef* Ctx, uint32_t BlockAddress)
{
  OSPI_RegularCmdTypeDef s_command = {0};

  /* Initialize the erase command */
  s_command.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  s_command.FlashId            = HAL_OSPI_FLASH_ID_1;
  s_command.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  s_command.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  s_command.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
  s_command.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  s_command.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  s_command.Address            = BlockAddress;
  s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           = HAL_OSPI_DATA_NONE;
  s_command.DummyCycles        = 0U;
  s_command.DQSMode            = HAL_OSPI_DQS_DISABLE;
  s_command.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
  
  s_command.Instruction = 0xD8; //IS25LX_OCTA_SECTOR_ERASE_128K_CMD;
      
  /* Send the command */
  if(HAL_OSPI_Command(Ctx, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  if (AutoPollingMemReady_1S_0_1S(&hospi1) != IS25LX_OK)
  {
    return IS25LX_ERROR;
  }

  return IS25LX_OK;
}

uint8_t OCTOSPI1_Init()
{
  if (HAL_OSPI_DeInit(&hospi1) != HAL_OK) 
  {
	  return IS25LX_ERROR;
	}  

   /*  Init ospi1 struct to Zero*/
  memset((void *)&hospi1, 0, sizeof(hospi1));
  OSPIM_CfgTypeDef sOspiManagerCfg = {0};

  /* OCTOSPI1 parameter configuration*/
  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThreshold = 1;
  hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
  hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_MICRON;
  hospi1.Init.DeviceSize = 24;
  hospi1.Init.ChipSelectHighTime = 1;
  hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
  hospi1.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
  hospi1.Init.ClockPrescaler = 4; //50 MHz
  hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_NONE;// HAL_OSPI_SAMPLE_SHIFTING_HALFCYCLE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE;
  hospi1.Init.ChipSelectBoundary = 0;
  hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
  hospi1.Init.MaxTran = 0; 
  hospi1.Init.Refresh = 200; //4us @ 50 MHz  

  if (HAL_OSPI_Init(&hospi1) != HAL_OK)
  {
    return IS25LX_ERROR;
  }
  sOspiManagerCfg.ClkPort = 1;
  sOspiManagerCfg.DQSPort = 1;
  sOspiManagerCfg.NCSPort = 1;
  sOspiManagerCfg.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;
  sOspiManagerCfg.IOHighPort = HAL_OSPIM_IOPORT_1_HIGH;
  sOspiManagerCfg.Req2AckTime = 1;
  if (HAL_OSPIM_Config(&hospi1, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }
  return IS25LX_OK;
}

void Flash_MAXTRAN_Cfg()
{
    //200 is 4us at 50 MHz
    MODIFY_REG(hospi1.Instance->DCR3, OCTOSPI_DCR3_MAXTRAN, 0x000000CB); 
}

uint8_t ResetMemory(OSPI_HandleTypeDef *hospi)
{
  OSPI_RegularCmdTypeDef sCommand = {0};
  OSPI_AutoPollingTypeDef sConfig;

  sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.Instruction = 0x66;
  sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode = HAL_OSPI_DATA_NONE;
  sCommand.DummyCycles = 0;
  sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

   /* Send the command */
  if(HAL_OSPI_Command(hospi,&sCommand,HAL_OSPI_TIMEOUT_DEFAULT_VALUE))
  {
    return IS25LX_ERROR;
  }

  /* Wait at least tSHSL3: min 30us */
  //HAL_Delay(1);

  sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.Instruction = 0x99;
  sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode = HAL_OSPI_DATA_NONE;
  sCommand.DummyCycles = 0;
  sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if(HAL_OSPI_Command(hospi,&sCommand,HAL_OSPI_TIMEOUT_DEFAULT_VALUE))
  {
    return IS25LX_ERROR;
  }

  /* Step 2: read the status register */
  sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.Instruction = 0x05; /* Read status regoister */
  sCommand.DataMode = HAL_OSPI_DATA_1_LINE;
  sCommand.NbData = 1;
  sCommand.DummyCycles = 0U;
  HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);

  sConfig.Match = 0x00;
  sConfig.Mask = 0x01;
  sConfig.MatchMode = HAL_OSPI_MATCH_MODE_AND;
  sConfig.Interval = 0x10;
  sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;
  if(HAL_OSPI_AutoPolling(hospi,&sConfig,HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }
  return IS25LX_OK;
}

uint8_t SetupDummyCycles(OSPI_HandleTypeDef *hospi)
{
  OSPI_RegularCmdTypeDef sCommand = {0};  
  uint8_t reg;
  
  /* Step 1: Write Enable must be set before configuration register write */
  if(WriteEnable_1S_0_0(hospi) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  /* Step 2: write the volatile configuration */
  sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;  
  sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;  
  sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.Instruction = 0x81; /* Write volatile configuration register */
  sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
  sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.Address = 0x00000001; /* dummy cycles configuration offset into volatile config register */  
  sCommand.DataMode = HAL_OSPI_DATA_1_LINE;
  sCommand.DummyCycles = 0U;
  sCommand.NbData = 1;
  sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
  
  if(HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  reg = 4; /* 5 dummy clock cycles at 20MHz*/
  if(HAL_OSPI_Transmit(hospi, (uint8_t*)&reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }
  return IS25LX_OK;
}

uint8_t SetupOctalDDRMode(OSPI_HandleTypeDef *hospi)
{
  OSPI_RegularCmdTypeDef sCommand = {0};  
  uint8_t reg;
    
  /* Step 1: Write Enable must be set before configuration register write */
  if(WriteEnable_1S_0_0(hospi) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  /* Step 2: write the volatile configuration */
  sCommand.Instruction = 0x81; /* Write volatile configuration register */  
  sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;  
  sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;  
  sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;    
  sCommand.AddressSize = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.Address=0x0; //offset into volatile config register
  sCommand.DataMode = HAL_OSPI_DATA_1_LINE;
  sCommand.DummyCycles = 0; /* zero dummy cycles for volatile config register */
  sCommand.NbData = 1; /* number of data bytes */
  sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;

  reg = 0xe7; /* page 29 - Table 6.6 volatile configuration register */
  /*
    FFh = Extended SPI (Default)
    DFh = Extended SPI without DQS
    E7h = Octal DDR
    C7h = Octal DDR without DQS
  */
  if(HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }
  if(HAL_OSPI_Transmit(hospi, (uint8_t*)&reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }
  return IS25LX_OK;
}

/***** Enter_4Byte_Address_Mode *****/
uint8_t Enter_4Byte_Address_Mode(OSPI_HandleTypeDef *hospi)
{
	OSPI_RegularCmdTypeDef sCommand = {0};

	// Assuming ExtendedSPI mode
	sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
	sCommand.Instruction        = 0x87;
	sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
	sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	sCommand.DataMode           = HAL_OSPI_DATA_NONE;
	sCommand.DummyCycles        = 0;
	sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
	sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

	if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE)	!= HAL_OK)
	{
		return IS25LX_ERROR;
	}

	return IS25LX_OK;
}

/************************** External Functions *************************/
uint8_t ExtFLASH_Init()
{
  /* return 0 for success */
  /* return 1 for failure */

    //__HAL_RCC_OSPI1_FORCE_RESET(); // completely reset peripheral
    //__HAL_RCC_OSPI1_RELEASE_RESET();  
    //if (HAL_OSPI_Abort(&hospi1) != HAL_OK) 
    //{        
    //   return IS25LX_ERROR;
    //}

    /*Initialaize OSPI*/
    if (OCTOSPI1_Init() != IS25LX_OK)
    {
      return IS25LX_ERROR;
    }

    if (ResetMemory(&hospi1) != IS25LX_OK)
    {
      return IS25LX_ERROR;
    }    
    /* Step 2: Setup the dummy cycles */
    /*if (SetupDummyCycles(&hospi1) != IS25LX_OK)
    {
      return IS25LX_ERROR;
    }*/
    //if (SetupOctalDDRMode(&hospi1) != IS25LX_OK)
    //{
    //  return IS25LX_ERROR;
    //}

    //if(Enter_4Byte_Address_Mode(&hospi1)!= IS25LX_OK)
    //{
    //  return IS25LX_ERROR;
   // }

   

    /* Step 4: Enable the memory mapped mode */
   // if (IS25LX_EnableMemoryMappedMode(&hospi1) != IS25LX_OK)
   // {
   //   return IS25LX_ERROR;
   // }
    
  return IS25LX_OK;
}

void EnblFlash_MemMapped()
{
  IS25LX_EnableMemoryMappedMode(&hospi1);
}

uint8_t IS25LX_ReadMemory_1S_1S_1S(uint32_t ReadAddr, uint32_t Size)
{
     OSPI_RegularCmdTypeDef s_command = {0};
     uint8_t Data[6];

  /* Initialize the reading of security register */
  s_command.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  s_command.FlashId            = HAL_OSPI_FLASH_ID_1;
  s_command.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  s_command.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  s_command.Instruction        = 0x03;;
  s_command.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
  s_command.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
  s_command.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  s_command.Address            = ReadAddr;
  s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode           =  HAL_OSPI_DATA_1_LINE;
  s_command.DataDtrMode        =  HAL_OSPI_DATA_DTR_DISABLE;
  s_command.DummyCycles        = 0;
  s_command.NbData             = 6;
  s_command.DQSMode            = HAL_OSPI_DQS_DISABLE;
  s_command.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if (HAL_OSPI_Command(&hospi1, &s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  /* Reception of the data */
  if (HAL_OSPI_Receive(&hospi1, Data, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return IS25LX_ERROR;
  }

  return IS25LX_OK;  
}

uint8_t IS25LX_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
  uint8_t ret = IS25LX_OK;
  uint32_t end_addr, current_size, current_addr;
  uint32_t data_addr;

    //if (HAL_OSPI_Abort(&hospi1) != HAL_OK) 
    //{        
    //   return IS25LX_ERROR;
    //}
    HAL_OSPI_Abort(&hospi1);

     /* Calculation of the size between the write address and the end of the page */
    current_size = IS25LX_PAGE_SIZE - (WriteAddr % IS25LX_PAGE_SIZE);

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > Size)
    {
      current_size = Size;
    }

    /* Initialize the address variables */
    current_addr = WriteAddr;
    end_addr = WriteAddr + Size;
    data_addr = (uint32_t)pData;

    /* Perform the write page by page */
    do
    {
      /* Check if Flash busy ? */
      if(AutoPollingMemReady_1S_0_1S(&hospi1) != IS25LX_OK)
      {
        ret = IS25LX_ERROR;
      }/* Enable write operations */
      else if(WriteEnable_1S_0_0(&hospi1) != IS25LX_OK)
      {
        ret = IS25LX_ERROR;
      }
      else
      {       
          /* Issue page program command */
          if(PageProgram1S_8S_8S(&hospi1, (uint8_t*)data_addr, current_addr, current_size) != IS25LX_OK)
          {
            ret = IS25LX_ERROR;
          }
        

        if (ret == IS25LX_OK)
        {
          /* Configure automatic polling mode to wait for end of program */
          if (AutoPollingMemReady_1S_0_1S(&hospi1) != IS25LX_OK)
          {
            ret = IS25LX_ERROR;
          }
          else
          {
            /* Update the address and size variables for next page programming */
            current_addr += current_size;
            data_addr += current_size;
            current_size = ((current_addr + IS25LX_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : IS25LX_PAGE_SIZE;
          }
        }
      }
    } while ((current_addr < end_addr) && (ret == IS25LX_OK));
  
  return ret;
}

uint8_t IS25LX_MassErase()
{
  uint8_t ret;

    //if (HAL_OSPI_Abort(&hospi1) != HAL_OK) 
    //{        
    //   return IS25LX_ERROR;
    //}
    HAL_OSPI_Abort(&hospi1);

    /* Check Flash busy ? */
    if(AutoPollingMemReady_1S_0_1S(&hospi1) != IS25LX_OK)
    {
      ret = IS25LX_ERROR;
    }/* Enable write operations */
    else if(WriteEnableOctal(&hospi1) != IS25LX_OK)
    {
      ret = IS25LX_ERROR;
    }/* Issue Chip erase command */
    else if(ChipErase(&hospi1) != IS25LX_OK)
    {
      ret = IS25LX_ERROR;
    }
    else
    {
      ret = IS25LX_OK;
    }  
  return ret;
}

uint8_t IS25LX_EraseBlock(uint32_t BlockAddress)
{
  uint8_t ret;

    //if (HAL_OSPI_Abort(&hospi1) != HAL_OK) 
    //{        
    //   return IS25LX_ERROR;
    //}
    HAL_OSPI_Abort(&hospi1);
  
    /* Check Flash busy ? */
    if(AutoPollingMemReady_1S_0_1S(&hospi1) != IS25LX_OK)
    {
      ret = IS25LX_ERROR;
    }/* Enable write operations */
    else if(WriteEnable_1S_0_0(&hospi1) != IS25LX_OK)
    {
      ret = IS25LX_ERROR;
    }/* Issue Block Erase command */
    else if(EraseSector_1S_1S_0(&hospi1, BlockAddress) != IS25LX_OK)
    {
      ret = IS25LX_ERROR;
    }
    else
    {
      ret = IS25LX_OK;
    }
  
  /* Return BSP status */
  return ret;
}

uint8_t IS25LX_GetStatus()
{
  static uint8_t reg[2];
  uint8_t ret;

   // if (HAL_OSPI_Abort(&hospi1) != HAL_OK) 
   // {        
   //    return IS25LX_ERROR;
   // }
  
    if(ReadFlagRegister(&hospi1, reg) != IS25LX_OK)
    {
      ret = IS25LX_ERROR;
    }/* Check the value of the register */
    else if ((reg[0] & (IS25LX_FLAG_P_FAIL | IS25LX_FLAG_E_FAIL | IS25LX_FLAG_P_SUSP | IS25LX_FLAG_E_SUSP)) != 0U)
    {
      ret = IS25LX_ERROR;
    }    
    else if(ReadStatusRegister(&hospi1, reg) != IS25LX_OK)
    {
      ret = IS25LX_ERROR;
    }/* Check the value of the register */
    else if ((reg[0] & IS25LX_SR_WIP) != 0U)
    {
      ret = IS25LX_ERROR;
    }
    else
    {
      ret = IS25LX_OK;
    }
  
  return ret;
}

uint8_t IS25LX_EnableMemoryMappedMode()
{
  OSPI_RegularCmdTypeDef sCommand = {0};
  OSPI_MemoryMappedTypeDef s_mem_mapped_cfg = {0};
  /* these settings assume the device is in octal DDR mode by now */
  sCommand.OperationType = HAL_OSPI_OPTYPE_READ_CFG;    
  sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.Instruction = 0xfd;
  sCommand.AddressMode = HAL_OSPI_ADDRESS_8_LINES;
  sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_ENABLE;
  sCommand.AddressSize = HAL_OSPI_ADDRESS_32_BITS;
  sCommand.Address = 0;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode = HAL_OSPI_DATA_8_LINES;
  sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_ENABLE;
  sCommand.DummyCycles = 16; 
  sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if(HAL_OSPI_Command(&hospi1,&sCommand,HAL_OSPI_TIMEOUT_DEFAULT_VALUE))
  {
    return 1;
  }

  /* Initialize the program command */
  sCommand.OperationType = HAL_OSPI_OPTYPE_WRITE_CFG;
  sCommand.Instruction = 0x12;
  sCommand.DummyCycles = 0;
  sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;

  /* Send the command */
  if(HAL_OSPI_Command(&hospi1,&sCommand,HAL_OSPI_TIMEOUT_DEFAULT_VALUE))
  {
    return 1;
  }

  s_mem_mapped_cfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_DISABLE;
  //s_mem_mapped_cfg.TimeOutPeriod = 10;
  /* set the FMODE field to 3 for memory mapped mode */
  if(HAL_OSPI_MemoryMapped(&hospi1,&s_mem_mapped_cfg) != HAL_OK)
  {
    return 1;
  }
  return 0;
}

uint8_t IS25LX_EnableMemoryMappedModeSDR()
{ /*1S-8D-8D*/
  OSPI_RegularCmdTypeDef sCommand = {0};
  OSPI_MemoryMappedTypeDef s_mem_mapped_cfg = {0};
  /* these settings assume the device is in octal DDR mode by now */
  sCommand.OperationType = HAL_OSPI_OPTYPE_READ_CFG;    
  sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.Instruction = 0xcc;
  sCommand.AddressMode = HAL_OSPI_ADDRESS_8_LINES;
  sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
  sCommand.AddressSize = HAL_OSPI_ADDRESS_32_BITS;
  sCommand.Address = 0;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode = HAL_OSPI_DATA_8_LINES;
  sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.DummyCycles = 16; 
  sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Send the command */
  if(HAL_OSPI_Command(&hospi1,&sCommand,HAL_OSPI_TIMEOUT_DEFAULT_VALUE))
  {
    return 1;
  }

  /* Initialize the program command */
  sCommand.OperationType = HAL_OSPI_OPTYPE_WRITE_CFG;
  sCommand.Instruction = 0x02;
  sCommand.DummyCycles = 0;
  sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;

  /* Send the command */
  if(HAL_OSPI_Command(&hospi1,&sCommand,HAL_OSPI_TIMEOUT_DEFAULT_VALUE))
  {
    return 1;
  }

  s_mem_mapped_cfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_DISABLE;
  //s_mem_mapped_cfg.TimeOutPeriod = 10;
  /* set the FMODE field to 3 for memory mapped mode */
  if(HAL_OSPI_MemoryMapped(&hospi1,&s_mem_mapped_cfg) != HAL_OK)
  {
    return 1;
  }
  return 0;
}