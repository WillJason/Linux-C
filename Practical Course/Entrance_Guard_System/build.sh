arm-none-linux-gnueabi-gcc -c spi_dev.c -o spi_dev.o
arm-none-linux-gnueabi-gcc -c main.c -o main.o 
arm-none-linux-gnueabi-gcc -c led.c -o led.o 
arm-none-linux-gnueabi-gcc -c sound.c -o sound.o 
arm-none-linux-gnueabi-gcc -c relay.c -o relay.o 
arm-none-linux-gnueabi-gcc -c rtc.c -o rtc.o
arm-none-linux-gnueabi-gcc -c card.c -o card.o

arm-none-linux-gnueabi-gcc -o rc522 spi_dev.o main.o led.o sound.o relay.o rtc.o card.o -static