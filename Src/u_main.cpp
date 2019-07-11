/**
  ******************************************************************************
  * @file           : u_main.cpp
  * @brief          : Main program body
  *                   This file contains the common defines of the application.
	* @attention			: Copyright(c) 2019 陈世纪
	*										Licensed under the GPL License
	* @versions				: V1.0
  ******************************************************************************
  */
  
  
#include "main.h"
#include "stdio.h"
#include "u_main.hpp"

#include "emw3080.hpp"

extern CAN_HandleTypeDef hcan;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


extern SRAM_HandleTypeDef hsram1;

///------------------------------///
extern uint8_t printf_en;

///------------------------------///


/**
  ******************************************************************************
  */
LED_OUT::LED_OUT(GPIO_TypeDef *GPIOx_,uint16_t GPIO_Pin_)
{
    GPIOx = GPIOx_;
    GPIO_Pin = GPIO_Pin_;
    PinState = GPIO_PIN_SET;
}

void LED_OUT::LD_Set(GPIO_PinState PinState_)
{
    PinState = PinState_;
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, PinState_);
}

GPIO_PinState LED_OUT::LD_Get(void)
{
    return PinState;
}

void LED_OUT::LD_Fan(void)
{
    if(PinState == GPIO_PIN_RESET)
    {
        PinState = GPIO_PIN_SET;
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
    }
    else
    { 
        PinState = GPIO_PIN_RESET;
        HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
    }
}
/**
  ******************************************************************************
  */

LED_OUT LD1(LD4_GPIO_Port,LD4_Pin);
LED_OUT LD2(LD5_GPIO_Port,LD5_Pin);



void Key_Manage(void)
{
		static 	bool S_Status =false;
						bool B_Status	=false;
	
		static uint32_t K_count=0;
	
		if((KEY3_GPIO_Port->IDR & KEY3_Pin) == 0)
		{
				B_Status = true;
			
				if(K_count<100000000)K_count++;
		}
		
		if(K_count == 1000000)
		{
				printf("\r\n开启快连\r\n");
				En_Smart_Wifi();
		}
		
		if(S_Status != B_Status)
		{		
				if(K_count > 2000 && K_count < 50000)
				{
						//printf("\r\n短按\r\n");
				}
				K_count = 0;
		}
		S_Status = B_Status;
}

//*****************************************

void u_main(void)
{
    uint8_t buf;
    
    emw3080_start();
    
    printf("UART OK !\r\n");
    HAL_UART_Receive_IT(&huart1,&buf,1);
    HAL_UART_Receive_IT(&huart2,&buf,1);
    while(true)
    {
        emw3080_loop(); 
				Key_Manage();	
    }
}



