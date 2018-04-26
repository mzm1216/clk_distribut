#ifndef MPU_LED_H
#define MPU_LED_H

#define MPU_LED_MAGIC 'L'

#define MPU_LED_ON   _IOW(MPU_LED_MAGIC, 0, int)
#define MPU_LED_OFF  _IOW(MPU_LED_MAGIC, 1, int)

#define FPGA_CONFIGn_SET  _IOW(MPU_LED_MAGIC, 2, int)
#define FPGA_CONFIGn_CLR  _IOW(MPU_LED_MAGIC, 3, int)
#define FPGA_CEn_SET  _IOW(MPU_LED_MAGIC, 4, int)
#define FPGA_CEn_CLR  _IOW(MPU_LED_MAGIC, 5, int)

#define NETCLK_GPIO_SET  _IOW(MPU_LED_MAGIC, 6, int)
#define NETCLK_GPIO_CLR  _IOW(MPU_LED_MAGIC, 7, int)

#define FPGA_RST_SET  _IOW(MPU_LED_MAGIC, 8, int)
#define FPGA_RST_CLR  _IOW(MPU_LED_MAGIC, 9, int)
#endif
