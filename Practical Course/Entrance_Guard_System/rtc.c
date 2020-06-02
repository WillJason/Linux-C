/*
 *	verion 1.0 by rty 20180717
 *  code apply to iTOP Series Development-Board :
 *	iTOP-4412(elite,supper),iTOP-4418,iTOP-6818,iTOP-iMX6Q/D,iTOP-IMX6UL
*/

#include <stdio.h>
#include <stdlib.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

void rtc_record(char *rtc_dev,char *FILE)
{
	int fd, retval;
	struct rtc_time rtc_tm;
	const char *rtc = rtc_dev;
	FILE *time_stream;
	
	fd = open(rtc, O_RDONLY);
	if (fd == -1){
		perror(rtc);
		exit(errno);
	}
	fprintf(stderr, "\n\t\t\tRTC Driver Test Example.\n\n");

	/* Read the RTC time/date */
	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
	if (retval == -1) {
		perror("RTC_RD_TIME ioctl");
		exit(errno);
	}
	close(fd);
	
	fprintf(stderr, "RTC date/time: %d/%d/%d %02d:%02d:%02d\n",
			rtc_tm.tm_mday, rtc_tm.tm_mon+1, rtc_tm.tm_year + 1900,
			rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
	
	time_stream = fopen(FILE,"a+");
	fprintf(time_stream,"%d/%d/%d %02d:%02d:%02d\n",
			rtc_tm.tm_mday, rtc_tm.tm_mon+1, rtc_tm.tm_year + 1900,
			rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);		
	fclose(time_stream);
	
}



