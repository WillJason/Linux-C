#ifndef RFIDMAIN_H
#define RFIDMAIN_H

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>

#include "spidev.h"
#include "rc522.h"

//#define Device_RTC "/dev/rtc0"
#define Device_RTC "/dev/rtc1"
#define Device_RC522 "/dev/rc522"
#define Device_LEDS "/dev/leds"
#define Device_BUZZER "/dev/buzzer_ctl"
#define Device_RELAY "/dev/relay_ctl"

#define FILE "testwr.txt"

#define led_0 0
#define led_1 1
#define Led_On 0
#define Led_Off 1

#define Buzzer_Off 0
#define Buzzer_On 1

#define Relay_Off 0
#define Relay_On 1

#endif