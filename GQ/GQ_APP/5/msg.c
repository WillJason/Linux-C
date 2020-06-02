#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>

struct msg_buf
    {
        int mtype;
        char data[255];
    };
 
int main()
{
        key_t key;
        int msgid;
        int ret;
        struct msg_buf msgbuf;
 
        key=ftok("/tmp/2",'a');
        printf("key =[%x]\n",key);
        msgid=msgget(key,IPC_CREAT|0666); /*通过文件对应*/

        if(msgid==-1)
        {
                printf("create error\n");
                return -1;
        }
 
        msgbuf.mtype = getpid();
        strcpy(msgbuf.data,"test haha");
        ret=msgsnd(msgid,&msgbuf,sizeof(msgbuf.data),IPC_NOWAIT);
        if(ret==-1)
        {
                printf("send message err\n");
                return -1;
        }
 
        memset(&msgbuf,0,sizeof(msgbuf));
        ret=msgrcv(msgid,&msgbuf,sizeof(msgbuf.data),getpid(),IPC_NOWAIT);
        if(ret==-1)
        {
                printf("recv message err\n");
                return -1;
        }
        printf("recv msg =[%s]\n",msgbuf.data);
 
}
