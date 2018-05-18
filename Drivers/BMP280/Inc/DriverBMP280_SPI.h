/*******************************************************************************
																																				 CEBRI�N
  ******************************************************************************
  */
#ifndef __DRIVERBMP280_SPI_H
#define __DRIVERBMP280_SPI_H

#include "stm32f2xx_hal.h"

uint8_t INTERFACE_Level0[4][100] = {{"\r\n\r\nBMP280 UART&SPI Interfaces. (I) Initiation of acquisition, (S) Stop.\r\n"},
																	{"(1)Acquisition frequency (1 to 5 seconds) of pressure and temperature.\r\n"},
																	{"(2)Read and write operations.\r\n"},
																	{"(3)Information for the user.\r\n"}};

uint8_t INTERFACE_LevelA[8][100] = {{"\r\nEnter an integer from 1 to 5: (1), (2), (3), (4), (5)."},
																	{"\r\nRegister reading (R), register writting (W)."},
																	{"\r\nIt is essential that the user knows how the BMP280 works."},
																	{"\r\nDo not forget to write in uppercase to interact with the interface."},																					
																	{"\r\nNo read and write errors are displayed due to enter incorrect values,"},
																	{"\r\nyou could receive wrong information from the interface if you do not use"},
																	{" correctly.\r\n"},
																	{"\r\nIf you wish to exit one of the options press (ESC)."}};

uint8_t INTERFACE_Reading[2][50] = {{"\r\nWrite register to read: 0x..."},
																		{"\r\nThe entered register has the value of 0x"}};
																		
uint8_t INTERFACE_Writting[3][100] = {{"\r\nWrite register to read: 0x..."},
																					{"\r\nWrite the register to overwrite: 0x..."},
																					{"\r\nEnter the value you wish to overwrite in the register: 0x..."}};

uint8_t ANSWER		[11][100] = {{"\r\nThe entered register has the value of 0x"},
																{"\r\nProgrammed timer\r\n"},
																{"\r\nOverwritten value\r\n"},
																{"X1:0x"}, {"   X2:0x"}, {"   Y1:0x"}, {"   Y2:0x"}, {"   Z1:0x"}, {"   Z2:0x"},{"\r\n"},
																{"\r\nThe value of the entered register is 0:0x"}};

/*Definici�n de las funciones de la librer�a ADXL345----------------------------*/
void ReadTransmission();
void WriteTransmission();
void TimerProgramming(char CharInAux);
void UARTInterface(void);
char OptionLevelA(char CharInAux);																					
void GoToSPIDriverBMP280(char CharInAux);

#endif