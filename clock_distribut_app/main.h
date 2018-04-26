#ifndef _MAIN_H_
#define _MAIN_H_
 
#define LED_FILE_NAME       "/dev/led"
#define EMIFA_FILE_NAME     "/dev/emifa_control"
#define TRIGGER_FILE_NAME   "/dev/ptp_trigger"
#define SPI0_FILE_NAME      "/dev/spi0"
#define PTP0_FILE_NAME      "/dev/ptp0"

#define CFG_FILE_NAME       "/etc/clock_distribut.conf"
#define NET_FILE_NAME       "/sys/class/net/eth0/carrier"


#define CFG_FILE_SIZE   10

#define CLOCK_MASTER   0x01
#define CLOCK_SLAVE    0x02
 
#define LED_ON     0x01
#define LED_OFF    0x02

#define FPGA_RST_LOW 0x01
#define FPGA_RST_HIGH 0x02

#define OSC_CLK    0x01
#define FPGA_CLK   0x02

int led_fd;
int emifa_fd;
int trigger_fd;
int spi0_fd;
int ptp0_fd;
clockid_t clkid;

int conf_fd ,net_fd;

#define GPS_CONNECT     0
#define GPS_UNCONNECT   1


#define PTP_VALID    0x2000  //PTP时钟有效指示
#define CLK_GPS      0x4000  //GPS时钟源选择
#define CLK_PTP      0xC000  //PTP时钟源选择
#define CLK_SLAVE    0x0800  //主/从时钟选择
#define OMAP_RUN_LED 0xefff  //omap运行指示亮



#endif
