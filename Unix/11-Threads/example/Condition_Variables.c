/*
�����һ����������ϣ��Է�ֹ����߳�ͬʱ����pthread_cond_wait()����pthread_cond_timedwait()��
��ͬ���ľ���������Race ? Condition����mutex��������������ͨ����PTHREAD_MUTEX_TIMED_NP��������Ӧ
����PTHREAD_MUTEX_ADAPTIVE_NP�������ڵ���pthread_cond_wait()ǰ�����ɱ��̼߳�����pthread_mutex_lock()��
�����ڸ��������ȴ�������ǰ��mutex��������״̬�������̹߳������ȴ�ǰ����������������Ӷ��뿪
pthread_cond_wait()֮ǰ��mutex�������¼������������pthread_cond_wait()ǰ�ļ���������Ӧ�� 

*/

#include <pthread.h>
#include <unistd.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct node {
int n_number;
struct node *n_next;
} *head = NULL;

/*[thread_func]*/
static void cleanup_handler(void *arg)
{
    printf("Cleanup handler of second thread./n");
    free(arg);
    (void)pthread_mutex_unlock(&mtx);
}
static void *thread_func(void *arg)
{
    struct node *p = NULL;

    pthread_cleanup_push(cleanup_handler, p);
    while (1) {
    pthread_mutex_lock(&mtx);           //���mutex��Ҫ��������֤pthread_cond_wait�Ĳ�����
    while (head == NULL)   {               //���whileҪ�ر�˵��һ�£�����pthread_cond_wait���ܺ����ƣ�Ϊ������Ҫ��һ��while (head == NULL)�أ���Ϊpthread_cond_wait����߳̿��ܻᱻ���⻽�ѣ�������ʱ��head != NULL������������Ҫ����������ʱ��Ӧ�����̼߳�������pthread_cond_wait
        pthread_cond_wait(&cond, &mtx);         // pthread_cond_wait���Ƚ��֮ǰ��pthread_mutex_lock������mtx��Ȼ�������ڵȴ����������ߣ�ֱ���ٴα����ѣ������������ǵȴ������������������ѣ����Ѻ󣬸ý��̻���������pthread_mutex_lock(&mtx);���ٶ�ȡ��Դ
                                                //����������ǱȽ������/*block-->unlock-->wait() return-->lock*/
    }
        p = head;
        head = head->n_next;
        printf("Got %d from front of queue/n", p->n_number);
        free(p);
        pthread_mutex_unlock(&mtx);             //�ٽ������ݲ�����ϣ��ͷŻ�����
    }
    pthread_cleanup_pop(0);
    return 0;
}

int main(void)
{
    pthread_t tid;
    int i;
    struct node *p;
    pthread_create(&tid, NULL, thread_func, NULL);   //���̻߳�һֱ�ȴ���Դ�����������ߺ������ߣ���������������߿����Ƕ�������ߣ���������֧����ͨ�ĵ��������ߣ����ģ����Ȼ�򵥣����Ǻ�ǿ��
    /*[tx6-main]*/
    for (i = 0; i < 10; i++) {
        p = malloc(sizeof(struct node));
        p->n_number = i;
        pthread_mutex_lock(&mtx);             //��Ҫ����head����ٽ���Դ���ȼ�����
        p->n_next = head;
        head = p;
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mtx);           //����
        sleep(1);
    }
    printf("thread 1 wanna end the line.So cancel thread 2./n");
    pthread_cancel(tid);             //����pthread_cancel����һ������˵�������Ǵ��ⲿ��ֹ���̣߳����̻߳��������ȡ���㣬�˳��̣߳��������ǵĴ���������ȡ����϶�����pthread_cond_wait()�ˡ�����ȡ�������Ϣ������Ȥ����google,���ﲻ��˵��
    pthread_join(tid, NULL);
    printf("All done -- exiting/n");
    return 0;
}
