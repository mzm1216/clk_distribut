/*
 * snmp.h
 *
 *  Created on: 2016年7月31日
 *      Author: scottie
 */

#ifndef SNMP_H_
#define SNMP_H_


#include "config.h"

#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __SNMP_PRINT
//#undef	__SNMP_PRINT

#define TRUE 	1
#define FALSE	0

#define SIZE 128

#define ZERO 0
#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4

#define IDLE 0
#define BUSY_TX 1
#define BUSY_RX 2
#define DISABLE 3
#define ALARM 4

#define		CARRIER		"carrier"
#define		CTC_OR_CDC		"ctcss/cdcss"
#define   CARRIER_AND_CTCSS_CDCSS  "carrier&ctcss/cdcss"
#define   CARRIER_OR_CTCSS_CDCSS   "carrier|ctcss/cdcss"

#define LINK_STATE_ON 1
#define LINK_STATE_OFF 0

#define  EXT_CLOCK_ON 1
#define  EXT_CLOCK_OFF 0

//#define  BTS_IDLE 1
//#define  BTS_BUSY 0

#define		KEY_ID_SEND		4321
#define		KEY_ID_RECV		5555
#define    	KEY_ID_ACK 		6666

#define STOP_ 0
#define START_ 1
#define CLCOK_SOURCE_BIT		(0x3<<14)
#define GPS_LOCK_STATE_BIT		(0x0080)
#define PTP_LOCK_STATE_BIT		(0x2000)
#define FPGA_WORK_STATE_MASTER			(0xaa)
#define NET_CONNECT_STATE_BIT	(49)
#define OMAP_RUN_STATE_BIT		(0x1000)


#if 0
#define ENABLE "enable"
#define DISABLE "disable"
#endif

#if 0
#define ENABLE "启用"
#define DISABLE "停用"
#endif
//#define SLOT_1 "1:"
//#define SLOT_2 "2:"

#define CSS_TYPE_NULL 0
#define CSS_TYPE_CTC  1
#define CSS_TYPE_CDC 2
#define CSS_TYPE_REV_CDC 3

#define CONV 1	//常规
#define MPT 2	//模拟集群
#define PDT 3	//数字集群
#define CONV_SIMULCAST 4	//常规同播
#define MPT_SIMULCAST 5	//模拟集群同播
#define PDT_SIMUCAST 6	//数字集群同播
#define STOP 7	//暂停工作

typedef enum Alarm {
	ALARMSOURCE_PowerOnSelfCheck, /* 开机自检告警 */
	ALARMSOURCE_Fan, /* 风扇异常告警 */
	ALARMSOURCE_OverTemperature, /* 高温告警 */
	ALARMSOURCE_ForwardPower, /* 前向功率告警*/
	ALARMSOURCE_Vswr, /* 驻波比异常告警 */
	ALARMSOURCE_TxUnlock, /* 发射失锁告警 */
	ALARMSOURCE_RxUnlock, /* 接收失锁告警 */
	ALARMSOURCE_OverVoltage, /* 电源过压告警 */
	ALARMSOURCE_LowVoltage, /* 电源低压告警 */

	ALARMSOURCE_PAOverTemperature, /* PA高温 */
	ALARMSOURCE_OpenCover, /* 开盖 */
	ALARMSOURCE_HighTxPower, /* 高发射功率 */
	ALARMSOURCE_LowTxPower /* 低发射功率 */
} AlarmCode;

/*
 typedef		struct{
 long mtype;
 char buffer[1024];
 }MsgBuff;
 */

#define MAX_SIZE    256

#define CMD_GET  0x1000     /* 读取命令 (SNMP -> APP) */
#define CMD_SET  0x2000     /* 设置命令 (SNMP -> APP) */
#define CMD_ACK  0x8000     /* 应答命令 (SNMP <- APP) */
#define CMD_TRAP 0xA000     /* 告警消息 (SNMP <- APP) */

#define		NET_NAME	"eth0"
#define   SINGLECAST  "s:"
#define   MULTICAST  "m:"
/*操作码定义*/
#define		MSG_CODE_NET_IPADDR	0x01  /*IP地址*/
#define		MSG_CODE_NET_MASK	0x02  /*子网掩码*/
#define		MSG_CODE_NET_GW	0x03   /*网关*/
#define		MSG_CODE_NET_MAC	0x04  /*网卡MAC地址*/
#define		MSG_CODE_NET_DB_IP	0x05  /*数据库IP地址*/
#define		MSG_CODE_NET_SC_IP	0x06  /*同播中心IP地址，只针对信道机有效*/
#define		MSG_CODE_NET_LDS_IP	0x07  /*调度服务器IP地址，只对同播中心有效*/
#define		MSG_CODE_NET_RQC_IP		0x08  /*录音服务器IP地址，只对同播中心有效*/
#define		MSG_CODE_NET_MGW_IP	0x09  /*媒体网关IP地址，只对同播中心有效*/

#define		MSG_CODE_BTS_TX_FREQ	0x10  /*信道发射频率*/
#define		MSG_CODE_BTS_RX_FREQ	0x11  /*信道接收频率*/
#define		MSG_CODE_BTS_TX_POWER	0x12  /*信道发射功率*/
#define		MSG_CODE_BTS_SQUELCH	0x13  /*静噪等级*/
#define		MSG_CODE_BTS_CH_MODE	0x14  /*信道工作模式*/
#define		MSG_CODE_BTS_CH_BAND	0x15   /*信道带宽*/
#define		MSG_CODE_BTS_TIME_DLY	0x16    /*发射延迟时间*/
#define		MSG_CODE_BTS_PORT_SL1	0x17   /*时隙1端口号*/
#define		MSG_CODE_BTS_PORT_SL2	0x18  /*时隙2端口号*/

//#define		MSG_CODE_BTS_LOCAL_PORT	0x19  /*本地接收端口*/

#define		MSG_CODE_BTS_MODEL	0x20  /*信道机型号*/
#define		MSG_CODE_BTS_ESN	0x21  /*信道机电子串号*/
#define		MSG_CODE_BTS_HW_VER	0x22  /*信道机硬件版本*/
#define		MSG_CODE_BTS_FW_VER	0x23  /*信道机软件版本*/
#define		MSG_CODE_BTS_VSWR	0x24  /*发射电压驻波比*/
#define		MSG_CODE_BTS_RSSI	0x25  /*接收场强*/
#define		MSG_CODE_BTS_ENV_TEMP	0x26  /*环境温度*/
#define		MSG_CODE_BTS_PA_TEMP	0x27  /*功放温度*/
#define		MSG_CODE_BTS_DC_VOLT	0x28  /*直流电压*/
#define		MSG_CODE_BTS_BAT_VOLT	0x29  /*电池电压*/
#define		MSG_CODE_BTS_FAN_RPM	0x2a /*风扇转速*/

#define      MSG_CODE_BTS_REAL_TX_POWER 0x2b /*实际发射功率*/
#define 	MSG_CODE_BTS_FAULT_MODE   0x002C/*故障弱化时工作模式*/

#define		MSG_CODE_CHC_RSSI_THRS	0x30  /*判选门限*/
#define		MSG_CODE_CHC_BUF_LEN	0x31  /*缓冲时长*/
#define 	MSG_CODE_EQU_TYPE   0x0032        /*设备类型*/

#define     MSG_CODE_VERSION   0x0034    /*同播中心软件版本*/
#define     MSG_CODE_BTS_EQU_STATE  0x0035        /*信道机状态*/

#define		MSG_CODE_BTS_SORCE	0x36  /*本地接收端口*/

#define		MSG_CODE_ALARM_STATE		0x40  /*报警信息*/
#define		MSG_CODE_LINK_STATE		0x41  /*链路状态*/
#define		MSG_CODE_EXTCLK_STATE		0x42  /*外部时钟状态*/

#define     MSG_CODE_BTS_TX_STATE 0x43 /*信道机发射状态*/
#define     MSG_CODE_SLOT_CONVERSATION_STATE 0x44 /*中心时隙通话状态*/
#define     MSG_CODE_PTT_STATE  0x45/*中心讲话方信息*/
#define     MSG_CODE_PROC_ABORT_STATE  0x46/*应用程序退出*/

#define     MSG_CODE_BTS_SQUELCH_MODE 0x50 /*Carrier和CTCSS/CDCSS(模拟常规/同播模式有效)*/
#define     MSG_CODE_BTS_RX_CSS 0x51 /*接收亚音设置(模拟常规模/同播式有效)*/
#define     MSG_CODE_BTS_TX_CSS  0x52/*发射亚音设置(模拟常规/同播模式有效)*/
#define     MSG_CODE_BTS_CC  0x53/*色码(数字常规/同播模式有效)*/
#define     MSG_CODE_DEV_ENABLE  0x54/*信道机启用/停用，enable/disable*/

/***************************时钟分配器操作吗************************************************/
#define     MSG_CODE_CDD_WORK_MODE 		0x60 /*工作模式*/
#define     MSG_CODE_CDD_APP_VER 		0x61 /*应用版本*/
#define     MSG_CODE_CDD_FPGA_VER  		0x62/*fpga版本号*/
#define     MSG_CODE_CDD_HW_VER  		0x63/*硬件版本*/
#define     MSG_CODE_CDD_MODEL_STATE  	0x64/*时钟分配器型号*/
#define     MSG_CODE_CDD_GPS_TIME  		0x65/*GPSs时间*/
#define     MSG_CODE_CDD_PTP_TIME  		0x66/*GPSs时间*/

#define		MSG_CODE_CDD_NET_IPADDR			MSG_CODE_NET_IPADDR  /*IP地址*/
#define		MSG_CODE_CDD_NET_MASK			MSG_CODE_NET_MASK  /*子网掩码*/
#define		MSG_CODE_CDD_NET_GW				MSG_CODE_NET_GW   /*网关*/
#define		MSG_CODE_CDD_NET_MAC			MSG_CODE_NET_MAC  /*网卡MAC地址*/
#define		MSG_CODE_CDD_NET_DB_IP			MSG_CODE_NET_DB_IP  /*数据库IP地址*/


#define     MSG_CODE_CDD_EQU_TYPE  		MSG_CODE_EQU_TYPE/*设备类型 0x32*/
#define     MSG_CODE_CDD_SORCE  		MSG_CODE_BTS_SORCE/*原地址 0x36*/

/***************************时钟分配器TRAP信息码************************************************/
//#define     MSG_CODE_CDD_CLK_SOURCE_TRAP 		0x70 /*时钟源状态*/
//#define     MSG_CODE_CDD_CLK_LOCK_TRAP 			0x71 /*时钟源锁定状态*/
//#define     MSG_CODE_CDD_TIME_TABLE_TRAP 		0x72 /*各类时间*/
//#define     MSG_CODE_CDD_CLK_CONVER_TRAP 		0x73 /*时钟转换状态*/
//#define     MSG_CODE_CDD_SECOND_PULSE_TRAP 		0x74 /*秒脉冲状态*/
//#define     MSG_CODE_CDD_NET_LINK_TRAP 			0x75 /*连接状态*/
//#define     MSG_CODE_CDD_EXT_LINK_TRAP 			0x43 /*连接状态*/

#define     SNMP_TRAP_QUEUE_CDD_CLK_SOURCE_TRAP 		0x70 /*时钟源状态*/
#define     SNMP_TRAP_QUEUE_CDD_GPS_LOCK_TRAP 			0x71 /*GPS锁定状态*/
#define     SNMP_TRAP_QUEUE_CDD_PTP_LOCK_TRAP 			0x72 /*PTP源锁定状态*/
#define     SNMP_TRAP_QUEUE_CDD_FPGA_WORK_TRAP 			0x73 /*FPGA工作状态*/
#define     SNMP_TRAP_QUEUE_CDD_NET_LINK_TRAP 			0x74 /*网络连接状态*/
#define     SNMP_TRAP_QUEUE_CDD_OMAP_RUN_TRAP 			0x75 /*OMAP运行状态*/

#define 	CDD_CLOCK_SOURCE_TRAP_GPS	0x1
#define 	CDD_CLOCK_SOURCE_TRAP_PTP	0x11


#define MAX_PARAM_NUM  32

//typedef struct {
//	uint8 mode[MAX_PARAM_NUM];
//	uint8 appver[MAX_PARAM_NUM];
//	uint8 fpgaver[MAX_PARAM_NUM];
//	uint8 hwver[MAX_PARAM_NUM];
//	uint8 cddtype[MAX_PARAM_NUM];
//	uint8 fpgastat[MAX_PARAM_NUM];
//	uint8 recvport[MAX_PARAM_NUM];
//} Cdd_roParam_T;

//typedef struct {
//	uint8 gpsclksources[MAX_PARAM_NUM];
//	uint8 ptpclksources[MAX_PARAM_NUM];
//	uint8 gpslock[MAX_PARAM_NUM];
//	uint8 ptplock[MAX_PARAM_NUM];
//	uint8 fpgatime[MAX_PARAM_NUM];
//	uint8 gpstime[MAX_PARAM_NUM];
//	uint8 Omaptime[MAX_PARAM_NUM];
//	uint8 phytime[MAX_PARAM_NUM];
//	uint8 sys2phy[MAX_PARAM_NUM];
//	uint8 phy2sys[MAX_PARAM_NUM];
//	uint8 secpulse[MAX_PARAM_NUM];
//	uint8 netlink[MAX_PARAM_NUM];
//} Cdd_trapParam_T;

/*时钟源状态:gsp、ptp*/
typedef struct {
	char cddGPSClkState[SIZE];
	char cddvigorSource[SIZE];
} Trap_cddClkSourceState_T;

/*gps时钟锁定状态*/
typedef struct {
	char cddGPSLockState[SIZE];
	char cddvigorSource[SIZE];
} Trap_cddGPSLockState_T;


/*PTP时钟锁定状态*/
typedef struct {
	char cddPTPLockState[SIZE];
	char cddvigorSource[SIZE];
} Trap_cddPTPLockState_T;

/*FPGA 工作状态*/
typedef struct {
	char cddFPGAWorkState[SIZE];
	char cddvigorSource[SIZE];
} Trap_cddFPGAWorkState_T;


/*网络连接脉冲*/
typedef struct {
	char cddNetLinkState[SIZE];
	char cddvigorSource[SIZE];
} Trap_cddNetLinkState_T;


/*  OMAP 运行状态*/
typedef struct {
	char cddOmapRunState[SIZE];
	char cddvigorSource[SIZE];
} Trap_cddOmapRunState_T;

#if 0

typedef struct {
	char btsLinkState[SIZE];
	char vigorSource[SIZE];
} Trap_btsLinkState_T;

typedef struct {
	char btsExtClock[SIZE];
	char vigorSource[SIZE];
} Trap_btsExtClock_T;


typedef struct {
	char btsTxState[255];
	char vigorSource[255];
}Trap_btsTxState_T;


typedef struct {
	char btsTxState[SIZE];
	char btsRealTxPower[SIZE];
	char btsVswr[SIZE];
	char btsRssi[SIZE];
	char btsEnvirTemperature[SIZE];
	char btsPaTemperature[SIZE];
	char btsDcVoltage[SIZE];
	char btsBatVoltage[SIZE];
	char btsFanSpeed[SIZE];
	char vigorSource[SIZE];
} Trap_btsState_T;

typedef struct {
	char sccSlotNum[SIZE];
	char sccBusy[SIZE];
	char sccCaller[SIZE];
	char sccCalled[SIZE];
	unsigned char sccRtpIpAddress[SIZE];
	char sccCallInfo[SIZE];
	char vigorSource[SIZE];
} Trap_mscSlotState_T;

typedef struct {
	unsigned char sccBtsIpAddress[SIZE];
	char sccBtsState[SIZE];
	char vigorSource[SIZE];
} Trap_mscBtsState_T;

typedef struct {
	char sccLinkState[SIZE];
	char vigorSource[SIZE];
} Trap_mscLinkState_T;

typedef struct {
	char sccCaller[SIZE];
	char sccCalled[SIZE];
	char sccTalker[SIZE];
	char vigorSource[SIZE];
} Trap_sccPttState_T;

typedef struct {
	char vigorSource[SIZE];
	char equipType[SIZE];
	unsigned char vigorDeviceState[SIZE];
} Trap_vigorHeartbeat_T;

typedef struct {
	char vigorSource[SIZE];
	char equipType[SIZE];
	unsigned char vigorDeviceState[SIZE];
} Trap_vigorProcAbort_T;
#endif

#define VT_3308_MIB TRUE
#define VT_3830_MIB TRUE

/*trap 缓冲区定义 */
#define SNMP_TRAP_QUEUE_ENTRY_LOG_ONLY      1
#define SNMP_TRAP_QUEUE_ENTRY_COMMUNITY     2
#define SNMP_TRAP_QUEUE_ENTRY_LOGGED        4
#define SNMP_TRAP_QUEUE_ENTRY_LOG_AND_TRAP  8
#define SNMP_TRAP_QUEUE_ENTRY_TRAP_ONLY     0x10
#define SNMP_TRAP_QUEUE_ENTRY_DEFAULT       SNMP_TRAP_QUEUE_ENTRY_LOG_AND_TRAP

#define SNMP_TRAP_QUEUE_FULL      1
#define SNMP_TRAP_QUEUE_ERROR     2
#define SNMP_TRAP_QUEUE_OVERLAP   4
#define SNMP_TRAP_QUEUE_OVER_WRITE      1
#define SNMP_TRAP_QUEUE_PEEK      1
#define SNMP_TRAP_MAX_COMM_STR_NAME_LEN   32
#define SNMP_TRAP_MAX_QUE_BUF_SIZE        (1024*128)
#define SNMP_TRAP_MAX_QUE_DATA_CNT        10
#define SNMP_TRAP_QUEUE_ENTRY_HDR_SIZE    ((size_t)(int)&((SNMP_TRAP_QUEUE_ENTRY_T *)0)->content)
#ifndef BOOL
#define BOOL  short
#endif

/*错误码定义*/
#define MSG_OK                 0
#define MSG_GENERROR           -1
#define MSG_TIMEOUT            -2
#define MSG_WRONGOPERCODE      -3
#define MSG_APPOPERERROR       -4

#define SNMP_TRAP_QUEUE_BTSEQUIPMENT ALARM_STATE
#define SNMP_TRAP_QUEUE_BTSLINKSTATE LINK_STATE
#define SNMP_TRAP_QUEUE_BTSEXTCLOCK  EXTCLK_STATE

typedef enum {
	SNMP_TRAP_QUEUE_COLDSTART = 0,
	SNMP_TRAP_QUEUE_WARMSTART,
	SNMP_TRAP_QUEUE_LINKDOWN,
	SNMP_TRAP_QUEUE_LINKUP,
	SNMP_TRAP_QUEUE_AUTHFAIL,
	SNMP_TRAP_QUEUE_EGPNEIGHBORLOSS,
	SNMP_TRAP_QUEUE_ENTERPRISESPECIFIC,
	SNMP_TRAP_QUEUE_XDJ_EQUIPMENT,
	SNMP_TRAP_QUEUE_XDJ_LINKSTATE,
	SNMP_TRAP_QUEUE_XDJ_EXTCLOCK,
	SNMP_TRAP_QUEUE_BTSTXSTATE,
	SNMP_TRAP_QUEUE_CTC_SLOTSTATE,
	SNMP_TRAP_QUEUE_CTCREPEATERSTATE,
	SNMP_TRAP_QUEUE_CTCLINKSTATE,
	SNMP_TRAP_QUEUE_SCCPTTSTATETRAP,
	SNMP_TRAP_QUEUE_VIGORHEARTBEAT,
	SNMP_TRAP_QUEUE_VIGORPROCABORT,
} SNMP_TRAP_TrapType_E;

typedef enum {
	TRAP_EVENT_SEND_TRAP_OPTION_DEFAULT = 0,
	TRAP_EVENT_SEND_TRAP_OPTION_LOG_AND_TRAP,
	TRAP_EVENT_SEND_TRAP_OPTION_LOG_ONLY,
	TRAP_EVENT_SEND_TRAP_OPTION_TRAP_ONLY,
} TRAP_EVENT_SendTrapOption_E;

union semun {
	int val; /* value for SETVAL */
	struct semid_ds *buf; /* buffer for IPC_STAT & IPC_SET */
	u_short *array; /* array for GETALL & SETALL */
};

/*消息数据定义*/
typedef struct {
	unsigned long oper_code; /*操作码*/
	int error_code; /*错误码, 0 is ok, -1 is error*/

	/*This union contains msg data*/
	union {
		unsigned char msg_data_str[MAX_SIZE]; /* 字符串数据 */
		int msg_data_int; /* 整形数据   */
		Trap_cddClkSourceState_T 	cdd_ClkSourceState_trap;/*时钟源状态*/
		Trap_cddGPSLockState_T 		cdd_GPSLockState_trap;	/*时钟锁定状态 */
		Trap_cddPTPLockState_T 		cdd_PTPLockState_trap;		/*集中时间*/
		Trap_cddFPGAWorkState_T 	cdd_FPGAState_trap;/*时钟转换状态*/
		Trap_cddNetLinkState_T		cdd_NetLinkState_trap;	/*网络连接状态*/		
		Trap_cddOmapRunState_T	 	cdd_OmapRunState_trap;	/*秒脉冲状态*/
		
		unsigned char msg_data_str_max[MAX_SIZE*5]; /* 字符串数据 */
	} u_data;
} SNMP_MSG_T;

typedef struct {
	long msg_type; /*消息类型 即get/set/ack/trap*/
	SNMP_MSG_T snmp_msg;
} QUEUE_MSG_T;

typedef struct{
	sem_t  mutex;
	sem_t full;
}Sem_Var;

typedef struct{
	sem_t trapsignal;
}SEM_SIGNAL;




//int32 MsgID_Send, MsgID_Recv;

typedef int (*SNMP_DATA)(QUEUE_MSG_T *);
typedef int (*SNMP_DATA_DEAL)(uint32 cmd,QUEUE_MSG_T *);
extern void * pthread_snmp_recv(void *arg);
extern void * pthread_snmp_send(void *arg);
extern void * pthread_snmp_send_ack(void *arg);
extern void * pthread_second_task(void *arg);





//extern	void * pthread_trap_recv(void *arg);
extern int Trap_Queue_Push(void *data, uint16 cmd, uint16 sub_cmd);
extern void Ack_realtxpower_Push(uint32 data);
extern void Ack_vswr_Push(uint16 pf, uint16 pr, uint16 vswr);
extern void Ack_rssi_Push(int data);
extern void Snmp_Process_init(void);

extern int snmp_init(void);

extern unsigned int s2ui(const char *s);
extern unsigned long long s2ull(const char *s);
#define OPERATE_SUCCESS 0

#ifdef __cplusplus
}
#endif

#endif /* SNMP_H_ */
