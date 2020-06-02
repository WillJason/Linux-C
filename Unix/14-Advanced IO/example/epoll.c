/*
select��ȱ��

		�߲����ĺ��Ľ��������1���̴߳����������ӵġ��ȴ���Ϣ׼���á�����һ����epoll��select��������ġ�
	��selectԤ��������һ���£�����ʮ�򲢷����Ӵ���ʱ������ÿһ����ֻ�����ٸ���Ծ�����ӣ�ͬʱ������ʮ������
	����һ�����Ƿǻ�Ծ�ġ�select��ʹ�÷����������ģ�
  ���صĻ�Ծ���� ==select��ȫ������ص����ӣ���
  ʲôʱ������select�����أ�������Ϊ��Ҫ�ҳ��б��ĵ���Ļ�Ծ����ʱ����Ӧ�õ��á����ԣ�����select�ڸ߲���ʱ��
  �ᱻƵ�����õġ����������Ƶ�����õķ����ͺ��б�Ҫ�������Ƿ���Ч�ʣ���Ϊ��������΢Ч����ʧ���ᱻ��Ƶ��������
  ���Ŵ�����Ч����ʧ���Զ��׼���ȫ�����������������ʮ��Ƶģ����ص�ֻ�����ٸ���Ծ���ӣ��Ȿ�������Ч��
  �ı��֡����Ŵ��ͻᷢ�֣����������������ʱ��select����ȫ���������ˡ�
  ���⣬��Linux�ں��У�select���õ���FD_SET�����޵ģ����ں����и�����__FD_SETSIZE������ÿ��FD_SET�ľ��������
  
   ��Σ��ں���ʵ�� select������ѯ��������ÿ�μ�ⶼ���������FD_SET�еľ������Ȼ��select����ִ��ʱ����
   FD_SET�еľ��������һ��������ϵ���� selectҪ���ľ����Խ��ͻ�Խ��ʱ���������������ҪҪ���ˣ�
   ��Ϊʲô����poll��������Ϊselect��poll���ڲ����Ʒ��沢û��̫��Ĳ��졣�����select���ƣ�pollֻ��ȡ����
   ������ļ������������ƣ���û�дӸ����Ͻ��select���ڵ����⡣
       
   ���������ǿ���ͼ������������Ϊ��Сʱ��select��epoll�ƺ����޶��ٲ�ࡣ���ǵ��������������Ժ�select���Ե�
   ���������ˡ�
   
   epoll�Ľӿڷǳ��򵥣�һ��������������
1. int epoll_create(int size);
����һ��epoll�ľ����size���������ں������������Ŀһ���ж�����������ͬ��select()�еĵ�һ��������������������fd+1��ֵ����Ҫע����ǣ���������epoll����������ǻ�ռ��һ��fdֵ����linux������鿴/proc/����id/fd/�����ܹ��������fd�ģ�������ʹ����epoll�󣬱������close()�رգ�������ܵ���fd���ľ���


2. int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
epoll���¼�ע�ắ��������ͬ��select()���ڼ����¼�ʱ�����ں�Ҫ����ʲô���͵��¼���������������ע��Ҫ�������¼����͡���һ��������epoll_create()�ķ���ֵ���ڶ���������ʾ������������������ʾ��
EPOLL_CTL_ADD��ע���µ�fd��epfd�У�
EPOLL_CTL_MOD���޸��Ѿ�ע���fd�ļ����¼���
EPOLL_CTL_DEL����epfd��ɾ��һ��fd��
��������������Ҫ������fd�����ĸ������Ǹ����ں���Ҫ����ʲô�£�struct epoll_event�ṹ���£�

 typedef union epoll_data {
     void *ptr;
    int fd;
      __uint32_t u32;
      __uint64_t u64;
  } epoll_data_t;
  
  struct epoll_event {
      __uint32_t events;  Epoll events *
     epoll_data_t data; * User data variable *

3. int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
�ȴ��¼��Ĳ�����������select()���á�����events�������ں˵õ��¼��ļ��ϣ�maxevents��֮�ں���
��events�ж����� maxevents��ֵ���ܴ��ڴ���epoll_create()ʱ��size������timeout�ǳ�ʱʱ��
�����룬0���������أ�-1����ȷ����Ҳ��˵��˵���������������ú���������Ҫ������¼���Ŀ���緵��0��ʾ�ѳ�ʱ��

����epoll������ETģʽ��ʱ�����ڶ����������readһ��û�ж���buffer�е����ݣ���ô�´ν��ò�����������֪ͨ�����buffer�����е������޻���������������µ������ٴε������д��������Ҫ����ΪETģʽ��fdͨ��Ϊ��������ɵ�һ�����⡪����α�֤���û�Ҫ��д������д�ꡣ

  Ҫ�����������ETģʽ�µĶ�д���⣬���Ǳ���ʵ�֣�
	���ڶ���ֻҪbuffer�л������ݾ�һֱ����
	����д��ֻҪbuffer���пռ����û�����д�����ݻ�δд�꣬��һֱд��
 ETģʽ�µ�accept����

  ��˼������һ�ֳ�������ĳһʱ�̣��ж������ͬʱ����������� TCP ��������˲����۶���������ӣ�
  �����Ǳ�Ե����ģʽ��epoll ֻ��֪ͨһ�Σ�accept ֻ����һ�����ӣ����� TCP ����������ʣ�µ����Ӷ��ò�������
   �����������£�����Ӧ�������Ч�Ĵ����أ�

  ����ķ����ǣ�����취���� while ѭ����ס accept ���ã������� TCP ���������е��������Ӻ����˳�ѭ����
  ���֪���Ƿ�������������е����������أ� accept  ���� -1 ���� errno ����Ϊ EAGAIN �ͱ�ʾ�������Ӷ������ꡣ 

  ����ET��accept���⣬��ƪ���ĵĲο���ֵ�ܸߣ��������Ȥ���������ӹ�ȥΧ��һ�¡�

	ETģʽΪʲôҪ�����ڷ�����ģʽ�¹���

  ��ΪETģʽ�µĶ�д��Ҫһֱ����дֱ���������ڶ�����������ʵ���ֽ���С�������ֽ���ʱ�Ϳ���ֹͣ����
  ���������ļ�������������Ƿ������ģ������һֱ����һֱд�Ʊػ������һ�������������Ͳ�����������
  epoll_wait���ˣ���������ļ������������񼢶���

С��

   LT��ˮƽ������Ч�ʻ����ET�����������ڴ󲢷���������������¡�����LT�Դ����дҪ��Ƚϵͣ������׳������⡣
   LTģʽ�����д�ϵı����ǣ�ֻҪ������û�б���ȡ���ں˾Ͳ���֪ͨ�㣬��˲��õ����¼���ʧ�������

   ET����Ե������Ч�ʷǳ��ߣ��ڲ�����������������£����LT�ٺܶ�epoll��ϵͳ���ã����Ч�ʸߡ����ǶԱ��Ҫ��ߣ�
   ��Ҫϸ�µĴ���ÿ�����󣬷������׷�����ʧ�¼��������
   �ӱ����Ͻ�����LT��ȣ�ETģ����ͨ������ϵͳ�������ﵽ��߲���Ч�ʵġ�
*/
//����һ�������ķ�����������
#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

using namespace std;

#define MAXLINE 5
#define OPEN_MAX 100
#define LISTENQ 20
#define SERV_PORT 5000
#define INFTIM 1000

void setnonblocking(int sock)
{
    int opts;
    opts=fcntl(sock,F_GETFL);
    if(opts<0)
    {
        perror("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(sock,F_SETFL,opts)<0)
    {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    int i, maxi, listenfd, connfd, sockfd,epfd,nfds, portnumber;
    ssize_t n;
    char line[MAXLINE];
    socklen_t clilen;


    if ( 2 == argc )
    {
        if( (portnumber = atoi(argv[1])) < 0 )
        {
            fprintf(stderr,"Usage:%s portnumber/a/n",argv[0]);
            return 1;
        }
    }
    else
    {
        fprintf(stderr,"Usage:%s portnumber/a/n",argv[0]);
        return 1;
    }



    //����epoll_event�ṹ��ı���,ev����ע���¼�,�������ڻش�Ҫ������¼�

    struct epoll_event ev,events[20];
    //�������ڴ���accept��epollר�õ��ļ�������

    epfd=epoll_create(256);
    struct sockaddr_in clientaddr;
    struct sockaddr_in serveraddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    //��socket����Ϊ��������ʽ

    //setnonblocking(listenfd);

    //������Ҫ������¼���ص��ļ�������

    ev.data.fd=listenfd;
    //����Ҫ������¼�����

    ev.events=EPOLLIN|EPOLLET;
    //ev.events=EPOLLIN;

    //ע��epoll�¼�

    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    char *local_addr="127.0.0.1";
    inet_aton(local_addr,&(serveraddr.sin_addr));//htons(portnumber);

    serveraddr.sin_port=htons(portnumber);
    bind(listenfd,(sockaddr *)&serveraddr, sizeof(serveraddr));
    listen(listenfd, LISTENQ);
    maxi = 0;
    for ( ; ; ) {
        //�ȴ�epoll�¼��ķ���

        nfds=epoll_wait(epfd,events,20,500);
        //�����������������¼�

        for(i=0;i<nfds;++i)
        {
            if(events[i].data.fd==listenfd)//����¼�⵽һ��SOCKET�û����ӵ��˰󶨵�SOCKET�˿ڣ������µ����ӡ�

            {
                connfd = accept(listenfd,(sockaddr *)&clientaddr, &clilen);
                if(connfd<0){
                    perror("connfd<0");
                    exit(1);
                }
                //setnonblocking(connfd);

                char *str = inet_ntoa(clientaddr.sin_addr);
                cout << "accapt a connection from " << str << endl;
                //�������ڶ��������ļ�������

                ev.data.fd=connfd;
                //��������ע��Ķ������¼�

                ev.events=EPOLLIN|EPOLLET;
                //ev.events=EPOLLIN;

                //ע��ev

                epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);
            }
            else if(events[i].events&EPOLLIN)//������Ѿ����ӵ��û��������յ����ݣ���ô���ж��롣

            {
                cout << "EPOLLIN" << endl;
                if ( (sockfd = events[i].data.fd) < 0)
                    continue;
                if ( (n = read(sockfd, line, MAXLINE)) < 0) {
                    if (errno == ECONNRESET) {
                        close(sockfd);
                        events[i].data.fd = -1;
                    } else
                        std::cout<<"readline error"<<std::endl;
                } else if (n == 0) {
                    close(sockfd);
                    events[i].data.fd = -1;
                }
                line[n] = '/0';
                cout << "read " << line << endl;
                //��������д�������ļ�������

                ev.data.fd=sockfd;
                //��������ע���д�����¼�

                ev.events=EPOLLOUT|EPOLLET;
                //�޸�sockfd��Ҫ������¼�ΪEPOLLOUT

                //epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);

            }
            else if(events[i].events&EPOLLOUT) // ��������ݷ���

            {
                sockfd = events[i].data.fd;
                write(sockfd, line, n);
                //�������ڶ��������ļ�������

                ev.data.fd=sockfd;
                //��������ע��Ķ������¼�

                ev.events=EPOLLIN|EPOLLET;
                //�޸�sockfd��Ҫ������¼�ΪEPOLIN

                epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
            }
        }
    }
    return 0;
}