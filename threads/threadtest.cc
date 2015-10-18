// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"

// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------


// 每个thread上CPU之后，调度算法没有让他下CPU的策略，必须由thread自己交出CPU Yield
void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}


//----------------------------------------------------------------------
// 测试最多启动128个线程
//----------------------------------------------------------------------
void OneThread(int num)
{
	int loop = 2;
	while(loop--){
		printf("I am the thread whose tid=%d\n", currentThread->GetThreadId());
		currentThread->Yield();	
	}
}

void RunManyThreadsTest()
{
	DEBUG('t', "Entering RunManyThreadsTest");
	
	for(int i=1;i<=200;i++)
	{
		char name[20] = "forked thread";
		Thread *t = new Thread(name);
		
		// 后面产生的线程有更高的优先级.将会被更先运行.
		t->setYouxianji(i);
		t->Fork(OneThread, i);
	}
	currentThread->Yield();
}

//----------------------------------------------------------------------
// 测试TS:列出全部线程信息功能
//----------------------------------------------------------------------

void ListThreadsStatus()
{
	for(int i=1;i<=10;i++){
		ListAllThreads();	
		Thread *t = new Thread("thread test~");
		t->Fork(OneThread, i);
	}
	currentThread->Yield();
}


//----------------------------------------------------------------------
// 测试抢占式调度
//----------------------------------------------------------------------
void SchedTestFunc1(int num)
{
	int loop = 5;
	while(loop--){
		printf("I am the thread whose tid=%d,youxianji is %d\n", currentThread->GetThreadId(), currentThread->getYouxianji() );
		currentThread->Yield();	
	}
}

void SchedTestFunc2(int num)
{
	int counter = 5;
	int i = 0;
	while(true){
		i++;
		if(i>counter) break;
		printf("Thread %d whose youxianji is %d is counting %d\n",num,currentThread->getYouxianji(),i);
		currentThread->Yield();
	}
	currentThread->Yield();
}

void SchedulerTest(){
	for(int i=1;i<=10;i++){
		char name[20] = "forked thread";
		Thread *t = new Thread(name);
		
		// 在这里设置优先级
		t->setYouxianji(10-i);
		t->Fork(SchedTestFunc2,i);
	}
	currentThread->Yield();
}

//----------------------------------------------------------------------
// 测试时间片轮转
//----------------------------------------------------------------------

void TimeSliceThread(int num){
	int counter = 20;
	int i = 0;
	
	while(true){
		i++;
		if(i>counter) break;
		printf("Thread %d whose youxianji is %d, timeslice left %d,  is counting %d\n",
				num,
				currentThread->getYouxianji(),
				currentThread->getTimeSlice(),
				i);
		// 由于是在内核态，我们在这里手动模拟时钟走字
		interrupt->OneTick();
		interrupt->OneTick();
		interrupt->OneTick();
	}
	//currentThread->Yield();
}


void TimeSliceSchedulerTest(){
	int threadnum = 15;
	for(int i=1;i<=threadnum;i++){
		char name[20] = "forked thread";
		Thread *t = new Thread(name);
		
		// 在这里设置优先级
		t->setYouxianji(threadnum-i);
		
		// 在这里分配时间片
		t->addTimeSlice(3);
		
		t->Fork(TimeSliceThread,i);
	}
	currentThread->Yield();
}

//----------------------------------------------------------------------
// 测试同步原语
//----------------------------------------------------------------------

Lock * lock;
Condition * cd;

void getLock(int num){
	lock->Acquire();
	//currentThread->Yield();
	lock->Release();
}

void releaseLock(int num){
	lock->Release();
}

void LockTest(){
	lock = new struct Lock("Lock test");
	Thread *t1 = new Thread("lock thread 1");
	Thread *t2 = new Thread("lock thread 2");
	t1->Fork(getLock,0);
	//t1->ReleaseLock(0);
	//t1->Fork(releaseLock,0);
	t2->Fork(getLock,0);
	//t2->ReleaseLock(0);
}

void ConditionWait(int num){
	lock->Acquire();
	cd->Wait(lock);
	cd->Signal(lock);
}

void ConditionTest(){
	cd = new struct Condition("Condition test");
	lock = new struct Lock("Lock test");
	Thread * t1 = new Thread("condition test thread 1");
	t1 -> Fork(ConditionWait,0);
	
}

Semaphore semNempty("empty", 6);
Semaphore semNfull("full", 0);
Semaphore sempool[6];
bool pool[6] = {0};

void Producer(int arg)
{
    while (1) {
        semNempty.P();
        int i,j;
        for (i = 0; i < 6; i++) {
			sempool[i].P();
            if (!pool[i]) {
                pool[i] = true;
                printf("%s produces %d |", currentThread->getName(), i);
                sempool[i].V();
                currentThread->Yield();
                break;
            }
			else{
				sempool[i].V();
			}
        }
        semNfull.V();
    }
}

void Consumer(int arg)
{
    while (1) {
        semNfull.P();
        int i,j;
        for (i = 0; i < 6; i++) {
			sempool[i].P();
            if (pool[i]) {
                pool[i] = false;
                printf("%s consumes %d |", currentThread->getName(), i);
                sempool[i].V();
                currentThread->Yield();
                break;
            }
			else{
				sempool[i].V();
			}
        }
        semNempty.V();
    }
}

void PCtest1(){
	Thread *producer[5], *consumer[3];

	    producer[0] = new Thread("Producer 0");
	    producer[1] = new Thread("Producer 1");
	    producer[2] = new Thread("Producer 2");
	    producer[3] = new Thread("Producer 3");
	    producer[4] = new Thread("Producer 4");
	    consumer[0] = new Thread("Consumer 0");
	    consumer[1] = new Thread("Consumer 1");
	    consumer[2] = new Thread("Consumer 2");

	for (int i = 0; i < 5; i++) {
	    producer[i]->Fork(Producer, 0);
	}

	for (int i = 0; i < 3; i++) {
	    consumer[i]->Fork(Consumer, 0);
	}
}

bool used[5];
Semaphore metux;
Condition res[5]; // 0 -> 01, 1-> 12, 2->23, 3->34, 4->40 
Lock * cdlock[5];	  // 条件变量使用的lock


void eat(int arg){
	metux.P();
	if(!used[arg] && !used[(arg+1)%5]){
		used[arg] = 1;
		used[(arg+1)%5] = 1;
		printf("ph %d take chopsticks %d and %d , he begins to eat.\n",arg,arg,(arg+1)%5);
		metux.V();
	}
	else{
		metux.V();
		cdlock[arg]->Acquire();
		res[arg].Wait(cdlock[arg]);
		cdlock[arg]->Release();
	}
}

void putChop(int arg){
	metux.P();
	printf("ph %d eat up, he put chopsticks %d and %d.\n",arg,arg,(arg+1)%5);
	used[arg] = 0;
	used[(arg+1)%5] = 0;
	
	for(int i =arg+1;i<arg+6;i++){
		int k = i%5;
		if(!used[k] && !used[(k+1)%5])
			res[k].Signal(cdlock[k]);
	}
	metux.V();
}


void Philosopher(int arg){
	
	int loop = 5;
	
	while(loop --){
		eat(arg);
		currentThread->Yield();
		putChop(arg);
	}
}

void PhilosopherTest(){
	Thread * thread[5];
	for(int i=0;i<5;i++){
		used[i] = false;
		res[i] = Condition();
		cdlock[i] = new struct Lock();
	}
	for(int i=0;i<5;i++){
		thread[i] = new Thread("hehe");
		thread[i] -> Fork(Philosopher,i);
	}
}

void RWLOCKtest(){
	
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
	// testnum 表示的是测试的号码
    switch (testnum) {
    case 1:
		ThreadTest1();
		break;
	case 2:
		RunManyThreadsTest();
		break;
	case 3:
		ListThreadsStatus();
    	break;
	case 4:
		SchedulerTest();
		break;
	case 5:
		TimeSliceSchedulerTest();
		break;		
	case 6:
		LockTest();
		break;
	case 7:
		ConditionTest();
		break;
	case 8:
		PCtest1();
		break;
	case 9:
		PhilosopherTest();
		break;
	case 10:
		RWLOCKtest();
		break;
	default:
		printf("No test specified.\n");
		break;
    }
}

