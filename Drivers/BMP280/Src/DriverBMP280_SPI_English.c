#include "stm32f2xx_hal.h"
#include "stdlib.h"
#include "DriverBMP280_SPI.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim2;
extern SPI_HandleTypeDef hspi1;

static char iX, TestCharInAux;
static char EntryNumber, EntryNumberRW;
static uint8_t number[5], MatrixTdata[2], MatrixRdata[20];

HAL_StatusTypeDef Comprobacion;

void ReadTransmission(void){
	int i = 0;
	uint8_t Reg2ReadAux8 = 0;
	Reg2ReadAux8 = Hex2NumConversion();													//Hex conversion
	MatrixTdata[0] = number[0];
	MatrixTdata[1] = number[1];
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	for(i = 0; i > 8000000; i++);
	Comprobacion = HAL_SPI_TransmitReceive(&hspi1, &Reg2ReadAux8, &MatrixRdata[0], 5, 10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart1,&INTERFACE_Reading[1][0], sizeof(INTERFACE_Reading[1]), 10);
	HAL_UART_Transmit(&huart1,&MatrixRdata[0], sizeof(MatrixRdata), 10);
}
//void WriteTransmission(){

//}
void TimerProgramming(char CharInAux){
	CharInAux = atoi(&CharInAux);
	htim2.Init.Period = (CharInAux * 1000) + 1;
	TIM_Base_SetConfig(TIM2, &htim2.Init);
	HAL_UART_Transmit(&huart1, &ANSWER[1][0], sizeof(ANSWER[1]), 10);
}
void UARTInterface(void){											//Show UART interface user
	char i = 0;
	while (i < 4){
		HAL_UART_Transmit(&huart1, &INTERFACE_Level0[i][0], sizeof(INTERFACE_Level0[i]), 10);
		i++;
	}
}
void OptionLevelA (char CharInAux){								//Show first message after select an option (1), (2) or (3)
	char i = 0;
	CharInAux = Char2NumConversion (CharInAux);
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
		iX = 0;
	}
}
void GoToSPIDriverBMP280(char CharInAux){					//Main driver function (Permit to select any interface UART option)
	char flagCarGoodAux = 0;												//For acceptable values
	TestCharInAux = CharInAux;
	flagCarGoodAux = ((CharInAux >= '0') && (CharInAux <= '9') || (CharInAux >= 'A') && (CharInAux <= 'F'))? 1 : 0;
	if(CharInAux == 0x1B || EntryNumber == 0){
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
				EntryNumber = EntryNumberRW = iX = 0;
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
	}
	else if(CharInAux >= '1' && CharInAux <= '5' && EntryNumber == 1){
		TimerProgramming(CharInAux);
		EntryNumber = 0;
	}
	else if(EntryNumber == 2){														//(R) or (W)
		if(CharInAux == 'R' && iX == 0){										//Show message: "Introduce value"
			EntryNumberRW = CharInAux;
			HAL_UART_Transmit(&huart1,&INTERFACE_Reading[0][0], sizeof(INTERFACE_Reading[0]), 10);
			iX++;
		}
		else if(EntryNumberRW == 'R' && iX == 1 && flagCarGoodAux == 1){				//Catch first value
			number[0] = CharInAux;
			iX++;			
		}
		else if(EntryNumberRW == 'R' && iX == 2 && flagCarGoodAux == 1 ){														//Catch second value
			number[1] = CharInAux;
			ReadTransmission();
			iX = 0;			
		}
//		else if(EntryNumberRW == 'W' && iX == 0){
//			Reg2Read32Aux = Hex2NumConversion(number);																		//Hex conversion		
//			HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, SPI_DATASIZE_8BIT, 10)
//			HAL_UART_Transmit(&huart1,&INTERFACE_Reading[1][0], sizeof(INTERFACE_Reading[1]), 10);
//			
//			sprintf(num3, "%02x", ValRegRec8);
//			for (j=0;num3[j]!='\0';++j)	if(num3[j]>='a'&&num3[j]<='z')num3[j]=num3[j]-desp;	//Uppercase
//			HAL_UART_Transmit(&huart1, num3, 2, 10);
//			
//			iX++;			
//		}
//		else if(CharInAux == 'W' && iX == 0){
//			EntryNumberRW = CharInAux;
//			iX++;
//		}
	}
}
int Hex2NumConversion (void){
	int Reg2Read32Aux = 0;
	sscanf((char*)&number, "%02x", &Reg2Read32Aux);
	return Reg2Read32Aux;
}
char Char2NumConversion (char CharInAux){
	CharInAux = atoi(&CharInAux);
	return CharInAux;
}
//int Num2HexConversion (void){
//	int Reg2Read32Aux = 0;
//	sprintf(num3, "%02x", ValRegRec8);
//	return Reg2Read32Aux;
//}

//void UpperCaseConversion (void){
//	
//}
