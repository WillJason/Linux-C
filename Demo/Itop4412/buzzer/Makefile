#CC = /usr/local/arm/4.3.2/bin/arm-linux-gcc
#CFLAGS = -I/usr/local/arm/4.3.2/arm-none-linux-gnueabi/libc/usr/include

CC = /usr/local/arm/arm-2009q3/bin/arm-none-linux-gnueabi-gcc
CFLAGS = -I/usr/local/arm/arm-2009q3/arm-none-linux-gnueabi/libc/usr/include

buzzertest: buzzertest.c
	$(CC) $(CFLAGS) -o buzzertest buzzertest.c -static
clean:
	rm -rf buzzertest
