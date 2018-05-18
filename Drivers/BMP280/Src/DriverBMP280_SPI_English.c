#include "DriverBMP280_SPI.h"
#include "stm32f2xx_hal.h"
#include "stdlib.h"

extern UART_HandleTypeDef huart1;
extern SPI_HandleTypeDef hspi1;

static char StaticCharIn1, CharIn2, CharIn3, CharIn4;
static char EntryNumber, FlagLevelB;
static uint8_t number[5];
void ReadTransmission(){

}
void WriteTransmission(){

}
void TimerProgramming(char CharInAux){
	CharInAux = atoi(&CharInAux);
	htim2.Init.Period = (CharInAux * 1000) + 1;
	TIM_Base_SetConfig(TIM2, &htim2.Init);
	HAL_UART_Transmit(&huart1, &ANSWER[1][0], sizeof(ANSWER[1]), 10);
}
void UARTInterface(void){
	char i = 0;
	while (i < 4){
		HAL_UART_Transmit(&huart1, &INTERFACE0[i][0], sizeof(INTERFACE0[i]), 100);
		i++;
	}
}
char OptionLevelA (char CharInAux){
	char i = 0;

	if(CharInAux == 1 || CharInAux == 2){
		HAL_UART_Transmit(&huart1, &INTERFACE_LevelA[CharInAux-1][0], sizeof(INTERFACE_LevelA[CharInAux-1]), 10);
	}
	else if(CharInAux == 3){
		i=2;
		while (i < 8){
			HAL_UART_Transmit(&huart1, &INTERFACE_LevelA[i][0], sizeof(INTERFACE_LevelA[i]), 10);
			i++;
		}
	}
	else{
		iR = iW = 0;
	}
}
void GoToSPIDriverBMP280(char CharInAux){
	char flagCarGoodAux = CharInAux;
	flagCarGoodAux = ((flagCarGoodAux > 0x30) && (flagCarGoodAux < 0x39) && (flagCarGoodAux > 0x41) && (flagCarGoodAux < 0x49))? 1 : 0;
	if(CharInAux == 0x1B || EntryNumber == 0){
		StaticCharIn1 = CharInAux;
		switch(CharInAux){
			case 0x31:												//(1)
				EntryNumber = 1;
				OptionLevelA (CharInAux);break;
			case 0x32:												//(2)->(R) or (W)
				EntryNumber = 2;
				OptionLevelA (CharInAux);break;
			case 0x33:												//(3)
				EntryNumber = 0;
				OptionLevelA (CharInAux);break;
			case 0x1B:												//(ESC)
				EntryNumber = 0;
		    __HAL_TIM_DISABLE(&htim2);
				UARTInterface();break;
			case 0x49:												//(I)
				EntryNumber = 16;
				__HAL_TIM_ENABLE(&htim2);break;
			case 0x53:												//(S)
				EntryNumber = 0;
		    __HAL_TIM_DISABLE(&htim2);
				UARTInterface();break;
		}
	else if(CharInAux >= 0x31 || CharInAux <= 0x32 || EntryNumber == 1){
		TimerProgramming(CharInAux);
		EntryNumber = 0;
	}
	else if(EntryNumber == 2){								//(R) or (W)
		if(CharInAux == 'R' && iX == 0){										//Show message: "Introduce value"
			FlagEntryNumber2 = CharInAux;
			HAL_UART_Transmit(&huart1,&INTERFACE_Reading[0][0], sizeof(INTERFACE_Reading[0]), 10);
			iX++;
		}
		else if(FlagEntryNumber2 == 'R' && iX == 1 && flagCarGoodAux == 1){
			number[0] = USART1->DR;
			iX++;			
		}
		else if(FlagEntryNumber2 == 'R' && iX == 2 && flagCarGoodAux == 1){
			number[1] = USART1->DR;
			HAL_UART_Transmit(&huart1,&INTERFACE_Reading[0][0], sizeof(INTERFACE_Reading[0]), 10);
			iX = 0;			
		}
		else if(FlagEntryNumber2 == 'R' && iX == 1){
			sscanf((char*)&num, "%02x", &Reg2Read32);																		//hex conversion		
			HAL_StatusTypeDef HAL_SPI_Receive(&hspi1, (uint8_t) Reg2Read32, SPI_DATASIZE_8BIT, 10);
			HAL_UART_Transmit(&huart1,&INTERFACE_Reading[1][0], sizeof(INTERFACE_Reading[1]), 10);
			
			sprintf(num3, "%02x", ValRegRec8);
			for (j=0;num3[j]!='\0';++j)	if(num3[j]>='a'&&num3[j]<='z')num3[j]=num3[j]-desp;	//Uppercase
			HAL_UART_Transmit(&huart1, num3, 2, 10);
			
			iX++;			
		}
		else if(CharInAux == 'W' && iX == 0){
			FlagEntryNumber2 = CharInAux;
			iX++;
		}
	}
}
