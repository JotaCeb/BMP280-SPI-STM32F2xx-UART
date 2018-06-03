/*******************************************************************************
																																				 CEBRI·N
  ******************************************************************************
  */
#ifndef __DRIVERBMP280_SPI_H
#define __DRIVERBMP280_SPI_H

#include "stdint.h"

uint8_t INTERFACE_Level0[5][100] = 	{{"\r\n\r\nInterfaces UART y SPI para el BMP280. (I) Inicio de adquisicion, (ESC) Stop.\r\n"},
																		{"(1)Frecuencia de adquisicion (De 1 a 5 segundos) de presion y temperatura.\r\n"},
																		{"(2)Operaciones de lectura y escritura.\r\n"},
																		{"(3)Resetear chip.\r\n"},
																		{"(4)Informacion de usuario.\r\n"}};

uint8_t INTERFACE_LevelA[8][100] = 	{{"\r\nIntroducir un entero 1 a 5: (1), (2), (3), (4), (5)."},
																		{"\r\nLectura de registro (R), escritura de registro (W)."},
																		{"\r\nChip reseteado"},
																		{"\r\nEs esencial que el usuario conocozca como trabaja el BMP280."},
																		{"\r\nNo olvide escribir en letras mayusculas para interactuar con la interfaz."},																					
																		{"\r\nLos errores de lectura y escritura no estan clasificados, el usuario puede"},
																		{"\r\nrecibir informacion erronea si no interactua correctamente con la interfaz"},
																		{"\r\nSi desea salir de una de las opciones pulse (ESC)."}};
															

uint8_t INTERFACE_Timing[1][100] = 	{"\r\nTimer programado.\r\n"};

uint8_t INTERFACE_Reading[2][50] = 	{{"\r\nEscriba el registro a leer: 0x..."},
																		{"\r\nEl registro introducido tiene el valor de 0x"}};
																		
uint8_t INTERFACE_Writing[3][100] = {{"\r\nEscriba el registro sobre el que desea sobreescribir: 0x..."},
																		{"\r\nEscriba el valor que desea sobreescribir: 0x..."},
																		{"\r\nValor enviado."}};

uint8_t INTERFACE_PT [6][100] = 		{{"\r\n\r\n0xF7: 0x"}, {"    0xF8: 0x"}, {"    0xF9: 0x"},
																		{"\r\n0xFA: 0x"}, {"    0xFB: 0x"}, {"    0xFC: 0x"}};

/*Function BMP280 driver definitions----------------------------------------------*/
void ResetChip(void);
void ReadTransmission(void);
void WriteValue(char iXAux);
void TimerProgramming(char CharInAux);
void UARTInterface(void);
void OptionLevelA(char CharInAux);																					
void GoToSPIDriverBMP280(char CharInAux);
void ReadTP(void);														
int Array2Uint8_tConver02(uint8_t *NumberAux);													
void Uint8_t2ArrayConver(uint8_t CharInAux, uint8_t *ArrayCharInAux);
int Char2HexConver(char CharInAux);
void UpperCaseConversion(uint8_t *NumberAux);
void UpperCaseConversion12 (uint8_t *NumberAux);

																		
#endif
