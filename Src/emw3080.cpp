/**
  ******************************************************************************
  * @file           : emw3080.cpp
  * @brief          : 3080 program body
  *                   This file contains the common defines of the application.
	* @attention			: Copyright(c) 2019 ������
	*										Licensed under the GPL License
	* @versions				: V1.0
  ******************************************************************************
  */


#include "main.h"
#include "stdio.h"
#include "u_main.hpp"
#include "emw3080.hpp"

/*         �ⲿ����             */

extern CAN_HandleTypeDef hcan;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


extern SRAM_HandleTypeDef hsram1;


extern LED_OUT LD1;
extern LED_OUT LD2;

/*888888888888888888888888888888*/



//֡���Ͷ���
#define BS_DAT 0xA1			//��֡/����֡
#define BS_DAT_G 0xA2		//��֡��δ����֡��
//----------



/*-----------ģ��״̬��ʶ����--------------------*/

//��������״̬
#define EMW_STATION_DOWN    0xA1    //δ����
#define EMW_STATION_UP      0xB1    //����
#define EMW_STATION_NULL	0xC1	//����״̬

uint8_t EMW_STATION	= EMW_STATION_NULL;	//Ĭ��δ����

//�Ƿ��յ�ģ��ȷ�ϱ�ʶ
#define Rx_OK 0xAA
#define Rx_ERROR 0x55
#define Rx_Null 0x00

uint8_t Rx_Default = Rx_Null;
uint8_t * Rx_Status = &Rx_Default;

//����ʹ�ܱ�ʶ
#define WIFI_SMART_START	0x55
#define WIFI_SMART_STOP		0x66
#define WIFI_SMART_ERROR    0x77
uint8_t EMW_WIFI_SMART = WIFI_SMART_STOP;	//����Ĭ�Ϲر�
uint8_t Rx_WIFI_SMART = Rx_Null;

//��������״̬
#define SERVER_DISCONNECTED	0x92	//���ӶϿ�
#define SERVER_CONNECTED		0x62	//���ӳɹ�
#define SERVER_CLOSED				0x32	//�ر�����
#define SERVER_NULL					0x02	//����״̬δ֪

uint8_t SERVER_STATUS = SERVER_NULL;	//Ĭ��δ֪

/*----------------------------------------------*/

__noinit__ uart_buf Uart1_buf[20]  SRAM__; 
__noinit__ uint8_t Uart1_Bs  SRAM__;    //buf���ڵ�λ��
__noinit__ uint64_t Uart1_ID  SRAM__;   //���ݴ�ID


__noinit__ uart_buf Uart2_buf[20]  SRAM__;
__noinit__ uint8_t Uart2_Bs  SRAM__;    //buf���ڵ�λ��
__noinit__ uint64_t Uart2_ID  SRAM__;   //���ݴ�ID




//@define END******************************************************



//@code start******************************************************
void USART1_IRQHandler_Calk(void)		//����1�ص�����
{
    uint8_t DAT = 0;
    
    if(USART1->SR&0x20)
    {
        DAT = USART1->DR;
        
        Uart1_buf[Uart1_Bs].buf[Uart1_buf[Uart1_Bs].buf_Size] = DAT;
        Uart1_buf[Uart1_Bs].buf_Size++;
        if(DAT == '\r')   											//֡�������
        {              
            Uart1_ID++;                         //֡ID+1
            Uart1_buf[Uart1_Bs].ID = Uart1_ID;  //����ǰ֡����ID
            Uart1_buf[Uart1_Bs].BS = BS_DAT;    //����ǰ֡���ϱ�ʶ
            
            Uart1_Bs++;Uart1_Bs %=20;           //�л�����һ֡
            Uart1_buf[Uart1_Bs].buf_Size = 0;
            return;
        }
        
        if(Uart1_buf[Uart1_Bs].buf_Size == 50)  //֡ƴ��
        {
            Uart1_ID++;                         //֡ID+1
            Uart1_buf[Uart1_Bs].ID = Uart1_ID;  //����ǰ֡����ID
            Uart1_buf[Uart1_Bs].BS = BS_DAT_G;  //����ǰ֡����ƴ�ӱ�ʶ
            
            Uart1_Bs++;Uart1_Bs %=20;           //�л�����һ֡
            Uart1_buf[Uart1_Bs].buf_Size = 0;
        }
        
    }
}


void USART2_IRQHandler_Calk(void)		//����2�ص�����
{
    static uint8_t S_DAT = 0; //��һ������
    uint8_t DAT = 0;
    
    if(USART2->SR&0x20)
    {
        DAT = USART2->DR;
        
        Uart2_buf[Uart2_Bs].buf[Uart2_buf[Uart2_Bs].buf_Size] = DAT;
        Uart2_buf[Uart2_Bs].buf_Size++;
        if((S_DAT == '\r') && (DAT == '\n'))    //֡�������
        {
            S_DAT = 0;                          //
            
            Uart2_ID++;                         //֡ID+1
            Uart2_buf[Uart2_Bs].ID = Uart2_ID;  //����ǰ֡����ID
            Uart2_buf[Uart2_Bs].BS = BS_DAT;    //����ǰ֡���ϱ�ʶ
            
            Uart2_Bs++;Uart2_Bs %=20;           //�л�����һ֡
            Uart2_buf[Uart2_Bs].buf_Size = 0;
            return;
        }
        
        if(Uart2_buf[Uart2_Bs].buf_Size == 50)  //֡ƴ��
        {
            Uart2_ID++;
            Uart2_buf[Uart2_Bs].ID = Uart2_ID;  //����ǰ֡����ID
            Uart2_buf[Uart2_Bs].BS = BS_DAT_G;  //����ǰ֡����ƴ�ӱ�ʶ
            
            Uart2_Bs++;Uart2_Bs %=20;           //�л�����һ֡
            Uart2_buf[Uart2_Bs].buf_Size = 0;
        }
        S_DAT = DAT;
    }
}
//END*********************************************************************



uint64_t C_UART1_ID = 0x0;
uint64_t C_UART2_ID = 0x0;

void emw3080_start(void)					//3080��ʼ��
{
    Uart1_Bs =0;    //buf���ڵ�λ��
    Uart1_ID =0;   	//���ݴ�ID
    Uart2_Bs =0;    //buf���ڵ�λ��
    Uart2_ID =0;   	//���ݴ�ID
    
    Uart1_buf[Uart1_Bs].buf_Size = 0;
    Uart2_buf[Uart2_Bs].buf_Size = 0;
}

#define SEND_ID "1,"

void emw3080_loop(void)					//3080��ѭ��
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
		
		EMW3080_State_Machine();	//3080ģ�������
		
		
		if(EMW_WIFI_SMART != WIFI_SMART_STOP)	//LEDָʾ��
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
		
		
		LD2.LD_Set(GPIO_PIN_SET);							//�������Ӻ�
		if(SERVER_STATUS == SERVER_CONNECTED)
		{
				LD2.LD_Set(GPIO_PIN_RESET);
			
				static uint32_t x1=0;
				x1++;
				if(x1==1000000)
				{
						x1=0;
						//��������ѭ������������
						Module_Tx(AT_CIPSEND);
						Module_Tx(SEND_ID);
						Module_Tx_Number(Str_Length(I_LIKE_));
						Module_Tx(SYS_r);
						Module_Tx(I_LIKE_);
						Module_Tx(SYS_r);
						
				}
		}
}

//**************************************���� or ����***************************************************


void EMW3080_State_Machine(void)				//������
{
	
		static uint8_t station_status = EMW_STATION_NULL;	//��¼����wifi����״̬
	
		static uint8_t switch_step_init = 0;		//��ʼ��
		static uint8_t switch_step_smart = 0;		//����
		
		static uint32_t Error_Times = 0;		//��ʼ����ʱ
	
	
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

				case 5: if(Rx_Default==Rx_OK){Rx_Default=Rx_Null;  printf("MCU:AT ���Թر�\r\n"); switch_step_init++;}
								if(Error_Times>1000000)switch_step_init--; else Error_Times++;
								break;

				case 6: break;
		}
		//---------------------------------------------------------
		
		
		static uint16_t SMART_Time = 0;
		if(EMW_WIFI_SMART != WIFI_SMART_STOP)	//����-------------------------------------------------------------------------------------------------
		{
				//�����;ģ�鷢������ظ����������˳�
				if(Rx_Status == &Rx_WIFI_SMART)
				if(*Rx_Status == Rx_ERROR){EMW_WIFI_SMART = WIFI_SMART_ERROR;switch_step_smart = 4;printf("\r\nError:_0xE002\r\n");} 		
				
				
				switch(switch_step_smart)
				{
						case 0: if((Rx_Status == &Rx_Default)&&(EMW_WIFI_SMART == WIFI_SMART_START))	//�ж�������������ռ�ñ�ʶ,�����ڷǴ���״̬
										{
												Rx_Status = &Rx_WIFI_SMART;				//�ض���ģ��ȷ�ϱ�ʶ
												*Rx_Status = Rx_Null;
												if(EMW_STATION == EMW_STATION_UP)	//�ж������Ƿ�����
												{ 
														Module_Tx(AT_WJAPQ);					//�������
														switch_step_smart = 1;
												}else
												{
														switch_step_smart = 2;				//δ������������Ӧ�ò���������δ֪���������ģ��ͶϿ����ӵ�����ϵ粻�Զ�����״̬��
												}
										}else																	
										{																			//��ʶ��ռ�ã��ȴ���ʱ	��������״̬�޳������Զ������ֹͣ������	
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
						case 1: if(EMW_STATION == EMW_STATION_DOWN) switch_step_smart = 2; 	//���ӶϿ��ж�
										break;
										
						//---------***********
						case 2: Module_Tx(AT_SMARTSTART);	switch_step_smart++; 							//���ӶϿ��ˣ���ʼ����
										break;
										
						//---------***********
						case 3: if((EMW_STATION == EMW_STATION_UP)&&(*Rx_Status == Rx_OK))	//�����Զ����ӣ��������Ŀ����
										{
												switch_step_smart = 6;																	//��������
										}
										break;
										
						case 4: Module_Tx(AT_SMARTSTOP); switch_step_smart++; 							//��������						����/-/-/-/-/-
										break;
										
						case 5: if(*Rx_Status == Rx_OK) switch_step_smart++; 								//�رտ����õ�ȷ��		����/-/-/-/-/-
										break;
										
						//---------***********
						case 6: if(EMW_WIFI_SMART == WIFI_SMART_START)EMW_WIFI_SMART = WIFI_SMART_STOP;	//�Ǵ���״̬��ֹͣ����
										
										Rx_Status = &Rx_Default;																		//��ԭģ��ȷ�ϱ�ʶ
										SMART_Time =0;
										switch_step_smart = 0;																			//
										break;
										
				}
		}//-----------------------���� WIFI SMART ----END------------------------------------------------------------------
		
		
		//wifi����״̬����-----------------------------------------------
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
void Orient_Str(const char str[])				//��ѯ�ش���䶨��
{
		_Orient_Str = str;
}

void De_Orient_Str(void)								//��ѯ�ش����ȡ������
{
		_Orient_Str = NULL;
}

uint32_t Dat_Lemgth = 0;
uint8_t Dat_start=0;
void Rx_manage(uart_buf * Rx_buff)			//����ģ����������/������
{
		uint8_t* Dat = Rx_buff->buf;
		uint8_t Size = Rx_buff->buf_Size;
		
		//---------------//��֡
		if((*Dat =='\r') || (*Dat =='\n'))	
		{
				if(Size<3)return;
		}
		
		//---------------�ظ�/״̬ ֡
		if(*Dat=='+')		
		{
				Dat++;
				//������ѯ�ظ���Ϣ����
				if(_Orient_Str == NULL)
				{
						if(Str_head_Cp(Dat,_Orient_Str)) //ͷ�Ա�
						{
								if(Str_Cp(Dat,_Orient_Str))	//��Ա�
								{
										
										return;
								}
						}
				}
				
				//WIFI�¼�֪ͨ��Ϣ����
				if(Str_head_Cp(Dat,_WEVENT)) //ͷ�Ա�*
				{
						if(Str_Cp(Dat,_WEVENT))	 //��Ա�*
						{
								Dat += Str_Length(_WEVENT);	//ƫ��
								Dat++;
								if(Str_head_Cp(Dat,_STATION_UP)) //ͷ�Ա�+
								{
										if(Str_Cp(Dat,_STATION_UP))	 //��Ա�+
										{
												EMW_STATION	= EMW_STATION_UP;//����
										}else 
										if(Str_Cp(Dat,_STATION_DOWN)) //��Ա�+
										{
												EMW_STATION	= EMW_STATION_DOWN;//�Ͽ�
										}else
										{
												EMW_STATION	= EMW_STATION_NULL;//δ֪
										}
								}
								
								return;
						}
				}
				
				//���������¼�֪ͨ��Ϣ����
				if(Str_head_Cp(Dat,_CIPEVENT)) //ͷ�Ա�
				{
						if(Str_Cp(Dat,_CIPEVENT))	//��Ա�
						{
								Dat += Str_Length(_CIPEVENT);			//ƫ��CIPEVENT
								Dat++;														//��
								
								if(Str_head_Cp(Dat,"1"))				//*********UDP/TCP����״̬******************
								{
										Dat++;											//1
										Dat++;											//,
										Dat += Str_Length(_SERVER);		//ƫ��SERVER
										Dat++;												//,
										
										if(Str_head_Cp(Dat,_CONNECTED))
										{
												if(Str_Cp(Dat,_CONNECTED))	
												{		//����
														SERVER_STATUS = SERVER_CONNECTED;
												}
										}
										else
										if(Str_head_Cp(Dat,_DISCONNECTED))
										{
												if(Str_Cp(Dat,_DISCONNECTED))	
												{		//�Ͽ�
														SERVER_STATUS = SERVER_DISCONNECTED;
												}
										}
										else
										if(Str_head_Cp(Dat,_CLOSED))
										{
												if(Str_Cp(Dat,_CLOSED))	
												{		//�ر�
														SERVER_STATUS = SERVER_CLOSED;
												}
										}
								}
								else
								if(Str_head_Cp(Dat,_SOCKET))		//*******���ݽ���******************
								{
										if(Str_Cp(Dat,_SOCKET))
										{
												Dat += Str_Length(_SOCKET);//ƫ��SOCKET
												Dat ++;											//��
												
												if(Str_head_Cp(Dat,"1"))
												{
														Dat++;				//1
														Dat++;				//��
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
		
		//---------------//��ȷ/����ʶ��
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
bool Str_Cp(uint8_t* dat,const char str[])				//�ַ����ȶԺ���
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

bool Str_head_Cp(uint8_t* dat,const char str[])		//�ַ����׶Ա�
{
		if(*dat != *str)
		{
				return false;
		}
		return true;
}

uint8_t Str_Length(const char str[])							//�ַ����㳤
{
		uint8_t length = 0;
		
		while(str[length])
		{
				length++;
		}
		
		return length;
}

void Module_Tx(const char str[])									//ģ��ָ���
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

void Module_Tx_Number(uint32_t i)									//ģ�����ַ���
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
		if(EMW_WIFI_SMART == WIFI_SMART_STOP)	//�������ڹر�״̬�²ſ��Կ���
			EMW_WIFI_SMART = WIFI_SMART_START;		//����������
}


//--------------------- E N D    -------------------------------



















