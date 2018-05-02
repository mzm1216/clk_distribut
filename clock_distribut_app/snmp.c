/*
 * snmp.c
 *
 *  Created on: 2016年7月31日
 *      Author: scottie
 */
//#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/time.h>
#include <sys/select.h>
#include <time.h>
//#include <curses.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/ioctl.h>
#include <signal.h>

#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <net/route.h>

//#include "SysLogPublic.h"
#include "config.h"
#include "freeLockQ.h"
#include "snmp.h"
#include "data_trap.h"


static uint8 *trap_clk_source[]=
{
	"AutoClock",
	"GPSClock",
	"RS422Clock",
	"PTPClock"
};

static uint8 *trap_Lock_state[]=
{
	"UnLock",
	"Lock"
};

static uint8 *trap_FPGA_Work_state[]=
{
	"FPGAslaveState",
	"FPGAmasterState",
	"Error"
};
	
static uint8 *trap_Net_state[]=
{
	"Disconnected",
	"Connected"
};
	
static uint8 *trap_arm_run_state[]=
{
	"Run",
	"Stop"
};

static uint8 *param_work_mode[]=
{
	"Master",
	"Slave",
	"Error"
};

extern CLOCK_PARA* cdd_get_clk_param(void);


#if 1
static int snmp_data_trap(QUEUE_MSG_T *queue, SN_DataObject *obj,
		unsigned long oper_code);
#endif

static int set_gateway(const unsigned char *ip);
static int get_gateway(char *gateway);

unsigned int s2ui(const char *s);
unsigned long long s2ull(const char *s);
static unsigned char s2uc(const char *s);
static ui2s(unsigned int value, unsigned char *p_str);
static int clean_MsgQueue(QUEUE_MSG_T *p_msg, int c);
static void trap(int cmd, int info, void *data);
static int get_net_parameter( char *para_get, int flag);
static int is_valid_ip(const unsigned char* ipaddr);
static int is_valid_netmask(const unsigned char* netmask);
static int set_netmask(const uint8* netmask);
static int set_ipaddress(const uint8* ip);

#if 1
static int snmp_data_ack(QUEUE_MSG_T *queue, SN_DataObject *obj,
		unsigned long oper_code);
static int snmp_data_set_ack(QUEUE_MSG_T *queue, SN_DataObject *obj,
		unsigned long oper_code, int error);

#endif
//static int snmp_data_0x00(QUEUE_MSG_T *msg);
static int snmp_data_0x01(QUEUE_MSG_T *msg);
static int snmp_data_0x02(QUEUE_MSG_T *msg);
static int snmp_data_0x03(QUEUE_MSG_T *msg);
static int snmp_data_0x04(QUEUE_MSG_T *msg);
static int snmp_data_0x05(QUEUE_MSG_T *msg);

static int snmp_data_0x32(QUEUE_MSG_T *msg);
static int snmp_data_0x36(QUEUE_MSG_T *msg);


static int snmp_data_0x60(QUEUE_MSG_T *msg);
static int snmp_data_0x61(QUEUE_MSG_T *msg);
static int snmp_data_0x62(QUEUE_MSG_T *msg);
static int snmp_data_0x63(QUEUE_MSG_T *msg);
static int snmp_data_0x64(QUEUE_MSG_T *msg);
static int snmp_data_0x65(QUEUE_MSG_T *msg);

//static int snmp_data_0x06(QUEUE_MSG_T *msg);
//static int snmp_data_0x07(QUEUE_MSG_T *msg);
//static int snmp_data_0x08(QUEUE_MSG_T *msg);
//static int snmp_data_0x09(QUEUE_MSG_T *msg);
//static int snmp_data_0x0a(QUEUE_MSG_T *msg);
//static int snmp_data_0x0b(QUEUE_MSG_T *msg);
//static int snmp_data_0x0c(QUEUE_MSG_T *msg);
//static int snmp_data_0x0d(QUEUE_MSG_T *msg);
//static int snmp_data_0x0e(QUEUE_MSG_T *msg);
//static int snmp_data_0x0f(QUEUE_MSG_T *msg);
//static int snmp_data_0x10(QUEUE_MSG_T *msg);

//static int snmp_data_0x11(QUEUE_MSG_T *msg);
//static int snmp_data_0x12(QUEUE_MSG_T *msg);
//static int snmp_data_0x13(QUEUE_MSG_T *msg);
//static int snmp_data_0x14(QUEUE_MSG_T *msg);
//static int snmp_data_0x15(QUEUE_MSG_T *msg);
//static int snmp_data_0x16(QUEUE_MSG_T *msg);
//static int snmp_data_0x17(QUEUE_MSG_T *msg);
//static int snmp_data_0x18(QUEUE_MSG_T *msg);
//static int snmp_data_0x19(QUEUE_MSG_T *msg);
//static int snmp_data_0x1a(QUEUE_MSG_T *msg);
//static int snmp_data_0x1b(QUEUE_MSG_T *msg);
//static int snmp_data_0x1c(QUEUE_MSG_T *msg);
//static int snmp_data_0x1d(QUEUE_MSG_T *msg);
//static int snmp_data_0x1e(QUEUE_MSG_T *msg);
//static int snmp_data_0x1f(QUEUE_MSG_T *msg);
//static int snmp_data_0x20(QUEUE_MSG_T *msg);

//static int snmp_data_0x21(QUEUE_MSG_T *msg);
//static int snmp_data_0x22(QUEUE_MSG_T *msg);
//static int snmp_data_0x23(QUEUE_MSG_T *msg);
//static int snmp_data_0x24(QUEUE_MSG_T *msg);
//static int snmp_data_0x25(QUEUE_MSG_T *msg);
//static int snmp_data_0x26(QUEUE_MSG_T *msg);
//static int snmp_data_0x27(QUEUE_MSG_T *msg);
//static int snmp_data_0x28(QUEUE_MSG_T *msg);
//static int snmp_data_0x29(QUEUE_MSG_T *msg);
//static int snmp_data_0x2a(QUEUE_MSG_T *msg);
//static int snmp_data_0x2b(QUEUE_MSG_T *msg);
//static int snmp_data_0x2c(QUEUE_MSG_T *msg);
//static int snmp_data_0x2d(QUEUE_MSG_T *msg);
//static int snmp_data_0x2e(QUEUE_MSG_T *msg);
//static int snmp_data_0x2f(QUEUE_MSG_T *msg);
//static int snmp_data_0x30(QUEUE_MSG_T *msg);

//static int snmp_data_0x31(QUEUE_MSG_T *msg);
static int snmp_data_0x32(QUEUE_MSG_T *msg);
//static int snmp_data_0x33(QUEUE_MSG_T *msg);
//static int snmp_data_0x34(QUEUE_MSG_T *msg);
//static int snmp_data_0x35(QUEUE_MSG_T *msg);
static int snmp_data_0x36(QUEUE_MSG_T *msg);
//static int snmp_data_0x37(QUEUE_MSG_T *msg);
//static int snmp_data_0x38(QUEUE_MSG_T *msg);
//static int snmp_data_0x39(QUEUE_MSG_T *msg);
//static int snmp_data_0x3a(QUEUE_MSG_T *msg);
//static int snmp_data_0x3b(QUEUE_MSG_T *msg);
//static int snmp_data_0x3c(QUEUE_MSG_T *msg);
//static int snmp_data_0x3d(QUEUE_MSG_T *msg);
//static int snmp_data_0x3e(QUEUE_MSG_T *msg);
//static int snmp_data_0x3f(QUEUE_MSG_T *msg);
//static int snmp_data_0x40(QUEUE_MSG_T *msg);

//static int snmp_data_0x41(QUEUE_MSG_T *msg);
//static int snmp_data_0x42(QUEUE_MSG_T *msg);

//static int snmp_data_0x43(QUEUE_MSG_T *msg);
//static int snmp_data_0x44(QUEUE_MSG_T *msg);
//static int snmp_data_0x45(QUEUE_MSG_T *msg);
//static int snmp_data_0x46(QUEUE_MSG_T *msg);
//static int snmp_data_0x47(QUEUE_MSG_T *msg);
//static int snmp_data_0x48(QUEUE_MSG_T *msg);
//static int snmp_data_0x49(QUEUE_MSG_T *msg);
//static int snmp_data_0x4a(QUEUE_MSG_T *msg);
//static int snmp_data_0x4b(QUEUE_MSG_T *msg);
//static int snmp_data_0x4c(QUEUE_MSG_T *msg);
//static int snmp_data_0x4d(QUEUE_MSG_T *msg);
//static int snmp_data_0x4e(QUEUE_MSG_T *msg);
//static int snmp_data_0x4f(QUEUE_MSG_T *msg);
//static int snmp_data_0x50(QUEUE_MSG_T *msg);
//static int snmp_data_0x51(QUEUE_MSG_T *msg);
//static int snmp_data_0x52(QUEUE_MSG_T *msg);

//static int snmp_data_0x53(QUEUE_MSG_T *msg);
//static int snmp_data_0x54(QUEUE_MSG_T *msg);

/*******************************************************/
static SN_DataObject objSnmp;
//static Cdd_roParam_T cdd_param;
//static Cdd_trapParam_T cdd_trapparam;


static int flag_trap = 0;
static Sem_Var sem_var;
static sem_t trapsignal;
/********************************************************/
#if 1

//static snmp_data_deal_fun[]={0x1,0x2,0x3,0x4,0x4,0x32,0x36,0x60,0x61,0x62,0x63,0x64,0x65,0x66};
static SNMP_DATA SnmpData[0x70] = { NULL};// snmp_data_0x01, snmp_data_0x02,
//		snmp_data_0x03, snmp_data_0x04, snmp_data_0x05,// snmp_data_0x06,
//		snmp_data_0x07, snmp_data_0x08, snmp_data_0x09, snmp_data_0x0a,
//		snmp_data_0x0b, snmp_data_0x0c, snmp_data_0x0d, snmp_data_0x0e,
//		snmp_data_0x0f, snmp_data_0x10, snmp_data_0x11, snmp_data_0x12,
//		snmp_data_0x13, snmp_data_0x14, snmp_data_0x15, snmp_data_0x16,
//		snmp_data_0x17, snmp_data_0x18, snmp_data_0x19, snmp_data_0x1a,
//		snmp_data_0x1b, snmp_data_0x1c, snmp_data_0x1d, snmp_data_0x1e,
//		snmp_data_0x1f, snmp_data_0x20, snmp_data_0x21, snmp_data_0x22,
//		snmp_data_0x23, snmp_data_0x24, snmp_data_0x25, snmp_data_0x26,
//		snmp_data_0x27, snmp_data_0x28, snmp_data_0x29, snmp_data_0x2a,
//		snmp_data_0x2b, snmp_data_0x2c, snmp_data_0x2d, snmp_data_0x2e,
//		snmp_data_0x2f, snmp_data_0x30, snmp_data_0x31, snmp_data_0x32,
//		snmp_data_0x33, snmp_data_0x34, snmp_data_0x35, snmp_data_0x36,
//		snmp_data_0x37, snmp_data_0x38, snmp_data_0x39, snmp_data_0x3a,
//		snmp_data_0x3b, snmp_data_0x3c, snmp_data_0x3d, snmp_data_0x3e,
//		snmp_data_0x3f, snmp_data_0x40, snmp_data_0x41, snmp_data_0x42,
//		snmp_data_0x43, snmp_data_0x44, snmp_data_0x45, snmp_data_0x46,
//		snmp_data_0x47, snmp_data_0x48, snmp_data_0x49, snmp_data_0x4a,
//		snmp_data_0x4b, snmp_data_0x4c, snmp_data_0x4d, snmp_data_0x4e,
//		snmp_data_0x4f, snmp_data_0x50, snmp_data_0x51, snmp_data_0x52,
//		snmp_data_0x53, snmp_data_0x54 
//};
#endif

/**************************检查************************************/
int Check_trap_flag()
{
	return flag_trap;
}


 /*************************************************************
  * 				更新参数信息
  *************************************************************/

void refresh_cdd_param()
{
	CLOCK_PARA * paramptr = cdd_get_clk_param();


	if(paramptr == NULL){
		return;
		}


	strcpy(paramptr->recvport,"192.168.1.100:888");

	
}

 
 /*************************************************************
  * 						 IP地址修改/查询
  *************************************************************/
 static int snmp_data_0x01(QUEUE_MSG_T *msg) {
	 long type = msg->msg_type;
	 char buffer[256] = { 0 };
	 QUEUE_MSG_T msg_ack;
 //  DataObject *obj = &objSnmp;
 
	 SN_DataObject *obj = &objSnmp;
	 struct in_addr addr;
	 int ret;
	 memset(&msg_ack, 0, sizeof(QUEUE_MSG_T));
	 SysInfoTrace(
			 "this is %s,QUEUE_MSG_T msg_type : %x,snmp_msg.oper_code : %x\n",
			 __FUNCTION__, msg->msg_type, msg->snmp_msg.oper_code);
 
	 if (type == CMD_GET) {  //查询
 
		 ret = get_net_parameter(buffer, SIOCGIFADDR);
		 if (ret != 0) {
			 SysErrorTrace("get local ip error!\n");
		 }
 
		 SysInfoTrace("QUEUE_MSG_T get local ip is: %s\n", buffer);
		 /* 需要一个回复失败
		  */
		 strcpy(msg_ack.snmp_msg.u_data.msg_data_str, buffer);
 
		 ret = snmp_data_ack(&msg_ack, obj, msg->snmp_msg.oper_code);
 
	 } else if (type == CMD_SET) {	 //修改
		 //addr.s_addr = msg->snmp_msg.u_data.msg_data_int;
 
		 SysInfoTrace("QUEUE_MSG_T set local ip is: %s\n",
				 msg->snmp_msg.u_data.msg_data_str);
 
		 //ret = set_ipaddress((uint8 *)inet_ntoa(addr));
		 ret = set_ipaddress(msg->snmp_msg.u_data.msg_data_str);
 
		 if (ret)
			 snmp_data_set_ack(&msg_ack, obj, msg->snmp_msg.oper_code, -1);
		 else if (!ret)
			 snmp_data_set_ack(&msg_ack, obj, msg->snmp_msg.oper_code, 0);
 
		 if (ret != 0) {
			 SysErrorTrace("ip address format is wrong!\n");
			 return -1;
		 }
		 /*
		  * 需要回复设置成功/失败
		  */
	 } else {
		 SysErrorTrace("msg type's format is wrong [%#x]\n", type);
		 return -1;
	 }
 
	 return 0;
 }
 
 /*************************************************************
  * 					 子网掩码地址修改/查询
  *************************************************************/
 static int snmp_data_0x02(QUEUE_MSG_T *msg) {
	 struct in_addr addr;
	 long type = msg->msg_type;
	 char buffer[256] = { 0 };
	 QUEUE_MSG_T msg_ack;
 //  DataObject *obj = &objSnmp;
	 SN_DataObject *obj = &objSnmp;
	 int ret;
	 memset(&msg_ack, 0, sizeof(QUEUE_MSG_T));
	 SysInfoTrace(
			 "this is %s,QUEUE_MSG_T msg_type : %x,snmp_msg.oper_code : %x\n",
			 __FUNCTION__, msg->msg_type, msg->snmp_msg.oper_code);
 
	 if (type == CMD_GET) {  //查询
		 ret = get_net_parameter(buffer, SIOCGIFNETMASK);
		 if (ret != 0) {
			 SysErrorTrace("get local ip error!\n");
		 }
		 /*
		  * 需要一个回复失败
		  */
		 SysInfoTrace("QUEUE_MSG_T get mask is: %s\n", buffer);
 
		 //msg_ack.snmp_msg.u_data.msg_data_int = inet_addr(buffer);
		 strcpy(msg_ack.snmp_msg.u_data.msg_data_str, buffer);
		 ret = snmp_data_ack(&msg_ack, obj, msg->snmp_msg.oper_code);
 
	 } else if (type == CMD_SET) {	 //修改
		 SysInfoTrace("QUEUE_MSG_T set mask is: %s\n",
				 msg->snmp_msg.u_data.msg_data_str);
		 //ret = set_netmask((uint8 *)inet_ntoa(addr));
		 ret = set_netmask(msg->snmp_msg.u_data.msg_data_str);
		 if (ret)
			 snmp_data_set_ack(&msg_ack, obj, msg->snmp_msg.oper_code, -1);
		 else if (!ret)
			 snmp_data_set_ack(&msg_ack, obj, msg->snmp_msg.oper_code, 0);
		 if (ret != 0) {
			 SysErrorTrace("ip address format is wrong!\n");
			 return -1;
		 }
	 } else {
		 SysErrorTrace("msg type's format is wrong [%#x]\n", type);
		 return -1;
	 }
 
	 return 0;
 }
 
 /*************************************************************
  * 						 网关地址修改/查询
  *************************************************************/
 static int snmp_data_0x03(QUEUE_MSG_T *msg) {
	 long type = msg->msg_type;
	 char buffer[256] = { 0 };
	 QUEUE_MSG_T msg_ack;
 //  DataObject *obj = &objSnmp;
	 SN_DataObject *obj = &objSnmp;
	 struct in_addr addr;
	 int ret;
	 memset(&msg_ack, 0, sizeof(QUEUE_MSG_T));
	 SysInfoTrace(
			 "this is %s,QUEUE_MSG_T msg_type : %x,snmp_msg.oper_code : %x\n",
			 __FUNCTION__, msg->msg_type, msg->snmp_msg.oper_code);
 
	 if (type == CMD_GET) {
		 ret = get_gateway(buffer);
		 if (ret != 0) {
			 SysErrorTrace("get gateway error!\n");
		 }
		 /*
		  * 需要一个回复失败
		  */
		 SysInfoTrace("QUEUE_MSG_T get gw is: %s\n", buffer);
		 //msg_ack.snmp_msg.u_data.msg_data_int = inet_addr(buffer);
		 strcpy(msg_ack.snmp_msg.u_data.msg_data_str, buffer);
		 ret = snmp_data_ack(&msg_ack, obj, msg->snmp_msg.oper_code);
 
	 } else if (type == CMD_SET) {
		 //addr.s_addr = msg->snmp_msg.u_data.msg_data_int;
 
		 SysInfoTrace("QUEUE_MSG_T set gw is: %s\n",
				 msg->snmp_msg.u_data.msg_data_str);
		 ret = set_gateway(msg->snmp_msg.u_data.msg_data_str);
		 if (ret)
			 snmp_data_set_ack(&msg_ack, obj, msg->snmp_msg.oper_code, -1);
		 else if (!ret)
			 snmp_data_set_ack(&msg_ack, obj, msg->snmp_msg.oper_code, 0);
		 //ret = set_gateway((uint8 *)inet_ntoa(addr));
		 if (ret != 0) {
			 SysErrorTrace("ip address format is wrong!\n");
			 return -1;
		 }
		 /*
		  * 需要回复设置成功/失败
		  */
	 } else {
		 SysErrorTrace("msg type's format is wrong [%#x]\n", type);
		 return -1;
	 }
 
	 return 0;
 }
 /*************************************************************
  * 							 MAC地址查询
  *************************************************************/
 static int snmp_data_0x04(QUEUE_MSG_T *msg) {
	 long type = msg->msg_type;
	 char buffer[256] = { 0 };
	 QUEUE_MSG_T msg_ack;
 //  DataObject *obj = &objSnmp;
	 SN_DataObject *obj = &objSnmp;
	 int ret;
	 memset(&msg_ack, 0, sizeof(QUEUE_MSG_T));
	 SysInfoTrace(
			 "this is %s,QUEUE_MSG_T msg_type : %x,snmp_msg.oper_code : %x\n",
			 __FUNCTION__, msg->msg_type, msg->snmp_msg.oper_code);
 
	 if (type != CMD_GET) {
		 SysErrorTrace("get mac msg type error[%#x]\n", type);
		 return -1;
	 }
 
	 ret = get_net_parameter(buffer, SIOCGIFHWADDR);
	 if (ret != 0) {
		 SysErrorTrace("get mac error!\n");
 
	 }
	 SysInfoTrace("QUEUE_MSG_T get mac is: %s\n", buffer);
	 /*
	  * 需要一个回复失败
	  */
 //msg_ack.snmp_msg.u_data.msg_data_int = inet_addr(buffer);
	 strcpy(msg_ack.snmp_msg.u_data.msg_data_str, buffer);
 
	 return 0;
 }
 
 /*************************************************************
  * 					 数据库ip地址
  *************************************************************/
 static int snmp_data_0x05(QUEUE_MSG_T *msg) {
 
	 return 0;
 }
 
 /*************************************************************
  * 				 设备类型
  *************************************************************/
 static int snmp_data_0x32(QUEUE_MSG_T *msg) {
 
	 long type = msg->msg_type;
	 unsigned char buffer[256] = { 0 };
	 char xpath[256] = { 0 };
	 QUEUE_MSG_T msg_ack;
 //  DataObject *obj = &objSnmp;
	 SN_DataObject *obj = &objSnmp;
	 int ret;
	 memset(&msg_ack, 0, sizeof(QUEUE_MSG_T));
#ifdef	__SNMP_PRINT
	 SysInfoTrace(
			 "this is %s,QUEUE_MSG_T msg_type : %x,snmp_msg.oper_code : %x\n",
			 __FUNCTION__, msg->msg_type, msg->snmp_msg.oper_code);
#endif
	 if (type == CMD_GET) {  //查询
		 strcpy(msg_ack.snmp_msg.u_data.msg_data_str,  "CLKD");
		 ret = snmp_data_ack(&msg_ack, obj, msg->snmp_msg.oper_code);
	 } else {
		 SysErrorTrace("msg type's format is wrong [%#x]\n", type);
		 return -1;
	 }
	 return 0;
 
 }
 
 /*************************************************************
  * 				 消息源
  *************************************************************/
 static int snmp_data_0x36(QUEUE_MSG_T *msg) {
	 long type = msg->msg_type;
	 QUEUE_MSG_T msg_ack;
	 SN_DataObject *obj = &objSnmp;
	 int ret = 0;
	 char buffer[255] = { 0 }, temp[16] = { 0 };
	 memset(&msg_ack, 0, sizeof(QUEUE_MSG_T));
	 if (type == CMD_GET) {
		 get_net_parameter(buffer, SIOCGIFADDR);
		 strcat(buffer, ":");
		 strcat(buffer,SNMPD_SOURCES_PROT);
		 
		 strcpy(msg_ack.snmp_msg.u_data.msg_data_str, buffer);
		 snmp_data_ack(&msg_ack, obj, msg->snmp_msg.oper_code);
	 }
 
	 return 0;
 }
 /*************************************************************
  * 				工作模式
  *************************************************************/
 static int snmp_data_0x60(QUEUE_MSG_T *msg) {
	 long type = msg->msg_type;
	 QUEUE_MSG_T msg_ack;
	 SN_DataObject *obj = &objSnmp;
	 int ret = 0,tmpnum=0;
	 char buffer[255] = { 0 }, temp[16] = { 0 };

	 
	 CLOCK_PARA * paramptr = cdd_get_clk_param();
	 memset(&msg_ack, 0, sizeof(QUEUE_MSG_T));
	 if (type == CMD_GET) {
	 	if((paramptr->device_mode == 1) ||
			(paramptr->device_mode == 2))
	 	{
	 		tmpnum = paramptr->device_mode -1;
	 	}
		else{
			tmpnum = 2;
			}
		 strcpy(msg_ack.snmp_msg.u_data.msg_data_str, param_work_mode[tmpnum]);
		 snmp_data_ack(&msg_ack, obj, msg->snmp_msg.oper_code);
	 }
 
	 return 0;
 }
 /*************************************************************
  * 				应用版本
  *************************************************************/
 static int snmp_data_0x61(QUEUE_MSG_T *msg) {
	 long type = msg->msg_type;
	 QUEUE_MSG_T msg_ack;
	 SN_DataObject *obj = &objSnmp;
	 int ret = 0;
	 char buffer[255] = { 0 }, temp[16] = { 0 };
	 CLOCK_PARA * paramptr = cdd_get_clk_param();
	 
	 memset(&msg_ack, 0, sizeof(QUEUE_MSG_T));
	 if (type == CMD_GET) {
		 get_arm_version(msg_ack.snmp_msg.u_data.msg_data_str);
		 snmp_data_ack(&msg_ack, obj, msg->snmp_msg.oper_code);
	 }
 
	 return 0;
 }
 /*************************************************************
  * 				FPGA版本
  *************************************************************/
 static int snmp_data_0x62(QUEUE_MSG_T *msg) {
	 long type = msg->msg_type;
	 QUEUE_MSG_T msg_ack;
	 SN_DataObject *obj = &objSnmp;
	 int ret = 0;
	 char buffer[255] = { 0 }, temp[16] = { 0 };
	 CLOCK_PARA * paramptr = cdd_get_clk_param();
	 
	 memset(&msg_ack, 0, sizeof(QUEUE_MSG_T));
	 if (type == CMD_GET) {
		 strcpy(msg_ack.snmp_msg.u_data.msg_data_str, paramptr->fpga_version);
		 snmp_data_ack(&msg_ack, obj, msg->snmp_msg.oper_code);
	 }
 
	 return 0;
 }
 /*************************************************************
  * 				硬件版本
  *************************************************************/
 static int snmp_data_0x63(QUEUE_MSG_T *msg) {
	 long type = msg->msg_type;
	 QUEUE_MSG_T msg_ack;
	 SN_DataObject *obj = &objSnmp;
	 int ret = 0;
	 char buffer[255] = { 0 }, temp[16] = { 0 };
	 CLOCK_PARA * paramptr = cdd_get_clk_param();
	 
	 memset(&msg_ack, 0, sizeof(QUEUE_MSG_T));
	 if (type == CMD_GET) {
		 get_hardware_version(msg_ack.snmp_msg.u_data.msg_data_str);			 
		 snmp_data_ack(&msg_ack, obj, msg->snmp_msg.oper_code);
	 }
 
	 return 0;
 }
 /*************************************************************
  * 				时钟分频器型号
  *************************************************************/
 static int snmp_data_0x64(QUEUE_MSG_T *msg) {
	 long type = msg->msg_type;
	 QUEUE_MSG_T msg_ack;
	 SN_DataObject *obj = &objSnmp;
	 int ret = 0;
	 char buffer[255] = { 0 }, temp[16] = { 0 };
	 CLOCK_PARA * paramptr = cdd_get_clk_param();
	 
	 memset(&msg_ack, 0, sizeof(QUEUE_MSG_T));
	 if (type == CMD_GET) {
		 strcpy(msg_ack.snmp_msg.u_data.msg_data_str, DEVICE_NAME);
		 
//	 strcpy(msg_ack.snmp_msg.u_data.msg_data_str, paramptr->device_name);
		 snmp_data_ack(&msg_ack, obj, msg->snmp_msg.oper_code);
	 }
 
	 return 0;
 }
 /*************************************************************
  * 				GPS时间
  *************************************************************/
 static int snmp_data_0x65(QUEUE_MSG_T *msg) {
	 long type = msg->msg_type;
	 QUEUE_MSG_T msg_ack;
	 SN_DataObject *obj = &objSnmp;
	 int ret = 0;
	 char buffer[255] = { 0 }, temp[16] = { 0 };
	 CLOCK_PARA * paramptr = cdd_get_clk_param();
	 
	 memset(&msg_ack, 0, sizeof(QUEUE_MSG_T));
	 if (type == CMD_GET) {
		 strcpy(msg_ack.snmp_msg.u_data.msg_data_str, paramptr->gps_time);
		 snmp_data_ack(&msg_ack, obj, msg->snmp_msg.oper_code);
	 }
 
	 return 0;
 }
 /*************************************************************
  * 				PTP时间
  *************************************************************/
 static int snmp_data_0x66(QUEUE_MSG_T *msg) {
	 long type = msg->msg_type;
	 QUEUE_MSG_T msg_ack;
	 SN_DataObject *obj = &objSnmp;
	 int ret = 0;
	 char buffer[255] = { 0 }, temp[16] = { 0 };
	 CLOCK_PARA * paramptr = cdd_get_clk_param();
	 
	 memset(&msg_ack, 0, sizeof(QUEUE_MSG_T));
	 if (type == CMD_GET) {
		 strcpy(msg_ack.snmp_msg.u_data.msg_data_str, paramptr->ptp_time);
		 snmp_data_ack(&msg_ack, obj, msg->snmp_msg.oper_code);
	 }
 
	 return 0;
 }

 /*************************************************************
  * 				时钟分配器版本
  *************************************************************/
 static int snmp_data_deal(QUEUE_MSG_T *msg) {
 
	 long type = msg->msg_type;
	 uint32 cmd = msg->snmp_msg.oper_code;
	 unsigned char buffer[256] = { 0 };
	 char xpath[256] = { 0 };
	 QUEUE_MSG_T msg_ack;
	 SN_DataObject *obj = &objSnmp;
	 int ret;
	 CLOCK_PARA * paramptr = cdd_get_clk_param();

	 memset(&msg_ack, 0, sizeof(QUEUE_MSG_T));
#ifdef	__SNMP_PRINT
	 SysInfoTrace(
			 "this is %s,QUEUE_MSG_T msg_type : %X,snmp_msg.oper_code : %X\n",
			 __FUNCTION__, msg->msg_type, msg->snmp_msg.oper_code);
#endif

	 if (type == CMD_GET) {  //查询
		 	//refresh_cdd_param();
			switch(cmd){

				
			
				case MSG_CODE_CDD_WORK_MODE:
					strcpy(msg_ack.snmp_msg.u_data.msg_data_str, param_work_mode[paramptr->device_mode-1]);
				break;

				case MSG_CODE_CDD_APP_VER:
					get_arm_version(msg_ack.snmp_msg.u_data.msg_data_str);

				case MSG_CODE_CDD_FPGA_VER:
					strcpy(msg_ack.snmp_msg.u_data.msg_data_str, paramptr->fpga_version);
				break;

				case MSG_CODE_CDD_HW_VER:
					get_hardware_version(msg_ack.snmp_msg.u_data.msg_data_str);				
				break;
				
				case MSG_CODE_CDD_MODEL_STATE:
					strcpy(msg_ack.snmp_msg.u_data.msg_data_str, paramptr->device_name);
				break;
				
				case MSG_CODE_CDD_GPS_TIME:
					strcpy(msg_ack.snmp_msg.u_data.msg_data_str, paramptr->gps_time);
				break;
				
				case MSG_CODE_CDD_PTP_TIME:
					strcpy(msg_ack.snmp_msg.u_data.msg_data_str, paramptr->ptp_time);
				break;

				case MSG_CODE_CDD_SORCE :
						get_net_parameter(buffer, SIOCGIFADDR);
						strcat(buffer, ":");
						strcat(buffer, "1234");
						strcpy(msg_ack.snmp_msg.u_data.msg_data_str, buffer);
					break;
				case MSG_CODE_CDD_EQU_TYPE :
						strcpy(msg_ack.snmp_msg.u_data.msg_data_str, "CLKD");
					break;

					
				
				default:
					
				break;
			}
		 ret = snmp_data_ack(&msg_ack, obj, msg->snmp_msg.oper_code);
	 } 
	 else {
		 SysErrorTrace("msg type's format is wrong [%#x]\n", type);
		 return -1;
	 }
	 return 0;
 
 }




void set_timer() {
	struct itimerval itv,oldtv;
	itv.it_interval.tv_sec = 3;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 3;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &itv, &oldtv);
}
void signal_handler() {

	printf("%s:%d\r\n",__FUNCTION__,__LINE__);
	sem_post(&trapsignal);

}


/*****************************************************************************************
 * 							与网关进程之间的数据接收（IPC 消息队列）
 *****************************************************************************************/
void * pthread_snmp_recv(void *arg) {
	int len;
    QUEUE_MSG_T msg = {0};
	QUEUE_MSG_T msg1;
	uint32 oper_code;
	int msgid = -1;
	int Msg_recv;
	//DataObject *obj = &objSnmp;
	int flag = 0;

	SysInfoTrace("-->pthread_snmp_recv start\n");

	pthread_detach(pthread_self());
//	signal(SIGALRM, signal_handler);


	Msg_recv = msgget((key_t) KEY_ID_RECV, IPC_EXCL); //5555
	if (Msg_recv < 0) {
		SysErrorTrace("Msg_recv = %d\n", Msg_recv);
		return NULL;
	}
	
	SysInfoTrace("msg_recv_queue[%d]\n\r",Msg_recv);

	while (1) {

//		Msg_recv = msgget((key_t) KEY_ID_RECV, IPC_EXCL); //5555
//		if (Msg_recv < 0) {
//			SysErrorTrace("Msg_recv ddd= %d\n", Msg_recv);
//			sleep(1);
//			continue;
//		}

		//SysErrorTrace("while___ ret = %d,errno = %d,len[%d]Msg_recv[%d]\n",
		//		len, errno,len,Msg_recv);
		memset(&msg, 0, sizeof(msg));
		len = msgrcv(Msg_recv, &msg, sizeof(QUEUE_MSG_T), (long int)0, 0);
		if (len < 0) {
			SysErrorTrace("receive msg from snmp error! ret = %d,errno = %d,len[%d]Msg_recv[%d]\r\n",
					len, errno,len,Msg_recv);
			SysErrorTrace("error[%s]\r\n",strerror(errno));
			//delay(8000000);
			usleep(600000);
			continue;
		} else {
#ifdef	__SNMP_PRINT
			SysDebugTrace(
					"receive msg from snmp success operation code is [%#x],msg_type is %0x,msg_data_str is %s!,Msg_recv = %d",
					msg.snmp_msg.oper_code, msg.msg_type,
					msg.snmp_msg.u_data.msg_data_str, Msg_recv);
#endif
			/*只有在接收到代理的一个查询之后，发送trap才有意义，否则代理会丢掉trap包,flag_trap标志用来保证收到代理的查询之后才能发送trap*/
				if ((msg.msg_type == CMD_GET || msg.msg_type == CMD_SET)
						&& flag_trap == 0)
					flag_trap = TRUE;
			/*在保证收到过代理的查询命令后，发送trap才有意义，并且flag标志用来保证只发一次*/
					//snmp_data_deal(&msg);
		oper_code = msg.snmp_msg.oper_code;
		SnmpData[oper_code](&msg);
		}


		

	}
	return NULL;
}

/*****************************************************************************************
 * 							与网关进程之间的数据发送（IPC 消息队列）
 *****************************************************************************************/
void * pthread_snmp_send(void *arg) {
	int len, ret,ret1 = 0;

	/*******!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1************/
#if 0
	DataObject *obj = &objSnmp;
	Msg_Param node;
#endif

#if 1
	SN_DataObject *obj = &objSnmp;
	SN_Msg_Param node;
#endif

	QUEUE_MSG_T msg = { 0 };
	QUEUE_MSG_T msg1 = { 0 };
	int MsgID_Send = 0;
	int value;
	SysInfoTrace("-->pthread_snmp_send start\n");

	pthread_detach(pthread_self());

	MsgID_Send = msgget((key_t) KEY_ID_SEND, IPC_EXCL);
	if (MsgID_Send < 0) {
		return NULL;
	}
	
	SysInfoTrace("msg_Send_queue[%d]\n\r",MsgID_Send);
	/*******!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1************/

#if 1
	while (1) {
		//sem_wait(&(obj->send.sem));
		//	SysInfoTrace("enter  pthread_send_trap\n");
		
		MsgID_Send = msgget((key_t) KEY_ID_SEND, IPC_EXCL);
		if (MsgID_Send < 0) {
			return NULL;
		}


		sem_wait(&(sem_var.full));
		sem_wait(&(sem_var.mutex));
		
		ret = SN_Free_Lock_Queue_Get(&node, (obj->send).queue);
		if (ret != 0) {
			
				SysInfoTrace("SN_Free_Lock_Queue_Get  3333\n\r");

			ret1 = SN_Free_Lock_Queue_Pop((obj->send).queue);

			if(ret1 != 0)
			{
			  SysErrorTrace(" SN_Free_Lock_Queue_Pop trap error\n");
			  sem_post(&(sem_var.mutex));
			  return NULL;
			}
			sem_post(&(sem_var.mutex));
			continue;
		} else {
			
//			SysInfoTrace("SN_Free_Lock_Queue_Get  4444\n\r");
					memcpy(&msg, node.data, node.length);
					ret1 = SN_Free_Lock_Queue_Pop((obj->send).queue);
					if(ret1 != 0)
					{
					  	SysErrorTrace(" SN_Free_Lock_Queue_Pop trap error\n");
						sem_post(&(sem_var.mutex));
					  	return NULL;
					}
			
			}
		
			sem_post(&(sem_var.mutex));
			
			len = msgsnd(MsgID_Send, &msg, sizeof(QUEUE_MSG_T), 0);
			if (len < 0) {
				SysErrorTrace("send msg to snmp error! errno = %d", errno);
				continue;
			}

		#ifdef	__SNMP_PRINT
					SysInfoTrace(
										"pthread_snmp_send trap :msg_type = %0x,msg operation code is [%#x] data is [%s]",
										msg.msg_type, msg.snmp_msg.oper_code,
										msg.snmp_msg.u_data.msg_data_str);
		#endif

		
		//}
		//sem_post(&(obj->send.sem));
	}
#endif
	return NULL;
}


void * pthread_snmp_send_ack(void *arg) {
	int len, ret,ret1 = 0;

#if 1
	SN_DataObject *obj = &objSnmp;
	SN_Msg_Param node;
#endif

	QUEUE_MSG_T msg = { 0 };
	QUEUE_MSG_T msg1 = { 0 };
	int MsgID_Send = 0;
	int value;
	SysInfoTrace("-->pthread_snmp_send_ack start\n");

	pthread_detach(pthread_self());

	MsgID_Send = msgget((key_t) KEY_ID_ACK, IPC_EXCL);
	if (MsgID_Send < 0) {
		return NULL;
	}
	
#if 1
	while (1) {
		sem_wait(&(obj->send_ack.sem));
		ret = SN_Free_Lock_Queue_Get(&node, (obj->send_ack).queue);
		if (ret != 0) {
			
			//sem_post(&(obj->send.sem));
			ret1 = SN_Free_Lock_Queue_Pop((obj->send_ack).queue);
			if(ret1 != 0)
			{
			  SysErrorTrace(" SN_Free_Lock_Queue_Pop error\n");
			  return NULL;
			}

			continue;
		} else {
			
			memcpy(&msg, node.data, node.length);
			
			len = msgsnd(MsgID_Send, &msg, sizeof(QUEUE_MSG_T) - sizeof(long), /*0*/
			0);
			if (len < 0) {
				SysErrorTrace("send msg to snmp error! errno = %d", errno);

				ret1 = SN_Free_Lock_Queue_Pop((obj->send_ack).queue);
				if(ret1 != 0)
				{
			 		 SysErrorTrace(" SN_Free_Lock_Queue_Pop error\n");
			  		 return NULL;
				}

				//sem_post(&(obj->send.sem));
				
				continue;
			}

			#ifdef	__SNMP_PRINT
			SysInfoTrace(
								"pthread_snmp_send_ack :msg_type = %0x,msg operation code is [%#x] data is [%s]",
								msg.msg_type, msg.snmp_msg.oper_code,
								msg.snmp_msg.u_data.msg_data_str);
			#endif
			
			ret1 = SN_Free_Lock_Queue_Pop((obj->send_ack).queue);
			if(ret1 != 0)
			{
			 		 SysErrorTrace(" SN_Free_Lock_Queue_Pop error\n");
			  		 return NULL;
			}
		}
		//sem_post(&(obj->send.sem));
	}
#endif
	return NULL;
}


void Get_CDD_Ipaddr(char *buf)
{
	//Cdd_roParam_T * paramptr= &cdd_param;

//	strcpy(buf,paramptr->recvport);
}
/*
 * 验证IP地址是否合法
 */
static int is_valid_ip(const unsigned char* ipaddr) {
	int ret = 0;
	struct in_addr inp;
	ret = inet_aton((const char *) ipaddr, &inp);
	if (0 == ret) {
		return -1;
	} else {
printf("inet_aton:ip=%u\n", ntohl(inp.s_addr));

	}

	return 0;
}
static int set_net_parameter( const uint8* addr, int flag) {
	struct ifreq ifr;
	struct sockaddr_in sin;
	int sockfd;

	if (addr == NULL) {
		perror("ip point is null!\n");
		return -1;
	}

	if (flag)

		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		fprintf(stderr, "Could not get socket.\n");
		perror("eth0\n");
		return -1;
	}

	snprintf(ifr.ifr_name, (sizeof(ifr.ifr_name) - 1), "%s", NET_NAME);

	/* Read interface flags */
	if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
		fprintf(stderr, "ifdown: shutdown ");
		perror(ifr.ifr_name);
		close(sockfd);
		return -1;
	}

	memset(&sin, 0, sizeof(struct sockaddr));
	sin.sin_family = AF_INET;
	inet_aton((const char *) addr, (struct in_addr *) &sin.sin_addr.s_addr);
	memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));
	if (ioctl(sockfd, flag, &ifr) < 0) {
		fprintf(stderr, "Cannot set IP address. ");
		perror(ifr.ifr_name);
		close(sockfd);
		return -1;
	}

	if (flag == SIOCSIFADDR) {    //如果是IP地址设置激活标志
		ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
	}

	close(sockfd);
	return 0;
}

/*
 * 先验证是否为合法IP，
 * 然后将掩码转化成32无符号整型，取反为000...00111...1，
 * 然后再加1为00...01000...0，此时为2^n，如果满足就为合法掩码
 */
static int is_valid_netmask(const unsigned char* netmask) {
	if (is_valid_ip(netmask) == 0) {
		unsigned int b = 0, i, n[4];
		sscanf((const char *) netmask, "%u.%u.%u.%u", &n[3], &n[2], &n[1],
				&n[0]);
		for (i = 0; i < 4; ++i) //将子网掩码存入32位无符号整型
			b += n[i] << (i * 8);
		b = ~b + 1;
		if ((b & (b - 1)) == 0) //判断是否为2^n
			return 0;
	}

	return -1;
}

/*
 * 设置子网掩码
 */
static int set_netmask(const uint8* netmask) {
	int ret;
	ret = is_valid_netmask(netmask);	//验证掩码合法性
	if (ret != 0) {

		return -1;
	}
	ret = set_net_parameter(netmask, SIOCSIFNETMASK);
	return ret;
}

/*
 * 设置IP地址
 */
static int set_ipaddress(const uint8* ip) {
	int ret;
	ret = is_valid_ip(ip);	//验证IP地址合法性
	if (ret != 0) {
		return -1;
	}
	ret = set_net_parameter(ip, SIOCSIFADDR);
	return ret;
}

static int get_net_parameter( char *para_get, int flag) {
	int socket_get, ret;
	struct sockaddr_in *sin;
	struct ifreq ifr;

	if (para_get == NULL) {
		SysInfoTrace("para_get is null!\n");
		return -1;
	}

	if ((socket_get = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		SysInfoTrace("socket create fail!\n");
		return -1;
	}

	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, NET_NAME, sizeof(ifr.ifr_name) - 1);

	ret = ioctl(socket_get, flag, &ifr);
	if (ret < 0) {
		SysInfoTrace("ioctl get fail!\n");
		return -1;
	}

	if (flag == SIOCGIFHWADDR) {	//mac

		sprintf(para_get, "%02x:%02x:%02x:%02x:%02x:%02x",
				(unsigned char) ifr.ifr_hwaddr.sa_data[0],
				(unsigned char) ifr.ifr_hwaddr.sa_data[1],
				(unsigned char) ifr.ifr_hwaddr.sa_data[2],
				(unsigned char) ifr.ifr_hwaddr.sa_data[3],
				(unsigned char) ifr.ifr_hwaddr.sa_data[4],
				(unsigned char) ifr.ifr_hwaddr.sa_data[5]);

		SysInfoTrace("mac address is [%s]\n", para_get);
	} else {
		sin = (struct sockaddr_in *) &(ifr.ifr_addr);
		strcpy(para_get, inet_ntoa(sin->sin_addr));
#ifdef	__SNMP_RPINT
		SysInfoTrace("ip/mask address is [%s]\n", para_get);
#endif
	}
	close(socket_get);

	return 0;
}




void Trap_msg_to_snmpd(uint32 opt_code){
	QUEUE_MSG_T msg_trap;
//	Cdd_trapParam_T * trapptr= &cdd_trapparam;
	char buffer[255] = { 0 }, temp[16] = { 0 };	
	uint tmpnum=0;
	CLOCK_PARA * trapptr = cdd_get_clk_param();

	if(trapptr == NULL){
		return;
		}

	get_net_parameter(buffer, SIOCGIFADDR);
	strcat(buffer, ":");
	
	strcat(buffer, SNMPD_SOURCES_PROT);
	
	SysInfoTrace("Trap_msg_to_snmpdIPaddr[%s]\r\n",buffer);
	
	memset(&msg_trap, 0, sizeof(QUEUE_MSG_T));

	switch(opt_code){
		case SNMP_TRAP_QUEUE_CDD_CLK_SOURCE_TRAP:

			strcpy(msg_trap.snmp_msg.u_data.cdd_ClkSourceState_trap.cddGPSClkState,trap_clk_source[(trapptr->clock_source>>14)&0x3]);
			strcpy(msg_trap.snmp_msg.u_data.cdd_ClkSourceState_trap.cddvigorSource, buffer);
			
		break;
		
		case SNMP_TRAP_QUEUE_CDD_GPS_LOCK_TRAP:
			
			tmpnum = (trapptr->gps_lockstate &GPS_LOCK_STATE_BIT)?1:0;
			strcpy(msg_trap.snmp_msg.u_data.cdd_GPSLockState_trap.cddGPSLockState ,trap_Lock_state[tmpnum]);
			strcpy(msg_trap.snmp_msg.u_data.cdd_GPSLockState_trap.cddvigorSource, buffer);
			
		break;
		
		
		case SNMP_TRAP_QUEUE_CDD_PTP_LOCK_TRAP:
			
			tmpnum = (trapptr->ptp_lockstate & PTP_LOCK_STATE_BIT)?1:0;
			strcpy(msg_trap.snmp_msg.u_data.cdd_PTPLockState_trap.cddPTPLockState ,trap_Lock_state[tmpnum]);
			strcpy(msg_trap.snmp_msg.u_data.cdd_PTPLockState_trap.cddvigorSource, buffer);
			
		break;

		
		case SNMP_TRAP_QUEUE_CDD_FPGA_WORK_TRAP:
						
			if((trapptr->device_fpgastate==FPGA_WORK_STATE_SLAVE) ||
				(trapptr->device_fpgastate==FPGA_WORK_STATE_MASTER)){
			
				tmpnum = (trapptr->device_fpgastate==FPGA_WORK_STATE_MASTER)?1:0;			
			}
			else{
				SysInfoTrace("FPGA_WORK_STATE[%x]\r\n",trapptr->device_fpgastate);
				tmpnum = 2;
				}
			
			strcpy(msg_trap.snmp_msg.u_data.cdd_FPGAState_trap.cddFPGAWorkState ,trap_FPGA_Work_state[tmpnum]);
			strcpy(msg_trap.snmp_msg.u_data.cdd_FPGAState_trap.cddvigorSource, buffer);
			
		break;

		case SNMP_TRAP_QUEUE_CDD_NET_LINK_TRAP:		
			
		tmpnum = (trapptr->net_connectstate==NET_CONNECT_STATE_BIT)?1:0;
			strcpy(msg_trap.snmp_msg.u_data.cdd_NetLinkState_trap.cddNetLinkState ,trap_Net_state[tmpnum]);		
			strcpy(msg_trap.snmp_msg.u_data.cdd_NetLinkState_trap.cddvigorSource, buffer);
		
		break;
		
		case SNMP_TRAP_QUEUE_CDD_OMAP_RUN_TRAP:
			
			tmpnum = (trapptr->arm_runstate & OMAP_RUN_STATE_BIT)?1:0;
			strcpy(msg_trap.snmp_msg.u_data.cdd_OmapRunState_trap.cddOmapRunState ,trap_arm_run_state[tmpnum]);
			strcpy(msg_trap.snmp_msg.u_data.cdd_OmapRunState_trap.cddvigorSource, buffer);
			
		break;

		
		default:
		break;
	}
	
	
	
	snmp_data_trap(&msg_trap, &objSnmp, opt_code);
	
}
void * pthread_second_task(void *arg) {
	
	int count=0;
	QUEUE_MSG_T msg;
	SysInfoTrace("-->pthread_second_task start\n");

	msg.msg_type = CMD_GET;
	msg.snmp_msg.oper_code = 0x32;
	while (1) {
		count++;
		if(count%5 == 0)
			{
			//set_ipaddress("192.168.1.111");
			}
		else{
			//set_ipaddress("192.168.1.112");
			
			}
		sem_wait(&trapsignal);
		refresh_cdd_param();
		if(Check_trap_flag()==TRUE){
			
			SysInfoTrace("%s:%d\n",__FUNCTION__,__LINE__);
			Trap_msg_to_snmpd(0x70);
			Trap_msg_to_snmpd(0x71);
			Trap_msg_to_snmpd(0x72);
			Trap_msg_to_snmpd(0x73);
			Trap_msg_to_snmpd(0x74);
			Trap_msg_to_snmpd(0x75);
		}
//			Trap_msg_to_snmpd(0x70);

//			snmp_data_deal(&msg);

		//	sleep(1);
		
	}

	return NULL;
}


#if 1
static void my_func(int sign_no) {
	int Msg_recv = 0, Msg_send = 0,Msg_send_ack = 0;
	Msg_recv = msgget((key_t) KEY_ID_RECV, IPC_EXCL); //5555
	if (Msg_recv < 0) {
		SysErrorTrace("Msg_recv is not alived Msg_recv = %d\n", Msg_recv);
		exit(0);
	} else {
		if (msgctl(Msg_recv, IPC_RMID, 0) < 0) {
			SysErrorTrace("delete message error\n");
			exit(0);
		}
		SysInfoTrace("rm msg_recv sucess\n");
	}

	Msg_send = msgget((key_t) KEY_ID_SEND, IPC_EXCL);
	if (Msg_send < 0) {
		exit(0);
	} else {

		if (msgctl(Msg_send, IPC_RMID, 0) < 0) {
			SysErrorTrace("delete message error\n");
			exit(0);
		}
		SysInfoTrace("rm msg_send sucess\n");
	}


	Msg_send_ack= msgget((key_t) KEY_ID_ACK, IPC_EXCL);
	if (Msg_send_ack< 0) {
		exit(0);
	} else {

		if (msgctl(Msg_send_ack, IPC_RMID, 0) < 0) {
			SysErrorTrace("delete message ACK error\n");
			exit(0);
		}
		SysInfoTrace("rm msg_send_ACK sucess\n");
	}

	/*******!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1************/
#if 0

	Free_Lock_Queue_Destroy(&(objSnmp.send));
#endif

#if 1
	SN_Free_Lock_Queue_Destroy(&(objSnmp.send.queue));
	
	sem_destroy(&(sem_var.full));
	sem_destroy(&(sem_var.mutex));

	//sem_destroy(&(objSnmp.send.sem));

	SN_Free_Lock_Queue_Destroy(&(objSnmp.send_ack.queue));
	sem_destroy(&(objSnmp.send_ack.sem));
#endif

//	Mutex_Var *var = ret_mutex_var();
//	pthread_mutex_destroy(&(var->count_mutex));
//	pthread_mutex_destroy(&(var->rw_mutex));

	exit(0);
}
#endif

void init_SnmpData()
{
	SnmpData[1] = snmp_data_0x01;
	SnmpData[2] = snmp_data_0x02;
	SnmpData[3] = snmp_data_0x03;
	SnmpData[4] = snmp_data_0x04;
	SnmpData[5] = snmp_data_0x05;
	SnmpData[0x32] = snmp_data_0x32;
	SnmpData[0x36] = snmp_data_0x36;
	SnmpData[0x60] = snmp_data_0x60;
	SnmpData[0x61] = snmp_data_0x61;
	SnmpData[0x62] = snmp_data_0x62;
	SnmpData[0x63] = snmp_data_0x63;
	SnmpData[0x64] = snmp_data_0x64;
	SnmpData[0x65] = snmp_data_0x65;
	SnmpData[0x66] = snmp_data_0x66;


}
/*****************************************************************************************
 * 										网关线程相关参数初始化
 *****************************************************************************************/
int snmp_init(void) {
	int ret;
	int32 MsgID_Send = -1, MsgID_Recv = -1,MsgID_Send_ack = -1;


	 struct sigaction action;
	 
	 action.sa_handler = my_func;
	 sigemptyset(&action.sa_mask);
	 action.sa_flags = 0;
	 sigaction(SIGINT, &action, 0);
	 sigaction(SIGQUIT, &action, 0);
	 sigaction(SIGTERM, &action, 0);


	MsgID_Send = msgget((key_t) KEY_ID_SEND, IPC_EXCL); //4321
	if (MsgID_Send < 0) {
		/** 初始化发送队列链表 **/
		MsgID_Send = msgget((key_t) KEY_ID_SEND, IPC_CREAT | 0666);
		if (MsgID_Send < 0) {
			SysErrorTrace("create snmp msg send error!");
			return -1;
		}

		SysInfoTrace(
				"this is %s,msg_id is not alived,new create,MsgID_Send = %d",
				__FUNCTION__, MsgID_Send);
	} else
		SysInfoTrace("this is %s,msg_id is alived, MsgID_Send = %d",
				__FUNCTION__, MsgID_Send);

	MsgID_Recv = msgget((key_t) KEY_ID_RECV, IPC_EXCL); //5555
	if (MsgID_Recv < 0) {
		/** 初始化接收队列链表 **/

		MsgID_Recv = msgget((key_t) KEY_ID_RECV, IPC_CREAT | 0666);
		if (MsgID_Recv < 0) {
			SysErrorTrace("create snmp msg recv error!");
			return -2;
		}
		SysInfoTrace(
				"this is %s,msg_id is not alived,new create,MsgID_Recv = %d",
				__FUNCTION__, MsgID_Recv);
	} else
		SysInfoTrace("this is %s,msg_id is alived,MsgID_Recv = %d",
				__FUNCTION__, MsgID_Recv);
	/** 初始化数据链表 信号量等参数 **/

#if 1
	MsgID_Send_ack = msgget((key_t) KEY_ID_ACK, IPC_EXCL); //6666
	if (MsgID_Send_ack < 0) {

		MsgID_Send_ack = msgget((key_t) KEY_ID_ACK, IPC_CREAT | 0666);
		if (MsgID_Send_ack < 0) {
			SysErrorTrace("create snmp msg send ack error!");
			return -2;
		}
		SysInfoTrace(
				"this is %s,msg_id is not alived,new create,MsgID_Send_ack = %d",
				__FUNCTION__, MsgID_Send_ack);
	} else
		SysInfoTrace("this is %s,msg_id is alived,MsgID_Send_ack = %d",
				__FUNCTION__, MsgID_Send_ack);
#endif

#if 1
	ret = SN_Free_Lock_Queue_Create(&(objSnmp.send.queue));
	if (-1 == ret)
	{
		SysErrorTrace("create snmp msg send queue error!");
		return -1;
	}

	sem_init(&(sem_var.full), 0, 0);
	sem_init(&(sem_var.mutex), 0, 1);
	sem_init(&(trapsignal), 0, 0);
	
	ret = SN_Free_Lock_Queue_Create(&(objSnmp.send_ack.queue));
	if (-1 == ret)
	{
		SysErrorTrace("create snmp msg send_ack queue error!");
		return -1;
	}
//	ret = SN_Free_Lock_Queue_Create(&(objSnmp.recv.queue));
//	if (-1 == ret)
//	{
//		SysErrorTrace("create snmp msg send_ack queue error!");
//		return -1;
//	}

	
	//sem_init(&(objSnmp.send.sem), 0, 0);
	sem_init(&(objSnmp.send_ack.sem), 0, 0);
	
	set_timer();
	signal(SIGALRM, signal_handler);

	init_SnmpData();

	

#endif
	return 0;
}


/*******!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1************/
#if 1
/*===========================================================================
 与网关进程的数据协议解析
 ===========================================================================*/
static int snmp_data_ack(QUEUE_MSG_T *queue, SN_DataObject *obj,
		unsigned long oper_code) {
	SN_Msg_Param node;
	int ret;
	queue->msg_type = CMD_ACK;
	queue->snmp_msg.error_code = 0;
	queue->snmp_msg.oper_code = oper_code;

	
	SysDebugTrace("@@@snmp_data_ack:CMD_ACK[%x]oper_code[%X]\r\n",CMD_ACK,oper_code);
	ret = SN_Free_Lock_Queue_Push((char *) queue, sizeof(QUEUE_MSG_T), 0,
			obj->send_ack.queue);
	if (ret == -1) {
		SysErrorTrace("push ack error\n");
		return -1;
	}
	
	sem_post(&(obj->send_ack.sem));

	return 0;
}

/*===========================================================================
 与网关进程的数据协议解析
 ===========================================================================*/
static int snmp_data_set_ack(QUEUE_MSG_T *queue, SN_DataObject *obj,
		unsigned long oper_code, int error) {
	SN_Msg_Param *node;
	int ret = 0;
	queue->msg_type = CMD_ACK;
	queue->snmp_msg.error_code = error;
	queue->snmp_msg.oper_code = oper_code;
	SysDebugTrace("@@@snmp_data_set_ack:CMD_ACK[%x]oper_code[%d],error[%d]\r\n",CMD_ACK,oper_code,error);
	ret = SN_Free_Lock_Queue_Push((char *) queue, sizeof(QUEUE_MSG_T), 0,
			obj->send_ack.queue);
	if (ret == -1) {
		SysErrorTrace("push set ack error\n");
		return -1;
	}
	
	sem_post(&(obj->send_ack.sem));

	return 0;
}

/*===========================================================================
 *
 * 						trap
 ===========================================================================*/
static int snmp_data_trap(QUEUE_MSG_T *queue, SN_DataObject *obj,
		unsigned long oper_code) {
	SN_Msg_Param *node;
	int ret = 0;
	queue->msg_type = CMD_TRAP;
	queue->snmp_msg.error_code = 0;
	queue->snmp_msg.oper_code = oper_code;

#ifdef	__SNMP_PRINT
	SysInfoTrace("snmp_data_trap msg_type = %0x,oper_code = %0x",
			queue->msg_type, oper_code);
#endif

	sem_wait(&(sem_var.mutex));
#ifdef  __SNMP_PRINT
        SysInfoTrace("snmp_data_trap befor msg_num = %d\n",queue_size(obj->send.queue));
#endif

	ret = SN_Free_Lock_Queue_Push((char *) queue, sizeof(QUEUE_MSG_T), 0,
			obj->send.queue);
	if (ret == -1) {
		sem_post(&(sem_var.mutex));
		SysErrorTrace("push trap error\n");
		return -1;
	}
#ifdef  __SNMP_PRINT
        SysInfoTrace("snmp_data_trap after msg_num = %d\n",queue_size(obj->send.queue));
#endif

	sem_post(&(sem_var.mutex));
	sem_post(&(sem_var.full));
	//sem_post(&(obj->send.sem));

	return 0;
}
#endif


#if	1
//获去GateWay
static int get_gateway(char *gateway) {
	FILE *fp;
	char buf[512];
	char cmd[128];
// char gateway[30];
	char *tmp;

	strcpy(cmd, "ip route");
	fp = popen(cmd, "r");
	if (NULL == fp) {
		perror("popen error");
		return -1;
	}
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		tmp = buf;
		while (*tmp && isspace(*tmp))
			++tmp;
		if (strncmp(tmp, "default", strlen("default")) == 0)
			break;
	}
	sscanf(buf, "%*s%*s%s", gateway);
	SysInfoTrace("default gateway:%s\n", gateway);

	pclose(fp);

	return 0;
}
#endif

#if 1
static int set_gateway(const unsigned char *ip) {

	int sockFd;
	struct sockaddr_in sockaddr;
	struct rtentry rt;
	if (is_valid_ip(ip) < 0) {
		SysInfoTrace("gateway was invalid!\n");
		return -1;
	}

	sockFd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockFd < 0) {
		perror("Socket create error.\n");
		return -1;
	}

	memset(&rt, 0, sizeof(struct rtentry));
	rt.rt_dst.sa_family = AF_INET;
	((struct sockaddr_in *) &rt.rt_dst)->sin_addr.s_addr = 0;
	rt.rt_genmask.sa_family = AF_INET;
	((struct sockaddr_in *) &rt.rt_genmask)->sin_addr.s_addr = 0;
	rt.rt_flags = RTF_UP;
	if (ioctl(sockFd, SIOCDELRT, &rt) < 0) {
		perror("ioctl(SIOCDELRT) error in DELT_default_route\n");
		close(sockFd);
		return -1;
	}

	memset(&rt, 0, sizeof(struct rtentry));
	memset(&sockaddr, 0, sizeof(struct sockaddr_in));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = 0;
	if (inet_aton(ip, &sockaddr.sin_addr) < 0) {
		perror("inet_aton error\n");
		close(sockFd);
		return -1;
	}

	memcpy(&rt.rt_gateway, &sockaddr, sizeof(struct sockaddr_in));
	((struct sockaddr_in *) &rt.rt_dst)->sin_family = AF_INET;
	((struct sockaddr_in *) &rt.rt_genmask)->sin_family = AF_INET;
	rt.rt_flags = RTF_GATEWAY;
	if (ioctl(sockFd, SIOCADDRT, &rt) < 0) {
		perror("ioctl(SIOCADDRT) error in set_default_route\n");
		close(sockFd);
		return -1;
	}

	return 0;

}

#endif

static int clean_MsgQueue(QUEUE_MSG_T *p_msg, int c) {
	int msqid = -1;
	int ret_value = MSG_OK;
	static int count = 0;

	msqid = msgget((key_t) c, IPC_EXCL);/*检查消息队列是否存在 */
	if (msqid < 0) {
		SysInfoTrace("%s:%d  fail to get msg queue\r\n", __FUNCTION__,
				__LINE__);
		return -1;
	}

	while (ret_value != -1) {
		ret_value = msgrcv(msqid, p_msg, sizeof(SNMP_MSG_T), CMD_ACK,
				IPC_NOWAIT);
	}
	return 0;
}

/****************************************************************************
 *
 *	业务进程，负责跟代理通讯，初始化snmp队列，完成接收、发送、应答线程的创建
 *
 *
 *
 *****************************************************************************/

