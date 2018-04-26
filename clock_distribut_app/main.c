#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <time.h>
#include <sys/ioctl.h>

#include "main.h"
#include "fpga_ctl.h"
#include "led_ctl.h"
#include "data_trap.h"
//#include "SysLogPublic.h"
#include "snmp.h"

//#include <linux/ptp_clock.h>

#include "ptp_clock.h"
#ifndef CLOCK_INVALID
#define CLOCK_INVALID -1
#endif

static unsigned char gps_status = GPS_UNCONNECT;

static unsigned char mach_type = 0;
static unsigned char cur_status = 0;
static unsigned char masterTOslave = 0;
static unsigned char wait_ptp_flag = 0;

static clockid_t get_clockid(int fd)
{
  #define CLOCKFD 3
  #define FD_TO_CLOCKID(fd)	((~(clockid_t) (fd) << 3) | CLOCKFD)

	return FD_TO_CLOCKID(fd);
}

pthread_mutex_t lock;

void fpga_rst_ctl(int status)
{
   if(status == FPGA_RST_LOW)
      ioctl(led_fd, FPGA_RST_CLR, 0);
   else if(status == FPGA_RST_HIGH)
      ioctl(led_fd, FPGA_RST_SET, 0);
}

void led_ctl(int status)
{
   if(status == LED_ON)
      ioctl(led_fd, MPU_LED_ON, 0);
   else if(status == LED_OFF)
      ioctl(led_fd, MPU_LED_OFF, 0);
}

void net_clk_choose(int status)
{
  if(status == OSC_CLK)
	  ioctl(led_fd, NETCLK_GPIO_SET, 0);
  else if(status == FPGA_CLK)
	  ioctl(led_fd, NETCLK_GPIO_CLR, 0);
}

int open_device(void)
{
    led_fd = open(LED_FILE_NAME, O_RDWR);
	if(led_fd < 0){
	   perror("open led");
	   return -1;
	}
	emifa_fd = open(EMIFA_FILE_NAME, O_RDWR);
	if(emifa_fd < 0){
	   perror("open emifa");
	   return -1;
	}
	trigger_fd = open(TRIGGER_FILE_NAME, O_RDWR);
	if(trigger_fd < 0){
	   perror("open trigger");
	   return -1;
	}
	ptp0_fd = open(PTP0_FILE_NAME, O_RDWR);
	if(ptp0_fd < 0){
	   perror("open ptp0");
	   return -1;
	}

	FPGA_disable(led_fd);

	return 0;
}

void close_device(void)
{
  if(led_fd >= 0)
	  close(led_fd);
  if(emifa_fd >= 0)
	  close(emifa_fd);
  if(trigger_fd >= 0)
	  close(trigger_fd);
  if(ptp0_fd >= 0)
	  close(ptp0_fd);
}

int get_conf(const char *buff)
{
   char *substr;

   substr = strstr(buff, "TYPE=");
   if(substr != NULL){
      substr = substr + strlen("TYPE=");
	  if(*substr=='M' || *substr=='m'){
		  printf("clock_type: master\n");
		  return CLOCK_MASTER;
	  }
	  else if(*substr=='S' || *substr=='s'){
		  printf("clock_type: slave\n");
		  return CLOCK_SLAVE;
	  }
	  else{
	      printf("TYPE=ERROR,read wrong type\n");
		  return -1;
	  }
   }
   else{
      perror("read TPYE"); 
	  return -1;
   }
}

void *thread_check_gps_status_handle(void *arg)
{
    unsigned char val; 
	static unsigned char delay_time = 0;

	for(;;)
	{
	   val = fpga_read_logic_status(emifa_fd);
	   ClockParameter.gps_lockstate = val & 0x80;  
	   if(val & 0x80){
	      gps_status = GPS_CONNECT;
	      printf("GPS_CONNECT!!!\n");
          
	   }
	   else{
	      gps_status = GPS_UNCONNECT;
	      printf("GPS_UNCONNECT!!!\n");
		  delay_time = 0;
	   }
       sleep(8);      
       system("./ntpq -pn -crl");
  
	}
	pthread_exit(0);
}

int send_system_time(void)
{

	struct tm *p_tm;
    struct timespec ts;
	int temp,i;
	int buff[6];

    clock_gettime(clkid, &ts);
    printf("ptp is time():%ld\n",ts.tv_sec);
	p_tm = gmtime(&ts.tv_sec);

	buff[5] = p_tm->tm_sec;

	buff[4] = p_tm->tm_min;
	buff[3] = p_tm->tm_hour;
	buff[2] = p_tm->tm_mday;
	buff[1] = p_tm->tm_mon+1;

	temp = p_tm->tm_year + 1900 - 2000;
	if(temp > 0)
	  buff[0] = temp;
	else
	  buff[0] = p_tm->tm_year;

    for(i=0; i<6; i++)
      fpga_write_ptp_time(emifa_fd, buff, i);

	for(i=0; i<6; i++)
	   buff[i] = fpga_read_fpga_time(emifa_fd, i);
	
	printf("%d-%d-%d %d:%d:%d\n", buff[0],buff[1],buff[2],buff[3],buff[4],buff[5]);
    
    if(buff[3] == 9 && buff[4] == 0  && buff[5] == 0)
       wait_ptp_flag = 1;

    return 0;
}

void *thread_send_ptp_time_handle(void *arg)
{
	int ptp_val;
    int ret ,net_ret;
	int ptp_status = 0;
	unsigned int fpga_status = 0;
    unsigned int fpga_date = 0;
    char buffnet = 0; 
    struct timespec ts;
    struct timeval tv;
    unsigned int ptp_num = 0;
    unsigned int fpga_config = 0;
    unsigned int net_flag = 0 ,net_count = 0;


    for(;;){

          fpga_date = fpga_read_work_status(emifa_fd);
          ClockParameter.device_fpgastate = fpga_date; 
	      printf("FPGA work status: 0x%x\n", fpga_date);

          if (clock_gettime(clkid, &ts)) {
			 perror("clock_gettime");
		  } else {
			 printf("clock time: %ld.%09ld or %s",
		   	        ts.tv_sec, ts.tv_nsec, ctime(&ts.tv_sec));
			 sprintf(ClockParameter.ptp_time, "%s",ctime(&ts.tv_sec)); 
		    }

          gettimeofday( &tv, NULL );
          printf("system time : %ld.%ld\n", tv.tv_sec, tv.tv_usec);
          sprintf(ClockParameter.gps_time, "%s",ctime(&tv.tv_sec));  

          net_fd = open(NET_FILE_NAME, O_RDONLY);
	      if(net_fd > 0){
	          net_ret = read(net_fd, &buffnet, 1);
	          close(net_fd);
	       }
	      else{
	          perror("open net carrier file");
	          pthread_exit(0);
	       }
	      ClockParameter.net_connectstate = buffnet; 
          printf("buffnet = %d\n",buffnet);

	      ret = read(trigger_fd, &ptp_val, sizeof(int));
	      if(ret < 0){
	         perror("read DP83640 trigger");
		     pthread_exit(0);
	      }
          led_ctl(LED_OFF);
	      if(ptp_val == 1){	
             ptp_num++;	     
			 fpga_status = fpga_read_config(emifa_fd);
			 fpga_status |= PTP_VALID; 

		     if(cur_status == CLOCK_SLAVE && buffnet == 49){  
                system("./ptp/testptp -S");
                if(ts.tv_sec == tv.tv_sec){
		          fpga_write_config(emifa_fd, (fpga_status | CLK_PTP) & OMAP_RUN_LED);  
                   fpga_config = fpga_read_config(emifa_fd);
                   ClockParameter.clock_source = fpga_config & 0xc000;   
                   ClockParameter.ptp_lockstate = fpga_config & 0x2000; 
                   ClockParameter.arm_runstate = fpga_config & 0x1000;  

                }
                else
                  printf("phy time syn chronizate system time error !!!\n");

                send_system_time();
                if((net_flag == 1) && (net_count >= 3)){
                    printf("--------------reboot slave ptp----------------\n");
                    net_flag = 0;
                    net_count = 0;
                    system("reboot");

                }
                led_ctl(LED_ON);
             }
            else if(cur_status == CLOCK_SLAVE && buffnet == 48){
                fpga_write_config(emifa_fd, (fpga_status & (~PTP_VALID)));
                fpga_config = fpga_read_config(emifa_fd);
                net_flag = 1;
                net_count++;
                printf("fpga_config == 0x%x\n",fpga_config);
                printf("slave mode net error!!!\n");
             }
            else if(cur_status == CLOCK_MASTER && buffnet == 49){   
                  printf("ptp_num11111 == %d\n",ptp_num);

                  if(ptp_num == 60){
                     system("./ptp/testptp -s");
                     fpga_write_config(emifa_fd, fpga_status & OMAP_RUN_LED);
                     fpga_config = fpga_read_config(emifa_fd);   
                    // printf("fpga_config == 0x%x\n",fpga_config);
                     ptp_num = 100;
                  }
                  else if(ptp_num == 3700){
                     system("./ptp/testptp -s");
                     ptp_num = 3700;
                  } 
                  else
                     printf("master wait for ptp time !!!\n");

                  if(ptp_num == 3800)
                     ptp_num = 3700;
                     
                  if(ts.tv_sec == tv.tv_sec){
		             fpga_write_config(emifa_fd, (fpga_status  | CLK_GPS) & OMAP_RUN_LED);  //如果是主机选择GPS时钟源
                     fpga_config = fpga_read_config(emifa_fd);
                     printf("system time synchronizate phy time OK !!!\n");
                  }
                  else
                     printf("system time synchronizate phy time error !!!\n"); 

                  ClockParameter.clock_source = fpga_config & 0x4000; 
                  ClockParameter.ptp_lockstate = fpga_config & 0x2000; 
                  ClockParameter.arm_runstate = fpga_config & 0x1000;  

                  send_system_time();
                  if(wait_ptp_flag == 1){
                    wait_ptp_flag = 0;
                    system("./ptp/testptp -s");
                  }
               
                  led_ctl(LED_ON);                              
	         }
            else
            	printf("----------error-----------\n");

              ptp_status = 1;
	   }

	   switch(cur_status){
	      case CLOCK_MASTER:
			  if(gps_status == GPS_UNCONNECT){
				  cur_status = CLOCK_SLAVE;
	              printf("GPS unconnect, change to slave clock.\n");
			  }
			  break;
		  case CLOCK_SLAVE:
			  if(mach_type == CLOCK_MASTER && gps_status == GPS_CONNECT && masterTOslave == 0){
			      cur_status = CLOCK_MASTER;
				  printf("GPS recover, change to master clock.\n");
                  
                  system("./ptp/ntpd -gN -4 -c /etc/ntp.conf");
	              printf("run command ntpd\n");

		          fpga_status = fpga_read_config(emifa_fd);
		          fpga_write_config(emifa_fd, (fpga_status & (~CLK_PTP)) | CLK_GPS);
	              led_ctl(LED_ON); 
			  }
			  break;
		  default:
			  printf("default\n");  
			  break;
	   }

	}

    pthread_exit(0);
}

int get_fpga_version(char *buffer)
{
	int ret;
	char version_buffer[128] = {0};

	if (buffer == NULL) {
		printf("point is null!");
		return -1;
	}
	 sprintf(version_buffer, "%x", fpga_read_version(emifa_fd));
	 ret = sprintf(buffer, "%c.%c.%c.%c", version_buffer[0], version_buffer[1], version_buffer[2], version_buffer[3]);
	return ret;
}

int main(int argc, const char *argv[])
{
    	int ret;
	char buff[CFG_FILE_SIZE];
	unsigned int fpga_date;
	int snmp_status;
	pthread_t thread_check_gps_status, thread_send_ptp_time;
	pthread_t pthSNMPRecv, pthSNMPSend, pthSNMPSendAck,pthsecTask;

	ArmVersion.wVerBuildYear = _VERSION_YEAR;
	ArmVersion.bVerBuildMonth = _VERSION_MONTH;
	ArmVersion.bVerBuildDay = _VERSION_DAY;
	ArmVersion.bVerBuildId = _VERSION_BUILD_ID;
	ArmVersion.bVerMajor = _VERSION_MAJOR;
	ArmVersion.bVerMinor = _VERSION_MINOR;
	ArmVersion.bVerRevision = _VERSION_;

	ret = open_device();
	if(ret < 0){
	   perror("open device");
	   goto err;
	}

   clkid = get_clockid(ptp0_fd);
   if (CLOCK_INVALID == clkid) {
	  fprintf(stderr, "failed to read clock id\n");
	  return -1;
   }    

    conf_fd = open(CFG_FILE_NAME, O_RDONLY);
	if(conf_fd > 0){
	   memset(&buff, 0, sizeof(buff));
	   ret = read(conf_fd, buff, CFG_FILE_SIZE);
	   close(conf_fd);
	}
	else{
	   perror("open clock_distribut.conf");
	   return -1;
	}

	if(ret > 0){
	   cur_status = get_conf(buff);
	   ClockParameter.device_mode = cur_status; 
	   mach_type = cur_status;
	}
	else{
	   perror("read clock_distribut.conf");
	   return -1;
	}

    net_clk_choose(OSC_CLK);
    fpga_rst_ctl(FPGA_RST_LOW);
    usleep(20);
    fpga_rst_ctl(FPGA_RST_HIGH);
    usleep(500);

    if(get_fpga_version(ClockParameter.fpga_version))
    	
    net_clk_choose(FPGA_CLK);

	fpga_date = fpga_read_config(emifa_fd);
    if(mach_type == CLOCK_SLAVE)
	   fpga_write_config(emifa_fd, fpga_date | CLK_SLAVE);  //设置从时钟
	else 
	   fpga_write_config(emifa_fd, fpga_date & (~CLK_SLAVE));  //设置主时钟

/*****************************************************************
     ****                      SNMP 初始化                   ****
*****************************************************************/
   #if	1
       snmp_status = snmp_init(); /* 网管初始化 */
   	if (snmp_status != 0) {
   		SysErrorTrace("init snmp error!\n");
   		exit(1);
   	} else {
   		SysInfoTrace("init snmp success!\n");
   	}
   #endif

	if(cur_status == CLOCK_MASTER){
      // net_clk_choose(FPGA_CLK);
	   printf("wait 60s to check GPS status ...\n");
       sleep(5);
	   pthread_mutex_init(&lock, NULL);
       if(pthread_create(&thread_check_gps_status, NULL, 
		  thread_check_gps_status_handle, NULL) != 0){
	      perror("fail to pthread_create check_gps_status");
		  return -1;
	   }
	   sleep(2);
	   printf("listen GPS status ...\n"); 
	  // led_ctl(LED_ON);
	}


    if(pthread_create(&thread_send_ptp_time, NULL, 
	   thread_send_ptp_time_handle, NULL) != 0){
	   perror("fail to pthread_create send_ptp_time_handle");
	   return -1;
	} 

 /*****************************************************************
  ****                      SNMP 接受，发送，响应线程              ****
 *****************************************************************/
#if	1
    snmp_status = pthread_create(&pthSNMPRecv, NULL, pthread_snmp_recv, NULL);
	if (snmp_status) {
		SysErrorTrace("create thread pthread_snmp_recv error!\n");
		goto leave;
	}
#endif

#if	1
	snmp_status = pthread_create(&pthSNMPSend, NULL, pthread_snmp_send, NULL);
	if (snmp_status) {
		SysErrorTrace("create thread pthread_snmp_send error!\n");
		goto leave;
	}
#endif

#if	1
	snmp_status = pthread_create(&pthSNMPSendAck, NULL, pthread_snmp_send_ack, NULL);
	if (snmp_status) {
		SysErrorTrace("create thread pthread_snmp_send error!\n");
		goto leave;
	}
#endif

	if(pthread_create(&pthsecTask, NULL, 
		pthread_second_task, NULL) != 0){
		perror("fail to pthread_create pthsecTask");
		return -1;
	} 

	for(;;){   

	}
   
	pthread_join(thread_check_gps_status, NULL);
	pthread_join(thread_send_ptp_time, NULL);
	pthread_mutex_destroy(&lock);
	return 0;
err:
	close_device();
leave:
	//Syslink_Delete();
	printf("<-- main:\n");
	return -1;
}
