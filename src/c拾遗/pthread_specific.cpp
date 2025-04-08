#include<stdio.h>  
#include<pthread.h>  
#include<string.h>
#include <unistd.h>


struct Data {
    int *data {nullptr};
    pthread_key_t *pkey {nullptr};
    pthread_t *tid {nullptr};
};

void func1(void *args)  
{  
        void *tmp = pthread_getspecific(*((Data *)args)->pkey);//同一线程内的各个函数间共享数据。  
        printf("%d is runing in %s\n", *((Data *)tmp)->data,__func__);  
   
}  
void *thread_func(void *args)  
{  

        pthread_setspecific(*((Data *)args)->pkey, args);
        int d = *((Data *)args)->data;
        void *tmp = pthread_getspecific(*(pthread_key_t *)((Data *)args)->pkey);//获得线程的私有空间
        d = *((Data *)tmp)->data;
        printf("%d is runing in %s\n",d,__func__);  
   
        d = (d)*100;//修改私有变量的值  
   
        printf("%ld TID %s\n",*(unsigned long int *)((Data *)tmp)->tid,__func__);  
        pthread_cancel(*(pthread_t *)((Data *)tmp)->tid);  // pthread_cancel 也可以触发 pthread_key_create 绑定的 deconstruct 回调
        sleep(d);
        func1(args);  
   
        return (void*)0;  
}

void Deconstruct(void *arg)
{
    printf("%s  %d  %p\n", __FUNCTION__, *((Data *)arg)->data, arg);
    delete (int *)((Data *)arg)->data;
    ((Data *)arg)->data = nullptr;
    // delete (pthread_key_t *)((Data *)arg)->pkey; // 线程A不能私自释放 pthread_key_t，有其他线程还需要使用，
    // ((Data *)arg)->pkey = nullptr;               //pthread_key_t可以采用栈上变量，或者使用线程安全的引用计数，控制 pthread_key_t 的释放。
    delete (Data *)arg;
    arg = nullptr;
}

int main()  
{  
        pthread_key_t *p_key = new pthread_key_t;  // 当前代码 p_key 内存泄露。可采用引用计数释放。
        pthread_t pa, pb;  
        int a=2;  
        int b=1;
        Data *d1 = new Data();
        d1->data = new int(a);
        d1->pkey = p_key;
        d1->tid = &pa;
        Data *d2 = new Data{new int(b), p_key, &pb};
        printf("%s  %d  %p\n", __FUNCTION__, *(d1->data), d1);
        printf("%s  %d  %p\n", __FUNCTION__, *(d2->data), d2);
        pthread_key_create(p_key, Deconstruct);  
        pthread_create(&pa, NULL, thread_func, d1);  
        pthread_create(&pb, NULL, thread_func, d2);  
        pthread_join(pa, NULL);  
        pthread_join(pb, NULL);  
        return 0;  
}  