#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

extern unsigned char UID[5];

void data_record(char *FILE)
{
	FILE *time_stream;
	int i;
	
	fprintf(stderr,"data_record:%x%x%x%x\n",
		UID[0],UID[1],UID[2],UID[3]);
	
	time_stream = fopen(FILE,"a+");
	fprintf(time_stream,":%x%x%x%x\n",
			UID[0],UID[1],UID[2],UID[3]);		
	fclose(time_stream);
}