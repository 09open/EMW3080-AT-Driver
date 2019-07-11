/**
  ******************************************************************************
  * @file           : emw3080.hpp
  * @brief          : Header for emw3080.hpp file.
  *                   This file contains the common defines of the application.
	* @attention			: Copyright(c) 2019 陈世纪
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
//模块
#define AT 								"AT\r"									//AT测试

#define AT_FWVER					"AT+FWVER?\r"						//查询固件版本号。
#define _FWVER						"FWVER"

#define AT_SYSTIME				"AT+SYSTIME?\r"					//查询系统运行时间。
#define _SYSTIME					"SYSTIME"

#define AT_FACTORY				"AT+FACTORY\r"					//恢复 模块出厂设置。

#define AT_REBOOT					"AT+REBOOT\r"						//重启模块。

//串口
#define AT_UARTE_OFF 			"AT+UARTE=OFF\r"				//串口回显关
#define AT_UARTE_ON 			"AT+UARTE=ON\r"					//串口回显开
#define _UARTE						"UARTE"

//WIFI
#define AT_WMAC						"AT+WMAC?\r"						//查询模块MAC地址
#define _WMAC							"WMAC"									

#define AT_WSCAN 					"AT+WSCAN\r"						//启动模块扫描
#define _WSCAN						"WSCAN"

#define AT_WJAP						"AT+WJAP=?\r"						//查询Station的名称和密码
#define _WJAP							"WJAP"	

#define AT_WJAPQ					"AT+WJAPQ\r"						//断开当前Station连接
#define _WJAPQ						"WJAPQ"

#define AT_WJAPS					"AT+WJAPS\r"						//查询当前Station连接状态
#define _WJAPS						"WJAPS"

//TCP 和 UDP 通信
#define AT_CIPSTART				"AT+CIPSTART="					//启动一个指定 id 的连接
#define AT_CIPSTOP 				"AT+CIPSTOP="						//断开一个指定 id 的连接
#define AT_CIPSTATUS			"AT+CIPSTATUS="					//查询一个指定 id 的连接状态
#define AT_CIPSEND				"AT+CIPSEND="						//通过串口向指定 id 的连接发送数据

//配网
#define AT_SMARTSTART 		"AT+SMARTSTART=2\r"			//启动 Airkiss 配网
#define AT_SMARTSTOP			"AT+SMARTSTOP=2\r"			//停止 Airkiss 配网

//SNTP 时间同步
#define AT_SNTPCFG				"AT+SNTPCFG=+8,cn.ntp.org.cn,pool.ntp.org\r"//设置NTP时区和域名

#define AT_SNTPTIME				"AT+SNTPTIME\r"					//获取NTP时间
#define _SNTPTIME					"SNTPTIME"							//

#define AT_RTCGET					"AT+RTCGET\r"						//获取RTC时间
#define _RTCGET						"RTCGET"

//***事件消息***
#define _WEVENT						"WEVENT"
#define _CIPEVENT					"CIPEVENT"


//状态
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
bool Str_head_Cp(uint8_t* dat,const char str[]);	//字符串首对比
uint8_t Str_Length(const char str[]);				

void Module_Tx(const char str[]);
void Module_Tx_Number(uint32_t i);								//模块数字发送

//API
void En_Smart_Wifi(void);

























#endif













