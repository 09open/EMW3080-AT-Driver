/**
  ******************************************************************************
  * @file           : emw3080.hpp
  * @brief          : Header for emw3080.hpp file.
  *                   This file contains the common defines of the application.
	* @attention			: Copyright(c) 2019 ������
	*										Licensed under the GPL License
	* @versions				: V1.0
  ******************************************************************************
  */


#ifndef __EMW3080_HPP
#define __EMW3080_HPP

#include "main.h"

struct uart_buf
{
    uint8_t BS;
    uint64_t ID;
    uint8_t buf_Size;
    uint8_t buf[50];
};





//-----------------------------------------------------------------------------
//ģ��
#define AT 								"AT\r"									//AT����

#define AT_FWVER					"AT+FWVER?\r"						//��ѯ�̼��汾�š�
#define _FWVER						"FWVER"

#define AT_SYSTIME				"AT+SYSTIME?\r"					//��ѯϵͳ����ʱ�䡣
#define _SYSTIME					"SYSTIME"

#define AT_FACTORY				"AT+FACTORY\r"					//�ָ� ģ��������á�

#define AT_REBOOT					"AT+REBOOT\r"						//����ģ�顣

//����
#define AT_UARTE_OFF 			"AT+UARTE=OFF\r"				//���ڻ��Թ�
#define AT_UARTE_ON 			"AT+UARTE=ON\r"					//���ڻ��Կ�
#define _UARTE						"UARTE"

//WIFI
#define AT_WMAC						"AT+WMAC?\r"						//��ѯģ��MAC��ַ
#define _WMAC							"WMAC"									

#define AT_WSCAN 					"AT+WSCAN\r"						//����ģ��ɨ��
#define _WSCAN						"WSCAN"

#define AT_WJAP						"AT+WJAP=?\r"						//��ѯStation�����ƺ�����
#define _WJAP							"WJAP"	

#define AT_WJAPQ					"AT+WJAPQ\r"						//�Ͽ���ǰStation����
#define _WJAPQ						"WJAPQ"

#define AT_WJAPS					"AT+WJAPS\r"						//��ѯ��ǰStation����״̬
#define _WJAPS						"WJAPS"

//TCP �� UDP ͨ��
#define AT_CIPSTART				"AT+CIPSTART="					//����һ��ָ�� id ������
#define AT_CIPSTOP 				"AT+CIPSTOP="						//�Ͽ�һ��ָ�� id ������
#define AT_CIPSTATUS			"AT+CIPSTATUS="					//��ѯһ��ָ�� id ������״̬
#define AT_CIPSEND				"AT+CIPSEND="						//ͨ��������ָ�� id �����ӷ�������

//����
#define AT_SMARTSTART 		"AT+SMARTSTART=2\r"			//���� Airkiss ����
#define AT_SMARTSTOP			"AT+SMARTSTOP=2\r"			//ֹͣ Airkiss ����

//SNTP ʱ��ͬ��
#define AT_SNTPCFG				"AT+SNTPCFG=+8,cn.ntp.org.cn,pool.ntp.org\r"//����NTPʱ��������

#define AT_SNTPTIME				"AT+SNTPTIME\r"					//��ȡNTPʱ��
#define _SNTPTIME					"SNTPTIME"							//

#define AT_RTCGET					"AT+RTCGET\r"						//��ȡRTCʱ��
#define _RTCGET						"RTCGET"

//***�¼���Ϣ***
#define _WEVENT						"WEVENT"
#define _CIPEVENT					"CIPEVENT"


//״̬
#define _STATION_UP				"STATION_UP"
#define _STATION_DOWN			"STATION_DOWN"
#define _AP_UP						"AP_UP"
#define _AP_DOWN					"AP_DOWN"

#define _SOCKET						"SOCKET"
#define _SERVER						"SERVER"

#define _CONNECTED				"CONNECTED"
#define _DISCONNECTED			"DISCONNECTED"
#define _CLOSED						"CLOSED"


//-----------------------------------------------------------------------------
#define AR_OK							"OK\r\n"
#define AR_ERROR					"ERROR\r\n"

#define I_LIKE_           "I Like you\r\n"

#define SYS_r							"\r"


void emw3080_start(void);
void emw3080_loop(void);


void EMW3080_State_Machine(void);

void Orient_Str(const char str[]);
void De_Orient_Str(void);

void Rx_manage(uart_buf * buff);

bool Str_Cp(uint8_t* dat,const char str[]);
bool Str_head_Cp(uint8_t* dat,const char str[]);	//�ַ����׶Ա�
uint8_t Str_Length(const char str[]);				

void Module_Tx(const char str[]);
void Module_Tx_Number(uint32_t i);								//ģ�����ַ���

//API
void En_Smart_Wifi(void);

























#endif













