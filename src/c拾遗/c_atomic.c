/******************************************************************
*
* 采用build函数实现c语言数据的原子操作，实现C语言的无锁队列的基础
*
*******************************************************************/

/*
The ‘__atomic’ builtins can be used with any integral scalar or pointer type that is 1, 2,
4, or 8 bytes in length. 16-byte integral types are also allowed if ‘__int128’ 
is supported by the architecture.
*/


#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// oid __atomic_store (type *ptr, type *val, int memorder);
// type __atomic_add_fetch (type *ptr, type val, int memorder);
// type __atomic_fetch_add (type *ptr, type val, int memorder);
// void __atomic_exchange (type *ptr, type *val, type *ret, int memorder);
// bool __atomic_compare_exchange_n (type *ptr, type *expected, type desired, bool weak, int success_memorder, int failure_memorder)

#define NUM_THREADS 100
int a = 10;
int b = 20;
int c = 3;

void * fun(void * argv){
	for(int i = 0; i < 1000000; i++){
		//a = a + 1;
        // __sync_add_and_fetch(&a, 1);
		__atomic_add_fetch(&a, 1, __ATOMIC_RELAXED);
	}
}


struct IA{
    int i;
    int a;
};

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
    // __atomic_exchange(&b, &c, &ret, __ATOMIC_RELAXED);
    __atomic_exchange(&b, &c, &c, __ATOMIC_RELAXED);
	
	printf("%d\n", a);
	printf("%d\n", b);
	printf("%d\n", c);
	printf("%d\n", ret);
	
    struct IA old;
    old.i = 5;
    old.a = 'H';

    struct IA new;
    new.i = 7;
    new.a = 'T';

    struct IA tmp;

    printf("old %d,%c,  new %d,%c\n", old.i, old.a, new.i, new.a);
    __atomic_exchange(&old, &new, &tmp, __ATOMIC_RELAXED);
    printf("old %d,%c,  new %d,%c\n", old.i, old.a, new.i, new.a);

	tmp.i = 23;
	tmp.a = 'F';	
	__atomic_compare_exchange(&old, &new, &tmp, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
    printf("old %d,%c,  new %d,%c\n", old.i, old.a, new.i, new.a);


    __int128 h = 0;
}
