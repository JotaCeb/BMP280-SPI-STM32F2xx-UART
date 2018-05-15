#ifndef __DRIVERBMP280_SPI_H
#define __DRIVERBMP280_SPI_H

void ReadTransmission();
void WriteTransmission();
void TimerProgramming();
void UARTInterface(void);
void GoToSPIDriverBMP280(char CharInAux);

#endif