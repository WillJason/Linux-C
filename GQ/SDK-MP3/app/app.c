/*
基于mini2440开发板，利用声卡和按键实现MP3功能，要求如下
1.按键1:播放，暂停
2.按键2:停止
3.按键3:上一曲
4.按键4:下一曲
主要涉及4部分的软件开发工作：
1.声卡驱动移植
2.按键驱动移植
3.madplay播放器移植
4.mp3播放处理程序
主要涉及知识点：
双向循环链表
信号
共享内存
多进程
IO多路复用
链表
字符串拼接
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

/*共享内存申请标记*/
#define PERM S_IRUSR|S_IWUSR													

/*双向循环列表：存放歌曲名*/
struct song				
{
	char mp3_name[20];
	struct song *prev;
	struct song *next;
};

/*孙子进程id号*/
pid_t gradchild;

/*子进程id号*/
pid_t pid;

/*共享内存描述标记*/
int shmid;

char *p_addr;

/*播放标记*/
int first_press=1; //1 -首次按下；0 -非首次按下 
int play_state = 0;//0 -暂停状态；1 -播放状态。

/*************************************************
Function name: play_song
Parameter    : struct song *
Description	 : 播放函数
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
		/*创建子进程，即孙子进程*/
		fd = fork();
		if(fd == -1)
		{	
			perror("fork");
			exit(1);
		}
		else if(fd == 0)
		{
			/*把歌曲名加上根路径*/
			strcat(my_song,current_song->mp3_name);
			p = my_song;
			len = strlen(p);

			/*去掉文件名最后的'\n'*/
			my_song[len-1]='\0';

			printf("THIS SONG IS %s\n",my_song);
			execl("/mp3/madplay","madplay",my_song,NULL);//执行成功将不返回！执行失败返回-1
			printf("\n\n\n");
		}
		else
		{
			/*内存映射*/
			c_addr = shmat(shmid,0,0);

			/*把孙子进程的id和当前播放歌曲的节点指针传入共享内存*/
			memcpy(c_addr,&fd,sizeof(pid_t));
			memcpy(c_addr + sizeof(pid_t)+1,&current_song,4);
			/*使用wait阻塞孙子进程，直到孙子进程播放完才能被唤醒；
			  当被唤醒时，表示播放MP3期间没有按键按下，则继续顺序播放下一首MP3*/
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
Description	 : 创建歌曲名的双向循环链表
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
Parameter    : pid_t *，struct song *
Description	 : 开始播放函数
Return		 : void
Argument     : void
**************************************************/
void play_Prepare_Run(pid_t *childpid,struct song *my_song)
{
	pid_t pid;
	int ret;
	/*创建子进程*/
	pid = fork();

	if(pid > 0)
	{
		*childpid = pid;
		play_state = 1;
		sleep(1);
		/*把孙子进程的pid传给父进程*/
		memcpy(&gradchild,p_addr,sizeof(pid_t));
	}
	else if(0 == pid)
	{	
		/*子进程播放MP3函数*/
		play_song(my_song);
	}
}
/*************************************************
Function name: song_pause
Parameter    : pid_t
Description	 : 暂停函数
Return		 : void
Argument     : void
**************************************************/
void song_pause(pid_t pid)
{
	printf("=======================PAUSE!PRESS K1 TO CONTINUE===================\n");
	kill(pid,SIGSTOP); //对孙子进程发送SKGSTOP信号
	play_state = 0;
}

/*************************************************
Function name: song_stop
Parameter    : pid_t
Description	 : 停止播放函数
Return		 : void
Argument     : void
**************************************************/
void song_stop(pid_t g_pid)
{

	printf("=======================STOP!PRESS K1 TO START PLAY===================\n");
	kill(g_pid,SIGKILL); //对孙子进程发送SKGKILL信号
	kill(pid,SIGKILL);   //对子进程发送SKGKILL信号
	first_press=1;

}

/*************************************************
Function name: continue_play
Parameter    : pid_t
Description	 : 继续函数
Return		 : void
Argument     : void
**************************************************/
void continue_play(pid_t pid)
{
	printf("===============================CONTINUE=============================\n");
	kill(pid,SIGCONT); //对孙子进程发送SIGCONT信号
	play_state=1;
}

/*************************************************
Function name: next_song
Parameter    : pid_t
Description	 : 下一首函数
Return		 : void
Argument     : void
**************************************************/
void next_song(pid_t next_pid)
{
	struct song *nextsong;

	printf("===============================NEXT MP3=============================\n");
	/*从共享内存获得孙子进程播放歌曲的节点指针*/
	memcpy(&nextsong,p_addr + sizeof(pid_t)+1,4);
	/*指向下首歌曲的节点*/
	nextsong = nextsong->next;
	/*杀死当前歌曲播放的子进程，孙子进程*/
	kill(pid,SIGKILL);
	kill(next_pid,SIGKILL);
	wait(NULL);
	play_Prepare_Run(&pid,nextsong);
}

/*************************************************
Function name: prev_song
Parameter    : pid_t
Description	 : 上一首函数
Return		 : void
Argument     : void
**************************************************/
void prev_song(pid_t prev_pid)
{
	struct song *prevsong;
	/*从共享内存获得孙子进程播放歌曲的节点指针*/
	printf("===============================PRIOR MP3=============================\n");
	memcpy(&prevsong,p_addr + sizeof(pid_t)+1,4);
	/*指向上首歌曲的节点*/
	prevsong = prevsong->prev;
	/*杀死当前歌曲播放的子进程，孙子进程*/
	kill(pid,SIGKILL);
	kill(prev_pid,SIGKILL);
	wait(NULL);
	play_Prepare_Run(&pid,prevsong);
}

/*************************************************
Function name: main
Parameter    : void
Description	 : 主函数
Return		 : int
Argument     : void
**************************************************/
int main(void)
{
	int buttons_fd;
	int key_value;
	struct song *head;
	/*打开设备文件*/
	buttons_fd = open("/dev/buttons", 0);
	if (buttons_fd < 0) {
		perror("open device buttons");
		exit(1);
	}


  /*创建播放列表*/
	head = creat_mp3_list();
	printf("===================================OPTION=======================================\n\n\n\n");
	printf("        K1:START/PAUSE     K2:STOP   K3:NEXT      K4:PRIOR\n\n\n\n");
	printf("================================================================================\n");


  /*共享内存：用于存放子进程ID，播放列表位置*/
	if((shmid = shmget(IPC_PRIVATE,5,PERM))== -1) //创建新的共享内存对象
		exit(1);
	//连接共享内存标识符为shmid的共享内存，连接成功后把共享内存区对象映射到调用进程的地址空间
	p_addr = shmat(shmid,0,0);
	memset(p_addr,'\0',1024);
	
	
	while(1) 
	{
		fd_set rds;
		int ret;

		FD_ZERO(&rds);
		FD_SET(buttons_fd, &rds);

		/*监听获取键值*/
		ret = select(buttons_fd + 1, &rds, NULL, NULL, NULL);//立即返回，轮询
		if (ret < 0) 
		{
			perror("select");
			exit(1);
		}
		if (ret == 0) 
			printf("Timeout.\n");
		else if (FD_ISSET(buttons_fd, &rds))//当有按键按下时
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
				
				/*首次播放，必须是按键1*/
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
				/*若不是首次播放，则根据不同键值处理*/
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
































