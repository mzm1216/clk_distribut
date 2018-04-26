#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include "fpga_ctl.h"
#include "data_trap.h"


CLOCK_PARA *  cdd_get_clk_param()
{
	return (&ClockParameter);
}

int get_arm_version(char *buffer)
{
	int ret;

	if (buffer == NULL) {
		printf("point is null!");
		return -1;
	}

	ret = sprintf(buffer, "%d.%d.%d.%d%d%d_%d", ArmVersion.bVerMajor,
			ArmVersion.bVerMinor, ArmVersion.bVerRevision,
			ArmVersion.wVerBuildYear, ArmVersion.bVerBuildMonth,
			ArmVersion.bVerBuildDay, ArmVersion.bVerBuildId);
	return ret;
}


int get_hardware_version(char *buffer)
{
	int ret;

	if (buffer == NULL) {
		printf("point is null!");
		return -1;
	}

	ret = sprintf(buffer, "%d.%d.%d.%d%d%d_%d", 1, 0, 0, 2018, 04, 12, 0);
	return ret;
}




