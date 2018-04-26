#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include "fpga_ctl.h"
#include "led_ctl.h"

void FPGA_enable(int fd)
{
   ioctl(fd, FPGA_CEn_SET, 0);
   ioctl(fd, FPGA_CONFIGn_CLR, 0);
}

void FPGA_disable(int fd)
{
   ioctl(fd, FPGA_CEn_CLR, 0);
   usleep(500);
   ioctl(fd, FPGA_CONFIGn_SET, 0);
}

unsigned int fpga_read_version(int fd)
{
   fpga_msg.addr = ADDR_VERSION;
   fpga_msg.rx_val = 0;
   ioctl(fd, EMIFA_READ, &fpga_msg);
   
   return fpga_msg.rx_val;
}

void fpga_rdwr_config(int fd, unsigned int val)
{  
  
   fpga_msg.addr = ADDR_CONFIG;
   fpga_msg.rx_val = 0;
   ioctl(fd, EMIFA_READ, &fpga_msg);

   fpga_msg.tx_val = fpga_msg.rx_val | val;
   ioctl(fd, EMIFA_WRITE, &fpga_msg);
}

unsigned int fpga_read_work_status(int fd)
{
   fpga_msg.addr = ADDR_WORK_STATUS;
   fpga_msg.rx_val = 0;
   ioctl(fd, EMIFA_READ, &fpga_msg);
   
   return fpga_msg.rx_val;
}

unsigned int fpga_read_logic_status(int fd)
{
   fpga_msg.addr = ADDR_LOGIC_STATUS;
   fpga_msg.rx_val = 0;
   ioctl(fd, EMIFA_READ, &fpga_msg);
   
   return fpga_msg.rx_val;
}

unsigned int fpga_read_fpga_time(int fd, int offset)
{ 
   fpga_msg.addr = ADDR_FPGA_TIME + offset;
   fpga_msg.rx_val = 0;
   ioctl(fd, EMIFA_READ, &fpga_msg);

   return fpga_msg.rx_val;
}

void fpga_write_ptp_time(int fd, int *buff, int offset)
{
   fpga_msg.addr = ADDR_PTP_TIME + offset;
   fpga_msg.tx_val = *(buff + offset);
   ioctl(fd, EMIFA_WRITE, &fpga_msg);
}

unsigned int fpga_read_ptp_time(int fd, int offset)
{
   fpga_msg.addr = ADDR_PTP_TIME + offset;
   fpga_msg.rx_val = 0;
   ioctl(fd, EMIFA_READ, &fpga_msg);

   return fpga_msg.rx_val;
}

unsigned int fpga_read_gps_time(int fd)
{
   int i;
 
   for(i=0; i<6; i++){    
      fpga_msg.addr = ADDR_GPS_TIME + i;
      fpga_msg.rx_val = 0;
      ioctl(fd, EMIFA_READ, &fpga_msg);
	  
	  printf("%d-", fpga_msg.rx_val);
   }
   printf("\n");

   return 0;
}

unsigned int fpga_read_config(int fd)
{
   fpga_msg.addr = ADDR_CONFIG;
   fpga_msg.rx_val = 0;
   ioctl(fd, EMIFA_READ, &fpga_msg);
   
   return fpga_msg.rx_val;
}

void fpga_write_config(int fd, int val)
{
   fpga_msg.addr = ADDR_CONFIG;
   fpga_msg.tx_val = val;
   ioctl(fd, EMIFA_WRITE, &fpga_msg);
}


