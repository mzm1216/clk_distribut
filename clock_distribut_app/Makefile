#CC = arm-arago-linux-gnueabi-gcc
CC = arm-none-linux-gnueabi-gcc
CFALGS = -c -Wall -O2

clock_distribut: main.o fpga_ctl.o freeLockQ.o snmp.o data_trap.o
	$(CC) main.o fpga_ctl.o  freeLockQ.o snmp.o data_trap.o -lcstl -lpthread -lrt -o clock_distribut -Wall -O2
main.o: main.c main.h 
	$(CC) main.c $(CFALGS)
fpga_ctl.o: fpga_ctl.c fpga_ctl.h
	$(CC) fpga_ctl.c $(CFALGS)
snmp.o: snmp.c snmp.h 
	$(CC) snmp.c $(CFALGS)
freeLockQ.o: freeLockQ.c freeLockQ.h
	$(CC) freeLockQ.c $(CFALGS)

data_trap.o:data_trap.c data_trap.h
		$(CC) data_trap.c $(CFALGS)

	
.PHONY:
	clean

clean:
	rm clock_distribut *.o
