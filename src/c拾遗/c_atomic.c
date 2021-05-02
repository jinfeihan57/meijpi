/******************************************************************
*
* 采用build函数实现c语言数据的原子操作，实现C语言的无锁队列的基础
*
*******************************************************************/


#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// oid __atomic_store (type *ptr, type *val, int memorder);
// type __atomic_add_fetch (type *ptr, type val, int memorder);
// type __atomic_fetch_add (type *ptr, type val, int memorder);
// void __atomic_exchange (type *ptr, type *val, type *ret, int memorder);

#define NUM_THREADS 100
int a = 10;
int b = 20;
int c = 3;

void * fun(void * argv){
	for(int i = 0; i < 1000000; i++){
		//a = a + 1;
		__atomic_add_fetch(&a, 1, __ATOMIC_RELAXED);
	}
}

int main(){
	
	pthread_t tids[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        //参数依次是：创建的线程id，线程参数，调用的函数，传入的函数参数
        int ret = pthread_create(&tids[i], NULL, fun, NULL);
        if (ret != 0) {
            printf("pthread_create error: error_code = %d\n", ret);
        }
    }

    //等各个线程退出后，进程才结束，否则进程强制结束了，线程可能还没反应过来；
    //pthread_exit(NULL);
	
	for(int i=0; i < NUM_THREADS; i++ ){
      int rc = pthread_join(tids[i], NULL);
      if (rc){
		 printf("Error:unable to join error: error_code = %d\n", rc);
         exit(-1);
      }

   }
	
/* 	__atomic_store(&a, &b, __ATOMIC_RELAXED);
	
	__atomic_add_fetch(&a, 1, __ATOMIC_RELAXED); */
	
	__atomic_fetch_add(&b, 2, __ATOMIC_RELAXED);

    int ret = 0;
    __atomic_exchange(&b, &c, &ret, __ATOMIC_RELAXED);
    __atomic_exchange(&b, &c, &c, __ATOMIC_RELAXED);
	
	printf("%d\n", a);
	printf("%d\n", b);
	printf("%d\n", c);
	printf("%d\n", ret);
	
	
}