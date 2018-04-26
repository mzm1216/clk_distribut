#ifndef FPGA_CTL_H
#define FPGA_CTL_H

#define EMIFA_CONTROL_MAGIC 'E'

struct emifa_msg{
   unsigned int addr;
   unsigned int tx_val;
   unsigned int rx_val;
};

struct emifa_msg fpga_msg;

#define ADDR_VERSION       0x00
#define ADDR_WORK_STATUS   0x01
#define ADDR_CONFIG        0x02
#define ADDR_RESERVE       0x03
#define ADDR_FPGA_TIME     0x04
#define ADDR_PTP_TIME      0x0A
#define ADDR_GPS_TIME      0x10
#define ADDR_RS422_TIME    0x16
#define ADDR_LOGIC_STATUS  0x1C
#define ADDR_GPDEI         0x1D
#define ADDR_DACC1I        0x1E
#define ADDR_DACC2I        0x1F
#define ADDR_EPR_RDDATAI   0x20

#define TIME_DATA_LENGTH   6

#define EMIFA_WRITE  _IOW(EMIFA_CONTROL_MAGIC, 0, struct emifa_msg)
#define EMIFA_READ   _IOR(EMIFA_CONTROL_MAGIC, 1, struct emifa_msg)

extern void FPGA_enable(int fd);
extern void FPGA_disable(int fd);
extern unsigned int fpga_read_version(int fd);
extern void fpga_rdwr_config(int fd, unsigned int val);
extern unsigned int fpga_read_work_status(int fd);
extern unsigned int fpga_read_logic_status(int fd);
extern unsigned int fpga_read_fpga_time(int fd, int offset);
extern void fpga_write_ptp_time(int fd, int *buff, int offset);
extern unsigned int fpga_read_ptp_time(int fd, int offset);
extern unsigned int fpga_read_config(int fd);
extern void fpga_write_config(int fd, int val);

#endif
