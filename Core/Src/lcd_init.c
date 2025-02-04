/*
 * spi_init.c
 *
 *  Created on: 03-Aug-2023
 *      Author: keshav.aggarwal
 */

#include "stm32h7xx_hal.h"

/*int8_t SPI_WriteComm(uint8_t in_data)
{
	uint8_t data[2];
	data[0] = in_data;
	data[1] = 0;	//command mode

	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 0);

	if (HAL_SPI_Transmit(&hspi1, (uint8_t *)&data, 2, 50) == HAL_OK) {

		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 1);
		return (1);
	}

	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 1);
	return (-1);
}*/

/*int8_t SPI_WriteData(uint8_t in_data)
{
	uint8_t data[2];
	data[0] = in_data;
	data[1] = 1;	//data mode

	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 0);

	if (HAL_SPI_Transmit(&hspi1, (uint8_t *)&data, 2, 50) == HAL_OK) {

		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 1);
		return (1);
	}
	
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, 1);
	return (-1);
}*/

#define LCD_MOSI_Pin GPIO_PIN_7
#define LCD_MOSI_GPIO_Port GPIOD

#define LCD_RST_Pin GPIO_PIN_9
#define LCD_RST_GPIO_Port GPIOG

#define LCD_CS_Pin GPIO_PIN_10
#define LCD_SPICLK_Pin GPIO_PIN_11
#define LCD_CS_SPICLK_GPIO_Port GPIOG

void LCD_GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */ 
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pins */
  GPIO_InitStruct.Pin = LCD_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LCD_MOSI_GPIO_Port, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LCD_CS_Pin|LCD_SPICLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LCD_CS_SPICLK_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LCD_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LCD_RST_GPIO_Port, &GPIO_InitStruct);

  //Init LCD Reset to Low
  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
  //Init CS to High	
  HAL_GPIO_WritePin(LCD_CS_SPICLK_GPIO_Port,LCD_CS_Pin,GPIO_PIN_SET);
  //Init Clk to Low
  HAL_GPIO_WritePin(LCD_CS_SPICLK_GPIO_Port,LCD_SPICLK_Pin,GPIO_PIN_RESET);
}

//RGB+9b_SPI(rise)
 void SPI_SendData(uint8_t i)
{  
   uint8_t n;
   uint8_t MASK_BIT=0x80;

   for(n=0; n<8; n++)			
   {       
		//SPI_CLK=0;
		HAL_GPIO_WritePin(LCD_CS_SPICLK_GPIO_Port,LCD_SPICLK_Pin,GPIO_PIN_RESET);		
		//__NOP(); __NOP();__NOP();__NOP();
		
		if(i&MASK_BIT)
		{//Bit is Set
			HAL_GPIO_WritePin(LCD_MOSI_GPIO_Port,LCD_MOSI_Pin,GPIO_PIN_SET);
		}
		else
		{//Bit is Cleared
			HAL_GPIO_WritePin(LCD_MOSI_GPIO_Port,LCD_MOSI_Pin,GPIO_PIN_RESET);
		}
		//__NOP(); __NOP();__NOP();__NOP();
		
		//SPI_CLK=1;
		HAL_GPIO_WritePin(LCD_CS_SPICLK_GPIO_Port,LCD_SPICLK_Pin,GPIO_PIN_SET);
		MASK_BIT>>=1;
		//__NOP(); __NOP();__NOP();__NOP();	  		
   }
}
void SPI_WriteComm(unsigned char i)
{
    //SPI_CS=0;
	HAL_GPIO_WritePin(LCD_CS_SPICLK_GPIO_Port,LCD_CS_Pin,GPIO_PIN_RESET);	
	  	//__NOP(); __NOP();__NOP();__NOP();		
	//SPI_CLK=0;
	HAL_GPIO_WritePin(LCD_CS_SPICLK_GPIO_Port,LCD_SPICLK_Pin,GPIO_PIN_RESET);
    //SPI_DI=0 ;
	HAL_GPIO_WritePin(LCD_MOSI_GPIO_Port,LCD_MOSI_Pin,GPIO_PIN_RESET);
	  	//__NOP(); __NOP();__NOP();__NOP(); 		
	
		//__NOP(); __NOP();__NOP();__NOP();		
	//SPI_CLK=1;
	HAL_GPIO_WritePin(LCD_CS_SPICLK_GPIO_Port,LCD_SPICLK_Pin,GPIO_PIN_SET);
		//__NOP(); __NOP();__NOP();__NOP();		
	SPI_SendData(i);
		
    //SPI_CS=1;
	HAL_GPIO_WritePin(LCD_CS_SPICLK_GPIO_Port,LCD_CS_Pin,GPIO_PIN_SET);
}



void SPI_WriteData(unsigned char i)
{ 
    //SPI_CS=0;
	HAL_GPIO_WritePin(LCD_CS_SPICLK_GPIO_Port,LCD_CS_Pin,GPIO_PIN_RESET);
	  	//__NOP(); __NOP();__NOP();__NOP();	
		//SPI_CLK=0;
	HAL_GPIO_WritePin(LCD_CS_SPICLK_GPIO_Port,LCD_SPICLK_Pin,GPIO_PIN_RESET);	
    //SPI_DI=1 ;
	HAL_GPIO_WritePin(LCD_MOSI_GPIO_Port,LCD_MOSI_Pin,GPIO_PIN_SET);
	  	//__NOP(); __NOP();__NOP();__NOP(); 		
	
		//__NOP(); __NOP();__NOP();__NOP();		
	//SPI_CLK=1;
	HAL_GPIO_WritePin(LCD_CS_SPICLK_GPIO_Port,LCD_SPICLK_Pin,GPIO_PIN_SET);
		//__NOP(); __NOP();__NOP();__NOP();		
	SPI_SendData(i);
		
    //SPI_CS=1;
	HAL_GPIO_WritePin(LCD_CS_SPICLK_GPIO_Port,LCD_CS_Pin,GPIO_PIN_SET);
} 



void LCD_SPI_Init()
{
	LCD_GPIO_Init();
/*---------------------------ST7701S Reset Sequence---------------------------*/
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, 0);
	HAL_Delay(100);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, 1);
    HAL_Delay(100);
   
    //HAL_Delay(10);

    SPI_WriteComm(0xFF);
	SPI_WriteData(0x77);
	SPI_WriteData(0x01);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x10); //Command2_BK0
	
	SPI_WriteComm(0xC0); //LINESET
	SPI_WriteData(0xE9);
	SPI_WriteData(0x03);

	SPI_WriteComm(0xC1); //PORCTRL - Porch Control
	SPI_WriteData(0x11);
	SPI_WriteData(0x02);
	
    SPI_WriteComm(0xC2); //INVSEL - Inversion Selection and Frame Rate Control
	SPI_WriteData(0x37);
	SPI_WriteData(0x08);
	
	SPI_WriteComm(0xC7); //SDIR - Source Direction Control
	SPI_WriteData(0x00); 

	SPI_WriteComm(0xB0); //PVGAMCTRL - Positive Voltage Gamma Control
	SPI_WriteData(0x00);
	SPI_WriteData(0x0D);
	SPI_WriteData(0x14);
	SPI_WriteData(0x0D);
	SPI_WriteData(0x10);
	SPI_WriteData(0x05);
	SPI_WriteData(0x02);
	SPI_WriteData(0x08);
	SPI_WriteData(0x08);
	SPI_WriteData(0x1E);
	SPI_WriteData(0x05);
	SPI_WriteData(0x13);
	SPI_WriteData(0x11);
	SPI_WriteData(0xA3);
	SPI_WriteData(0x29);
	SPI_WriteData(0x18);
	
    SPI_WriteComm(0xB1); //NVGAMCTRL - Negative Voltage Gamma Control
	SPI_WriteData(0x00);
	SPI_WriteData(0x0C);
	SPI_WriteData(0x14);
	SPI_WriteData(0x0C);
	SPI_WriteData(0x10);
	SPI_WriteData(0x05);
	SPI_WriteData(0x03);
	SPI_WriteData(0x08);
	SPI_WriteData(0x07);
	SPI_WriteData(0x20);
	SPI_WriteData(0x05);
	SPI_WriteData(0x13);
	SPI_WriteData(0x11);
	SPI_WriteData(0xA4);
	SPI_WriteData(0x29);
	SPI_WriteData(0x18);
	
	SPI_WriteComm(0xFF);
	SPI_WriteData(0x77);
	SPI_WriteData(0x01);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x11); //Command2_BK1

	SPI_WriteComm(0xB0); //VRHS - Vop Amplitude Setting
	SPI_WriteData(0x6C);
	
	SPI_WriteComm(0xB1); //VCOMS - VCOM apmplitude setting
	SPI_WriteData(0x43);
	
	SPI_WriteComm(0xB2);
	SPI_WriteData(0x07);

	SPI_WriteComm(0xB3);
	SPI_WriteData(0x80);
	
    SPI_WriteComm(0xB5);
	SPI_WriteData(0x47);
	
    SPI_WriteComm(0xB7);
	SPI_WriteData(0x8A);
	
    SPI_WriteComm(0xB8);
	SPI_WriteData(0x20);
	
    SPI_WriteComm(0xC1);
	SPI_WriteData(0x78);
	
    SPI_WriteComm(0xC2);
	SPI_WriteData(0x78);
	
    SPI_WriteComm(0xD0);
	SPI_WriteData(0x88);
		
	SPI_WriteComm(0xE0);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x02);
	
    SPI_WriteComm(0xE1);
	SPI_WriteData(0x08);
	SPI_WriteData(0x00);
	SPI_WriteData(0x0A);
	SPI_WriteData(0x00);
	SPI_WriteData(0x07);
	SPI_WriteData(0x00);
	SPI_WriteData(0x09);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x33);
	SPI_WriteData(0x33);
	
    SPI_WriteComm(0xE2);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	
    SPI_WriteComm(0xE3);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x33);
	SPI_WriteData(0x33);
	
    SPI_WriteComm(0xE4);
	SPI_WriteData(0x44);
	SPI_WriteData(0x44);
	
    SPI_WriteComm(0xE5);
	SPI_WriteData(0x0E);
	SPI_WriteData(0x60);
	SPI_WriteData(0xA0);
	SPI_WriteData(0xA0);
	SPI_WriteData(0x10);
	SPI_WriteData(0x60);
	SPI_WriteData(0xA0);
	SPI_WriteData(0xA0);
	SPI_WriteData(0x0A);
	SPI_WriteData(0x60);
	SPI_WriteData(0xA0);
	SPI_WriteData(0xA0);
	SPI_WriteData(0x0C);
	SPI_WriteData(0x60);
	SPI_WriteData(0xA0);
	SPI_WriteData(0xA0);
	
    SPI_WriteComm(0xE6);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x33);
	SPI_WriteData(0x33);
	
    SPI_WriteComm(0xE7);
	SPI_WriteData(0x44);
	SPI_WriteData(0x44);
	
    SPI_WriteComm(0xE8);
	SPI_WriteData(0x0D);
	SPI_WriteData(0x60);
	SPI_WriteData(0xA0);
	SPI_WriteData(0xA0);
	SPI_WriteData(0x0F);
	SPI_WriteData(0x60);
	SPI_WriteData(0xA0);
	SPI_WriteData(0xA0);
	SPI_WriteData(0x09);
	SPI_WriteData(0x60);
	SPI_WriteData(0xA0);
	SPI_WriteData(0xA0);
	SPI_WriteData(0x0B);
	SPI_WriteData(0x60);
	SPI_WriteData(0xA0);
	SPI_WriteData(0xA0);
	
    SPI_WriteComm(0xEB);
	SPI_WriteData(0x02);
	SPI_WriteData(0x01);
	SPI_WriteData(0xE4);
	SPI_WriteData(0xE4);
	SPI_WriteData(0x44);
	SPI_WriteData(0x00);
	SPI_WriteData(0x40);
	
    SPI_WriteComm(0xEC);
	SPI_WriteData(0x02);
	SPI_WriteData(0x01);
	
    SPI_WriteComm(0xED);
	SPI_WriteData(0xAB);
	SPI_WriteData(0x89);
	SPI_WriteData(0x76);
	SPI_WriteData(0x54);
	SPI_WriteData(0x01);
	SPI_WriteData(0xFF);
	SPI_WriteData(0xFF);
	SPI_WriteData(0xFF);
	SPI_WriteData(0xFF);
	SPI_WriteData(0xFF);
	SPI_WriteData(0xFF);
	SPI_WriteData(0x10);
	SPI_WriteData(0x45);
	SPI_WriteData(0x67);
	SPI_WriteData(0x98);
	SPI_WriteData(0xBA);
	
	SPI_WriteComm(0xFF);
	SPI_WriteData(0x77);
	SPI_WriteData(0x01);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	HAL_Delay(10);
	
	SPI_WriteComm(0x36); //MADCTL - Display Data Access Control
	SPI_WriteData(0x00); //0x18
	
	SPI_WriteComm(0x3A); //COLMOD - Interface Pixel Format
	SPI_WriteData(0x77); //  RGB 24bits
	
	SPI_WriteComm(0x11);   //Sleep-Out
	SPI_WriteData(0x00);
	HAL_Delay(120);
	
	SPI_WriteComm(0x29);   //DISPON - Display On
	SPI_WriteData(0x00);
	HAL_Delay(10);	 
}
