/**
  ******************************************************************************
  * @file           : emw3080.cpp
  * @brief          : 3080 program body
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

/*         外部引用             */

extern CAN_HandleTypeDef hcan;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


extern SRAM_HandleTypeDef hsram1;


extern LED_OUT LD1;
extern LED_OUT LD2;

/*888888888888888888888888888888*/



//帧类型定义
#define BS_DAT 0xA1			//单帧/结束帧
#define BS_DAT_G 0xA2		//断帧（未结束帧）
//----------



/*-----------模块状态标识定义--------------------*/

//网络连接状态
#define EMW_STATION_DOWN    0xA1    //未连接
#define EMW_STATION_UP      0xB1    //连接
#define EMW_STATION_NULL	0xC1	//其他状态

uint8_t EMW_STATION	= EMW_STATION_NULL;	//默认未连接

//是否收到模块确认标识
#define Rx_OK 0xAA
#define Rx_ERROR 0x55
#define Rx_Null 0x00

uint8_t Rx_Default = Rx_Null;
uint8_t * Rx_Status = &Rx_Default;

//快连使能标识
#define WIFI_SMART_START	0x55
#define WIFI_SMART_STOP		0x66
#define WIFI_SMART_ERROR    0x77
uint8_t EMW_WIFI_SMART = WIFI_SMART_STOP;	//快连默认关闭
uint8_t Rx_WIFI_SMART = Rx_Null;

//数据连接状态
#define SERVER_DISCONNECTED	0x92	//连接断开
#define SERVER_CONNECTED		0x62	//连接成功
#define SERVER_CLOSED				0x32	//关闭连接
#define SERVER_NULL					0x02	//连接状态未知

uint8_t SERVER_STATUS = SERVER_NULL;	//默认未知

/*----------------------------------------------*/

__noinit__ uart_buf Uart1_buf[20]  SRAM__; 
__noinit__ uint8_t Uart1_Bs  SRAM__;    //buf现在的位置
__noinit__ uint64_t Uart1_ID  SRAM__;   //数据串ID


__noinit__ uart_buf Uart2_buf[20]  SRAM__;
__noinit__ uint8_t Uart2_Bs  SRAM__;    //buf现在的位置
__noinit__ uint64_t Uart2_ID  SRAM__;   //数据串ID




//@define END******************************************************



//@code start******************************************************
void USART1_IRQHandler_Calk(void)		//串口1回调函数
{
    uint8_t DAT = 0;
    
    if(USART1->SR&0x20)
    {
        DAT = USART1->DR;
        
        Uart1_buf[Uart1_Bs].buf[Uart1_buf[Uart1_Bs].buf_Size] = DAT;
        Uart1_buf[Uart1_Bs].buf_Size++;
        if(DAT == '\r')   											//帧传输完成
        {              
            Uart1_ID++;                         //帧ID+1
            Uart1_buf[Uart1_Bs].ID = Uart1_ID;  //给当前帧加上ID
            Uart1_buf[Uart1_Bs].BS = BS_DAT;    //给当前帧加上标识
            
            Uart1_Bs++;Uart1_Bs %=20;           //切换到下一帧
            Uart1_buf[Uart1_Bs].buf_Size = 0;
            return;
        }
        
        if(Uart1_buf[Uart1_Bs].buf_Size == 50)  //帧拼接
        {
            Uart1_ID++;                         //帧ID+1
            Uart1_buf[Uart1_Bs].ID = Uart1_ID;  //给当前帧加上ID
            Uart1_buf[Uart1_Bs].BS = BS_DAT_G;  //给当前帧加上拼接标识
            
            Uart1_Bs++;Uart1_Bs %=20;           //切换到下一帧
            Uart1_buf[Uart1_Bs].buf_Size = 0;
        }
        
    }
}


void USART2_IRQHandler_Calk(void)		//串口2回调函数
{
    static uint8_t S_DAT = 0; //上一个数据
    uint8_t DAT = 0;
    
    if(USART2->SR&0x20)
    {
        DAT = USART2->DR;
        
        Uart2_buf[Uart2_Bs].buf[Uart2_buf[Uart2_Bs].buf_Size] = DAT;
        Uart2_buf[Uart2_Bs].buf_Size++;
        if((S_DAT == '\r') && (DAT == '\n'))    //帧传输完成
        {
            S_DAT = 0;                          //
            
            Uart2_ID++;                         //帧ID+1
            Uart2_buf[Uart2_Bs].ID = Uart2_ID;  //给当前帧加上ID
            Uart2_buf[Uart2_Bs].BS = BS_DAT;    //给当前帧加上标识
            
            Uart2_Bs++;Uart2_Bs %=20;           //切换到下一帧
            Uart2_buf[Uart2_Bs].buf_Size = 0;
            return;
        }
        
        if(Uart2_buf[Uart2_Bs].buf_Size == 50)  //帧拼接
        {
            Uart2_ID++;
            Uart2_buf[Uart2_Bs].ID = Uart2_ID;  //给当前帧加上ID
            Uart2_buf[Uart2_Bs].BS = BS_DAT_G;  //给当前帧加上拼接标识
            
            Uart2_Bs++;Uart2_Bs %=20;           //切换到下一帧
            Uart2_buf[Uart2_Bs].buf_Size = 0;
        }
        S_DAT = DAT;
    }
}
//END*********************************************************************



uint64_t C_UART1_ID = 0x0;
uint64_t C_UART2_ID = 0x0;

void emw3080_start(void)					//3080初始化
{
    Uart1_Bs =0;    //buf现在的位置
    Uart1_ID =0;   	//数据串ID
    Uart2_Bs =0;    //buf现在的位置
    Uart2_ID =0;   	//数据串ID
    
    Uart1_buf[Uart1_Bs].buf_Size = 0;
    Uart2_buf[Uart2_Bs].buf_Size = 0;
}

#define SEND_ID "1,"

void emw3080_loop(void)					//3080主循环
{
    if(C_UART1_ID<Uart1_ID)
    {
        C_UART1_ID++;
        
        for(uint8_t x=0;x<20;x++)
        {
            if(Uart1_buf[x].ID == C_UART1_ID)
            {
                HAL_UART_Transmit(&huart2,Uart1_buf[x].buf,Uart1_buf[x].buf_Size,500);
                break;
            }
        }
    }
    
    if(C_UART2_ID<Uart2_ID)
    {
        C_UART2_ID++;
        
        for(uint8_t x=0;x<20;x++)
        {
            if(Uart2_buf[x].ID == C_UART2_ID)
            {
                HAL_UART_Transmit(&huart1,Uart2_buf[x].buf,Uart2_buf[x].buf_Size,500);
								Rx_manage(&Uart2_buf[x]);
								
								
                break;
            }
        }
    }
		
		EMW3080_State_Machine();	//3080模块调度器
		
		
		if(EMW_WIFI_SMART != WIFI_SMART_STOP)	//LED指示灯
		{
				static uint32_t x;
				x++;
				if(x%10000==0)
				LD1.LD_Fan();
		}else
		{
				if(EMW_STATION == EMW_STATION_UP)
				LD1.LD_Set(GPIO_PIN_RESET);
				else
				LD1.LD_Set(GPIO_PIN_SET);
		}
		
		
		LD2.LD_Set(GPIO_PIN_SET);							//数据连接后
		if(SERVER_STATUS == SERVER_CONNECTED)
		{
				LD2.LD_Set(GPIO_PIN_RESET);
			
				static uint32_t x1=0;
				x1++;
				if(x1==1000000)
				{
						x1=0;
						//网络连接循环发送心跳包
						Module_Tx(AT_CIPSEND);
						Module_Tx(SEND_ID);
						Module_Tx_Number(Str_Length(I_LIKE_));
						Module_Tx(SYS_r);
						Module_Tx(I_LIKE_);
						Module_Tx(SYS_r);
						
				}
		}
}

//**************************************调度 or 处理***************************************************


void EMW3080_State_Machine(void)				//调度器
{
	
		static uint8_t station_status = EMW_STATION_NULL;	//记录以往wifi连接状态
	
		static uint8_t switch_step_init = 0;		//初始化
		static uint8_t switch_step_smart = 0;		//快连
		
		static uint32_t Error_Times = 0;		//初始化超时
	
	
		switch(switch_step_init)
		{
				case 0:Module_Tx(AT_REBOOT);switch_step_init++; Error_Times = 0; break;
		
				case 1:if(Rx_Default==Rx_OK){Rx_Default=Rx_Null; printf("MCU:restart !\r\n"); switch_step_init++;}
								if(Error_Times>1000000)switch_step_init--; else Error_Times++;
									break;
			
				case 2: Module_Tx(AT); switch_step_init++; Error_Times = 0; break;

				case 3: if(Rx_Default==Rx_OK){Rx_Default=Rx_Null; printf("MCU:AT command Is Ok !\r\n"); switch_step_init++;}
								if(Error_Times>1000000)switch_step_init--; else Error_Times++;
								break;

				case 4: Module_Tx(AT_UARTE_OFF); switch_step_init++; Error_Times = 0; break;

				case 5: if(Rx_Default==Rx_OK){Rx_Default=Rx_Null;  printf("MCU:AT 回显关闭\r\n"); switch_step_init++;}
								if(Error_Times>1000000)switch_step_init--; else Error_Times++;
								break;

				case 6: break;
		}
		//---------------------------------------------------------
		
		
		static uint16_t SMART_Time = 0;
		if(EMW_WIFI_SMART != WIFI_SMART_STOP)	//快连-------------------------------------------------------------------------------------------------
		{
				//如果中途模块发来错误回复，则处理完退出
				if(Rx_Status == &Rx_WIFI_SMART)
				if(*Rx_Status == Rx_ERROR){EMW_WIFI_SMART = WIFI_SMART_ERROR;switch_step_smart = 4;printf("\r\nError:_0xE002\r\n");} 		
				
				
				switch(switch_step_smart)
				{
						case 0: if((Rx_Status == &Rx_Default)&&(EMW_WIFI_SMART == WIFI_SMART_START))	//判断有无其他程序占用标识,并处于非错误状态
										{
												Rx_Status = &Rx_WIFI_SMART;				//重定向模块确认标识
												*Rx_Status = Rx_Null;
												if(EMW_STATION == EMW_STATION_UP)	//判断网络是否连接
												{ 
														Module_Tx(AT_WJAPQ);					//连接则断
														switch_step_smart = 1;
												}else
												{
														switch_step_smart = 2;				//未连接则跳过，应该不存在连接未知的情况（新模块和断开连接的情况上电不自动更新状态）
												}
										}else																	
										{																			//标识被占用，等待超时	。（错误状态无程序处理自动清除并停止快连）	
												SMART_Time++; 
												if(SMART_Time%1000==999)
												{
														if(SMART_Time>9000)
														{
																EMW_WIFI_SMART = WIFI_SMART_STOP;
														}
												}
										}				
										break;
										
						//---------***********
						case 1: if(EMW_STATION == EMW_STATION_DOWN) switch_step_smart = 2; 	//连接断开判断
										break;
										
						//---------***********
						case 2: Module_Tx(AT_SMARTSTART);	switch_step_smart++; 							//连接断开了，开始快连
										break;
										
						//---------***********
						case 3: if((EMW_STATION == EMW_STATION_UP)&&(*Rx_Status == Rx_OK))	//网络自动连接，代表快连目标达成
										{
												switch_step_smart = 6;																	//结束快连
										}
										break;
										
						case 4: Module_Tx(AT_SMARTSTOP); switch_step_smart++; 							//结束快连						跳过/-/-/-/-/-
										break;
										
						case 5: if(*Rx_Status == Rx_OK) switch_step_smart++; 								//关闭快连得到确认		跳过/-/-/-/-/-
										break;
										
						//---------***********
						case 6: if(EMW_WIFI_SMART == WIFI_SMART_START)EMW_WIFI_SMART = WIFI_SMART_STOP;	//非错误状态则停止快连
										
										Rx_Status = &Rx_Default;																		//还原模块确认标识
										SMART_Time =0;
										switch_step_smart = 0;																			//
										break;
										
				}
		}//-----------------------快连 WIFI SMART ----END------------------------------------------------------------------
		
		
		//wifi连接状态管理-----------------------------------------------
		if(EMW_STATION != station_status)
		{
				station_status = EMW_STATION;
				
				switch(EMW_STATION)
				{
					case EMW_STATION_DOWN :printf("MCU: wifi station down!\r\n");break;
					case EMW_STATION_UP   :printf("MCU: wifi station up!\r\n"  );break;
					case EMW_STATION_NULL :printf("MCU: wifi station NULL!\r\n");break;
				}
		}//-------------------------------------------------------------
		
		
		
		
		
}

//------------------------------------------------------------

const char* _Orient_Str = NULL;
void Orient_Str(const char str[])				//查询回答语句定向
{
		_Orient_Str = str;
}

void De_Orient_Str(void)								//查询回答语句取消定向
{
		_Orient_Str = NULL;
}

uint32_t Dat_Lemgth = 0;
uint8_t Dat_start=0;
void Rx_manage(uart_buf * Rx_buff)			//接收模块数据整理/处理器
{
		uint8_t* Dat = Rx_buff->buf;
		uint8_t Size = Rx_buff->buf_Size;
		
		//---------------//空帧
		if((*Dat =='\r') || (*Dat =='\n'))	
		{
				if(Size<3)return;
		}
		
		//---------------回复/状态 帧
		if(*Dat=='+')		
		{
				Dat++;
				//主动查询回复消息解析
				if(_Orient_Str == NULL)
				{
						if(Str_head_Cp(Dat,_Orient_Str)) //头对比
						{
								if(Str_Cp(Dat,_Orient_Str))	//体对比
								{
										
										return;
								}
						}
				}
				
				//WIFI事件通知消息解析
				if(Str_head_Cp(Dat,_WEVENT)) //头对比*
				{
						if(Str_Cp(Dat,_WEVENT))	 //体对比*
						{
								Dat += Str_Length(_WEVENT);	//偏移
								Dat++;
								if(Str_head_Cp(Dat,_STATION_UP)) //头对比+
								{
										if(Str_Cp(Dat,_STATION_UP))	 //体对比+
										{
												EMW_STATION	= EMW_STATION_UP;//连接
										}else 
										if(Str_Cp(Dat,_STATION_DOWN)) //体对比+
										{
												EMW_STATION	= EMW_STATION_DOWN;//断开
										}else
										{
												EMW_STATION	= EMW_STATION_NULL;//未知
										}
								}
								
								return;
						}
				}
				
				//网络连接事件通知消息解析
				if(Str_head_Cp(Dat,_CIPEVENT)) //头对比
				{
						if(Str_Cp(Dat,_CIPEVENT))	//体对比
						{
								Dat += Str_Length(_CIPEVENT);			//偏移CIPEVENT
								Dat++;														//：
								
								if(Str_head_Cp(Dat,"1"))				//*********UDP/TCP连接状态******************
								{
										Dat++;											//1
										Dat++;											//,
										Dat += Str_Length(_SERVER);		//偏移SERVER
										Dat++;												//,
										
										if(Str_head_Cp(Dat,_CONNECTED))
										{
												if(Str_Cp(Dat,_CONNECTED))	
												{		//连接
														SERVER_STATUS = SERVER_CONNECTED;
												}
										}
										else
										if(Str_head_Cp(Dat,_DISCONNECTED))
										{
												if(Str_Cp(Dat,_DISCONNECTED))	
												{		//断开
														SERVER_STATUS = SERVER_DISCONNECTED;
												}
										}
										else
										if(Str_head_Cp(Dat,_CLOSED))
										{
												if(Str_Cp(Dat,_CLOSED))	
												{		//关闭
														SERVER_STATUS = SERVER_CLOSED;
												}
										}
								}
								else
								if(Str_head_Cp(Dat,_SOCKET))		//*******数据接收******************
								{
										if(Str_Cp(Dat,_SOCKET))
										{
												Dat += Str_Length(_SOCKET);//偏移SOCKET
												Dat ++;											//，
												
												if(Str_head_Cp(Dat,"1"))
												{
														Dat++;				//1
														Dat++;				//，
														Dat_Lemgth=0;
														
														do
														{
																Dat_Lemgth *= 10;
																Dat_Lemgth += (*Dat - '0');
																Dat++;
														}
														while(*Dat != ',');
														Dat++;
														
														Dat_start = (Dat - Rx_buff->buf);
														if((Size - Dat_start - 2) == Dat_Lemgth)
														{
																printf("\r\nDat_Lemgth = %d;\r\n",Dat_Lemgth);
																HAL_UART_Transmit(&huart1,Dat,Dat_Lemgth,500);
																printf("\r\n");
														}
														
												}
										}
								}
								
								return;
						}
				}
		//**IF*END**//
		}
		
		//---------------//正确/错误识别
		if(Str_Cp(Dat,AR_OK))
		{
				*Rx_Status=Rx_OK;
				//printf("ok\r\n");
				return;
		}
		
		if(Str_Cp(Dat,AR_ERROR))
		{
				*Rx_Status=Rx_ERROR;
				//printf("error\r\n");
				return;
		}
		
		//---------------END------------
		
}
//*********************************END*************************************


//******************************Tool Start*********************************
bool Str_Cp(uint8_t* dat,const char str[])				//字符串比对函数
{
		uint8_t i=0;
	
		do
		{
				if(str[i] != dat[i])
				{
						return false;
				}
				i++;
		}
		while(str[i]);
		return true;
}

bool Str_head_Cp(uint8_t* dat,const char str[])		//字符串首对比
{
		if(*dat != *str)
		{
				return false;
		}
		return true;
}

uint8_t Str_Length(const char str[])							//字符串算长
{
		uint8_t length = 0;
		
		while(str[length])
		{
				length++;
		}
		
		return length;
}

void Module_Tx(const char str[])									//模块指令发送
{
		uint16_t length=0;
		while(str[length])
		{
				length++;
		}
		if(length>0)
		{
				HAL_UART_Transmit(&huart2,(uint8_t *)str,length,500);
		}
}

void Module_Tx_Number(uint32_t i)									//模块数字发送
{
		uint32_t x = 1000000000;
		uint8_t y = 0x00;
		uint8_t buf_;
	
		
		while(true)
		{
				buf_ = '0';
				buf_ += ((i / x) % 10);
			
				if(x == 0)break;
				x /= 10;
			
				if(buf_ != '0')
				{
						y = 0x55;
				}
				
				if(y == 0x55)
				{
						HAL_UART_Transmit(&huart2,&buf_,1,500);
				}
				
		}
}	

//***********************************END************************************


//--------------------- A P I    Code --------------------------
void En_Smart_Wifi(void)
{
		if(EMW_WIFI_SMART == WIFI_SMART_STOP)	//快连处于关闭状态下才可以开启
			EMW_WIFI_SMART = WIFI_SMART_START;		//·····
}


//--------------------- E N D    -------------------------------



















