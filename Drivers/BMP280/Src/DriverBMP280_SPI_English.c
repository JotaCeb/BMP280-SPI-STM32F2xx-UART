/**************************************************************************
****************SPI interface between stm32f2xx and BMP280*****************
***************************************************************************
*****************UART interface between user and stm32f2xx*****************
																																		CEBRIáN
***************************************************************************
*/
#include "stm32f2xx_hal.h"
#include "stdlib.h"
#include "DriverBMP280_SPI.h"

extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim2;
extern SPI_HandleTypeDef hspi1;

static char iX;
static char EntryNumber, EntryNumberRW;
static uint8_t number[4], MatrixTdata[2], MatrixRdata[7];

HAL_StatusTypeDef Comprobacion;

void ResetChip(void){												//(3)	Reset chip OK
	MatrixTdata[0] = 0xE0 & 0x7F;																														//Register E0 + Write operation
	MatrixTdata[1] = 0xB6;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);																		//SPI NSS low
	HAL_SPI_TransmitReceive(&hspi1, &MatrixTdata[0], &MatrixRdata[0], 2, 10);								//Reset => Write B6 in E0 register
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);																			//SPI NSS high
}
void ReadTransmission(void){								//(2) Show the ridden register OK
	uint8_t Reg2ReadAux8 = 0,  UartTdata[2];
	Reg2ReadAux8 = Array2Uint8_tConver02(&number[0]);																				//Hex conversion
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);																		//SPI NSS low
	HAL_SPI_TransmitReceive(&hspi1, &Reg2ReadAux8, &MatrixRdata[0], 3, 10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);																			//SPI NSS high
	Uint8_t2ArrayConver(MatrixRdata[1], &UartTdata[0]);
	HAL_UART_Transmit(&huart1,&INTERFACE_Reading[1][0], sizeof(INTERFACE_Reading[1]), 10);	//Show 0x
	UpperCaseConversion (&UartTdata[0]);
	HAL_UART_Transmit(&huart1,&UartTdata[0], 2, 10);												//Show data
}
void WriteValue(char iXAux){								//(2) (W)rite operation OK
	MatrixTdata[0]= (Array2Uint8_tConver02(&number[0])) & 0x7F;																//Hex conversion & MSB to 0 for writing operation
	MatrixTdata[1]= Array2Uint8_tConver02(&number[2]);																				//Hex conversion
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);																			//SPI NSS low
	HAL_SPI_TransmitReceive(&hspi1, &MatrixTdata[0], &MatrixRdata[0], 2, 10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);																				//SPI NSS high
	HAL_UART_Transmit(&huart1,&INTERFACE_Writing[2][0], sizeof(INTERFACE_Writing[2]), 10);		//Value sent
}
void TimerProgramming(char CharInAux){			//(1)
	CharInAux = atoi(&CharInAux);
	htim2.Init.Period = (CharInAux * 1000) + 1;
	TIM_Base_SetConfig(TIM2, &htim2.Init);
	HAL_UART_Transmit(&huart1, &INTERFACE_Timing[0][0], sizeof(INTERFACE_Timing), 10);
}
void UARTInterface(void){															//Show UART interface user
	char i = 0;
	while (i < 5){
		HAL_UART_Transmit(&huart1, &INTERFACE_Level0[i][0], sizeof(INTERFACE_Level0[i]), 10);
		i++;
	}
}
void OptionLevelA (char CharInAux){										//Show first message after select an option (1), (2), (3) or (4)
	char i = 0;
	CharInAux = Char2HexConver(CharInAux);
	if(CharInAux == 1 || CharInAux == 2){
		HAL_UART_Transmit(&huart1, &INTERFACE_LevelA[CharInAux-1][0], sizeof(INTERFACE_LevelA[CharInAux-1]), 10);
	}
	else if(CharInAux == 3){
		ResetChip();
		HAL_UART_Transmit(&huart1, &INTERFACE_LevelA[CharInAux-1][0], sizeof(INTERFACE_LevelA[CharInAux-1]), 10);
	}
	else if(CharInAux == 4){
		i=3;
		while (i < 8){
			HAL_UART_Transmit(&huart1, &INTERFACE_LevelA[i][0], sizeof(INTERFACE_LevelA[i]), 10);
			i++;
		}
	}
}
void GoToSPIDriverBMP280(char CharInAux){												//Main driver function (Permit to select any interface UART option)
	char flagCarGoodAux = 0;							//For acceptable values
	flagCarGoodAux = (((CharInAux >= '0') && (CharInAux <= '9')) || ((CharInAux >= 'A') && (CharInAux <= 'F')))? 1 : 0;
	if(CharInAux == 0x1B || EntryNumber == 0){
		switch(CharInAux){
			case 0x31:												//(1)
				EntryNumber = 1;
				OptionLevelA (CharInAux);break;
			case 0x32:												//(2)->(R) or (W)
				EntryNumber = 2;
				OptionLevelA (CharInAux);break;
			case 0x33:												//(3) Reset
				EntryNumber = 3;
				OptionLevelA (CharInAux);break;
			case 0x34:												//(4) Info
				EntryNumber = 4;
				OptionLevelA (CharInAux);break;
			case 0x1B:												//(ESC)
				EntryNumber = EntryNumberRW = iX = 0;
		    __HAL_TIM_DISABLE(&htim2);
				UARTInterface();break;
			case 0x49:												//(I)
				EntryNumber = 5;
				__HAL_TIM_ENABLE(&htim2);break;
		}
	}
	else if(CharInAux >= '1' && CharInAux <= '5' && EntryNumber == 1){
		TimerProgramming(CharInAux);
	}
	else if(EntryNumber == 2){																						//(R) or (W)
		if(CharInAux == 'R' && iX == 0){																		//Show message: "Introduce value"
			EntryNumberRW = CharInAux;
			HAL_UART_Transmit(&huart1,&INTERFACE_Reading[0][0], sizeof(INTERFACE_Reading[0]), 10);
			iX++;
		}
		else if(EntryNumberRW == 'R' && iX == 1 && flagCarGoodAux == 1){		//Catch first value
			number[0] = CharInAux;
			iX++;			
		}
		else if(EntryNumberRW == 'R' && iX == 2 && flagCarGoodAux == 1 ){		//Catch second value
			number[1] = CharInAux;
			ReadTransmission();
			iX = 0;			
		}
		else if(CharInAux == 'W' && iX == 0){																												//Show message: "For Introduce register"
			EntryNumberRW = CharInAux;
			HAL_UART_Transmit(&huart1,&INTERFACE_Writing[0][0], sizeof(INTERFACE_Writing[0]), 10);
			iX++;
		}
		else if(EntryNumberRW == 'W' && iX == 1 && flagCarGoodAux == 1){														//Catch first value
			number[0] = CharInAux;
			iX++;			
		}
		else if(EntryNumberRW == 'W' && iX == 2 && flagCarGoodAux == 1 ){														//Catch second value
			number[1] = CharInAux;
			HAL_UART_Transmit(&huart1,&INTERFACE_Writing[1][0], sizeof(INTERFACE_Writing[1]), 10);		//Register to write 0x
			iX++;			
		}
		else if(EntryNumberRW == 'W' && iX == 3 && flagCarGoodAux == 1 ){														//Catch second value
			number[2] = CharInAux;
			iX++;			
		}
		else if(EntryNumberRW == 'W' && iX == 4 && flagCarGoodAux == 1 ){														//Catch second value
			number[3] = CharInAux;
			WriteValue(iX);
			iX++;
		}
	}
}
void ReadTP(void){
	uint8_t Reg2Read8Aux = 0xF7, j = 0, i = 0, k = 0, t = 0,  UartTdata[12];
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);																					//SPI NSS low
	HAL_SPI_TransmitReceive(&hspi1, &Reg2Read8Aux, &MatrixRdata[0], 7, 10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);																						//SPI NSS high
	while(i < 6){
		Uint8_t2ArrayConver(MatrixRdata[(i+1)], &UartTdata[j]);										
		i++; j++; j++;
	}
	UpperCaseConversion12 (&UartTdata[0]);																													//Uppercase if there are some letter
	while(k < 12){
		HAL_UART_Transmit(&huart1,&INTERFACE_PT[t][0], sizeof(INTERFACE_PT[t]), 10);									//Show P: 0x
		HAL_UART_Transmit(&huart1,&UartTdata[k], 2, 10);																							//Show pressure data
		t++; k++; k++;
	}
}
int Array2Uint8_tConver02(uint8_t *NumberAux){														//Conversion from array to uint8_t
	int Reg2Read32Aux = 0;
	sscanf(NumberAux, "%02x", &Reg2Read32Aux);
	return Reg2Read32Aux;
}
void Uint8_t2ArrayConver(uint8_t CharInAux, uint8_t *ArrayCharInAux){			//Conversion from uint8_t to array
	sprintf(ArrayCharInAux, "%02x", CharInAux);
}
int Char2HexConver(char CharInAux){																				//Conversion from char to int
	return (atoi(&CharInAux));
}
void UpperCaseConversion(uint8_t *NumberAux){														//Uppercase if there are some letter
	uint8_t shift = 'a' - 'A';
	if((*NumberAux>='a')&&(*NumberAux<='z')) *NumberAux=*NumberAux-shift;						//Uppercase
	NumberAux++;
	if((*NumberAux>='a')&&(*NumberAux<='z')) *NumberAux=*NumberAux-shift;						//Uppercase
}
void UpperCaseConversion12(uint8_t *NumberAux){														//Uppercase if there are some letter
	uint8_t shift = 'a' - 'A', i = 0;
	while(i < 12){
		if((*NumberAux>='a')&&(*NumberAux<='z')) *NumberAux=*NumberAux-shift;					//Uppercase
		NumberAux++;
		i++;
	}
}
