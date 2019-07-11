/**
  ******************************************************************************
  * @file           : u_main.hpp
  * @brief          : Header for u_main.hpp file.
  *                   This file contains the common defines of the application.
	* @attention			: Copyright(c) 2019 ³ÂÊÀ¼Í
	*										Licensed under the GPL License
	* @versions				: V1.0
  ******************************************************************************
  */
  
  
#ifndef __U_MAIN_HPP
#define __U_MAIN_HPP

#include "main.h"



class LED_OUT
{
    private:
        GPIO_TypeDef *GPIOx;
        uint16_t GPIO_Pin;
        GPIO_PinState PinState;
    
    public:
        LED_OUT(GPIO_TypeDef *GPIOx_,uint16_t GPIO_Pin);
        void LD_Set(GPIO_PinState PinState);
        GPIO_PinState LD_Get(void);
        void LD_Fan(void);
};















#endif
