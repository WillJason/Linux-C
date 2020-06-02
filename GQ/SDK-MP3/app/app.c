/*
����mini2440�����壬���������Ͱ���ʵ��MP3���ܣ�Ҫ������
1.����1:���ţ���ͣ
2.����2:ֹͣ
3.����3:��һ��
4.����4:��һ��
��Ҫ�漰4���ֵ��������������
1.����������ֲ
2.����������ֲ
3.madplay��������ֲ
4.mp3���Ŵ������
��Ҫ�漰֪ʶ�㣺
˫��ѭ������
�ź�
�����ڴ�
�����
IO��·����
����
�ַ���ƴ��
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/*�����ڴ�������*/
#define PERM S_IRUSR|S_IWUSR													

/*˫��ѭ���б���Ÿ�����*/
struct song				
{
	char mp3_name[20];
	struct song *prev;
	struct song *next;
};

/*���ӽ���id��*/
pid_t gradchild;

/*�ӽ���id��*/
pid_t pid;

/*�����ڴ��������*/
int shmid;

char *p_addr;

/*���ű��*/
int first_press=1; //1 -�״ΰ��£�0 -���״ΰ��� 
int play_state = 0;//0 -��ͣ״̬��1 -����״̬��

/*************************************************
Function name: play_song
Parameter    : struct song *
Description	 : ���ź���
Return		 : void
Argument     : void
**************************************************/
void play_song(struct song *current_song)
{
	pid_t fd;
	char *c_addr;
	char *p;
	int len;
	char my_song[30]="/mp3/song/";
	while(current_song)
	{
		/*�����ӽ��̣������ӽ���*/
		fd = fork();
		if(fd == -1)
		{	
			perror("fork");
			exit(1);
		}
		else if(fd == 0)
		{
			/*�Ѹ��������ϸ�·��*/
			strcat(my_song,current_song->mp3_name);
			p = my_song;
			len = strlen(p);

			/*ȥ���ļ�������'\n'*/
			my_song[len-1]='\0';

			printf("THIS SONG IS %s\n",my_song);
			execl("/mp3/madplay","madplay",my_song,NULL);//ִ�гɹ��������أ�ִ��ʧ�ܷ���-1
			printf("\n\n\n");
		}
		else
		{
			/*�ڴ�ӳ��*/
			c_addr = shmat(shmid,0,0);

			/*�����ӽ��̵�id�͵�ǰ���Ÿ����Ľڵ�ָ�봫�빲���ڴ�*/
			memcpy(c_addr,&fd,sizeof(pid_t));
			memcpy(c_addr + sizeof(pid_t)+1,&current_song,4);
			/*ʹ��wait�������ӽ��̣�ֱ�����ӽ��̲�������ܱ����ѣ�
			  ��������ʱ����ʾ����MP3�ڼ�û�а������£������˳�򲥷���һ��MP3*/
			if(fd == wait(NULL))
			{
				current_song = current_song->next;
				printf("THE NEXT SONG IS %s\n",current_song->mp3_name);
			}
		}
	}
}

/*************************************************
Function name: creat_mp3_list
Parameter    : void
Description	 : ������������˫��ѭ������
Return		 : struct song *
Argument     : void
**************************************************/
struct song *creat_mp3_list(void)
{	
	FILE *fd;
	size_t size;
	size_t len;
	char *line = NULL;
	struct song *head;
	struct song *p1;
	struct song *p2;
	system("ls /mp3/song >song_list");
	fd = fopen("song_list","r");

	p1 = (struct song *)malloc(sizeof(struct song));

	printf("==================================song list=====================================\n");
	system("ls /mp3/song");	
	printf("\n");
	printf("================================================================================\n");
	size = getline(&line,&len,fd);

	strncpy(p1->mp3_name,line,strlen(line));
	head = p1;
	while((size = getline(&line,&len,fd)) != -1)
	{	
		p2 = p1;
		p1 = (struct song *)malloc(sizeof(struct song));
		strncpy(p1->mp3_name,line,strlen(line));
		p2->next = p1;
		p1->prev = p2;	
	}
	p1->next = head;
	head->prev = p1;
	p1 = NULL;
	p2 = NULL;
	system("rm -rf song_list");
	return head;
}
/*************************************************
Function name: play_Prepare_Run
Parameter    : pid_t *��struct song *
Description	 : ��ʼ���ź���
Return		 : void
Argument     : void
**************************************************/
void play_Prepare_Run(pid_t *childpid,struct song *my_song)
{
	pid_t pid;
	int ret;
	/*�����ӽ���*/
	pid = fork();

	if(pid > 0)
	{
		*childpid = pid;
		play_state = 1;
		sleep(1);
		/*�����ӽ��̵�pid����������*/
		memcpy(&gradchild,p_addr,sizeof(pid_t));
	}
	else if(0 == pid)
	{	
		/*�ӽ��̲���MP3����*/
		play_song(my_song);
	}
}
/*************************************************
Function name: song_pause
Parameter    : pid_t
Description	 : ��ͣ����
Return		 : void
Argument     : void
**************************************************/
void song_pause(pid_t pid)
{
	printf("=======================PAUSE!PRESS K1 TO CONTINUE===================\n");
	kill(pid,SIGSTOP); //�����ӽ��̷���SKGSTOP�ź�
	play_state = 0;
}

/*************************************************
Function name: song_stop
Parameter    : pid_t
Description	 : ֹͣ���ź���
Return		 : void
Argument     : void
**************************************************/
void song_stop(pid_t g_pid)
{

	printf("=======================STOP!PRESS K1 TO START PLAY===================\n");
	kill(g_pid,SIGKILL); //�����ӽ��̷���SKGKILL�ź�
	kill(pid,SIGKILL);   //���ӽ��̷���SKGKILL�ź�
	first_press=1;

}

/*************************************************
Function name: continue_play
Parameter    : pid_t
Description	 : ��������
Return		 : void
Argument     : void
**************************************************/
void continue_play(pid_t pid)
{
	printf("===============================CONTINUE=============================\n");
	kill(pid,SIGCONT); //�����ӽ��̷���SIGCONT�ź�
	play_state=1;
}

/*************************************************
Function name: next_song
Parameter    : pid_t
Description	 : ��һ�׺���
Return		 : void
Argument     : void
**************************************************/
void next_song(pid_t next_pid)
{
	struct song *nextsong;

	printf("===============================NEXT MP3=============================\n");
	/*�ӹ����ڴ������ӽ��̲��Ÿ����Ľڵ�ָ��*/
	memcpy(&nextsong,p_addr + sizeof(pid_t)+1,4);
	/*ָ�����׸����Ľڵ�*/
	nextsong = nextsong->next;
	/*ɱ����ǰ�������ŵ��ӽ��̣����ӽ���*/
	kill(pid,SIGKILL);
	kill(next_pid,SIGKILL);
	wait(NULL);
	play_Prepare_Run(&pid,nextsong);
}

/*************************************************
Function name: prev_song
Parameter    : pid_t
Description	 : ��һ�׺���
Return		 : void
Argument     : void
**************************************************/
void prev_song(pid_t prev_pid)
{
	struct song *prevsong;
	/*�ӹ����ڴ������ӽ��̲��Ÿ����Ľڵ�ָ��*/
	printf("===============================PRIOR MP3=============================\n");
	memcpy(&prevsong,p_addr + sizeof(pid_t)+1,4);
	/*ָ�����׸����Ľڵ�*/
	prevsong = prevsong->prev;
	/*ɱ����ǰ�������ŵ��ӽ��̣����ӽ���*/
	kill(pid,SIGKILL);
	kill(prev_pid,SIGKILL);
	wait(NULL);
	play_Prepare_Run(&pid,prevsong);
}

/*************************************************
Function name: main
Parameter    : void
Description	 : ������
Return		 : int
Argument     : void
**************************************************/
int main(void)
{
	int buttons_fd;
	int key_value;
	struct song *head;
	/*���豸�ļ�*/
	buttons_fd = open("/dev/buttons", 0);
	if (buttons_fd < 0) {
		perror("open device buttons");
		exit(1);
	}


  /*���������б�*/
	head = creat_mp3_list();
	printf("===================================OPTION=======================================\n\n\n\n");
	printf("        K1:START/PAUSE     K2:STOP   K3:NEXT      K4:PRIOR\n\n\n\n");
	printf("================================================================================\n");


  /*�����ڴ棺���ڴ���ӽ���ID�������б�λ��*/
	if((shmid = shmget(IPC_PRIVATE,5,PERM))== -1) //�����µĹ����ڴ����
		exit(1);
	//���ӹ����ڴ��ʶ��Ϊshmid�Ĺ����ڴ棬���ӳɹ���ѹ����ڴ�������ӳ�䵽���ý��̵ĵ�ַ�ռ�
	p_addr = shmat(shmid,0,0);
	memset(p_addr,'\0',1024);
	
	
	while(1) 
	{
		fd_set rds;
		int ret;

		FD_ZERO(&rds);
		FD_SET(buttons_fd, &rds);

		/*������ȡ��ֵ*/
		ret = select(buttons_fd + 1, &rds, NULL, NULL, NULL);//�������أ���ѯ
		if (ret < 0) 
		{
			perror("select");
			exit(1);
		}
		if (ret == 0) 
			printf("Timeout.\n");
		else if (FD_ISSET(buttons_fd, &rds))//���а�������ʱ
		{
			int ret = read(buttons_fd, &key_value, sizeof key_value);
			if (ret != sizeof key_value) 
			{
				if (errno != EAGAIN)
					perror("read buttons\n");
				continue;
			} 
			else
			{
				//printf("buttons_value: %d\n", key_value+1);
				
				/*�״β��ţ������ǰ���1*/
				if(first_press){
					switch(key_value)
					{	
					case 0:
						play_Prepare_Run(&pid,head);
						first_press=0;
						break;
					case 1:
					case 2:
					case 3:
						printf("=======================PRESS K1 TO START PLAY===================\n");
						break;
				    default:
						printf("=======================PRESS K1 TO START PLAY===================\n");
						break;
					} //end switch
				}//end if(first_press)
				/*�������״β��ţ�����ݲ�ͬ��ֵ����*/
				else if(!first_press){
				    switch(key_value)
					{
					case 0:
						//printf("play_state:%d\n",play_state);
						if(play_state)
							song_pause(gradchild);
						else
							continue_play(gradchild);
						break;
					case 1:
						song_stop(gradchild);
						break;
					case 2:
						next_song(gradchild);
						break;
					case 3:
						prev_song(gradchild);
						break;
					} //end switch
			 }//end if(!first_press)

			}
				
		}
	}

	close(buttons_fd);
	return 0;
}
































