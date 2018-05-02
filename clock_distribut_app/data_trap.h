#ifndef DATA_TRAP_H
#define DATA_TRAP_H

/*
 * 版本格式：bVerMajor_bVerMinor_bVerRevision_wVerBuildYear_bVerBuildMonth_bVerBuildDay_bVerBuildId
 * 例如：1.0.1.20180412_1
 */
typedef struct app_version {

	 unsigned int bVerMajor;                      /*<  major version number                */
	 unsigned int bVerMinor;                      /*<  minor version number                */
	 unsigned int bVerRevision;                   /*<  revision version number             */
	 unsigned int bVerBuildId;                    /*<  in the same day, build id           */
	 unsigned int bVerBuildDay;                   /*<  build day 1-31                      */
	 unsigned int bVerBuildMonth;                 /*<  build month 1-12                     */
	 unsigned int wVerBuildYear;                 /*<  build year 2015-2099                */

}VERSION;

typedef struct parameter {

    char fpga_version[128];      //FPGA版本0
	char device_name[128];	     // 时钟分配器型号0
	char ptp_time[128];     //ptp时间 0
	char gps_time[128];     //gps时间0
	char recvport[128];		//本机ip
    unsigned int device_mode; //设备模式  01主模式  02从模式   			1
    unsigned int device_fpgastate;  //fpga的工作状态 				1 5a=M aa=S

	
    unsigned int clock_source;   //gps时钟源选择			1--gps  11 ptp			1		
    unsigned int gps_lockstate;  //gps锁定状态				80 lock !80 un
    unsigned int ptp_lockstate;  //ptp锁定状态				2000 lock !2000 un
    unsigned char net_connectstate;  //网络连接状态			49=con  48=dis
    unsigned int arm_runstate;    //arm运行指示状态			0x1000=stop ~0x1000 = run	

}CLOCK_PARA;

#define		_VERSION_YEAR 	2018
#define		_VERSION_MONTH	04
#define		_VERSION_DAY	27
#define		_VERSION_MAJOR	1
#define		_VERSION_MINOR	0
#define		_VERSION_	0
#define		_VERSION_BUILD_ID	1

VERSION ArmVersion;
CLOCK_PARA ClockParameter;
#define DEVICE_NAME		"Model:Clock_distribut"


 CLOCK_PARA *  cdd_get_clk_param(void);
 int get_arm_version(char *buffer);
 
 int get_hardware_version(char *buffer);
#endif
